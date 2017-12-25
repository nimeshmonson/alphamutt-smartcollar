#include <SPI.h>  
#include "RF24.h" 

RF24 myRadio (7, 8); 
float weight;
boolean trigger = false;
const int accelXPin = A0;
const int accelYPin = A1;
const int accelZPin = A2;
float xVal = 0;
float yVal = 0;
float zVal = 0;
float sofaWeight = 0;
float tempSofaWeight = 0;
byte addresses[][6] = {"0"}; 

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  myRadio.begin(); 
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate(RF24_250KBPS) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
}


void loop()  
{
  if ( myRadio.available()) 
  {
    
    trigger = detectSurge(accelXPin, accelYPin, accelZPin);
    
    while (myRadio.available())
    {
      myRadio.read( &weight, sizeof(weight) );
      Serial.print("Weight = "); 
      Serial.println(weight);
    }
    
    if(sofaWeight == 0)
    {
      sofaWeight = weight;
    }

    
    Serial.print("Sofa Weight = "); 
    Serial.println(sofaWeight); 
    
    if(trigger == true){
      Serial.println("Trigger Detected");
      for(int i = 0; i < 3000; i += 100){
        myRadio.read( &weight, sizeof(weight));
        if(weight > sofaWeight){
          break;
        }
        delay(100);
      }
      
      if(weight > sofaWeight){
        Serial.println("Increase in weight detected");
        while((weight >= 1.01 * sofaWeight)){
          myRadio.read( &weight, sizeof(weight));
          Serial.println("REMOVE WEIGHT TO EXIT LOOP");
          digitalWrite(9,HIGH);
          analogWrite(5,4000);
          delay(100);
        }
        Serial.println("OUT OF THE LOOP");
        digitalWrite(9,LOW);
        digitalWrite(5,LOW);
        //noTone(5);
      }
      Serial.println("OUT OF THE INNER IF");
    }
    sofaWeight = weight;
    Serial.print("\n");
  }
}
boolean detectSurge(int x, int y, int z){
  int tempX = analogRead(x);
  int tempY = analogRead(y);
  int tempZ = analogRead(z);
  if (tempX >= 1.05 * xVal || tempY >= 1.05 * yVal || tempZ >= 1.05 * zVal){
    xVal = tempX;
    yVal = tempY;
    zVal = tempZ;
    return true;
  }
    xVal = tempX;
    yVal = tempY;
    zVal = tempZ;
    return false;  
}
