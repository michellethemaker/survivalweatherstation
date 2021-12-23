//==============================================================================================================
//using DVDdoug's 'World's Simplest Effect', see https://forum.arduino.cc/t/dance-party-light-adressable-led-strip-controlled-by-software-soundwave-analyser/547091/4
//use a bias circuit!
//============================================================================================================== 
// Global Variables
int Bias = 512;                           //Nominally half of the 1023 range = 512.  Adjust as necessary
int NoiseThreshold = 6;                   //Increase to minimize false triggers with silence or background noise
int Analog;                               // The analog reading (with the bias subtracted-out)
int Max;                                  // The waveform peak over a short period of time
int Average;                              // A "sampled" moving average of Max values used as the LED-on threshold (not the true-average).

unsigned long ReadStartTime;             //millis() used to set time
int SampleTime = 50;                     //Sample for short time to find waveform peak 
unsigned long ArrayStartTime;            //millis() again 
int ArrayTime = 1000;                    //Update array once per second 

const int ArraySize = 20;                // 20 values in the moving-average array
int ArrayIndex = 0;                      // Index/pointer
int AverageArray[ArraySize];             // Saves a sample of 20 Max values for averaging

//Function prototype
int UpdateAverage(int Max);               //UpdateAverage() function prototype


//==============================================================================================================  
void setup()
{   
  Serial.begin(9600);

  for(int i = 0; i < 20; i++)
    AverageArray[i] = 0;              //initialize the array with zeros 
}
 

// Start main loop ============================================================================================
void loop()
{
  Max = 0;                                  //Initilize/reset before running while() loop
  ReadStartTime = millis();                 //Save/update loop-starting time

  // Find waveform peak loop ==================================================================================
  // Takes readings in a "fast loop" to find the waveform peak.
  // At 20Hz a half-cycle is 25mS, so 25ms is the minimum time to find the peak (at deep bass frequencies).
  // A longer sample of about 100mS may work better with incancescant lamps (allowing the light to reach full-brightness).  
  
  while (millis() - ReadStartTime < SampleTime)
  {
    Analog = abs(analogRead(A1) - Bias);                 // Read, take out the 2.5V bias/offset, make positive. 

    if (Analog > Max)                                    //Find Max before exiting loop 
      Max = Analog; 
  }// End waveform peak loop ==================================================================================

  //TURN THE LED ON/OFF =================================================================================== 
  if (Average < NoiseThreshold)             // Make sure LED is off with silence or background noise
    Average = NoiseThreshold;         

  if(Max > Average)                         //If the latest Max reading is > average, turn-on the LED
    Serial.println("ON!");
  else  
    Serial.println("OFF!");

  if (millis() - ArrayStartTime > ArrayTime)  // Time to update array?
    Average = UpdateAverage(Max);            // Call the UpdateAverage() function once per second
   
}  // End of main loop ==========================================================================================
 


// UpdateAverage() function =================================================================================
// Takes-in latest Max reading, updates the array, returns the average
int UpdateAverage(int Max) 
{ 
  int Sum = 0;    // Inialize/reset Sum before calculating average
  AverageArray[ArrayIndex] = Max;                //Update one array element with the latest Max 

  for(int i = 0; i < 20; i++)                    //Sum-up the data in the array
  {
    Sum = Sum + AverageArray[i];  
  }

  Average = Sum/20;                              // Find average of the saved Max values

  ArrayIndex++;                                  //Next index for next time
  if (ArrayIndex > 19)
    ArrayIndex = 0;                              //Back to the beginning of the circuar buffer

  //Print information (Optional, once per second for testing/debugging)
  Serial.print (" Average = ");
  Serial.print (Average);
  Serial.print (" Max = ");
  Serial.println (Max);
  
  ArrayStartTime = millis();                     //New Array Start Time  
  
  return Average;        
} // End UpdateAverage() ====================================================================================
