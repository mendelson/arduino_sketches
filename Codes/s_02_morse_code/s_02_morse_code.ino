/*
*
* Dot duration = 200 ms
* Dash duration = 3*(Dot duration) = 600 ms
* Time between a dash or a dot = Dot duration = 200 ms
* Time between two letters = Dash duration = 600 ms
* Time between two words = 7*(Dot duration) = 1400 ms
*
*/
int dotDuration = 200;
int dashDuration = 3*dotDuration;
int flashInterval = dotDuration;
int letterInterval = dashDuration;
int wordInterval = 7*dotDuration;
int control = 0;
int ledPin = 12;

char* alphabet [] = {
  /* A = */ ".-",
  /* B = */ "-...",
  /* C = */ "-.-.",
  /* D = */ "-..",
  /* E = */ ".",
  /* F = */ "..-.",
  /* G = */ "--.",
  /* H = */ "....",
  /* I = */ "..",
  /* J = */ ".---",
  /* K = */ "-.-",
  /* L = */ ".-..",
  /* M = */ "--",
  /* N = */ "-.",
  /* O = */ "---",
  /* P = */ ".--.",
  /* Q = */ "--.-",
  /* R = */ ".-.",
  /* S = */ "...",
  /* T = */ "-",
  /* U = */ "..-",
  /* V = */ "...-",
  /* W = */ ".--",
  /* X = */ "-..-",
  /* Y = */ "-.--",
  /* Z = */ "--.."
};

char* punctuation [] = {
  /* . = */ ".-.-.-",
  /* , = */ "--..--",
  /* ? = */ "..--..",
  /* ' = */ ".----.",
  /* ! = */ "-.-.--",
  /* / = */ "-..-.",
  /* ( = */ "-.--.",
  /* ) = */ "-.--.-",
  /* & = */ ".-...",
  /* : = */ "---...",
  /* ; = */ "-.-.-.",
  /* = = */ "-...-",
  /* + = */ ".-.-.",
  /* - = */ "-....-",
  /* _ = */ "..--.-",
  /* " = /* ".-..-.",
  /* $ = */ "...-..-",
  /* @ = */ ".--.-.",
};

char* number [] = {
  /* 0 = */ "-----",
  /* 1 = */ ".----",
  /* 2 = */ "..---",
  /* 3 = */ "...--",
  /* 4 = */ "....-",
  /* 5 = */ ".....",
  /* 6 = */ "-....",
  /* 7 = */ "--...",
 /* 8 = */ "---..",
 /* 9 = */ "----."
};

void setup ()
{
  pinMode (ledPin, OUTPUT);
  
  Serial.begin (9600);
}

void loop ()
{
  int c;
  int control = 0;
  
  if ( Serial.available() )
  {
    Serial.println ("Message Received: ");
  }
  
  while ( Serial.available() )
  {
    control = 1;
    
    c = Serial.read ();
    
    Serial.print ( char (c) );
    
    if ( c >= 'A' && c <= 'Z' )
    {
      letterFlash ( alphabet [c - 'A'] );
    }
    else if ( c >= 'a' && c <= 'z' )
    {
      letterFlash ( alphabet [c - 'a'] );
    }
    else if ( c >= '0' && c <= '9' )
    {
      letterFlash ( number [c - '0'] );
    }
    else if ( c == ' ' )
    {
     delay ( wordInterval );      // gap between words  
    }
    //Punctuation
    else
    {
      punct (c);
    }
  }
  if (control == 1)
  {
    Serial.println ("\nEnd of Message...\n");
    control = 0;
  }
}

void punct (char c)
{
  switch (c)
  {
    case '.':
      letterFlash ( punctuation[0] );
      break;
    case ',':
      letterFlash ( punctuation[1] );
      break;
    case '?':
      letterFlash ( punctuation[2] );
      break;
    case 39:
      letterFlash ( punctuation[3] );
      break;
    case '!':
      letterFlash ( punctuation[4] );
      break;
    case '/':
      letterFlash ( punctuation[5] );
      break;
    case '(':
      letterFlash ( punctuation[6] );
      break;
    case ')':
      letterFlash ( punctuation[7] );
      break;
    case '&':
      letterFlash ( punctuation[8] );
      break;
    case ':':
      letterFlash ( punctuation[9] );
      break;
    case ';':
      letterFlash ( punctuation[10] );
      break;
    case '=':
      letterFlash ( punctuation[11] );
      break;
    case '+':
      letterFlash ( punctuation[12] );
      break;
    case '-':
      letterFlash ( punctuation[13] );
      break;
    case '_':
      letterFlash ( punctuation[14] );
      break;
    case '"':
      letterFlash ( punctuation[15] );
      break;
    case '$':
      letterFlash ( punctuation[16] );
      break;
    case '@':
      letterFlash ( punctuation[17] );
      break;

    //If the following condition hap
    default:
      digitalWrite (ledPin, HIGH);
      delay (10000); // 10 seconds on
      digitalWrite (ledPin, LOW);
      delay ( flashInterval );

      break;
  }
}

// Flashes the morse code for a letter
void letterFlash (char* letter)
{
  int i = 0;
  
  while ( letter [i] != NULL )
  {
    singleFlash ( letter [i] );
    i++;
  }
  
  delay (letterInterval);
}

// Flashes a dot or a dash
void singleFlash (char s)
{
  if ( s == '.')
  {
    digitalWrite (ledPin, HIGH);
    delay (dotDuration);
  }
  else if ( s == '-')
  {
    digitalWrite (ledPin, HIGH);
    delay (dashDuration);
  }
  //If the following condition happens, check your alphabet vector, because there is something wrong!
  else
  {
    digitalWrite (ledPin, HIGH);
    delay (10000); // 10 seconds on
  }
  
  digitalWrite (ledPin, LOW);
  delay ( flashInterval );
}
