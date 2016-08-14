#include "PinDefines.h"
#include "MCP73871.h"
#include "PA6C.h"
#include "MPU9250.h"
#include "Audio_OF.h"
#include "Adafruit_ZeroTimer.h"
#include <Wire.h>
#include <RTCZero.h>


//MPU 0x68, POT 0x2F
//GPS_TX, RX = Serial
//Serial Monitor = SerialUSB
MCP73871 battery;
RTCZero rtc;
PA6C gps;
MPU9250 mpu9250;
Audio_OF audio;

// Adafruit_ZeroTimer zt4 = Adafruit_ZeroTimer(4);

// // timer 4 callback, set dac output!
// volatile uint16_t dacout=0;
// void Timer4Callback0(struct tc_module *const module_inst)
// {
//   //analogWrite(A0, dacout++); // too slow!

//   // we'll write the DAC by hand
//   // wait till it's ready
//   while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
//   // and write the data  
//   DAC->DATA.reg = dacout++;

//   // wraparound when we hit 10 bits  
//   if (dacout == 0x400) {
//     dacout = 0;
//   }
// }

void setup() {
  init_pins();
  init_comms();
  //audio.initAudio();
  audio.enableAmp();
  analogWriteResolution(10);
  analogWrite(A0, 128); // initialize the DAC

  rtc.begin(); // initialize RTC

  // zt4.configure(TC_CLOCK_PRESCALER_DIV1, // prescaler
  //               TC_COUNTER_SIZE_8BIT,   // bit width of timer/counter
  //               TC_WAVE_GENERATION_MATCH_PWM  // match style
  //               );

  // zt4.setPeriodMatch(10000, 1, 0); // ~350khz, 1 match, channel 0
  // zt4.setCallback(true, TC_CALLBACK_CC_CHANNEL0, Timer4Callback0);  // set DAC in the callback
  //zt4.enable(true);


  mpu9250.resetMPU9250(); // Reset registers to default in preparation for device calibration
  mpu9250.calibrateMPU9250(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  
  SerialUSB.print("x gyro bias = ");
  SerialUSB.println(gyroBias[0]);
  SerialUSB.print("y gyro bias = ");
  SerialUSB.println(gyroBias[1]);
  SerialUSB.print("z gyro bias = "); 
  SerialUSB.println(gyroBias[2]);
  SerialUSB.print("x accel bias = "); 
  SerialUSB.println(accelBias[0]);
  SerialUSB.print("y accel bias = "); 
  SerialUSB.println(accelBias[1]);
  SerialUSB.print("z accel bias = "); 
  SerialUSB.println(accelBias[2]);
  delay(2000);
  mpu9250.initMPU9250(); 
  SerialUSB.println("MPU9250 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
  mpu9250.initAK8963(magCalibration);
  SerialUSB.println("AK8963 initialized for active data mode...."); // Initialize device for active mode read of magnetometer
  SerialUSB.print("Accelerometer full-scale range (g)= ");
  SerialUSB.println(2.0f*(float)(1<<Ascale));
  SerialUSB.print("Gyroscope full-scale range (deg/s)= ");
  SerialUSB.println(250.0f*(float)(1<<Gscale));
  if(Mscale == 0) SerialUSB.println("Magnetometer resolution = 14  bits");
  if(Mscale == 1) SerialUSB.println("Magnetometer resolution = 16  bits");
  if(Mmode == 2) SerialUSB.println("Magnetometer ODR = 8 Hz");
  if(Mmode == 6) SerialUSB.println("Magnetometer ODR = 100 Hz");
  delay(2000);
  mpu9250.getAres(); // Get accelerometer sensitivity
  mpu9250.getGres(); // Get gyro sensitivity
  mpu9250.getMres(); // Get magnetometer sensitivity
  SerialUSB.print("Accelerometer sensitivity is LSB/g : ");
  SerialUSB.println(1.0f/aRes);
  SerialUSB.print("Gyroscope sensitivity is LSB/deg/s : ");
  SerialUSB.println(1.0f/gRes);
  SerialUSB.print("Magnetometer sensitivity is LSB/G : ");
  SerialUSB.println(1.0f/mRes);
  magbias[0] = +470.;  // User environmental x-axis correction in milliGauss, should be automatically calculated
  magbias[1] = +120.;  // User environmental x-axis correction in milliGauss
  magbias[2] = +125.;  // User environmental x-axis correction in milliGauss
}


int dacValue = 0;
int i=0;

void loop() {
  audio.setvolumeRight(i);
  //audioout();

  //gps.getGPRMC();


  if(mpu9250.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {  // On interrupt, check if data ready interrupt

    mpu9250.readAccelData(accelCount);  // Read the x/y/z adc values   
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];   
    az = (float)accelCount[2]*aRes - accelBias[2];  
   
    mpu9250.readGyroData(gyroCount);  // Read the x/y/z adc values
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes - gyroBias[2];   
  
    mpu9250.readMagData(magCount);  // Read the x/y/z adc values   
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental corrections
    mx = (float)magCount[0]*mRes*magCalibration[0] - magbias[0];  // get actual magnetometer value, this depends on scale being set
    my = (float)magCount[1]*mRes*magCalibration[1] - magbias[1];  
    mz = (float)magCount[2]*mRes*magCalibration[2] - magbias[2];   
  }


  SerialUSB.print("Accel x y z: "); SerialUSB.print("\t"); 
    SerialUSB.print(1000*ax); SerialUSB.print("\t"); 
    SerialUSB.print(1000*ay); SerialUSB.print("\t"); 
    SerialUSB.println(1000*az);

  SerialUSB.print("Gyro x y z: "); SerialUSB.print("\t"); 
    SerialUSB.print(gx); SerialUSB.print("\t"); 
    SerialUSB.print(gy); SerialUSB.print("\t"); 
    SerialUSB.println(gz); 

  SerialUSB.print("Mag x y z: "); SerialUSB.print("\t"); 
    SerialUSB.print(mx); SerialUSB.print("\t"); 
    SerialUSB.print(my); SerialUSB.print("\t"); 
    SerialUSB.println(mz); 
    
    tempCount = mpu9250.readTempData();  // Read the adc values
    temperature = ((float) tempCount) / 333.87f + 21.0f; // Temperature in degrees Centigrade
    SerialUSB.print(" temperature (C)= ");
    SerialUSB.println(temperature); 

  gps.getGPRMC();



  delay(500);
}





void audioout(){
  byte seconds=rtc.getSeconds();
  byte seconds2=rtc.getSeconds();
  while(seconds == seconds2){
    int dacValue = !dacValue;
    delayMicroseconds(600);
    // Generate a voltage between 0 and 3.3V.
    analogWrite(PIN_DAC0, dacValue);
    seconds2=rtc.getSeconds();
  }
}