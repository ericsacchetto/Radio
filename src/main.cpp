#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <Wire.h>
#include "Timer.h"


//Definitions

Timer t;

//LCD
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line

//Menu
int pageCounter = 1;
int up = 1;
int down = 2;
int right = 3;
int left = 4;
int timeSet = 5;

//Debounce
unsigned long debounceTime = 0;
unsigned long debounceDelay = 2000UL;
unsigned long timerBackligth = 0;

//BigFont
byte x10[8] = {0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00};
byte x11[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C};
byte x12[8] = {0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00};
byte x13[8] = {0x1F,0x1F,0x1F,0x07,0x07,0x07,0x07,0x07};
byte x14[8] = {0x1F,0x1F,0x1F,0x1C,0x1C,0x1C,0x1C,0x1C};
byte x15[8] = {0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07};
byte x16[8] = {0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00};
byte x17[8] = {0x00,0x00,0x00,0x00,0x0E,0x0E,0x0E,0x00};

byte row = 0,col = 0;

//RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int hourDecimal;
int hourUnit;
int minDecimal;
int minUnit;
int secDecimal;
int secUnit;
int aRead = 0;

//BigFont Characters
void doNumber(byte num, byte r, byte c) {
  lcd.setCursor(c,r);
  switch(num) {
    case 0: lcd.write(byte(4)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.write(byte(1)); lcd.write(byte(5));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 1: lcd.write(byte(0)); lcd.write(byte(1));
    lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(1));
    lcd.setCursor(c,r+2); lcd.write(byte(0)); lcd.write(byte(2)); break;

    case 2: lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(2));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 3: lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.write(byte(6)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 4: lcd.write(byte(1)); lcd.write(byte(5));
    lcd.setCursor(c,r+1); lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(0)); break;

    case 5: lcd.write(byte(4)); lcd.write(byte(2));
    lcd.setCursor(c,r+1); lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 6: lcd.write(byte(1)); lcd.print(" ");
    lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 7: lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(5));
    lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(0)); break;

    case 8: lcd.write(byte(4)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(2)); break;

    case 9: lcd.write(byte(4)); lcd.write(byte(3));
    lcd.setCursor(c,r+1); lcd.write(byte(2)); lcd.write(byte(3));
    lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(0)); break;

    case 11: lcd.setCursor(c,r); lcd.write(byte(7)); lcd.setCursor(c,r+1); lcd.write(byte(7)); break;
  }
}


int readKey(){
  int read = 0;
  int key = 0;
  read = analogRead(A0);
  delay(600);
  if(millis() - debounceTime > debounceDelay){  //Not working
    debounceTime = millis();                    //Not working
    if(read>1000){key=0;}
    else if(read<20){key=1;}
    else if(read>120 && read<140){key=2;}
    else if(read>310 && read<340){key=3;}
    else if(read>470 && read<500){key=4;}
    else if(read>710 && read<750){key=5;}
    return key;
  }
}


void setup(){

  Serial.begin(9600);

  //Manual RTC adjust (YYYY, MM, DD, HH, MM, SS)
  //rtc.adjust(DateTime(2019, 4, 21, 12, 58, 0));

  //LCD init
  Wire.begin();
  lcd.init();

  lcd.begin(20, 4);
  lcd.createChar(0, x10);                      // digit piece
  lcd.createChar(1, x11);                      // digit piece
  lcd.createChar(2, x12);                      // digit piece
  lcd.createChar(3, x13);                      // digit piece
  lcd.createChar(4, x14);                      // digit piece
  lcd.createChar(5, x15);                      // digit piece
  lcd.createChar(6, x16);                      // digit piece
  lcd.createChar(7, x17);                      // digit piece (colon)

}

void loop(){

  //RTC
  DateTime now = rtc.now();

  //Debounce
  if(analogRead(A0) < 1000){
    int button = readKey();
    switch(button){
      case 1:{
        lcd.clear();
        button = 0;
        if(pageCounter <3){
          pageCounter = pageCounter +1;
        }
        else if(pageCounter == 3){
          pageCounter = 1;
        }
      }
      break;

      case 2:{
        lcd.clear();
        if(pageCounter > 1){
          pageCounter = pageCounter - 1;
        }
        else{
          pageCounter = 1;
        }
      }
      break;
    }
  }


  //Screens
  switch(pageCounter){
    case 1:{
      if(analogRead(A0) < 1000){
        lcd.backlight();
        timerBackligth = millis();

      }
      if(millis() - timerBackligth > 5000){
        lcd.noBacklight();
  
      }

      if (now.hour()>9){hourDecimal = (now.hour()/10);}
      else{hourDecimal = 0;}
      hourUnit = now.hour()%10;

      if (now.minute()>9){minDecimal = (now.minute()/10);}
      else{minDecimal = 0;}
      minUnit = now.minute()%10;

      if (now.second()>9){secDecimal = (now.second()/10);}
      else{secDecimal = 0;}
      secUnit = now.second()%10;

      int year = 0;
      if (now.year()>99) {
        year = now.year() - 2000;
      }

      row = 0;
      doNumber( hourDecimal,row,2);
      doNumber( hourUnit,row,5);
      doNumber(11,row,7);
      doNumber( minDecimal,row,8);
      doNumber( minUnit,row,11);
      doNumber(11,row,13);
      doNumber( secDecimal,row,14);
      doNumber( secUnit,row,17);

      char dateBuffer[12];
      sprintf(dateBuffer, "%02d/%02d/%02d", now.day(), now.month(), year);
      lcd.setCursor(4,3);
      lcd.print(dateBuffer);
      lcd.print(" ");
      lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    }
    break;

    case 2:{
      lcd.setCursor(5,0);
      lcd.print("Page 2");
    }
    break;

    case 3:{
      lcd.setCursor(5, 1);
      lcd.print("page 3");
    }
    break;

  }

}
