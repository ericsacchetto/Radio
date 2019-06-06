#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <Wire.h>



//Definitions

//LCD
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line

//Menu
int pageCounter = 1;

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

//Read analog port and debounce
int read = 0;
unsigned long debounceTime;
bool buttonPress = false;
int readKey(){
  int key = 0;
  if(analogRead(A0) < 1000){
    read = analogRead(A0);
    buttonPress = true;
    debounceTime = millis();
  }
  //delay(200);
  if(buttonPress){
    if((millis() - debounceTime) > 200){
      if(read<20){key=1;}
      if(read>120 && read<140){key=2;}
      if(read>310 && read<340){key=3;}
      if(read>470 && read<500){key=4;}
      if(read>710 && read<750){key=5;}
      buttonPress = false;
      Serial.print(key);
      return key;
    }
  }
  /*
  if(read>1000){key=0;}
  else if(read<20){key=1;}
  else if(read>120 && read<140){key=2;}
  else if(read>310 && read<340){key=3;}
  else if(read>470 && read<500){key=4;}
  else if(read>710 && read<750){key=5;}
  return key;
  */
}

int setHour, setMinute, setSecond, setDay, setMonth, setYear;
void showTime(){
  DateTime now = rtc.now();

  //Showing hour in two digits format
  String myHourString = "";
  setHour = now.hour();
  if(setHour < 10 ){
    myHourString = '0';
  }
  myHourString.concat(setHour);

  //Showing minute in two digits fotmat
  String myMinString = "";
  setMinute = now.minute();
  if(setMinute < 10 ){
    myMinString = '0';
  }
  myMinString.concat(setMinute);

  //Showing seconds in two digits format
  String mySecString = "";
  setSecond = now.second();
  if(setSecond < 10 ){
    mySecString = '0';
  }
  mySecString.concat(setSecond);

  //Showing day in two digits format
  String myDayString = "";
  setDay = now.day();
  if(setDay < 10 ){
    myDayString = '0';
  }
  myDayString.concat(setDay);

  //Showing month in two digits format
  String myMonString = "";
  setMonth = now.month();
  if(setMonth < 10 ){
    myMonString = '0';
  }
  myMonString.concat(setMonth);

  //Showing year in two digits format
  String myYearString = "";
  if(now.year() > 99){
    setYear = now.year()-2000UL;
  }
  if(setYear < 10){
    myYearString = '0';
  }
  myYearString.concat(setYear);

  lcd.setCursor(1, 3);
  lcd.print(myHourString);
  lcd.print(":");
  lcd.print(myMinString);
  lcd.print(":");
  lcd.print(mySecString);
  lcd.print(" ");
  lcd.print(myDayString);
  lcd.print("/");
  lcd.print(myMonString);
  lcd.print("/");
  lcd.print(myYearString);
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

int counter = 0;
int itemCounter = 0;
String item;
unsigned long timerBackligth = 0;
bool backlightFlag = false;
void loop(){
  //Activate backlight and set screen 2 when switched on
  if(digitalRead(4) == 0){
    pageCounter = 1;
    if(analogRead(A0) > 1000){
      lcd.noBacklight();
    }
    else if((analogRead(A0) < 1000)){
      backlightFlag = true;
      timerBackligth = millis();
    }
    if(backlightFlag){
      lcd.backlight();
      if((millis() - timerBackligth) > 5000){
        backlightFlag = false;
      }
    }
  }

  if(digitalRead(4) == 1 && pageCounter == 1){
    lcd.backlight();
    lcd.clear();
    pageCounter = 2;
  }

  //RTC
  DateTime now = rtc.now();


  //Screens
  if(pageCounter == 1){

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

    else if(pageCounter == 2){

      showTime();

      if(readKey() == 5){
        lcd.clear();
        pageCounter = 3;
      }
    }

    else if(pageCounter == 3){

      if(counter == 0){
        setHour = now.hour();
        setMinute = now.minute();
        setSecond = now.second();
        setDay = now.day();
        setMonth = now.month();
        setYear = now.year();
        counter = 1;
      }

      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(item);

      if(itemCounter == 0){item = "Hour";}
      else if(itemCounter == 1){item = "Minute";}
      else if(itemCounter == 2){item = "Second";}
      else if(itemCounter == 3){item = "Day";}
      else if(itemCounter == 4){item = "Month";}
      else if(itemCounter == 5){item = "Year";}

/*
      if(readKey() == 1){
        itemCounter++;
        Serial.print("ReadKey 1");
      }
      if(readKey() == 2){
        itemCounter--;
        Serial.print("ReadKey 2");
      }
*/
      //Showing hour in two digits format
      String myHourString = "";
      if(itemCounter == 0){
        if(readKey() == 3){setHour++;}
        if(setHour == 25){setHour = 0;}
        if(readKey() == 4){setHour--;}
        if(setHour == -1){setHour = 24;}
        if(setHour < 10 ){myHourString = '0';}
      myHourString.concat(setHour);
      }

      //Showing minute in two digits fotmat
      String myMinString = "";
      if(itemCounter == 1){
        if(readKey() == 3){
          setMinute++;
        }
        if(setMinute == 60){setMinute = 0;}
      }
      if(itemCounter == 1){
        if(readKey() == 4){
          setMinute--;
        }
        if(setMinute == -1){setMinute = 59;}
      }
      if(setMinute < 10 ){
        myMinString = '0';
      }
      myMinString.concat(setMinute);

      //Showing seconds in two digits format
      String mySecString = "";
      if(itemCounter == 2){
        if(readKey() == 3){
          setSecond++;
        }
        if(setSecond == 60){setSecond = 0;}
      }
      if(itemCounter == 2){
        if(readKey() == 4){
          setSecond--;
        }
        if(setSecond == -1){setSecond = 59;}
      }
      if(setSecond < 10 ){
        mySecString = '0';
      }
      mySecString.concat(setSecond);

      //Showing day in two digits format
      String myDayString = "";
      if(itemCounter == 3){
        if(readKey() == 3){
          setDay++;
        }
        if(setDay == 32){setDay = 0;}
      }
      if(itemCounter == 3){
        if(readKey() == 4){
          setDay--;
        }
        if(setDay == -1){setDay = 31;}
      }
      if(setDay < 10 ){
        myDayString = '0';
      }
      myDayString.concat(setDay);

      //Showing month in two digits format
      String myMonString = "";
      if(itemCounter == 4){
        if(readKey() == 3){
          setMonth++;
        }
        if(setMonth == 13){setMonth = 0;}
      }
      if(itemCounter == 4){
        if(readKey() == 4){
          setMonth--;
        }
        if(setMonth == -1){setMonth = 12;}
      }
      if(setMonth < 10 ){
        myMonString = '0';
      }
      myMonString.concat(setMonth);

      //Showing year in two digits format
      String myYearString = "";
      if(itemCounter == 5){
        if(readKey() == 3){
          setYear++;
        }
        if(setYear == 100){setYear = 0;}
      }
      if(itemCounter == 5){
        if(readKey() == 4){
          setYear--;
        }
        if(setYear == -1){setYear = 99;}
      }
      if(setYear > 99){
        setYear -= 2000;
      }
      if(setYear < 10){
        myYearString = '0';
      }
      myYearString.concat(setYear);

      lcd.setCursor(1, 2);
      lcd.print(myHourString);
      lcd.print(":");
      lcd.print(myMinString);
      lcd.print(":");
      lcd.print(mySecString);
      lcd.print(" ");
      lcd.print(myDayString);
      lcd.print("/");
      lcd.print(myMonString);
      lcd.print("/");
      lcd.print(myYearString);


      if(readKey() == 5){
        rtc.adjust(DateTime(setYear, setMonth, setDay, setHour, setMinute, setSecond));
        lcd.clear();
        pageCounter = 2;
      }

    }

}
