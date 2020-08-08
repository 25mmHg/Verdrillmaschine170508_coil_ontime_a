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

// ensure this library description is only included once
#ifndef Half_Stepper_h
#define Half_Stepper_h

// library interface description
class Stepper {
  public:
    // constructors:
    Stepper(long int current_position, long int current_speed, int steps_for_revolution, unsigned int max_ramp, unsigned int max_coil_ontime, bool fullstep, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);

    // target_speed setter method:
    void setTargetSpeed(float whatSpeed);
    // ramp setter method:
    void setRamp(unsigned int whatRamp);
    // coil-ontime setter method:
    void setCoilOntime(unsigned int whatCoilOntime);
    // fullstep setter method:
    void setFullStep();
    // halfstep setter method:
    void setHalfStep(); 


    // version getter method:
    int getVersion(void);
    // position getter method:
    long int getPosition(void);
    // speed getter method:
    long int getspeed(void);


    // mover method (distance):
    void moveSteps(int steps_to_move);
    // mover method (destination):
    void moveTarget(int target_position);

    
  private:
    void stepMotor(int this_step);

    int direction;            // Direction of rotation
    unsigned long step_delay; // delay between steps, in us, based on speed
    unsigned int coil_ontime; // max ontime for coil in us, based on powerstage and stepper coil
    unsigned int ramp;        // max_ramp TODO
    long int current_position; // current position in steps
    long int current_speed;    // current speed in steps per second
    int steps_for_revolution; // total number of steps this motor can take
    int steps_on_current_revolution; // which step the motor is on

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;
    
    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif


