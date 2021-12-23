/*
 Example: Control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Released into the public domain.
 */


int checkPin = 4;  
int lightPin = 5;  
int busyStatus  = 0;
int flagg =0;

void setup ()
  {
  Serial.begin (9600);
  pinMode (checkPin, INPUT);
    pinMode (lightPin, OUTPUT);
  }  // end of setup

void loop ()
  {
    flagg = digitalRead(checkPin);
    if(flagg)
    Serial.println("yepe");
    else
    Serial.println("nope");

     delay (1000); 
     flagg = digitalRead(checkPin);
    if(flagg)
    Serial.println("yepe1");
    else
    Serial.println("nope1");
    
     digitalWrite(lightPin,HIGH);
     flagg = digitalRead(checkPin);
         if(flagg)
    Serial.println("yepe2");
    else
    Serial.println("nope2");
    delay (1000); 
     flagg = digitalRead(checkPin);
    if(flagg)
    Serial.println("yepe3");
    else
    Serial.println("nope3");
     delay (4000); 
     digitalWrite(lightPin,LOW);

     
  // other code here ...
   
  }  // end of loop
 
