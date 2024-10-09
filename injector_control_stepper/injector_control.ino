/* Injector Control
  Reilly Downing - Owen Beer, Megan LaBelle, Naeamah Rabeea
  April 2024

  This program establishes serial control over an automated microinjector.
  
*/


#include <Stepper.h>
#include <AccelStepper.h>
 
AccelStepper injector(AccelStepper::FULL4WIRE, 8, 9, 10, 11); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5


int steps = 0;
int stepsprev = 200;
bool restart = false;
bool safety = false;

// for hamiltion 7000.5 --could maybe make a lookup for both syringes
// 8.33333333 nL per mm
float HAM_7000_5_DIV = 25 / 3;

// given lead/pitch of our rod
float lead = 0.6096;


void stop() {
  pinMode(13, INPUT);
  Serial.println("Stopped\n");
  injector.stop();
  injector.setCurrentPosition(0);
}



void start_injector(bool inject) {
  float flow, lin_speed, vol;

  if (inject) {
    Serial.println("**Injection Mode**"); 
  }
  else {
    Serial.println("**Extraction Mode**");
  }
  
  Serial.println("Input Flow Rate (nL/min):"); 
  while (true) {
    if (Serial.available()) {                            
      flow = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }
 
  
  Serial.println("\nInput Volume (nL):"); 
  while (true) {
    if (Serial.available()) {                            
      vol = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }
  
  lin_speed = flow / HAM_7000_5_DIV; 

  float rpm = lin_speed / lead; // handy for stepper motors
  float steppsec = rpm / 60.0 * 200.0;

  steps = 1 / (HAM_7000_5_DIV * lead / vol / stepsprev);

  //Debug Info
  Serial.print("flow rate: ");
  Serial.println(flow);
  Serial.print("lin_speed: ");
  Serial.println(lin_speed);
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("vol: ");
  Serial.println(vol); 
  

  
  Serial.println("\nReady. Hit Physical Button to begin."); 
  
  while (true) {
    if (digitalRead(13)==HIGH) {        // Hangs until button is pressed
      while (digitalRead(13)==HIGH){
            // Hangs until button is released
      }
      break;
    }
  }
  if (inject) {
    Serial.println("Injecting...");
    injector.move(-steps);
  }
  else {
    Serial.println("Extracting...");
    injector.move(steps);
  }
  injector.setSpeed(steppsec);
  
  while(digitalRead(12)==LOW || !inject) {   // Should stop when button is pressed -- but may not be immediate
    injector.runSpeedToPosition();

    if (injector.currentPosition()==injector.targetPosition()) {
        injector.setCurrentPosition(0);
        restart = true;
        break;
    }
  }
  
  if (!restart) {
    injector.setCurrentPosition(0);
    safety = true;
  }

  steps = 0;
}

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis

int upthresh = 640;
int downthresh = 400;

void manual() {
  Serial.println("Begin Manual Control. Press the Physical Button to exit.");
  while(digitalRead(13)==LOW) {
      // read analog X and Y analog values
    xValue = analogRead(A2);

    if (xValue > upthresh) {
      float flow, lin_speed, vol;
      flow = 300;
      vol = 500;

      lin_speed = flow / HAM_7000_5_DIV; 

      float rpm = lin_speed / lead; // handy for stepper motors
      float steppsec = rpm / 60.0 * 200.0;

      steps = 1 / (HAM_7000_5_DIV * lead / vol / stepsprev);

      injector.move(-steps);
      injector.setSpeed(steppsec);

      while(digitalRead(12)==LOW && xValue > upthresh) {
        xValue = analogRead(A2);
        injector.runSpeedToPosition();

        if (injector.currentPosition()==injector.targetPosition()) {
            injector.setCurrentPosition(0);
            restart = true;
            break;
        }
      }
    }
    if (xValue < downthresh) {
      float flow, lin_speed, vol;
      flow = 300;
      vol = 500;

      lin_speed = flow / HAM_7000_5_DIV; 

      float rpm = lin_speed / lead; // handy for stepper motors
      float steppsec = rpm / 60.0 * 200.0;

      steps = 1 / (HAM_7000_5_DIV * lead / vol / stepsprev);

      injector.move(steps);
      injector.setSpeed(steppsec);

      while(xValue < downthresh) {
        xValue = analogRead(A2);
        injector.runSpeedToPosition();

        if (injector.currentPosition()==injector.targetPosition()) {
            injector.setCurrentPosition(0);
            restart = true;
            break;
        }
      }

    }
  }
  restart = true;
}


void printInstructions() {
  Serial.println("Input the number associated with a mode to begin: ");
  Serial.println("`1` - Extraction Mode");
  Serial.println("`2` - Injection Mode");
  Serial.println("`3` - Manual Joystick Control");
  Serial.println("`0` - Stop");
  Serial.println();
  Serial.println("Press the reset button on the device to exit any mode and see this message again.");
}

void setup() {
  Serial.begin(115200);      //Set Baud Rate (115200 default rate in VS Code Serial Monitor)
  Serial.println("Run keyboard control");
  Serial.println("This Injector is controlled through serial communication. This has been tested using VS Code's arduino extension, though Arduino IDE's serial monitor or any other serial interface should work.");
  Serial.println("In VS Code, click into the message box below and type the desired value. Then, hit enter to send the message to the injector.");
  printInstructions();
  injector.setCurrentPosition(0);
  injector.setMaxSpeed(1000);
}



void loop() {
  if(restart){
    Serial.println("Finished\n");
    printInstructions();
    restart = false;
  } else if (safety) {
    Serial.println("Safety tripped, please extract and reset.\n");
    printInstructions();
    safety = false;
  }
  if(Serial.available()){
    char val = Serial.read();
    Serial.println(val);
    if(val != -1)
    {
      switch(val)
      {
      case '1': // Mode to Fill the syringe
        start_injector (false);   
        break;
      case '2': // Mode for injection
        start_injector (true);
        break;
      case '3':
        manual();
        break;

      case '0':
        stop();
        break;
      }
    }
    else stop();
  }
}