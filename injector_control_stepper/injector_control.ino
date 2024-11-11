/* Injector Control
  Reilly Downing - Owen Beer, Megan LaBelle, Naeamah Rabeea
  April 2024

  This program establishes serial control over an automated microinjector.
  
*/


#include <Stepper.h>
#include <AccelStepper.h>
 
AccelStepper injector(AccelStepper::DRIVER, 8, 9); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5


long steps = 0;
int stepsprev = 1600;
bool restart = false;
bool safety = false;

// for hamiltion 7000.5 --could maybe make a lookup for both syringes
// 8.33333333 nL per mm
const float HAM_7000_5_DIV = 25 / 3;
const float HAM_7001_DIV = 50 / 3;

float syringe_div;

// given lead/pitch of our rod (mm/rotation)
float lead = 0.6096;


void stop() {
  injector.stop();
  injector.setCurrentPosition(0);
  Serial.println("Stopped\n");
}


long getsteps(float vol) {
  return (vol * stepsprev) / (syringe_div * lead );
}

float getsteppsec(float flow) {
  float lin_speed = flow / syringe_div;
  float rpm = lin_speed / lead; // handy for stepper motors
  return rpm / 60.0 * stepsprev;
}

void start_injector(bool inject) {
  float flow, vol;

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
      if (flow < 1 || flow > 1000) {
        Serial.println("Flow rate must be between 1 and 1000 nL/min\nInput Flow Rate (nL/min):");
      } else {break;}
    }
  }
 
  
  Serial.println("\nInput Volume (nL):"); 
  while (true) {
    if (Serial.available()) {                            
      vol = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      if (vol < 1 || vol > 500) {
        Serial.println("Volume must be between 1 and 500 nL\nInput Volume (nL):");
      } else {break;}
    }
  }
  
  float steppsec = getsteppsec(flow);

  steps = getsteps(vol);

  //Debug Info
  Serial.print("flow rate: \t");
  Serial.println(flow);
  Serial.print("vol: \t\t");
  Serial.println(vol); 
  Serial.print("Steps/sec: \t");
  Serial.println(steppsec);
  Serial.print("Steps: \t\t");
  Serial.println(steps);

  
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
  
  while( (digitalRead(12)==LOW || !inject) ) {   
    injector.runSpeedToPosition();

    if(digitalRead(13)==HIGH) {
      Serial.println("**\n**PAUSING - Press Button Again to Resume, Press Reset to Stop\n**\n");
      while(true){
        delay(1);
        if (digitalRead(13)==LOW) {   
          delay(1);     // tiniest debouncing code to prevent errant pausing
          while (digitalRead(13)==LOW){/* Hangs until button is Pressed*/}
          while(digitalRead(13)==HIGH){/* Hangs until button is released*/}
          delay(1);
          Serial.println("**\n**RESUMING \n**\n");
          break;
        }
      }
    }

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
      
      float steppsec = getsteppsec(600);
      steps = getsteps(500);

      injector.move(-steps);
      injector.setSpeed(steppsec);

      while(digitalRead(12)==LOW && xValue > upthresh) {
        xValue = analogRead(A2);
        injector.runSpeedToPosition();
      }
    }
    if (xValue < downthresh) {
      float steppsec = getsteppsec(600);
      steps = getsteps(500);

      injector.move(steps);
      injector.setSpeed(steppsec);

      while(xValue < downthresh) {
        xValue = analogRead(A2);
        injector.runSpeedToPosition();
      }

    }
  }
  injector.setCurrentPosition(0);
  restart = true;
}

void select_syringe() {
  bool selected = false;
  Serial.println("Input the number associated with the desired syringe: ");
  Serial.println("`1` - Hamilton 7000.5 0.5uL");
  Serial.println("`2` - Hamilton 7001 1uL");
  Serial.println("`3` - Custom");
  Serial.println("`4` - Exit without changing");
  while (!selected) {
    if(Serial.available()){
      char val = Serial.read();
      switch(val)
      {
      case '1': // Mode to Fill the syringe
        syringe_div = HAM_7000_5_DIV;
        Serial.println("Hamilton 7000.5 0.5uL Selected.\n");
        selected = true;
        break;
      case '2': // Mode for injection
        syringe_div = HAM_7001_DIV;
        Serial.println("Hamilton 7001 1uL Selected.\n");
        selected = true;
        break;
      case '3':
        syringe_div = get_div();
        Serial.println("Custom syringe set.\n");
        selected = true;
        break;
      case '4':
        Serial.println("Syringe not updated.\n");
        selected = true;
        break;
      }
    }
  }
  restart = true;
}

float get_div() {
  float div;
  Serial.println("Input Custom Syringe Volume per Millimeter (nL/mm):"); 
  while (true) {
    if (Serial.available()) {                            
      div = Serial.parseFloat(); //returns a zero if it times out or no valid ints are available
      if (div < 1 || div > 1000) {
        Serial.println("Syringe gradation must be between 1 and 1000 nanoliters per linear millimeter.\nInput value (nL/mm):");
      } else {break;}
    }
  }
  return div;
}


void printInstructions() {
  Serial.println("Input the number associated with a mode to begin: ");
  Serial.println("`1` - Extraction Mode");
  Serial.println("`2` - Injection Mode");
  Serial.println("`3` - Manual Joystick Control");
  Serial.println("`4` - Select Syringe");
  Serial.println();
  Serial.println("Press the reset button on the device to exit any mode and see this message again.");
}

void setup() {
  Serial.begin(115200);      //Set Baud Rate (115200 default rate in VS Code Serial Monitor)
  syringe_div = HAM_7000_5_DIV;
  Serial.println("Run keyboard control");
  Serial.println("This Injector is controlled through serial communication. This has been tested using VS Code's arduino extension, though Arduino IDE's serial monitor or any other serial interface should work.");
  Serial.println("In VS Code, click into the message box below and type the desired value. Then, hit enter to send the message to the injector.");
  Serial.println("Default Syringe: 0.5uL");
  printInstructions();
  injector.setCurrentPosition(0);
  injector.setMaxSpeed(8000);
}



void loop() {
  injector.setCurrentPosition(0);
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
      case '4':
        select_syringe();
        break;

      }
    }
    else stop();
  }
}