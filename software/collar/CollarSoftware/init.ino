void init_pins(){
   pinMode(CHARGE_PG,  INPUT_PULLUP);
  pinMode(CHARGE_S1,  INPUT_PULLUP);
  pinMode(CHARGE_S2,  INPUT_PULLUP);
  pinMode(GPS_FIX,    INPUT);
  pinMode(RF_INT,     INPUT);
  pinMode(MPU_INT,    INPUT);
  
  pinMode(AMP_EN,     OUTPUT);
  pinMode(RF_CS,      OUTPUT);
  pinMode(FLASH_CS,   OUTPUT);
  pinMode(AUX_0,      OUTPUT);
  pinMode(AUX_1,      OUTPUT);
  pinMode(AUX_2,      OUTPUT);
}

void init_comms(){
  Wire.begin();           //I2C
  SerialUSB.begin(9600);  //Serial Monitor
  Serial1.begin(9600);    //GPS

}