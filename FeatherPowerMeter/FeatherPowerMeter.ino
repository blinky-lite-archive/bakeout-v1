int loopDelaymS = 1;
const int analogInPin = A1;  
float bitsPerAmp = 8.532;
float amps = 0;
float watts = 0;
float powerCal = 1.0;

float sensorValue = 0;
float avgValue = 533.0;
float nsamples = 500.0;
float varianceValue = 0.0;
float varianceAvgValue = 0.0;
float rmsAvgValue = 0;
float powerOffset = 4.0;
float bandwidth = 2.0;
int icount = 0;
int icountMax = 1000;
boolean writePi = true;
const int buffLength = 32;  
char inputBuff[buffLength];
int buffPointer = 0;
 

void setup() 
{
  Serial.begin(9600);
  if (writePi) {Serial1.begin(9600);} else {Serial.begin(9600);}
  nsamples = ((float) icountMax) / bandwidth;
  pinMode(LED_BUILTIN, OUTPUT);
  delay(200);
  buffPointer = 0;
  for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
}

void loop()
{
  String serialRead = readSerial();
//  if (serialRead != "") Serial.println(serialRead);
  if (serialRead.lastIndexOf("powerOffset")  > -1) powerOffset = newFloatSetting("powerOffset", serialRead);
  if (serialRead.lastIndexOf("powerCal")  > -1)    powerCal    = newFloatSetting("powerCal", serialRead);
  if (serialRead.lastIndexOf("bandwidth")  > -1)   
  {
    bandwidth    = newFloatSetting("bandwidth", serialRead);
    nsamples = ((float) icountMax) / bandwidth;
  }

  sensorValue = (float) analogRead(analogInPin);
  avgValue = avgValue + (sensorValue - avgValue) / nsamples;
  varianceValue = (sensorValue - avgValue) * (sensorValue - avgValue);
  varianceAvgValue = varianceAvgValue + (varianceValue - varianceAvgValue) / nsamples;
  rmsAvgValue = sqrt(varianceAvgValue);
  amps = powerCal * rmsAvgValue / bitsPerAmp;
  watts = (amps * 233.0) - powerOffset;

  ++icount;
  if (icount == (icountMax / 2)) digitalWrite(LED_BUILTIN, HIGH);
  if (icount == icountMax)
  {
    if (!writePi) Serial.println(watts);
    if (writePi) Serial1.println(watts);
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

