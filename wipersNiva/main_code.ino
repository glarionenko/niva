const int wipersRelayOutPin = 2;
const int washerRelayOutPin = 3;
const int wipersInPin = A0;
const int washerInPin = A1;
const int maxWashingTime = 15000;
const int thirdModePause = 5000;
const int secondModePause = 10000;

GLWipers wipers;

void setup() {
  wipers.init(wipersInPin, washerInPin, wipersRelayOutPin, washerRelayOutPin, maxWashingTime, secondModePause, thirdModePause);
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  wipers.loop();
  
}
