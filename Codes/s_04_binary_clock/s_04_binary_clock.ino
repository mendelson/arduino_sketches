/*
* Author: Mateus Mendelson
* Web: https://sites.google.com/site/mateusmendelson/
*
* Project: Binary Clock.
* Date: 20/08/2013
*
*/

/* Headers */
#include <TimerOne.h>

/* Matrices */
// This matrix contains the binary numbers, from 0 to 9. Note that they have been inverted (the most left bit is the LSB)
boolean number [10][4] = {
  /* 0 = */ 0, 0, 0, 0,
  /* 1 = */ 1, 0, 0, 0,
  /* 2 = */ 0, 1, 0, 0,
  /* 3 = */ 1, 1, 0, 0,
  /* 4 = */ 0, 0, 1, 0,
  /* 5 = */ 1, 0, 1, 0,
  /* 6 = */ 0, 1, 1, 0,
  /* 7 = */ 1, 1, 1, 0,
  /* 8 = */ 0, 0, 0, 1,
  /* 9 = */ 1, 0, 0, 1
};

// Clock pins
int seconds[2][4] = {
  {0, 1, 2, 3},
  {4, 5, 6}
};

int minutes[2][4] = {
  {7, 8, 9, 10},
  {11, 12, 13}
};

/* Note that analog pins are also represented by numbers. So:
* Analog0 = 14
* Analog1 = 15
* Analog2 = 16
* Analog3 = 17
* Analog4 = 18
* Analog5 = 19
*
*/
int hour[2][4] = {
  {14, 15, 16, 17},
  {18, 19}
};

/* Vectors */
// This vector controls the clock
int control[6] = {0, 0, 0, 0, 0, 0};

/* Setup */
void setup ()
{
  int i;
  
  // Setting pins
  for (i = 0; i <= 3; i++)
  {
    pinMode (seconds[0][i], OUTPUT);
  }
  
  for (i = 0; i <= 2; i++)
  {
    pinMode (seconds[1][i], OUTPUT);
  }
  
  for (i = 0; i <= 3; i++)
  {
    pinMode (minutes[0][i], OUTPUT);
  }
  
  for (i = 0; i <= 3; i++)
  {
    pinMode (minutes[1][i], OUTPUT);
  }
  
  for (i = 0; i <= 3; i++)
  {
    pinMode (hour[0][i], OUTPUT);
  }
  
  for (i = 0; i <= 1; i++)
  {
    pinMode (hour[1][i], OUTPUT);
  }
  
  // Setting initial time
  Serial.begin (9600);
  setTime ();
  // We have to stop serial communication in order to use pins 0 and 1
  // Comment the following line in order to use the function "void print_state ()" (for debug purposes)
  Serial.end ();
  
  // Setting timer
  Timer1.initialize (1000000); // set a timer of length 1000000 microseconds (or 1.0 sec - or 1Hz) => the clock will be updated every 1 second
  Timer1.attachInterrupt ( update_clock_state ); // attach the service routine here
}

/* Main loop */
void loop ()
{
  // Do nothing, wait for Timer1 to call clock's update
}

// This function sets a time, so that the clock can start counting
void setTime ()
{
  // Initial time
  int h[2] = {0, 0};
  int m[2] = {0, 0};
  int s[2] = {0, 0};
  unsigned long waiting = 3000000;
  boolean input = false;

  Serial.println ("\n----- Enter current time -----");
  
  // This loop checks if the input is a valid hour
  do
  {
    Serial.print ("=> Hours (24h pattern, 2 digits): ");
    while ( !Serial.available() && waiting > 0)
    {
      waiting--;
      // waiting for input data
    }
    // Checking if there was input data
    if (waiting > 0)
    {
      input = true;
    }
    if (input)
    {
      h[1] = Serial.read () - 48; // Since "Serial.read ()" reads its input's ascii value, we have to decrease 48 in order store the correct value
    }
    
    while ( !Serial.available() && waiting > 0)
    {
      // waiting for input data
    }
    if (input)
    {
      h[0] = Serial.read () - 48;
    }
    
    Serial.print (h[1]);
    Serial.println (h[0]);
  }while ( (h[1] == 2 && h[0] >= 4) || h[1] >= 3 || h[1] < 0 );
  
  // This loop checks if the input is a valid minute
  do
  {
    Serial.print ("=> Minutes (2 digits): ");
    while ( !Serial.available() && waiting > 0)
    {
      // waiting for input data
    }
    if (input)
    {
      m[1] = Serial.read () - 48;
    }
    
    while ( !Serial.available() && waiting > 0)
    {
      // waiting for input data
    }
    if (input)
    {
      m[0] = Serial.read () - 48;
    }
    Serial.print (m[1]);
    Serial.println (m[0]);
  }while ( m[1] == 6 || m[1] < 0 );
  
  // This loop checks if the input is a valid second
  do
  {
    Serial.print ("=> Seconds (2 digits): ");
    while ( !Serial.available() && waiting > 0)
    {
      // waiting for input data
    }
    if (input)
    {
      s[1] = Serial.read () - 48;
    }
    
    while ( !Serial.available() && waiting > 0)
    {
      // waiting for input data
    }
    if (input)
    {
      s[0] = Serial.read () - 48;
    }
    Serial.print (s[1]);
    Serial.println (s[0]);
  }while ( s[1] == 6 || s[1] < 0 );
  
  
  // Setting clock state
  control[0] = s[0] - 1;
  control[1] = s[1];
  control[2] = m[0];
  control[3] = m[1];
  control[4] = h[0];
  control[5] = h[1];
}

// Updating clock stete
void update_clock_state ()
{
  for (int i = 0; i < 6; i++)
  {
    switch (i)
    {
      // First digit (seconds) and First digit (minutes)
      case 0:
        if (control[i] <= 8)
        {
          control[i]++;
        }
        else
        {
          control[i] = 0;
          control[i + 1]++;
        }
        
        break;
      case 1:
        if (control[i] > 5)
        {
          control[i] = 0;
          control[i + 1]++;
        }
        
        break;
      case 2:
        if (control[i] > 9)
        {
          control[i] = 0;
          control[i + 1]++;
        }
        
        break;
      // Second digit (seconds) and Second digit (minutes)
      case 3:
        if (control[i] > 5)
        {
          control[i] = 0;
          control[i + 1]++;
        }
        
        break;
      // First and Second digits (hours)
      case 4:
        if (control[i] > 9)
        {
          control[i] = 0;
          control[i + 1]++;
        }
        
        break;
      case 5:
        if (control[i - 1] >= 4 && control[i] == 2)
        {
          reset_control ();
        }
        
        break;
    }
  }
  
  // Updating LEDs
  update_leds ();
  
  //print_state ();
}

// Resets the clock, that is, mid-night
void reset_control ()
{
  for (int i = 0; i <= 5; i++)
  {
    control[i] = 0;
  }
}

// Sets output LEDs to the current state of the clock
void update_leds ()
{
  setSeconds ();
  setMinutes ();
  setHour ();
}

// Sets seconds' vectors
void setSeconds ()
{
  flash (number[control[0]], seconds[0], 4);
  flash (number[control[1]], seconds[1], 3);
}

// Sets minutes' vectors
void setMinutes ()
{
  flash (number[control[2]], minutes[0], 4);
  flash (number[control[3]], minutes[1], 3);
}

// Sets hours' vectors
void setHour ()
{
  flash (number[control[4]], hour[0], 4);
  flash (number[control[5]], hour[1], 2);
}

// Sets a LED vector
void flash (boolean vector[4], int* pins, int size_pins)
{
  int i;
  
  for (i = 0; i < size_pins; i++)
  {
    digitalWrite (pins[i], vector[i]);
  }
}

// This function prints the current clock's state (for debug purposes)
void print_state ()
{
  Serial.print (control[5]);
  Serial.print (control[4]);
  Serial.print ("|");
  Serial.print (control[3]);
  Serial.print (control[2]);
  Serial.print ("|");
  Serial.print (control[1]);
  Serial.println (control[0]);
}
