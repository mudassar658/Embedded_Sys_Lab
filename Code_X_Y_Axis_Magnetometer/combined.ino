#include <SoftwareSerial.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

SoftwareSerial SwSerial(10, 11); // RX, TX
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#include <Wire.h>

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

static byte databuf[10] = {0};

static byte buf[17] = {0};
static bool bEn = false;

void clearBuf()
{
  for(int i=0; i<17; i++)
    buf[i] = 0;
}

void setup(){  
  //Initialize Serial and I2C communications
  Serial.begin(115200);
  Wire.begin();  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
  
  lcd.begin(16,2);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Gyroscope Data");

  databuf[0] = 0xff;
  databuf[1] = 0x11;
  databuf[2] = 0xdd;
}

void loop(){
  
  int x,y,z; //triple axis data
  int x1, x2, y1, y2;
  int xmin,xmax,ymin,ymax,zmin,zmax;
  xmin=0; xmax=0; ymax=0; ymin = 0; zmin=0;zmax=0;
  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x1 = Wire.read()<<8; //X msb
    x2 = Wire.read(); //X lsb
    x = x1 | x2;
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y1 = Wire.read()<<8; //Y msb
    y2 = Wire.read(); //Y lsb
    y = y1 | y2;
  }

    databuf[3] = x1;
    databuf[4] = x2;
    databuf[5] = y1;
    databuf[6] = y2;
    databuf[7] = 0x00;
  

  lcd.setCursor(0,1);
  lcd.print("X: ");
  lcd.setCursor(3,1);
  lcd.print(x);
  lcd.setCursor(8,1);
  lcd.print("Y: ");
  lcd.setCursor(11,1);  
  lcd.print(y);

  Serial.println();
  
//  Serial.print("  z: ");
//  Serial.println(z);  
  delay(500);
}
