/*
 * Stepper.cpp - Stepper library for Wiring/Arduino - Version 1.0.9
 *
 * Original library        (0.1)   by Tom Igoe.
 * Two-wire modifications  (0.2)   by Sebastian Gassner
 * Combination version     (0.3)   by Tom Igoe and David Mellis
 * Bug fix for four-wire   (0.4)   by Tom Igoe, bug fix from Noah Shibley
 * High-speed stepping mod         by Eugene Kozlenko
 * Timer rollover fix              by Eugene Kozlenko
 * Five phase five wire    (1.1.0) by Ryan Orendorff
 *
 * only Halfstepper 8Phase (1.0.9) by 25mmHg
 * [this version is better for high speed stepper]
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * The sequence of controls 4 signals from Arduino Motorshield: 
 * 
 *
 *   Step  dirA dirB pwmA pwmB
 *
 *    1     1    x    1    0
 *    2     1    1    1    1
 *    3     x    1    0    1
 *    4     0    1    1    1
 *    5     0    x    1    0
 *    6     0    0    1    1
 *    7     x    0    0    1
 *    8     1    0    1    1
 *    
 */

#include "Arduino.h"
#include "Half_Stepper.h"

/*
 *   constructor with pwm as motor_pin3 and ...4
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int steps_for_revolution, unsigned int max_ramp, unsigned int max_coil_ontime, int motor_pin_1, int motor_pin_2, // dirA, dirB
int motor_pin_3, int motor_pin_4) // pwmA, pwmB
{
  this->steps_on_current_revolution = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->steps_for_revolution = steps_for_revolution; // total number of steps 4 revolution for this motor
  this->ramp = max_ramp;    // max_ramp TODO
  this->coil_ontime = max_coil_ontime; // max ontime for coil, based on powerstage and stepper coil

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);
  pinMode(this->motor_pin_4, OUTPUT);
}

/*
 * Sets the speed in revs per minute
 */
void Stepper::setSpeed(float whatSpeed)
{
  this->step_delay = (unsigned long)  60UL * 1000000UL / (this->steps_for_revolution * whatSpeed);
}

/*
 * Sets the ramp
 */
void Stepper::setRamp(unsigned int what_ramp)
{
  ; //TODO
}

/*
 * Sets the coilOntime in us
 */
void Stepper::setCoilOntime(unsigned int whatCoilOntime)
{
  ; //TODO
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void Stepper::step(int steps_to_move)
{
  int steps_left = abs(steps_to_move);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
    unsigned long now = micros();
    // move only if the appropriate delay has passed:
    if (now - this->last_step_time >= this->step_delay)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1)
      {
        this->steps_on_current_revolution++;
        if (this->steps_on_current_revolution == this->steps_for_revolution) {
          this->steps_on_current_revolution = 0;
        }
      }
      else
      {
        if (this->steps_on_current_revolution == 0) {
          this->steps_on_current_revolution = this->steps_for_revolution;
        }
        this->steps_on_current_revolution--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      stepMotor(this->steps_on_current_revolution % 8);
    }
    // beware overheat of powerstage and coils
    else if (now - this->last_step_time >= this->coil_ontime)
    {
      analogWrite(motor_pin_3, 128);
      analogWrite(motor_pin_4, 128);
    }
    else;
  }
}

/*
 * Moves the motor forward or backwards in halfstep mode.
 * like hh0lll0hhh0lll0h //A
 *      0hhh0lll0hhh0lll //B
 *      |/-\|/-\|/-\|/-\ //revolutions
 */
void Stepper::stepMotor(int thisStep)
{
  switch (thisStep) {
    case 0:  // 1110
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 1:  // 1111
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 2:  //0101
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 3:  //0111
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 4:  //0010
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 5:  //0011
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 6:  //1001
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 7:  //1011
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, HIGH);
    break;
  }
}

/*
 * version() returns the version of the library:
 */
int Stepper::version(void)
{
  return 6;
}

