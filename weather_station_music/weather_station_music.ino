/*
 Example: Control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Released into the public domain.
 */

#include <Wtv020sd16p.h>

int resetPin = 5;  // The pin number of the reset pin.
int clockPin = 6;  // The pin number of the clock pin.
int dataPin = 7;  // The pin number of the data pin.
int busyPin = 8;  // The pin number of the busy pin.
int busyStatus  = 0;
int flagg =0;
int vol;
int volmax=0, volmin=900;
/*
Create an instance of the Wtv020sd16p class.
 1st parameter: Reset pin number.
 2nd parameter: Clock pin number.
 3rd parameter: Data pin number.
 4th parameter: Busy pin number.
 */
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

void setup() {
  //Initializes the module.
  wtv020sd16p.reset();
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //init pins
  pinMode(busyPin, INPUT);

}

void loop() {
  busyStatus = digitalRead(busyPin);
  Serial.print(F("busy val 1 ")); Serial.println(busyStatus);
  vol = analogRead(A1);
  Serial.print("vol: "); Serial.println(600-vol);
  if(vol>volmax) volmax=vol;
  if(vol<volmin) volmin= vol;
  
  delay(300);
  //Plays synchronously an audio file. Busy pin is used for this method.
  if(busyStatus==0)
  {
    flagg=0;
    Serial.println("no songs ");
    Serial.print("max: ");Serial.print(volmax);Serial.print("min: ");Serial.print(volmin);Serial.print("diff: ");Serial.println(volmax-volmin);
    volmax=0; volmin=900;
  }
  if (flagg ==0){
  Playsong();
  }

}

void Playsong(){
   wtv020sd16p.asyncPlayVoice(31);
  Serial.println("playing");
  Serial.print(F("busy val 2 ")); Serial.println(busyStatus);
  //Plays asynchronously an audio file.
  
  delay(2000);
  flagg =1;
  Serial.println(" flag: "+flagg);
  return;
}
