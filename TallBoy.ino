/*
 * Hardware setup:
 * 
 * 
 */

// establish pin constants
long ConsoleBps = 19200;
const int sensorTrigPin = 14;             // sensorTrigPin tells distance sensor to start/stop a reading
const int sensorEchoPin = 15;             // sensorEchoPin is where distance sensor provides feedback
const int cylinderTransistorGatePin = 38; // cylinderTransistorGatePin will flip the transistor to power the pneumatic solenoid valve's 12v power
const int minTriggerDistance = 10;        // minimum distance, inches,  object must be away in order to trigger
const int maxTriggerDistance = 72;        // maximum distance, inches, object must be away in order to trigger
const int cylinderUpMillis = 500;         // cylinderUpMillis is number of millis to hold creature up
const int resetMillis = 500;              // resetMillis is number of millis to delay before ready to run again
const int requiredHitCount = 2;           // number of matching hits from distance sensor to trigger rise
const int soundTriggerPin = 18;           // soundTriggerPin is the pin # linked to the sound card, bringing it LOW turns on sound

// establish variables for ultrasonic distance sensor
long duration, inches, countOfHits, counter;

boolean serialOn = true; //sometimes writing to Serial hangs board, so this flag turns off writing to Serial in log()

void setup() {
  // initialize serial communication bits per second:
  Serial.begin(ConsoleBps); 

  log("Setting output on cylinderTransistorGatePin ()");
  pinMode(cylinderTransistorGatePin, OUTPUT);

  // ensure lid and floor lifters are down/lowered - power off transistor (and thus solenoid)
  digitalWrite(cylinderTransistorGatePin, LOW);      
  log("cylinderTransistorGatePin set to LOW voltage - ensuring lid is down/lowered");

 //setup sensor pins
  pinMode(sensorTrigPin, OUTPUT);
  log("Sensor Trig pin set to output");
  pinMode(sensorEchoPin, INPUT);
  log("Sensor Echo pin set to input");

  // setup sound pin and ensure sound is off to start
  pinMode(soundTriggerPin, OUTPUT);
  digitalWrite(soundTriggerPin, HIGH);

  
  log("Finished setup()...");
}

void loop() {
  normalLogic();
}

void normalLogic() {
  counter++;
  countOfHits = 0;
  inches = getSensorDistance();
  log("Run 1: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  delay(1);
  if(countOfHits < requiredHitCount ) 
  {
    inches = getSensorDistance();
    log("Run 2: " + String(inches) + "in");
    if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
    }
   }
  delay(1);
  if(countOfHits < requiredHitCount ) 
  {
    inches = getSensorDistance();
    log("Run 3: " + String(inches) + "in");
    if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
    }
  }
  
  // print out distance to console
  log("Count of hits: " + String(countOfHits));
  
//   delay(2000);

 if(countOfHits >= requiredHitCount )
 {
   //raise body by triggering transistor (which powers solenoid)
   log("Rumble box...");

   turnSoundOn();
   extendCylinder();
   delay(cylinderUpMillis);
   collapseCylinder();
   extendCylinder();
   delay(cylinderUpMillis);
   collapseCylinder();

   extendCylinder();
   delay(cylinderUpMillis);
   collapseCylinder();
   turnSoundOff();
   
   delay(resetMillis);
 }
}


long getSensorDistance()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:  
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(sensorEchoPin, HIGH);

  // convert the time into a distance
  return microsecondsToInches(duration);
}

long microsecondsToInches(long microseconds)
{
// According to Parallax's datasheet for the PING))), there are
// 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
// second). This gives the distance travelled by the ping, outbound
// and return, so we divide by 2 to get the distance of the obstacle.
// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
return microseconds / 74 / 2;
}

void turnSoundOn()
{
   log("turnSoundOn() running");
   //turn on sound - should play only once
   digitalWrite(soundTriggerPin, HIGH); //force HIGH just to be sure it is off before turning on
   delay(10);                           //slight delay to allow board to register the HIGH state
   digitalWrite(soundTriggerPin, LOW);  //trigger the sound
}

void turnSoundOff()
{
   log("turnSoundOff() running");
   digitalWrite(soundTriggerPin, HIGH); //turn off sound
}

void extendCylinder()
{
     log("extendCylinder() running");
   digitalWrite(cylinderTransistorGatePin, HIGH);     // powers transistor and an LED so we can see it happen  
}

void collapseCylinder()
{
     log("collapseCylinder() running");
   digitalWrite(cylinderTransistorGatePin, LOW);     // powers transistor and an LED so we can see it happen  
}

void log(String logLine)
{
  if(serialOn) Serial.println(String(millis()) + ": " + logLine);
}
