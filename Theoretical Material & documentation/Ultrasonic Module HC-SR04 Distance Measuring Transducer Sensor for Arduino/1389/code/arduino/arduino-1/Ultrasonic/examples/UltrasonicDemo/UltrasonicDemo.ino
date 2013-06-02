#include "Ultrasonic.h"
Ultrasonic ultrasonic(12,13);
 int CMM;
void setup() {
    Serial.begin(9600);
}

void loop()
{
  CMM = ultrasonic.Ranging(CM); 
  Serial.print(CMM);//Serial display distance
  Serial.println("cm"); 
  delay(100);
}




