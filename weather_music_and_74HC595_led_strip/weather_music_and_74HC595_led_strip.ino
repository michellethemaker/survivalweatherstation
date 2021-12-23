/*
 Example: Control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Released into the public domain.
 */

#include <Wtv020sd16p.h>

//music chip
int resetPin = 5;  // The pin number of the reset pin.
int clockPin = 6;  // The pin number of the clock pin.
int dataPin = 7;  // The pin number of the data pin.
int busyPin = 8;  // The pin number of the busy pin.
int busyStatus  = 0;


//sETUP of Arduino pins used as shift registor
int ledlatchPin = 3;  // Latch pin(pin 12) of 74HC595 is connected to Digital pin 2
int ledclockPin = 4; // Clock pin(pin 11) of 74HC595 is connected to Digital pin 3
int leddataPin = 2;  // Data pin(pin 14) of 74HC595 is connected to Digital pin 1
byte leds = 0;    // Variable to hold the pattern of which LEDs are currently turned on or off
int bitlevel;
uint8_t valueled;

//other variables
int flagg =0;
int vol;
int volmax=0, volmin=900;

Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

void setup() {
  //Initializes the music module.
  wtv020sd16p.reset();
  
  //init music's busy pin
  pinMode(busyPin, INPUT);
  
  //initialise LED chip
  pinMode(ledlatchPin, OUTPUT);
  pinMode(leddataPin, OUTPUT);  
  pinMode(ledclockPin, OUTPUT);

  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
}

void loop() {
  updateShiftRegister(0b00000000);// Initially turns all the LEDs off, by giving the variable 'leds' the value 0 
  busyStatus = digitalRead(busyPin);
  Serial.print(F("busy val 1 ")); Serial.println(busyStatus);
  vol = analogRead(A1);
  Serial.print("vol: "); Serial.println(600-vol);
  if(vol>volmax) volmax=vol;
  if(vol<volmin) volmin= vol;
  whatlevel(vol, volmin, volmax); 
  updateShiftRegister(valueled);
  delay(300);
  
  //Plays synchronously an audio file. Busy pin is used for this method.
  if(busyStatus==0)
  {
    flagg=0;
    Serial.println("no songs ");
    Serial.print("max: ");Serial.print(volmax);Serial.print("min: ");Serial.print(volmin);Serial.print("diff: ");Serial.println(volmax-volmin);
    volmax=0; volmin=900;
  }
  
  if (flagg ==0){ Playsong(); }

}

void Playsong(){
   wtv020sd16p.asyncPlayVoice(2);
  Serial.println("playing");
  Serial.print(F("busy val 2 ")); Serial.println(busyStatus);
  //Plays asynchronously an audio file.
  
  delay(2000);
  flagg =1;
  Serial.println(" flag: "+flagg);
  return;
}

void whatlevel(int value,int x, int y)
{
  Serial.print("max: ");Serial.print(y);Serial.print("min: ");Serial.println(x);
    if (value <= x) //write 0000
  {
      valueled = 0b00000000 ;
    }
    
    if (value > x && value<=x+1) //write 0001
  {
      valueled = 0b00000001 ;
    }
    
    if (value > x+1 && value<=x+2) //write 0010
  {
      valueled = 0b00000011 ;
    }
    
    if (value > x+2 && value<=x+3) //write 0011
  {
      valueled = 0b00000111 ;
    }
    
    if (value > x+3 && value<=x+4) //write 0100
  {
      valueled = 0b00001111 ;
    }
    
    if (value > x+4 && value<=x+5) //write 0101
  {
      valueled = 0b00011111 ;
    }
    
    if (value > x+5 && value<=x+6) //write 0110
  {
      valueled = 0b00111111 ;
    }
    
    if (value > x+6) //write 0110
  {
      valueled = 0b01111111;
    }

  Serial.print("led level: "); Serial.println(valueled);
  
}

 void updateShiftRegister(const uint8_t value)
{
   digitalWrite(ledlatchPin, LOW);
   shiftOut(leddataPin, ledclockPin, LSBFIRST, value);
   digitalWrite(ledlatchPin, HIGH);
}
