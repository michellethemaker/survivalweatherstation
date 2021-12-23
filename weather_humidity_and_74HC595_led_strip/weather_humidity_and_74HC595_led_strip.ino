int HumidityPin = A3;
int Vh;
int Humid;


//sETUP of Arduino pins used as shift registor
int latchPin = 3;  // Latch pin(pin 12) of 74HC595 is connected to Digital pin 2
int clockPin = 4; // Clock pin(pin 11) of 74HC595 is connected to Digital pin 3
int dataPin = 2;  // Data pin(pin 14) of 74HC595 is connected to Digital pin 1
byte leds = 0;    // Variable to hold the pattern of which LEDs are currently turned on or off
int bitlevel;
uint8_t valueled;

void setup() {
Serial.begin(9600);


  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}

void loop() {
   
  updateShiftRegister(0b00000000);// Initially turns all the LEDs off, by giving the variable 'leds' the value 0 
  
  Vh = analogRead(HumidityPin);
  Serial.print("Humid: ");
  Serial.println(Vh);
  Humid=Vh/3.3;
  Serial.print("Rel Humidity: ");
  Serial.print(Humid);
  Serial.println("%");

   
  whatlevel(); 

  updateShiftRegister(valueled);
  delay(900);
  
//  for (int i = 0; i < bitlevel; i++) // Turn all the LEDs ON one by one. i only have 7 sections of LEDs so actually i can use 7 instead of 8
//    {
//      Serial.print("led level: "); Serial.println(i);
//      bitSet(leds, i);    // Set the bit that controls that LED in the variable 'leds'
//      updateShiftRegister(valueled);
//      delay(300);
//    }
}

void whatlevel()
{
    if (Humid <= 30) //write 0001
  {
      valueled = 0b00000001 ;
    }
    
    if (Humid > 30 && Humid<=40) //write 0010
  {
      valueled = 0b00000011 ;
    }
    
    if (Humid > 40 && Humid<=50) //write 0011
  {
      valueled = 0b00000111 ;
    }
    
    if (Humid > 50 && Humid<=60) //write 0100
  {
      valueled = 0b00001111 ;
    }
    
    if (Humid > 60 && Humid<=70) //write 0101
  {
      valueled = 0b00011111 ;
    }
    
    if (Humid > 70 && Humid<=80) //write 0110
  {
      valueled = 0b00111111 ;
    }
    
    if (Humid > 80) //write 0110
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
