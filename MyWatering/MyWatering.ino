/* Code body from ReadAnalogVoltage.ino */

#include <string.h>

int lvlPin = A0;
int soilPin = A1;
int relayPin = 2;
int wateringtime=5; /// watering process time constant in "second";
int RH_setpoint = 45;
int LVL_setpoint = 35;
unsigned long detectingtime=60000; /// if soil_realRH > [RH set point] then sleep for 60 second. 
unsigned long standbytime=60000;
String prompt;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int lvl_sensorValue = analogRead(lvlPin);
  int lvl_realHeight = map(lvl_sensorValue, 0, 670, 0, 100);
  int soil_sensorValue = analogRead(soilPin);
  int soil_realRH = map(soil_sensorValue, 0, 670, 0, 100);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // float lvl_voltage = lvl_sensorValue * (5.0 / 1023.0);
  // float soil_voltage = soil_sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  prompt.concat("lvl= ");
  prompt.concat(lvl_realHeight);
  prompt.concat(", soilRH= ");
  prompt.concat(soil_realRH);
  //Serial.println(lvl_sensorValue);
  // Serial.println(lvl_realHeight);
  // Serial.println(soil_realRH);
  Serial.println(prompt);
  prompt = "";
  // lvl_realHeight=61;
  delay(100);
  if(lvl_realHeight > LVL_setpoint){
    // soil_realRH = 30;
    if(soil_realRH < RH_setpoint){
      digitalWrite(relayPin, HIGH);
      Serial.println("watering...");
      delay(wateringtime * 1000);
    }
    else{
      digitalWrite(relayPin, LOW);
      //prompt = "";
      standbytime=detectingtime/2;
      prompt.concat("RH > setpoint=[");
      prompt.concat(RH_setpoint);
      prompt.concat("]. Stand by ");
      prompt.concat(standbytime/1000);
      prompt.concat(" sec");
      Serial.println(prompt);
      delay(standbytime);
    }
  } else
  {
    prompt = "";
    prompt.concat("Lower water level ... setpoint=[");
    prompt.concat(LVL_setpoint);
    prompt.concat("], Please refill water tank!!");
    Serial.println(prompt);
    digitalWrite(relayPin, LOW);
    delay(detectingtime/30);
  }
  prompt = "";  
}
