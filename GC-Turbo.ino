#include "Nintendo.h"

//Change pins 0 and 1 if planning on using Serial.

#define GC_CTRL 1 //Grey
#define GC_CON 2 //Brown

#define UP_BUTTON_PIN 6 //Orange
#define DOWN_BUTTON_PIN 5 //Yellow
#define CHANGE_BUTTON_PIN 0 //Purple

#define TMCLK 3 //Green
#define TMDIO 4 //Blue

#define TM_ALLOWED

#ifdef TM_ALLOWED
#include <TM1637.h>
TM1637 tm(TMCLK, TMDIO);
#endif

CGamecubeController controller(GC_CTRL);
CGamecubeConsole console(GC_CON);
Gamecube_Report_t gcc;

enum TurboButton
{
  None, A, B, X, Y, L, R, Start, DPadUp, DPadDown, DPadLeft, DPadRight
};

TurboButton TurboConfig;

int val, outputValInt, timeStart, timeEnd, timeToWait;
bool turboSend, ledToggle, GetNewTurboConfig, ButtonPressed;

void setup() 
{
  //Serial.begin(9600);
  
  timeStart = millis();
  val = 10;
  outputValInt = -1;

  TurboConfig = None;

  // Set up debug led
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CHANGE_BUTTON_PIN, INPUT_PULLUP);

#ifdef TM_ALLOWED
  tm.begin();
  tm.colonOn();
  #endif
}

void loop()
{
  if(digitalRead(CHANGE_BUTTON_PIN) == LOW)
  {
    #ifdef TM_ALLOWED
    tm.colonOff();
    tm.clearScreen();      
    tm.display(3);
    delay(1000);
    
    tm.clearScreen();
    tm.display(2);
    delay(1000);
     
    tm.clearScreen();
    tm.display(1);
    delay(1000);
    tm.colonOn();
    #else
    delay(1000);
    #endif
    
    GetNewTurboConfig = true;
  }
  
  if(digitalRead(UP_BUTTON_PIN) == LOW)
  {
    ButtonPressed = true;
    if(val < 30)
    {
      val++;
    }  
  }

  if(digitalRead(DOWN_BUTTON_PIN) == LOW)
  {
    ButtonPressed = true;
    if(val > 0)
    {
      val--;
    }  
  }

  if(ButtonPressed)
  {
    ButtonPressed = false;
    delay(100);
  }
  
  noInterrupts();
  
  controller.read();
  gcc = controller.getReport();

  if(GetNewTurboConfig)
  {
    TurboConfig = getNewTurboButton(gcc);          
  }
  timeEnd = millis();

  outputTM1637(val, GetNewTurboConfig);
  GetNewTurboConfig = false;
  
  if(val > 0) 
  {   
   timeEnd = millis(); 
   timeToWait = 100/(val*2); //60hz

   //Serial.println(timeToWait);
   //Serial.print("val: "); Serial.println((timeEnd - timeStart));
   
    if((timeEnd - timeStart) > timeToWait)
    {
      timeStart = millis();
      ledToggle = !ledToggle;//true;
    }
    else
    {
      //ledToggle = false;
    }
    //digitalWrite(LED_BUILTIN, ledToggle);
  }

  if(gcc.z)
  {
    if(TurboConfig == A)
    {
       gcc.a = ledToggle;
    }
    else if (TurboConfig == B)
    {
      gcc.b = ledToggle;
    }
    else if (TurboConfig == X)
    {
      gcc.x = ledToggle;
    }
    else if (TurboConfig == Y)
    {
      gcc.y = ledToggle;
    }
    else if (TurboConfig == L)
    {
      gcc.l = ledToggle;
    }
    else if (TurboConfig == R)
    {
      gcc.r = ledToggle;
    }
    else if (TurboConfig == Start)
    {
      gcc.start = ledToggle;
    }
    else if (TurboConfig == DPadUp)
    {
      gcc.dup = ledToggle;
    }
    else if (TurboConfig == DPadDown)
    {
      gcc.ddown = ledToggle;
    }
    else if (TurboConfig == DPadLeft)
    {
      gcc.dleft = ledToggle;
    }
    else if (TurboConfig == DPadRight)
    {
      gcc.dright = ledToggle;
    }
  }
  
  console.write(gcc);
  
  interrupts();
}

void outputTM1637(int value, bool force)
{
  #ifdef TM_ALLOWED
  if(value != outputValInt || force)
  {
    outputValInt = value;
    
    String valString = getTurboString(TurboConfig);
    
    if(value < 10)
    {
      valString += " ";  
    }
    valString += value;  
      
    tm.clearScreen();
    tm.display(valString);
  }  
  #endif
}

TurboButton getNewTurboButton(Gamecube_Report_t gc)
{
  if(gc.a)
  {
    return A;
  }
  if(gc.b)
  {
    return B;
  }
  if(gc.x)
  {
    return X;
  }
  if(gc.y)
  {
    return Y;
  }
  if(gc.l)
  {
    return L;
  }
  if(gc.r)
  {
    return R;
  }
  if(gc.start)
  {
    return Start;
  }
  if(gc.dup)
  {
    return DPadUp;
  }
  if(gc.ddown)
  {
    return DPadDown;
  }
  if(gc.dleft)
  {
    return DPadLeft;
  }
  if(gc.dright)
  {
    return DPadRight;
  }
  return None;
}

String getTurboString(TurboButton tb)
{
  if (tb == A)
  {
    return "A ";
  }
  if (tb == B)
  {
    return "B ";
  }
  if (tb == X)
  {
    //Can't x with 7 segment display :(
    return "H ";
  }
  if (tb == Y)
  {
    return "Y ";
  }
  if (tb == L)
  {
    return "L ";
  }
  if (tb == R)
  {
    return "R ";
  }
  if (tb == Start)
  {
    return "ST";
  }
  if (tb == DPadUp)
  {
    return "DU";
  }
  if (tb == DPadDown)
  {
    return "DD";
  }
  if (tb == DPadLeft)
  {
    return "DL";
  }
  if (tb == DPadRight)
  {
    return "DR";
  }
  return "  ";
}
