/* Injector Control
  Reilly Downing - Owen Beer, Megan LaBelle, Naeamah Rabeea
  April 2024

  This program establishes serial control over an automated microinjector.
  See documentation for instructions [Google Docs Link]

*/


#include <Stepper.h>
#include <AccelStepper.h>
 
AccelStepper injector(AccelStepper::FULL4WIRE, 8, 9, 10, 11); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5


int steps = 0;
int stepsprev = 200;
bool restart = false;

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



void inject() {
  float flow, lin_speed, vol;
  Serial.println("**Injection Mode** \nInput Flow Rate (nL/min):"); 
  while (true) {
    if (Serial.available()) {                            
      flow = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }

  lin_speed = flow / HAM_7000_5_DIV; 

  float rpm = lin_speed / lead; // handy for stepper motors
  float steppsec = rpm / 60.0 * 200.0;
  
  
  Serial.println("\nInput Volume (nL):"); 
  while (true) {
    if (Serial.available()) {                            
      vol = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }
  
  //Debug Info
  Serial.print("flow rate: ");
  Serial.println(flow);
  Serial.print("lin_speed: ");
  Serial.println(lin_speed);
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("vol: ");
  Serial.println(vol); 
  
  steps = 1 / (HAM_7000_5_DIV * lead / vol / stepsprev);
  
  Serial.println("\nReady. Hit Physical Button to Inject."); 
  
  while (true) {
    if (digitalRead(13)==HIGH) {                            
      break; // Hangs until button is pressed
    }
  }

  Serial.println("injecting...");
  injector.move(-steps);
  injector.setSpeed(steppsec);
  
  while(true) {
    injector.runSpeedToPosition();

    if (injector.currentPosition()==injector.targetPosition()) {
        injector.setCurrentPosition(0);
        restart = true;
        break;
    }
  }
  
  steps = 0;
}



void extract() {
  float flow, lin_speed, vol;

  Serial.println("**Extraction Mode** \nInput Flow Rate (nL/min):"); 
  while (true) {
    if (Serial.available()) {                            
      flow = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }

  lin_speed = flow / HAM_7000_5_DIV;   

  float rpm = lin_speed / lead; 
  float steppsec = rpm / 60.0 * 200.0;

  
  Serial.println("\nInput Volume (nL):"); 
  while (true) {
    if (Serial.available()) {                            
      vol = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      break;
    }
  }
  
  //Debug Info
  Serial.print("flow rate: ");
  Serial.println(flow);
  Serial.print("lin_speed: ");
  Serial.println(lin_speed);
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("vol: ");
  Serial.println(vol); 
  
  steps = 1 / (HAM_7000_5_DIV * lead / vol / stepsprev);
  
    Serial.println("\nReady. Hit Physical Button to Extract."); 
  
  while (true) {
    if (digitalRead(13)==HIGH) {                            
      break; // Hang until the button is pressed
    }
  }

  Serial.println("Extracting...");
  injector.move(steps);
  injector.setSpeed(steppsec);
  
  while(true) {
    injector.runSpeedToPosition();

    if (injector.currentPosition()==injector.targetPosition()) {
        injector.setCurrentPosition(0);
        restart = true;
        break;
    }
  }
  
  steps = 0;
}



void setup() {
  Serial.begin(115200);      //Set Baud Rate (115200 default rate in VS Code Serial Monitor)
  Serial.println("Run keyboard control");
  Serial.println("This Injector is controlled through serial communication. This has been tested using VS Code's arduino extension, though Arduino IDE's serial monitor or any other serial interface should work.");
  Serial.println("In VS Code, click into the message box below and type the desired value. Then, hit enter to send the message to the injector.");
  Serial.println("Input the number associated with a mode to begin: ");
  Serial.println("`1` - Extraction Mode");
  Serial.println("`2` - Injection Mode");
  Serial.println("`0` - Stop");
  Serial.println();
  Serial.println("Press the reset button on the device to exit any mode and see this message again.");
  injector.setCurrentPosition(0);
  injector.setMaxSpeed(1000);
}



void loop() {
  if(restart){
    //Serial.println("This Injector is controlled through serial communication. This has been tested using VS Code's arduino extension, though Arduino IDE's serial monitor or any other serial interface should work.");
    //Serial.println("In VS Code, click into the message box below and type the desired value. Then, hit enter to send the message to the injecctor.");
    Serial.println("Finished\n");
    Serial.println("Input the number associated with a mode to begin: ");
    Serial.println("`1` - Extraction Mode");
    Serial.println("`2` - Injection Mode");
    Serial.println("`0` - Stop");
    Serial.println();
    Serial.println("Press the reset button on the device to exit any mode and see this message again.");
    restart = false;
  }
  if(Serial.available()){
    char val = Serial.read();
    Serial.println(val);
    if(val != -1)
    {
      switch(val)
      {
      case '1': // Mode to Fill the syringe
        extract ();   
        break;
      case '2': // Mode for injection
        inject ();
        break;

      case '0':
        stop();
        break;
      }
    }
    else stop();
  }
}