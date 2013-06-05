/*
* Author: Mateus Mendelson
* Web: https://sites.google.com/site/mateusmendelson/
*
* Project: Blinking LED example.
* Date: 06/02/2013
*
*/

int ledPin = 12;                // LED connected to digital pin 12

void setup()
{
  pinMode(ledPin, OUTPUT);      // sets the digital pin (ledPin) as output
}

void loop()
{
  digitalWrite(ledPin, HIGH);   // sets the LED on
  delay(500);                  // waits for 0.5 second
  digitalWrite(ledPin, LOW);    // sets the LED off
  delay(300);                  // waits for 0.3 second
}
