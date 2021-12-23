/* portable weather station with temperature/humidity functions,
    audio via a WTV020-SD-16P module,
    volume sensitive light array via 74HC595-shift register,
    and an emergency flashlight with adjustable brightness.
  Created by michelle lim m m, June 2021.
  Please credit when used.
*/

#include <Wtv020sd16p.h>
//initialise weather & flash in SW1
int switch_weather = 9;
int switch_flash = 12;

//initialise thermistor
int ThermistorPin = A2;
int Vo;
float R1 = 2180; //resistor value. adjust if temp is off. smaller for higher temps
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int pb_1 = 10;
int pb_2 = 11;
int flag_weather = 0; //if 0= temp, if 1= humid

//initialise humidity
int HumidityPin = A3;
int Vh;
int Humid;

//sETUP of Arduino pins used as shift registor
int ledlatchPin = 3;  // Latch pin(pin 12) of 74HC595 is connected to Digital pin 2
int ledclockPin = 4; // Clock pin(pin 11) of 74HC595 is connected to Digital pin 3
int leddataPin = 2;  // Data pin(pin 14) of 74HC595 is connected to Digital pin 1
byte leds = 0;    // Variable to hold the pattern of which LEDs are currently turned on or off
int bitlevel;
uint8_t valueled;

//music chip
int resetPin = 5;  // The pin number of the reset pin.
int clockPin = 6;  // The pin number of the clock pin.
int dataPin = 7;  // The pin number of the data pin.
int busyPin = 8;  // The pin number of the busy pin.
int busyStatus  = 0;
  
//other variables
int powerPin = 1;
int flagg =0;
int vol;
int volmax=0, volmin=900;
int pbchanged =1;
int swstatus =0;
int playonce = 0;
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

void setup() {
  //Initializes the music and power led module.
  wtv020sd16p.reset();
  pinMode(powerPin, OUTPUT);

  
  //Initializes the music module.
  pinMode(switch_weather, INPUT);
  pinMode(switch_flash, INPUT_PULLUP);
  //init music's busy pin
  pinMode(busyPin, INPUT);
  
  //initialise LED chip
  pinMode(ledlatchPin, OUTPUT);
  pinMode(leddataPin, OUTPUT);  
  pinMode(ledclockPin, OUTPUT);

  //setup push buttons
  pinMode(pb_1, INPUT);
  pinMode(pb_2, INPUT);
  
  
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));

}

void loop() {
  //updateShiftRegister(0b00000000);// Initially turns all the LEDs off, by giving the variable 'leds' the value 0 
  
  if(playonce == 0) {
    
    Serial.println("BOOM");   
    delay(500);
    for (int i = 0; i < 7; i++) // Turn all the LEDs ON one by one. i only have 7 sections of LEDs so actually i can use 7 instead of 8
  {
    //Serial.print("led level: "); Serial.println(i);  //for debugginh
    bitSet(leds, i);    // Set the bit that controls that LED in the variable 'leds'
    digitalWrite(ledlatchPin, LOW);
   shiftOut(leddataPin, ledclockPin, LSBFIRST, leds);
   digitalWrite(ledlatchPin, HIGH);
    delay(300);
  }
  wtv020sd16p.playVoice(1);
  playonce=1;
  delay(3300);
  }
    
  //add flag for last state so we continue state if needed.=
  int ht_status = digitalRead(switch_weather);
  Serial.print("ht_status captured: ");  Serial.println(ht_status);

switch (ht_status) {
    
    case 1: //SW1 weather mode
    if(swstatus==0)wtv020sd16p.stopVoice();
      Serial.println("weather time");
      swstatus=1;
        if(digitalRead(pb_1)==HIGH)
        {
          pbchanged=1;
          
          flag_weather =0;
          Serial.println("PB 1 pressed");
          Serial.print("pbchanged: ");Serial.println(pbchanged);
        }
        if(digitalRead(pb_2)==HIGH)
        {
          pbchanged=1;
          
          flag_weather =1;
          Serial.println("PB 2 pressed");
          Serial.print("pbchanged: ");Serial.println(pbchanged);
        }
      weather(); //weather time
      break;
      
    case 0: //SW1 flash/music mode
      swstatus =0;
      int flash_status = digitalRead(switch_flash);
      if (flash_status ==1){ //it's SW1 music mode!
    
      Serial.println("music time");
  //music currently playing---------------
  busyStatus = digitalRead(busyPin);
  Serial.print(F("busy val 1 ")); Serial.println(busyStatus);
  vol = analogRead(A1);
  Serial.print("vol: "); Serial.println(vol);Serial.println("");
  if(vol>volmax) volmax=vol;
  if(vol<volmin) volmin= vol;
  whatmusiclevel(vol, volmin, volmax); 
  updateShiftRegister(valueled);
  if(digitalRead(pb_1)==HIGH) {
    wtv020sd16p.stopVoice();
    delay(3000);
    Playsong();
  }
  if(digitalRead(pb_2)==HIGH){
    volmax=0;
    volmin=900;
  }
  delay(100);
  
  //Plays synchronously an audio file. Busy pin is used for this method.
  if(busyStatus==0)
  {
    flagg=0;
    Serial.println("no songs ");
    Serial.print("max: ");Serial.print(volmax);Serial.print("min: ");Serial.print(volmin);Serial.print("diff: ");Serial.println(volmax-volmin);
    volmax=0; volmin=900;//reset vol limits
    
  }
  
  if (flagg ==0){ Playsong();} //run new song only if flagg=0
  //--------------------------------------
      break;
    }

  else{
    Serial.println("flash time");
    //flashlight. do nothin
      }
      break;
      
    default:
    Serial.println("default SW1 mode activated");
      break;
  }
}


void Playsong() {
  
  
  int adder = (analogRead(A1))%10;
  int song = random(0,41)+adder;
  if(song>=42){
    int subtracter = random(10,15);
    song=song-subtracter;
  }
//  int portioned2 = portioned%2; //get even/odd. this section for manipulating advert frequency if needed 
//  
//  if((portioned% 2 ) ==0)
//  song = random(2,15);
//  if(portioned% 2 !=1)
//  song = random(16,31); 
//  else song = random(16,31);

  Serial.print("song number: ");  Serial.println(song);
  wtv020sd16p.asyncPlayVoice(song);
  //Serial.print(F("busy val 2 ")); Serial.println(busyStatus);
  //Plays asynchronously an audio file.
  flagg =1;
  //Serial.print("flag: ");Serial.println(flagg); //for debugginh
  return;
}

void weather() {
  
  Serial.print("pbchangeddd?: ");Serial.println(pbchanged);
  if (pbchanged==1) {

    //wtv020sd16p.asyncPlayVoice(0);
    Serial.println("play weather channel");
    pbchanged =0;
  }
  
  switch(flag_weather){ //if 0= temp, if 1= humid
  
  case 0: //temperature
    Vo = analogRead(ThermistorPin);
    Serial.print("temp: ");
    Serial.println(Vo);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    Tc = T - 273.15;
    Tf = (Tc * 9.0)/ 5.0 + 32.0; 

    Serial.print("Temperature: "); 
    Serial.print(Tf);
    Serial.print(" F; ");
    Serial.print(Tc);
    Serial.println(" C"); 
    led_ind(Tc, 23,36);
    updateShiftRegister(valueled);
    delay(800);
  return;
  
  case 1: //humidity
  Vh = analogRead(HumidityPin);
  Serial.print("Humid: ");
  Serial.println(Vh);
  Humid=(Vh+23)*2;
  Serial.print("Rel Humidity: ");
  Serial.print(Humid);
  Serial.println("%");
  led_ind(Humid, 40,100);
  updateShiftRegister(valueled);
  delay(800);
  return;

  default:
  return;
  }
}

void led_ind(int value, int x, int y){
  int unit = (y - x)/7;
    
    if (value <= (unit+x)) //write 0001
  {
      valueled = 0b00000000 ;
    }
    
    if (value > (unit+x) && value<=(2*unit+x)) //write 0010
  {
      valueled = 0b00000001 ;
    }
    
    if (value > (2*unit+x) && value<=(3*unit+x)) //write 0011
  {
      valueled = 0b00000011 ;
    }
    
    if (value > (3*unit+x) && value<=(4*unit+x)) //write 0100
  {
      valueled = 0b00000111 ;
    }
    
    if (value > (4*unit+x) && value<=(5*unit+x)) //write 0101
  {
      valueled = 0b00001111 ;
    }
    
    if (value > (5*unit+x) && value<=(6*unit+x)) //write 0110
  {
      valueled = 0b00011111 ;
    }
    
    if (value > (6*unit+x) && value<=(7*unit+x)) //write 0110
  {
      valueled = 0b00111111 ;
    }  
      
    if (value > (7*unit+x)) //write 0110
  {
      valueled = 0b01111111 ;
    }
    Serial.print("led level: "); Serial.println(valueled);
}



void whatmusiclevel(int value,int x, int y)
{
  Serial.print("max: ");Serial.print(y);Serial.print(" min: ");Serial.println(x);
    if (value <= x) //write 0000
  {
      valueled = 0b00000000 ;
    }
    
    if (value > x && value<=x+1) //write 0001
  {
      valueled = 0b00000001 ;
    }
    
    if (value > x+1 && value<=x+3) //write 0010
  {
      valueled = 0b00000011 ;
    }
    
    if (value > x+3 && value<=x+5) //write 0011
  {
      valueled = 0b00000111 ;
    }
    
    if (value > x+5 && value<=x+6) //write 0100
  {
      valueled = 0b00001111 ;
    }
    
    if (value > x+6 && value<=x+8) //write 0101
  {
      valueled = 0b00011111 ;
    }
    
    if (value > x+8 && value<=x+10) //write 0110
  {
      valueled = 0b00111111 ;
    }
    
    if (value > x+10) //write 0110
  {
      valueled = 0b01111111;
    }

  Serial.print("led level for vol: "); Serial.println(valueled);
  
}


 void updateShiftRegister(const uint8_t value)
{
   digitalWrite(ledlatchPin, LOW);
   shiftOut(leddataPin, ledclockPin, LSBFIRST, value);
   digitalWrite(ledlatchPin, HIGH);
}
