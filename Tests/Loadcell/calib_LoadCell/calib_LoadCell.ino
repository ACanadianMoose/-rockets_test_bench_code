#include "HX711.h"
#include "Teensy_PinDefinition.h"
#include <LiquidCrystal_I2C.h>

#define KNOWN_WEIGTH 1.416

LiquidCrystal_I2C lcd(0x27,16,2);

HX711 scale50;
float weigth = 0;

int LCDCount = 0;

bool offsetCheck = 0;

void setup() {
  // put your setup code here, to run once:
  scale50.begin(PIN_LC350_DAT, PIN_LC350_CLK);
/*   scale50.set_scale(LC50_DIVIDER);
  scale50.set_offset(LC50_OFFSET); */
  Serial.print("50 Kg Scale initialized.\n");
Wire.begin();
  LCD_init(); 
}

void loop() {
  // put your main code here, to run repeatedly:


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Make sure the");
  lcd.setCursor(0,1);
  lcd.print("scale is empty");

  delay(5000);

while(offsetCheck){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(scale50.get_units(50));
  while(1);
}

  scale50.tare();
  scale50.set_scale();


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Put known weigth");
  lcd.setCursor(0,1);
  lcd.print("on scale");

  delay(5000);

  float RawReading = scale50.get_units(50);
  float scale = RawReading/KNOWN_WEIGTH;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("remove weigth");

  delay(5000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(scale);

  scale50.set_scale(scale);
  scale50.tare();	



  while(1){
    unsigned long loopStartTime = micros();

    while(!scale50.is_ready());
    weigth = scale50.get_units();
    


    unsigned long loopEndTime = micros();
    float freq = 1000000/float((loopEndTime - loopStartTime)); // 10^-9

    if(LCDCount >= 50){
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(weigth);
      lcd.setCursor(9,1);
      lcd.print(freq);
      LCDCount=0;
    }
    else LCDCount++;
  }
}
