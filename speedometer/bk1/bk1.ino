#define B 3950              // B-коэффициент
#define SERIAL_R 10000      // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 10000  // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25        // номинальная температура (при которой TR = 100 кОм)
//----------

#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


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
//---------voltmetr

float vin = 0.0;
float vout = 0.0;
float R1V = 4500.0;  //
float R2V = 1800.0;  //
float value = 0;

//----------oil press
float oil_press = 0;
float R1P = 3500;
float R2P = 0;



//----------oil temp
int oil_temp = 0;
float R1OT = 3500;
float R2OT = 0;
float buffer = 0;
float oil_temp_out = 0;

//----------fuel
int fuel = 0;

//----------engine temp
int engine_temp = 0;


//-----------tacho

volatile int tacho_counter = 0;
int tacho_counter_temp = 0;
long int old_time_tacho = 0;
int RPM = 0;

//------------speed

volatile int speed_counter = 0;
int speed_counter_temp = 0;
long int old_time_speed = 0;
float speed = 0;

//------------led
int led = 0;
int old_led = 0;


#define led_pin 4

#define input_tacho 2
#define input_speed 3

#define input_fuel A6
#define input_temp_engine A3
#define input_temp_oil A2
#define input_press_oil A1

#define input_temp_out A7
#define input_volt A0

#define color1 0, 50, 0
#define color2 50, 0, 0
#define color0 0, 0, 0

SoftwareSerial mySerial(5, 6);  // RX, TX

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, led_pin, NEO_GRB + NEO_KHZ800);

float out_temp = 0;




void setup() {
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'

  for (int i = 0; i < 7; i++) {
    strip.setPixelColor(i, color1);
    strip.show();
    delay(20);
  }

  for (int i = 6; i > -1; i--) {
    strip.setPixelColor(i, color0);
    strip.show();
    delay(20);
  }

  for (int i = 0; i < 7; i++) {
    strip.setPixelColor(i, color2);
    strip.show();
    delay(20);
  }

  for (int i = 6; i > -1; i--) {
    strip.setPixelColor(i, color0);
    strip.show();
    delay(20);
  }



  pinMode(input_fuel, INPUT);
  pinMode(input_temp_engine, INPUT);
  pinMode(input_temp_oil, INPUT);
  pinMode(input_press_oil, INPUT);

  pinMode(input_volt, INPUT);
  pinMode(input_temp_out, INPUT);


  Serial.begin(9600);
  mySerial.begin(9600);

  attachInterrupt(0, tacho, FALLING);
  attachInterrupt(1, speed_inter, FALLING);
}

void loop() {

  if (RPM < 500) led = 0;
  else if (RPM > 499 && RPM < 1000) led = 1;
  else if (RPM > 999 && RPM < 2000) led = 2;
  else if (RPM > 1999 && RPM < 3000) led = 3;
  else if (RPM > 2999 && RPM < 4000) led = 4;
  else if (RPM > 3999 && RPM < 5000) led = 5;
  else if (RPM > 4999 && RPM < 6000) led = 6;
  if (5999 < RPM) led = 7;

  if (old_led != led) {
    for (int i = 0; i < 7; i++) strip.setPixelColor(i, color0);
    if (led > 0 && led < 5) {
      for (int i = 0; i < led; i++) strip.setPixelColor(i, color1);
    } else if (led > 4) {
      for (int i = 0; i < led; i++) strip.setPixelColor(i, color2);
    }
    strip.show();
    old_led = led;
  }

  if (millis() - old_time_tacho > 500) {
    noInterrupts();
    tacho_counter_temp = tacho_counter;
    tacho_counter = 0;
    interrupts();
    old_time_tacho = millis();
    RPM = tacho_counter_temp * 60;
    //-----
    //-----
    mySerial.print("G");
    mySerial.println(RPM);
  }

  if (millis() - old_time_speed > 500) {
    noInterrupts();
    speed_counter_temp = speed_counter;
    speed_counter = 0;
    interrupts();
    old_time_speed = millis();
    speed = speed_counter_temp * 60;
    //speed--;
    // if (speed < 0) speed = 180;
    //-------
    mySerial.print("H");
    mySerial.println(speed);
  }

  //-------


  //-------
  out_temp = Getterm(analogRead(input_temp_out));
  out_temp *= 10;
  mySerial.print("A");
  mySerial.println(out_temp);
  delay(100);
  //-------
  value = analogRead(input_volt);
  vout = (value * 5.0) / 1024.0;  // see text
  vin = vout / (R2V / (R1V + R2V));
  vin += 0.7;
  vin *= 10;

  mySerial.print("B");
  mySerial.println(vin);

  delay(100);
  //--------
  value = analogRead(input_press_oil);

  oil_press = magic(value, 1050, 4300, 1572);



  mySerial.print("C");
  mySerial.println(oil_press * 10);
  //mySerial.println(oil_press * 10);
  delay(100);
  //--------
  //value = analogRead(input_temp_oil);

  value = magic(analogRead(input_temp_oil), 1050, 4300, 1572);



  oil_temp = Getterm_res(value);

  mySerial.print("G");
  mySerial.println(value);

  mySerial.print("D");
  mySerial.println(oil_temp + 40);
  delay(100);
  //--------
  value = analogRead(input_temp_engine);

  // engine_temp--;
  // if (engine_temp < -40) engine_temp = 130;

  mySerial.print("E");
  mySerial.println(engine_temp + 40);
  delay(100);
  //--------
  value = analogRead(input_fuel);

  // fuel--;
  // if (fuel < 0) fuel = 50;

  mySerial.print("F");
  mySerial.println(fuel);

  //strip.setPixelColor(5, 255, 0, 0);
  //strip.show();
  //delay(100);
  //strip.setPixelColor(5, 0, 0, 0);
  //strip.show();
  delay(100);
}


float magic(int valve, float R1, float R2, float R3) {
  float result;
  float x = valve * 0.0049;
  result = (((((x * R2) / R3) + x) * R1) / ((vin / 10 - (((x * R2) / R3) + x)) - (x / R3)));
  return (result);
}


float Getterm(int RawADC) {
  float tr = 1023.0 / RawADC - 1;
  tr = SERIAL_R / tr;
  float steinhart;
  steinhart = tr / THERMISTOR_R;            // (R/Ro)
  steinhart = log(steinhart);               // ln(R/Ro)
  steinhart /= B;                           // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_T + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;              // Invert
  steinhart -= 273.15;
  return steinhart;
}


float Getterm_res(float tr) {
  float steinhart;
  steinhart = tr / 3520;             // (R/Ro)
  steinhart = log(steinhart);        // ln(R/Ro)
  steinhart /= 3950;                 // 1/B * ln(R/Ro)
  steinhart += 1.0 / (20 + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;       // Invert
  steinhart -= 273.15;
  return steinhart;
}

void tacho() {
  tacho_counter++;
}

void speed_inter() {
  speed_counter++;
}
