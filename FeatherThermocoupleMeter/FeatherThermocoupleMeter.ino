int loopDelaymS = 1;
const int analogInPin = A1;  
const int greenLED = 10;  
float calFactor = 1.0;

float sensorValue = 0;
float avgValue = 0.0;
float nsamples = 500.0;
float offset = 0.0;
float bandwidth = 2.0;
float outputValue = 0.0;
int icount = 0;
int icountMax = 1000;
int icountLED = 0;
int icountLEDMax = 500;

boolean writePi = true ;
const int buffLength = 32;  
char inputBuff[buffLength];
int buffPointer = 0;
 

void setup() 
{
  if (writePi) {Serial1.begin(9600);} else {Serial.begin(9600);}
  nsamples = ((float) icountMax) / bandwidth;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(greenLED                                                                                                                                                                                                                                                                                                                                                                                                                                                                  , OUTPUT);
  delay(200);
  buffPointer = 0;
  for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
}

void loop()
{
  String serialRead = readSerial();
//  if (serialRead != "") Serial.println(serialRead);
  if (serialRead.lastIndexOf("offset")  > -1) offset = newFloatSetting("offset", serialRead);
  if (serialRead.lastIndexOf("calFactor")  > -1)    calFactor    = newFloatSetting("calFactor", serialRead);
  if (serialRead.lastIndexOf("bandwidth")  > -1)   
  {
    bandwidth    = newFloatSetting("bandwidth", serialRead);
    nsamples = ((float) icountMax) / bandwidth;
  }
  if (serialRead.lastIndexOf("temp")  > -1)   
  {
    if (!writePi) Serial.println(outputValue);
    if (writePi) Serial1.println(outputValue);
    digitalWrite(greenLED, HIGH);
    icountLED = 0;
  }
  if (icountLED < icountLEDMax) ++icountLED;
  if (icountLED == icountLEDMax) digitalWrite(greenLED, LOW);
 
  sensorValue = (float) analogRead(analogInPin);
  avgValue = avgValue + (sensorValue - avgValue) / nsamples;
  outputValue = calFactor * (avgValue - offset);

  ++icount;
  if (icount == (icountMax / 2)) 
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (icount == icountMax)
  {
    digitalWrite(LED_BUILTIN, LOW);
    icount = 0;
  }
  
  delay(loopDelaymS);
}
int newIntSetting(String setParse, String input, int low, int high)
{
   String newSettingString = input.substring(input.lastIndexOf(setParse) + setParse.length() + 1,input.length());
   int newSetting = constrain(newSettingString.toInt(),low,high);
   return newSetting;
}
float newFloatSetting(String setParse, String input)
{
   String newSettingString = input.substring(input.lastIndexOf(setParse) + setParse.length() + 1,input.length());
   char floatbuf[32];
   newSettingString.toCharArray(floatbuf, sizeof(floatbuf));
   return atof(floatbuf);
}
String readSerial()
{
  if(writePi)
  {
    String inputString = "";
    while(Serial1.available() > 0)
    {
      char lastRecvd = Serial1.read();
      if (lastRecvd == '!')
      {
        inputString = String((char*) inputBuff);
        buffPointer = 0;
        for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
        return inputString;
      } 
      else 
      {
        inputBuff[buffPointer] = lastRecvd;
        ++buffPointer;
      }
    }
  }
  else
  {
    String inputString = "";
    while(Serial.available() > 0)
    {
      char lastRecvd = Serial.read();
      if (lastRecvd == '!')
      {
        inputString = String((char*) inputBuff);
        buffPointer = 0;
        for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
        return inputString;
      } 
      else 
      {
        inputBuff[buffPointer] = lastRecvd;
        ++buffPointer;
      }
    }
  }
  return "";
}

