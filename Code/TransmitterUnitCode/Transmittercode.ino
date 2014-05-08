//Transmitter Code
//David Burson FYP

int analogPin = A0;
int x = 0;
int ledPin = 13;                 // LED connected to digital pin 13

void setup()
{
  pinMode(analogPin, INPUT);
  pinMode(ledPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(1200);  // Hardware supports up to 2400, but 1200 gives longer range
}

void loop()
{
  x = analogRead(analogPin);    // read the input pin
  float voltage = x * (5.0 / 1023.0);
  
  if (voltage>3) {
    Serial.println(voltage);             // debug value
    Serial.println("\r\n");
    
    writeUInt(288); // Put any number you want to send here (71 is just a test)
    
   digitalWrite(ledPin, HIGH);   // sets the LED on
    delay(1000);                  // waits for a second
    digitalWrite(ledPin, LOW);    // sets the LED off
    delay(10000); 
  }
}


