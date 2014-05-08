#include <EEPROM.h>

#include <SoftwareSerial.h>
SoftwareSerial gsmSerial(2,3);
 
char gsm_char,gsm_char_send, data[256], ph_number[13];//="+353879149715";
int x;

#define LED_PIN 13
#define vibratePin 5

void setup()
{
  pinMode(LED_PIN, OUTPUT); 
  pinMode(vibratePin, OUTPUT);
  Serial.begin(1200);  // Hardware supports up to 2400, but 1200 gives longer range
  
  gsmSerial.begin(1200);
  
  if(EEPROM.read(0)!=255){
 for(int y=0; y<13; y++){
   ph_number[y] = EEPROM.read(y);
 }
 Serial.println("\rThe emergency contact number is: \r");
 Serial.println(ph_number);
 }
 
   gsmSerial.print("AT+CMGF=1 \r"); // Setting GSM Module to message mode
   delay(600);

  Serial.begin(1200);  // Hardware supports up to 2400, but 1200 gives longer range
  gsmSerial.begin(1200);
  
   //--- turn on TC35 ---
  // wire pin 8 Arduino to IGT pin on TC35
  // it grounds IGN pin for 100 ms
  // this is the same as pressing the button
  // on the TC35 to start it up

  pinMode(8, INPUT);
  digitalWrite(8, LOW);
  pinMode(8, OUTPUT);
  delay(100);
  pinMode(8, INPUT);
  
  delay(1000);  
  
  // Checking for any new messages containing a set command to reset emergency number
  for(int i=0;i<3;i++){
  checkMessage();
  resetSMS();
 delay(500); 
  }
  
}

void loop()
{
  boolean light_led = false;

  if (readUInt(true) == 288) // Check to see if we got the 88 test number
  {
    light_led = true;
      Serial.println('Received');             // debug value

//  }
  
//  if (light_led)
//  {
      digitalWrite(LED_PIN, HIGH);
    
     gsmSerial.write("ATD+353879149715;\r"); //code to call phone
     digitalWrite(vibratePin, HIGH);
     delay(2000);
     digitalWrite(vibratePin, LOW);
     delay(8500); //11 second delay as takes about 5 seconds to connect call
     gsmSerial.print("ATH0;\r"); //kill call
     
     digitalWrite(LED_PIN, LOW);

  }
}

void delete_All_SMS(){
  for(int i = 1; i <= 10; i++) {
    gsmSerial.print("AT+CMGD=");
    gsmSerial.println(i);
    Serial.print("deleting SMS ");
    Serial.println(i);
    delay(500);
  }

}
  
void sendSMS(){
 
  gsmSerial.print("AT+CMGF=1\r");
  delay(1000);
 gsmSerial.println("AT+CMGS=\"+353879149715\""); //AT command to send SMS
 delay(500); //the length of this delay is very important. 400 is too low
 gsmSerial.print("Hello World"); //Print the message
 delay(100);
 gsmSerial.println((char)26);
  Serial.println("\r\nSMS Sent\r\n");

}

void checkMessage() {
  
  gsmSerial.println("AT+CMGR=1 \r");    //Reads the first SMS
         
        for (x=0;x < 255;x++){            
            data[x]='\0';                        
        } 
        x=0;
        do{
            while(gsmSerial.available()==0);
            data[x]=gsmSerial.read();  
            x++;           
            //if(data[x-1]==0x0D&&data[x-2]=='"'){
                //x=0;
           // }
        }while(!(data[x-1]=='K'&&data[x-2]=='O'));

        data[x-3]='\0';        //finish the string before the OK
        
        delay(1000); //delay so as CMGR command returns a value before printing data

        Serial.println("\r\nMessage contains: \r");
        Serial.println(data);    //shows the message  
  
}

void resetSMS() {
  boolean contains = false;
  for (x = 0; x<strlen(data); x++) {
    if (data[x]=='S'&&data[x+1]=='e'&&data[x+2]=='t'){
      contains = true;
      break;
        }
        else {
            contains = false;
        }   
    }
  if (contains == true){
        Serial.println("This SMS contains the word; 'Set'. \r");
        for (x = 0; x<strlen(data); x++) {
        if (data[x]=='+'&&data[x+1]=='3'&&data[x+2]=='5'){  //finds characters beginning with +35 and then saves the number
          for(int i=0;i<13;i++){
            ph_number[i]=data[x];
            EEPROM.write(i, data[x]);
            
            x++;
          }
          Serial.println("This message was sent from: ");
          Serial.println(ph_number); Serial.println("\rThis is the new emergency call number.\r");
          delete_All_SMS();
          }
        }
  }
  else{
    Serial.println("There is no set message at this time. \r");
  } 
       
}
