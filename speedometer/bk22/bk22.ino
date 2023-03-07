
#include <MCUFRIEND_kbv.h>



#include "Adafruit_GFX.h"


MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 115, TS_RT = 909, TS_TOP = 75, TS_BOT = 886;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 3;    //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


//---------serial_event
/*
  A - outdoor temp
  B - voltmetr
  C - oil press
  D - oil temp
  E - engine temp
  F - fuel leavle
  G - tacho
  H - speed
*/

//--------
int int_temp = 0;
float old_out_temp = 0;
float old_press = 0;
int old_fuel = 0;
int fuel = 0;

int old_oil_temp = 0;

int int_out_temp = 0;
float out_temp = 0;
int flag_read = 0;
char a;
String input_string = "";

char name_object = 0;
String input_string_dont_name = "";

//----------
int RPM = 0;
int new_RPM = 0;
int old_RPM = 0;

int speed = 0;
int new_speed = 0;
int old_speed = 0;

int new_out_temp = 0;
int new_volt = 0;
float volt = 0;
int int_volt = 0;
float old_volt = 0;

//----------
int new_press = 0;
float oil_press = 0;
int int_oil_press = 0;

int oil_temp = 0;

int new_oil_temp = 0;

int engine_temp = 0;
int new_engine_temp = 0;
int old_engine_temp = 0;

int fuel_leavle = 0;
int new_fuel = 0;



void setup() {
  uint16_t tmp;

  Serial.begin(9600);

  tft.reset();
  ID = tft.readID();
  tft.begin(ID);
  Serial.begin(9600);
  tft.setRotation(Orientation);
  tft.fillScreen(BLACK);

  //tft.drawRect(0, 0, 124, 122, WHITE);
  tft.setTextSize(1);
  for (int i = 0; i < 9; i++) {
    tft.setCursor((i * 35) + 5, 35);
    tft.println(i);
  }
  tft.setTextSize(2);
  tft.setCursor(85, 105); //x ,y
  tft.setTextColor(WHITE);
  tft.print("RPM");
  tft.setCursor(240, 105);
  tft.print("KM/H");


  tft.setTextSize(5);
  tft.setCursor(8, 55); //x ,y
  tft.setTextColor(CYAN, BLACK);
  tft.print(RPM);
  tft.setCursor(280, 55);
  tft.print("0");

  tft.drawLine(0, 30, 320, 30, WHITE);
  tft.drawLine(0, 120, 320, 120, WHITE);
  tft.drawLine(0, 167, 320, 167, BLUE);
  tft.drawLine(0, 215, 320, 215, BLUE);

  tft.setTextSize(2);
  tft.setCursor(15, 220); //x ,y
  tft.setTextColor(WHITE, BLACK);
  //tft.print("-3,5");
  tft.setCursor(210, 220);
  tft.print("65323 km");
  tft.setTextSize(1);
  tft.setCursor(68, 217); //x ,y
  tft.print("o");

  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(5, 127);
  tft.print("n/a");

  tft.setCursor(90, 127);
  tft.print("oil");
  tft.setCursor(90, 145);
  tft.print("temp");

 // tft.drawRect(59, 127, 95, 15, WHITE);
 // tft.fillRect(61, 129, 43, 11, GREEN);

  tft.setCursor(241, 145);
  tft.print("fuel");

  //tft.drawRect(220, 130, 310, 145, WHITE);
  //tft.fillRect(222, 132, 308, 143, GREEN);

 // tft.drawRect(220, 127, 95, 15, WHITE);
 // tft.fillRect(222, 129, 51, 11, GREEN);

  tft.setCursor(5, 177);
  tft.print("n/a");

  tft.setCursor(90, 177);
  tft.print("bar");
  tft.setCursor(90, 195);
  tft.print("press");

  //tft.drawRect(60, 177, 155, 192, WHITE);
  //tft.fillRect(62, 179, 105, 190, GREEN);

 // tft.drawRect(59, 177, 95, 15, WHITE);
 // tft.fillRect(61, 179, 32, 11, GREEN);

  tft.setCursor(165, 177);
  tft.print("n/a");
  tft.setCursor(241, 177);
  tft.print("engine");
  tft.setCursor(241, 195);
  tft.print("temp");

  //tft.drawRect(220, 177, 310, 192, WHITE);
  //tft.fillRect(222, 179, 98, 190, GREEN);

 // tft.drawRect(220, 177, 95, 15, WHITE);
 // tft.fillRect(222, 179, 33, 11, GREEN);

  tft.setTextSize(1);
  tft.setCursor(35, 124); //x ,y
  tft.print("o");

  tft.setCursor(201, 174); //x ,y
  tft.print("o");



  //tft.fillRect(3, 3, 82, 22, YELLOW); //tacho
  //  tft.drawRect(10, 10, 82, 20, WHITE);

  pinMode(A5, INPUT);
  tft.setTextWrap(1);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
}

void loop() {


  Parser();

  if (new_out_temp == 1) {
    if (old_out_temp != out_temp) {
      tft.setTextSize(2);
      tft.setCursor(15, 220);
      tft.setTextColor(WHITE, BLACK);
      if(out_temp > -50 && out_temp < 120) tft.print(out_temp, 1);
      else tft.print("F");
      old_out_temp = out_temp;
    }
    new_out_temp = 0;
  }

  if (new_volt == 1) {
    if (old_volt > 9.9 && volt < 10)  tft.fillRect(125, 220, 47, 22, BLACK);
    tft.setTextSize(2);
    if (volt < 10) {
      tft.setCursor(140, 220);
    }
    else {
      tft.setCursor(129, 220);
    }
    tft.setTextColor(WHITE, BLACK);
    tft.print(volt, 1);
    new_volt = 0;
    old_volt = volt;
  }

  if (new_press == 1) {
    if (old_press != oil_press) {
      tft.setTextSize(4);
      tft.setCursor(5, 173); //x ,y
        if(oil_press < 0.8) tft.setTextColor(RED, BLACK);
        else if(oil_press < 5) tft.setTextColor(WHITE, BLACK);
        else tft.setTextColor(RED, BLACK);
      tft.fillRect(5, 173, 60, 35, BLACK);
      tft.print(oil_press, 1);
      old_press = oil_press;
    }
    new_press = 0;
  }

  if (new_oil_temp == 1) {
    if (old_oil_temp != oil_temp) {
      tft.setTextSize(4);
      tft.setCursor(5, 127); //x ,y
        if (oil_temp < 40) tft.setTextColor(BLUE, BLACK);
        else if (oil_temp < 95) tft.setTextColor(WHITE, BLACK);
        else tft.setTextColor(RED, BLACK);
      tft.fillRect(1, 125, 88, 35, BLACK);
      if(oil_temp < 0) tft.print("low");
      else tft.print(oil_temp);
      old_oil_temp = oil_temp;
    }
    new_oil_temp = 0;
  }



  if (new_engine_temp == 1) {
    if (old_engine_temp != engine_temp) {
      tft.setTextSize(4);
        if (engine_temp < 40) tft.setTextColor(BLUE, BLACK);
        else if (engine_temp < 95) tft.setTextColor(WHITE, BLACK);
        else tft.setTextColor(RED, BLACK);
      tft.setCursor(165, 177);
      tft.fillRect(165, 175, 75, 35, BLACK);
      tft.print(engine_temp);

      old_engine_temp = engine_temp;
    }
    new_engine_temp = 0;
  }

 
  
  if(new_fuel == 1) {
    if(old_fuel != fuel) {
      tft.setTextSize(4);
        if(fuel < 7) tft.setTextColor(RED, BLACK);
        else if(fuel < 15) tft.setTextColor(YELLOW, BLACK);
        else tft.setTextColor(WHITE, BLACK);
      tft.setCursor(165, 130);
      tft.fillRect(165, 130, 60, 35, BLACK);
      tft.print(fuel);
      old_fuel = fuel;
    }
    new_fuel = 0;
  }



  if(new_RPM == 1) {
    if(old_RPM != RPM) {
      tft.setTextSize(5);
        if(RPM > 6000) tft.setTextColor(RED, BLACK);
        else tft.setTextColor(CYAN, BLACK);
        
        if(old_RPM < 10 && RPM > 9) tft.fillRect(1, 55, 160, 45, BLACK);
        if(old_RPM > 9 && RPM < 9) tft.fillRect(1, 55, 160, 45, BLACK);
        if(old_RPM < 1000 && RPM > 999) tft.fillRect(1, 55, 160, 45, BLACK);
        if(old_RPM > 999 && RPM < 999) tft.fillRect(1, 55, 160, 45, BLACK);
        tft.setCursor(8, 55); //x ,y
        tft.print(RPM);
        //3, 3, x, 22
//        if(RPM > old_RPM) tft.fillRect(old_RPM/28, 3, (RPM - old_RPM)/28, 22, YELLOW);
//        else tft.fillRect(RPM/28, 3, (old_RPM - RPM)/28, 22, BLACK);
          if(RPM > old_RPM) tft.fillRect(3, 3, RPM/28, 22, YELLOW);
          else tft.fillRect(320, 3, -(320 - RPM/28), 22, BLACK);
        old_RPM = RPM;
    }
    new_RPM = 0;
  }


 if(new_speed == 1) {
    if(old_speed != speed) {
      tft.setTextSize(5);
        if(speed > 140) tft.setTextColor(RED, BLACK);
        else tft.setTextColor(CYAN, BLACK);
        if(old_speed < 10 && speed > 9) tft.fillRect(160, 55, 150, 35, BLACK);
        if(old_speed > 9 && speed < 10) tft.fillRect(160, 55, 150, 35, BLACK);
        if(old_speed > 99 && speed < 100) tft.fillRect(160, 55, 150, 35, BLACK);
        if(old_speed < 1000 && speed > 999) tft.fillRect(160, 55, 150, 35, BLACK);
        if(old_speed > 999 && speed < 1000) tft.fillRect(160, 55, 150, 35, BLACK);
        tft.setCursor(190, 55); //x ,y
        tft.print(speed);
        old_speed = speed;
    }
    new_speed = 0;
  }
 
 


}

void Parser() {
  if (Serial.available() > 0)
  {
    a = Serial.read();
    if (a == '\n') {
      flag_read = 1;

      name_object = input_string[0];
      input_string_dont_name = input_string.substring(1);

      switch (name_object) {
        case 'A':
          int_temp = input_string_dont_name.toInt();
          out_temp = float(int_temp) / 10;
          new_out_temp = 1;
          break;

        case 'B':
          int_temp = input_string_dont_name.toInt();
          volt = float(int_temp) / 10;
          new_volt = 1;
          break;

        case 'C':
          int_temp = input_string_dont_name.toInt();
          oil_press = float(int_temp) / 10;
          new_press = 1;
          break;

        case 'D':
          int_temp = input_string_dont_name.toInt();
          oil_temp = int_temp - 40;
          new_oil_temp = 1;
          break;

        case 'E':
          int_temp = input_string_dont_name.toInt();
          engine_temp = int_temp - 40;
          new_engine_temp = 1;
          break;

        case 'F':
          int_temp = input_string_dont_name.toInt();
          fuel = int_temp;
          new_fuel = 1;
          break;

        case 'G':
          int_temp = input_string_dont_name.toInt();
          RPM = int_temp;
          new_RPM = 1;
          break;


        case 'H':
          int_temp = input_string_dont_name.toInt();
          speed = int_temp;
          new_speed = 1;
          break;


      }
      input_string = "";
    }
    else {
      input_string += char(a);
    }
  }
}

