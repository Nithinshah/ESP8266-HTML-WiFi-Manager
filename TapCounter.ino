byte tapCount = 0;
long int tapTime = millis();

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
Serial.begin(9600);
}

void loop()
{
int sensorValue = analogRead(A0);
Serial.println(sensorValue);

if(sensorValue > 20) //detect a tap //change the value according to the speaker you are using
{
   digitalWrite(LED_BUILTIN, HIGH);
delay(70);
digitalWrite(LED_BUILTIN, LOW);
tapCount = tapCount + 1; //increment tap count
tapTime = millis(); //record current system time
}

if(tapTime < (millis() - 400)) //if more than 0.4 seconds have elapsed since last tap
{
for(int x=0; x < tapCount; x++)
{ 
pinMode(A0,OUTPUT);
tone(A0, 1500);
delay(200);
noTone(A0);
delay(200);
pinMode(A0,INPUT);
}

tapCount = 0;

}
delay(5); // delay in between reads for stability
}
