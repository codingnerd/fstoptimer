#include <LiquidCrystal.h>
#include <Timer.h>
#include "Fraction.h"

#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {
  14, 15, 16, 17, 18, 19}; // the analog 0-5 pins are also known as 14-19
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed' 
volatile byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

double long base = 0.0;
const long double deltat = 1;
long double fincrement = 0.0;
const long double deltafincrementbase = 1./3.;

const byte striplength = 6;
byte stripstep=0;

struct frac inc = {0,3};
struct frac incdelta = {1,12};

long double deltafstep;
long double basetime;
long double totaltime;

long double total=0.0;

static byte lampon = 0;

static byte mode = 0;

int ledPin = 10;

void setup() {
  byte i;

  // pin13 LED
  pinMode(ledPin, OUTPUT);

  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  // Run timer2 interrupt every 15 ms 
  TCCR2A = 0;
  TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= 1<<TOIE2;

  lcd.begin(16, 2);
  displaystuff (mode, total, base, fincrement,inc,stripstep);
  analogWrite(6,150);
}


long double calc_time (byte stripstep, long double base, struct frac inc) {

  long double ans;

  if (0==stripstep) {
    ans=(long double) base ;
  } 
  else {
    for (byte i = 0; i < stripstep; i++) {
      ans=(long double) base * ( pow(2.0,double( (i+1) * inc.num)/double(inc.den)) - pow(2.0,double(i*inc.num)/double(inc.den)));
    }
  }
  return ans; 
}

void displaystuff (byte mode, long double total, long double base, long double fincrement, struct frac inc, byte stripstep) {
  struct redfrac dispinc = reducefrac(inc);
  if (2==mode) {
    // calculation of the exposure duration for the given stripstep value:
    totaltime = calc_time(stripstep, base, inc);
  } 
  else {
    totaltime = (long double) base * pow(2.0,double(inc.num)/double(inc.den));
  }

  lcd.setCursor(0, 0);
  lcd.print(mode);
  lcd.print(" ");
  lcd.print((double)totaltime);
  lcd.print(" ");

  String test;

  redfrac2string(dispinc,&test);

  lcd.print(test);
  lcd.print(" ");

  if (2==mode) {
    lcd.print("s:");
    lcd.print(String(stripstep));
    lcd.print("        ");
  } 
  else {
    lcd.print("s:-");
    lcd.print("        ");
  }
  //-----------
  lcd.setCursor(0, 1); 
  lcd.print("b:");
  lcd.print((double)base);
  lcd.print("        ");
}

SIGNAL(TIMER2_OVF_vect) {
  check_switches();
}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();

  for (index = 0; index < NUMBUTTONS; index++) {

    currentstate[index] = digitalRead(buttons[index]);   // read the button

    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}

void loop() {
  
  if (lampon) digitalWrite(ledPin,HIGH);
  
  for (byte i = 0; i < NUMBUTTONS; i++) {
    if (justreleased[i]) {
      justreleased[i] = 0;

      if (5==i) {
        //turn on the lamp for the given time
        long double realmilliseconds = totaltime * 1000.0;
        long int milliseconds = (long int) floor(realmilliseconds+0.5);
        String myString = String(milliseconds);
        lcd.setCursor(0, 1);
        lcd.print("         ");
        lcd.setCursor(0, 1);
        lcd.print(myString);
        lcd.print(" ms");
        digitalWrite(ledPin, HIGH);
        delay(milliseconds);
        digitalWrite(ledPin, LOW);
        lcd.print(" done");
      }

      if (4==i) {
        if (0 == lampon) {
          lampon = 1;
          digitalWrite(ledPin,HIGH);
        }
          else {
            lampon = 0;
            digitalWrite(ledPin,LOW);  
          }
      }     
      if (0==i) {
        mode++;
        if (mode > 2) {
          mode = 0;
        }
        displaystuff (mode, total, base, fincrement, inc,stripstep);    
      }   
    }
    if ((pressed[i])) {
      if (2 == mode) {
        // test strip mode  
          if (2==i) {
          if (stripstep<striplength) 
            stripstep++;
          displaystuff (mode, total, base, fincrement, inc, stripstep);
          delay(300);
        }
        if (1==i) {
          if (stripstep > 0)
            stripstep--;
          displaystuff (mode, total, base, fincrement, inc,stripstep);
          delay(300);
        }  
        // end of test strip mode
      }      
      if (0 == mode) {
        // base time setting    
        if (2==i) {
          base += deltat;
          displaystuff (mode, total, base, fincrement, inc,stripstep);
          delay(300);
        }
        if (1==i) {
          base -= deltat;
          if (base <= 0.0)
            base = 0.0;
          displaystuff (mode, total, base, fincrement,inc,stripstep);
          delay(300);
        }
        // end of base time setting
      }

      if (1 == mode) {
        // f-increment setting    
        if (2==i) {
          fincrement += deltafincrementbase;
          inc = addfrac(inc, incdelta);
          displaystuff (mode, total, base, fincrement, inc,stripstep);
          delay(300);
        }
        if (1==i) {
          fincrement -= deltafincrementbase;
          inc = subtractfrac(inc, incdelta);
          if (base <= 0.0)
            base = 0.0;
          displaystuff (mode, total, base, fincrement, inc,stripstep);
          delay(300);
        }
        // end of f-increment setting
      }
    }
  }
}


