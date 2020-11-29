#include <ArduinoJson.h>
#include <Servo.h>

#define ledRed 13
#define ledGreen 12
#define beepPin 9

#define ultrasonicTriggerPin 6
#define ultrasonicEchoPin 7

#define servoPin 10

// set useHC_SR04 to 0 and useUltrasonicRanger to 1 if you have the Ultrasonic Ranger sensor.
#define useHC_SR04 1
#define useUltrasonicRanger 0
#define useServo 1

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int servoPos = 0;

Servo servo;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  pinMode(beepPin, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  servo.attach(servoPin);
  //beep(50);
  //beep(50);
  //beep(50);
  //delay(1000);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(inputString);
    bool led = root["led"];
    bool beepV = root["beep"];
    int angleV = root["angle"];
    if (led) {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledGreen, LOW);
    }
    if (beepV) {
      beep(200);
    }
    if(angleV >= 0 && angleV <= 180){
      servoPos = angleV;
    }
    
    // Serial.println(beepV);
    // Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
  }
  if (useUltrasonicRanger) {
    int duration = pingUltrasonicRanger();
    int cm = microsecondsToCentimeters(duration);
    sendRange(cm);
  }
  if (useHC_SR04) {
    int duration = pingHCSR04();
    int cm = microsecondsToCentimeters(duration);
    sendRange(cm);
  }

  if(useServo){
    servo.write(servoPos); 
  }

}

// source: http://www.mikrocontroller-elektronik.de/ultraschallsensor-hc-sr04/
int pingHCSR04() {
  long mtime = 0;
  pinMode(ultrasonicTriggerPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  digitalWrite(ultrasonicTriggerPin, LOW);
  delayMicroseconds(3);
  noInterrupts();
  digitalWrite(ultrasonicTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTriggerPin, LOW);
  mtime = pulseIn(ultrasonicEchoPin, HIGH);
  interrupts();
  return mtime;
}

// source: https://www.arduino.cc/en/Tutorial/Ping
int pingUltrasonicRanger() {
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(ultrasonicEchoPin, OUTPUT);
  digitalWrite(ultrasonicEchoPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicEchoPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrasonicEchoPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(ultrasonicEchoPin, INPUT);
  duration = pulseIn(ultrasonicEchoPin, HIGH);
  return duration;

}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void sendRange(int range_cm) {

  Serial.print("{\"range\":");
  Serial.print(range_cm);
  Serial.print(", \"unit\":\"cm\"}");
  Serial.println();

  delay(100);
}

// source: http://www.hobbytronics.co.uk/arduino-tutorial7-piezo-beep
void beep(unsigned char delayms) {
  analogWrite(beepPin, 20);      // Almost any value can be used except 0 and 255
  // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(beepPin, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available() && !stringComplete) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


