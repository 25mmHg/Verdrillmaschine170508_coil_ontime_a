// Chapter 5 - Motor Control
// Stepper Motor Bipolar
// Compatible with the Arduino Motor Shiled:
// http://arduino.cc/en/Main/ArduinoMotorShieldR3
// By Cornel Amariei for Packt Publishing
// New by 25mmHg

#include "Half_Stepper.h"

// Declare the used pins 4 Stepper motor
const unsigned char DIR_A = 12;
const unsigned char DIR_B = 13;
const unsigned char PWM_A = 3;  // no interferences with millis and delay, zeropwr-fullpwr --> 0-255
const unsigned char PWM_B = 11; // no interferences with millis and delay, zeropwr-fullpwr --> 0-255
//const unsigned char BRK_A = 8;
//const unsigned char BRK_B = 9;
const unsigned int STPS4TURN = 400;
const unsigned int MAX_COIL_ONTIME = 20000;
const unsigned int MAX_RAMP = 1;

// Declare a Stepper motor
Stepper mystepper(STPS4TURN, MAX_COIL_ONTIME, MAX_RAMP, DIR_A, DIR_B, PWM_A, PWM_B);
const unsigned int ABS_MAX_RPM    = 3000;
const unsigned int ABS_MIN_RPM    = 1;
const unsigned int ABS_MAX_TURNS  = 1000;
const unsigned int ABS_MIN_ON     = 0;
const unsigned int ABS_MAX_ON     = 50000;

// Declare header 2 show
const char SET_TURN_RIGHT_HEADER   = 'r';
const char SET_TURN_LEFT_HEADER    = 'l';
const char SET_RPM_MIN_HEADER      = 'a';
const char SET_RPM_MAX_HEADER      = 'e';
const char SET_ONTIME_MAX_HEADER   = 'o';
const char SET_START_ENGINE_HEADER = ' ';
const char SET_HELP_HEADER         = '?';

// Declare a Start BUTTON or Footswitch
const int BUTTON = A5;

// Declare the initial numbers of Lturns, Rturns, glblPwr and start/end-actglblRpm reading later from EEprom
unsigned int glblmaxRturns = 100;
unsigned int glblmaxLturns = 15;
unsigned int glblStrtRpm = 200;
unsigned int glblEndRpm = 1000;
unsigned int glblCoilOntime = 2000;

boolean wait4startEngine = true;
unsigned int counter = 0;
unsigned int actglblRpm;

void showInstructions() {
  Serial.println(F("#####################################"));
  Serial.println(F("#     PiESCHENER WERKSTAETTEN       #"));
  Serial.println(F("#         VERDRiLLMASCHINE          #"));
  Serial.println(F("#             LEGENDE               #"));
  Serial.println(F("#                                   #"));
  Serial.println(F("#  ZAHL + r setzt Rechtsdrehen      #"));
  Serial.println(F("#  ZAHL + l setzt Linksdrehen       #"));
  Serial.println(F("#  ZAHL + a setzt Anlaufdrehzahl    #"));
  Serial.println(F("#  ZAHL + e setzt Enddrehzahl       #"));
  Serial.println(F("#  ZAHL + o setzt Pulsdauer Max(us) #"));
  Serial.println(F("#  SPACE    startet JOB             #"));
  Serial.println(F("#  ?        zeigt diesen Hilfetext  #"));
  Serial.println(F("#####################################"));
  Serial.println();
}

void showValues()
{
  Serial.println(F("#####################################"));
  Serial.print(F("   Rechtsdrehen = "));
  Serial.println(glblmaxRturns);
  Serial.print(F("    Linksdrehen = "));
  Serial.println(glblmaxLturns);
  Serial.print(F(" Anlaufdrehzahl = "));
  Serial.println(glblStrtRpm);
  Serial.print(F("    Enddrehzahl = "));
  Serial.println(glblEndRpm);
  Serial.print(F("  Pulsdauer Max = "));
  Serial.println(glblCoilOntime);
  Serial.println(F("#####################################"));
}

void line() {
  Serial.println(F("-------------------------------------"));
}

void fehler() {
  Serial.println(F("Fehler: Wert abgewiesen"));
  line();
}


void getValues() {
  if (digitalRead(BUTTON) == LOW) {
    wait4startEngine = false;
    Serial.println(F("BUTTON PRESSED"));
  }
  static unsigned int val = 0;
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch >= '0' && ch <= '9') {
      val = val * 10 + ch - '0';
    }
    else if (ch == SET_TURN_RIGHT_HEADER) {
      if (val <= ABS_MAX_TURNS) {
        glblmaxRturns = val;
        Serial.print(F("...setze Rechtsdrehen auf "));
        Serial.println(val);
        line();
      }
      else
        fehler();
      showValues();
      val = 0;
    }
    else if (ch == SET_TURN_LEFT_HEADER) {
      if (val <= ABS_MAX_TURNS) {
        glblmaxLturns = val;
        Serial.print(F("...setze Linksdrehen auf "));
        Serial.println(val);
        line();
      }
      else
        fehler();
      showValues();
      val = 0;
    }
    else if (ch == SET_RPM_MIN_HEADER) {
      if (val >= ABS_MIN_RPM && val <= ABS_MAX_RPM) {
        glblStrtRpm = val;
        Serial.print(F("...setze Anlaufdrehzahl auf "));
        Serial.println(val);
        line();
      }
      else
        fehler();
      showValues();
      val = 0;
    }
    else if (ch == SET_RPM_MAX_HEADER) {
      if (val >= ABS_MIN_RPM && val <= ABS_MAX_RPM) {
        glblEndRpm = val;
        Serial.print(F("...setze Enddrehzahl auf "));
        Serial.println(val);
        line();
      }
      else
        fehler();
      showValues();
      val = 0;  
    }
    else if (ch == SET_ONTIME_MAX_HEADER) {
      if (val >= ABS_MIN_ON && val <= ABS_MAX_ON) {
        glblCoilOntime = val;
        Serial.print(F("...setze Pulsdauer Maximum auf "));
        Serial.println(val);
        line();
      }
      else
        fehler();
      showValues();
      val = 0;  
    }
    else if (ch == SET_HELP_HEADER)
    {
      Serial.println(F("...Hilfe angefordert "));
      showInstructions();
      showValues();
      val = 0;
    }
    else if (ch == SET_START_ENGINE_HEADER) {
      wait4startEngine = false;
      Serial.println(F("SPACEBAR PRESSED"));
      val = 0;
    }
    else Serial.println(F("Fehler: Wert nicht interpretierbar "));
  }
  else;
}

void setPower(unsigned char pwr4that) {
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  if (pwr4that == 0) {
    digitalWrite(PWM_A, LOW);
    digitalWrite(PWM_B, LOW);
  }
  else if (pwr4that >= 255) {
    digitalWrite(PWM_A, HIGH);
    digitalWrite(PWM_B, HIGH);
  }
  else {
    analogWrite(PWM_A, pwr4that);
    analogWrite(PWM_B, pwr4that);
  }
}

// need 10 turns for 1k rpm
void accelerateRturn(int endRpm) {
  while (actglblRpm < endRpm) {
    mystepper.setSpeed(actglblRpm++);
    mystepper.step((actglblRpm>>8)+1);
  }
  while (actglblRpm > endRpm) {
    mystepper.setSpeed(actglblRpm--);
    mystepper.step((actglblRpm>>8)+1);
  }
}

// need 10 turns for 1k rpm
void accelerateLturn(int endlRpm) {
  while (actglblRpm < endlRpm) {
    mystepper.setSpeed(actglblRpm++);
    mystepper.step((actglblRpm>>8)+1);
  }
  while (actglblRpm > endlRpm) {
    mystepper.setSpeed(actglblRpm--);
    mystepper.step((actglblRpm>>8)+1);
  }
}

void turnsR(unsigned int n) {
  mystepper.setSpeed(actglblRpm);
  while (n > 0 && digitalRead(BUTTON)) {
    mystepper.step( STPS4TURN);
    n--;
  }
}

void turnsL(unsigned int n) {
  mystepper.setSpeed(actglblRpm);
  while (n > 0 && digitalRead(BUTTON)) {
    mystepper.step(-STPS4TURN);
    n--;
  }
}

/*void stopAndBrake(){
  delay(10);// H-bridges are still on Power = strong STOP
  digitalWrite(BRK_A, HIGH); // H-bridge short coil
  digitalWrite(BRK_B, HIGH); // dito
  }

  void stopAndRelease(){
  digitalWrite(BRK_A, LOW); // H-bridge release coil
  digitalWrite(BRK_B, LOW); // dito
  }*/


void doWfbJob() {
  actglblRpm = glblStrtRpm;
  setPower(255);
  accelerateRturn(glblEndRpm);
  turnsR(glblmaxRturns - 20);
  accelerateRturn(glblStrtRpm);
  //stop
  delay(10);
  //release
  setPower(128);
  delay(3000);
  mystepper.setSpeed(glblStrtRpm);
  setPower(255);
  turnsL(glblmaxLturns);
  //stop
  delay(10);
  //release
  setPower(0);
}

void setup() {
  //pinMode(BRK_A, OUTPUT);
  //pinMode(BRK_B, OUTPUT);
  //digitalWrite(BRK_A, HIGH); // brake at startup
  //digitalWrite(BRK_B, HIGH); // dito
  setPower(0);

  // Pullup for BUTTON
  digitalWrite(BUTTON, HIGH);

  // set timer 2 divisor to 8 for PWM frequency of 3921.16 Hz
  TCCR2B = TCCR2B & B11111000 | B00000010;

  // run serial Interface
  Serial.begin(9600);
  showInstructions();
}

void loop() {
  while (wait4startEngine){
    getValues();
  }
  showValues();
  Serial.println(F("does job"));
  doWfbJob();
  counter++;
  Serial.print(F("finished job number "));
  Serial.println(counter);
  wait4startEngine = true;
}


