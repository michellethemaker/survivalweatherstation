int ThermistorPin = A2;
int Vo;
float R1 = 2180; //resistor value. adjust if temp is off. smaller for higher temps
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int pb_1 = 10;
int pb_2 = 11;
int flag_weather = 0; //if 0= temp, if 1= humid

//sETUP of Arduino pins used as shift registor
int latchPin = 3;  // Latch pin(pin 12) of 74HC595 is connected to Digital pin 2
int clockPin = 4; // Clock pin(pin 11) of 74HC595 is connected to Digital pin 3
int dataPin = 2;  // Data pin(pin 14) of 74HC595 is connected to Digital pin 1 
int bitlevel;
uint8_t valueled;// Variable to hold the pattern of which LEDs are currently turned on or off

void setup() {
Serial.begin(9600);


  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(pb_1, INPUT);
  pinMode(pb_2, INPUT);
}

void loop() {
   
  updateShiftRegister(0b00000000);// Initially turns all the LEDs off, by giving the variable 'leds' the value 0 
  if(digitalRead(pb_1)==HIGH)
  {
    flag_weather =0;
    Serial.println("PB 1 pressed");
  }
  if(digitalRead(pb_2)==HIGH)
  {
    flag_weather =1;
    Serial.println("PB 2 pressed");
  }
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

   
  whatlevel(); 

  updateShiftRegister(valueled);
  delay(900);

}

void whatlevel()
{
    if (Tc <= 25) //write 0001
  {
      valueled = 0b00000001 ;
    }
    
    if (Tc > 25 && Tc<=26) //write 0010
  {
      valueled = 0b00000011 ;
    }
    
    if (Tc > 26 && Tc<=27) //write 0011
  {
      valueled = 0b00000111 ;
    }
    
    if (Tc > 27 && Tc<=28) //write 0100
  {
      valueled = 0b00001111 ;
    }
    
    if (Tc > 28 && Tc<=29) //write 0101
  {
      valueled = 0b00011111 ;
    }
    
    if (Tc > 29 && Tc<=30) //write 0110
  {
      valueled = 0b00111111 ;
    }
    
    if (Tc > 30) //write 0110
  {
      valueled = 0b01111111;
    }

  Serial.print("led level: "); Serial.println(valueled);
  
}

 void updateShiftRegister(const uint8_t value)
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, value);
   digitalWrite(latchPin, HIGH);
}
