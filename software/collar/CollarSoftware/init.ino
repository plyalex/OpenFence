void init_pins(){
  pinMode(CHARGE_PG,  INPUT_PULLUP);
  pinMode(CHARGE_S1,  INPUT_PULLUP);
  pinMode(CHARGE_S2,  INPUT_PULLUP);
  pinMode(GPS_FIX,    INPUT);
  pinMode(RF_INT,     INPUT);
  pinMode(MPU_INT,    INPUT);
  
  pinMode(AMP_EN,     OUTPUT);
  pinMode(RF_CS,      OUTPUT);
  digitalWrite(RF_CS, LOW);
  pinMode(FLASH_CS,   OUTPUT);
  digitalWrite(FLASH_CS, LOW);
  pinMode(AUX_0,      OUTPUT);
  pinMode(AUX_1,      OUTPUT);
  pinMode(AUX_2,      OUTPUT);

  pinMode(DAC_OUT,        INPUT);
}

void init_comms(){
  Wire.begin();           //I2C
  SerialUSB.begin(9600);  //Serial Monitor
  Serial1.begin(9600);    //GPS

}

void init_mpu(){
  //#define printMPU
  //#define calibMag
  mpu9250.resetMPU9250(); // Reset registers to default in preparation for device calibration
  mpu9250.calibrateMPU9250(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers 
  #ifdef printMPU
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
  #endif
  delay(200);
  mpu9250.initMPU9250(); 
  mpu9250.initAK8963(magCalibration);
  mpu9250.getAres(); // Get accelerometer sensitivity
  mpu9250.getGres(); // Get gyro sensitivity
  mpu9250.getMres(); // Get magnetometer sensitivity

  #ifdef printMPU
    SerialUSB.println("MPU9250 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
    SerialUSB.println(magCalibration[0]);
    SerialUSB.println(magCalibration[1]);
    SerialUSB.println(magCalibration[2]);
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
    SerialUSB.print("Accelerometer sensitivity is LSB/g : ");
    SerialUSB.println(1.0f/aRes);
    SerialUSB.print("Gyroscope sensitivity is LSB/deg/s : ");
    SerialUSB.println(1.0f/gRes);
    SerialUSB.print("Magnetometer sensitivity is LSB/G : ");
    SerialUSB.println(1.0f/mRes);
  #endif

  #ifdef calibMag
    mpu9250.magcalMPU9250(magbias, magscale);
    SerialUSB.println("AK8963 mag biases (mG)"); SerialUSB.println(magbias[0]); SerialUSB.println(magbias[1]); SerialUSB.println(magbias[2]); 
    SerialUSB.println("AK8963 mag scale (mG)"); SerialUSB.println(magscale[0]); SerialUSB.println(magscale[1]); SerialUSB.println(magscale[2]); 
    delay(2000); // add delay to see results before serial spew of data
  #endif

  magbias[0] = +30;//-90;//+470.; // User environmental x-axis correction in milliGauss, should be automatically calculated
  magbias[1] = +380;//+120.;      // User environmental y-axis correction in milliGauss
  magbias[2] = -200;//+125.;        // User environmental z-axis correction in milliGauss


}

void init_gps(){
  GPSSerial.println("$PMTK220,1000*1F"); //Set update frequency to 1Hz (Period 1000ms)
  GPSSerial.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"); //Print GPRMC and GPGGA only
}