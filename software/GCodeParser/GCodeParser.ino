#include <Servo.h>
// define the parameters of our machine.
float X_STEPS_PER_INCH = 48;
float X_STEPS_PER_MM = 40;
int X_MOTOR_STEPS   = 100;

float Y_STEPS_PER_INCH = 48;
float Y_STEPS_PER_MM  = 40;
int Y_MOTOR_STEPS   = 100;

float Z_STEPS_PER_INCH = 48;
float Z_STEPS_PER_MM   = 40;
int Z_MOTOR_STEPS    = 100;

//our maximum feedrates
long FAST_XY_FEEDRATE = 800000;
long FAST_Z_FEEDRATE = FAST_XY_FEEDRATE;

/* speed, time from 0 to 1000
800000,   4.1
50000,    5.27
20000,    7.08
15000,    8.01
10000,    10.1
5000,     16.15
2500,     28.24
2150,     32.14
2000,     34.28
*/

// Units in curve section
#define CURVE_SECTION_INCHES 0.019685
#define CURVE_SECTION_MM 0.5

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
int SENSORS_INVERTING = 1;

// How many temperature samples to take.  each sample takes about 100 usecs.


/****************************************************************************************
* digital i/o pin assignment
*
* this uses the undocumented feature of Arduino - pins 14-19 correspond to analog 0-5
****************************************************************************************/

/*
int X_STEP_PIN = 8;
int X_DIR_PIN = 11;
int X_ENABLE_PIN = 4;
int X_MIN_PIN = A4;
int X_MAX_PIN = A5;

int Y_STEP_PIN = 12;
int Y_DIR_PIN = 13;
int Y_ENABLE_PIN = 4;
int Y_MIN_PIN = A1;
int Y_MAX_PIN = A0;

int Z_STEP_PIN = A3;
int Z_DIR_PIN = 9;
int Z_ENABLE_PIN = 4;
int Z_MIN_PIN = A1;
int Z_MAX_PIN = A0;
int Z_ENABLE_SERVO = 1;
*/

// NEW PIN ASSIGNMENT:
int X_STEP_PIN = 10;
int X_DIR_PIN = 11;
int X_ENABLE_PIN = -1;
int X_MIN_PIN = 17;
int X_MAX_PIN = 16;

int Y_STEP_PIN = 9;
int Y_DIR_PIN = 3;
int Y_ENABLE_PIN = -1;
int Y_MIN_PIN = 12;
int Y_MAX_PIN = 13;

int Z_STEP_PIN = 15;
int Z_DIR_PIN = -1;
int Z_ENABLE_PIN = -1;
int Z_MIN_PIN = -1;
int Z_MAX_PIN = -1;

int Z_ENABLE_SERVO = 1;
#define COMMAND_SIZE 128

char commands[COMMAND_SIZE];
byte serial_count;
int no_data = 0;

Servo servo;

int currentPosServo = 90;
int targetPosServo = 90;
bool comment = false;
void setup()
{
    //Do startup stuff here
    Serial.begin(115200);
    while(!Serial); // wait
    Serial.println("\nSerial OK.");

    if(Z_ENABLE_SERVO==1){
        servo.attach(Z_STEP_PIN);
    }
    //other initialization.
    init_process_string();
    init_steppers();
    process_string("G90",3);//Absolute Position
    Serial.println("start");
}

void loop()
{

    char c;
    //read in characters if we got them.
    if (Serial.available() > 0)
    {
        c = Serial.read();
        no_data = 0;
        //newlines are ends of commands.
        if (c != '\n')
        {
            if(c==0x18){
                Serial.println("Grbl 1.0");
            }else{
                if (c == '('){
                    comment = true;
                }
                // If we're not in comment mode, add it to our array.
                if (!comment)
                {
                    commands[serial_count] = c;
                    serial_count++;
                }
                if (c == ')'){
                    comment = false; // End of comment - start listening again
                }
            }

        }
    }else
    {
        no_data++;
        delayMicroseconds(100);

        //if theres a pause or we got a real command, do it
        if (serial_count && (c == '\n' || no_data > 100))
        {
            //process our command!
            process_string(commands, serial_count);
            //clear command.
            init_process_string();
        }

        //no data?  turn off steppers
        if (no_data > 1000){
            disable_steppers();
        }
    }
    //        return;
    //                delay(5);
    //                int dPos = abs(currentPosServo-targetPosServo);
    //                if(currentPosServo<targetPosServo){
    //                   currentPosServo += dPos>8?6:1;
    //                }else if(currentPosServo>targetPosServo){
    //                   currentPosServo -= dPos>8?6:1;
    //                }


}
