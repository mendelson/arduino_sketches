/*
* Author: Mateus Mendelson
* Web: https://sites.google.com/site/mateusmendelson/
*
* Project: Push button to change traffic and pedestrian signal.
* Date: 06/05/2013
*
* To have a better understanding on how "Debouncing" works, see: http://sheepdogguides.com/arduino/aht0bounce1.htm
*
*/

// Traffic signal
int redT = 7;
int yellowT = 8;
int greenT = 12;

//Pedestrian signal
int redP = 2;
int greenP = 4;

int button = 9;

void setup ()
{
  pinMode (redT, OUTPUT);
  pinMode (yellowT, OUTPUT);
  pinMode (greenT, OUTPUT);

  pinMode (redP, OUTPUT);
  pinMode (greenP, OUTPUT);

  pinMode (button, INPUT);
  digitalWrite(button, HIGH); //Yes... WRITE to the input pin...#
                              //This causes the Arduino to "attach" an internal pull up
                              //resistor to the input pin.
  // Setting initial state
  setTrafficState (LOW, LOW, HIGH);
  setPedestrianState (HIGH, LOW);
}

void loop ()
{
  delay (10); // This delay is used to implement "debouncing". When you press the switch button, its value varies some times before stabilizing.
  
  if (!digitalRead (button))
  {
    // Waits for 2 secs after pressing the button
    delay (2000);
    
    setTrafficState (LOW, HIGH, LOW);
    // Stays yellow for 2 secs
    delay (2000);
    
    setTrafficState (HIGH, LOW, LOW);
    // Waits for 0.5 sec before allowing pedestrians to walk
    delay (500);
    
    setPedestrianState (LOW, HIGH);
    // Pedestrians have green signal for 4 secs
    delay (4000);
    
    // Gives a warning that the pedestrian signal is about to close, and, then, closes
    closePedestrianWalk ();
    
    // Reseting traffic signal state
    setTrafficState (LOW, LOW, HIGH);
    
    // This delay is used not to let pedestrians press the button anytime they want. After closing their signal, they must wait for 3 more seconds before pressing the button again
    delay (3000);
  }
}

// Sets the traffic signal state
void setTrafficState (int r, int y, int g)
{
  digitalWrite (redT, r);
  digitalWrite (yellowT, y);
  digitalWrite (greenT, g);
}

// Sets the pedestrian signal state
void setPedestrianState (int r, int g)
{
  digitalWrite (redP, r);
  digitalWrite (greenP, g);
}

// Gives the pedestrians a signal that it is going to close (red light blinks) and, then, it closes
void closePedestrianWalk ()
{
  setPedestrianState (LOW, LOW);
    
  for (int i = 0; i < 3; i++)
  {
    digitalWrite (redP, HIGH);
    delay (300);
    
    digitalWrite (redP, LOW);
    delay (300);
   }
    
   setPedestrianState (HIGH, LOW);
   // Gives 0.3 secs for the pedestrians to stop walking
   delay (600);
}
