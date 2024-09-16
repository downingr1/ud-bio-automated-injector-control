# **UD Automated Stereotaxic Injector**

This project has been developed by Owen Beer, Reilly Downing, and Megan LaBelle in support of Dr. Aaron Sathyanesan and Dr. Krishna Kidambi with the University of Dayton Biology and Mechanical Engineering departments.

The Automated Injector is constructed with 3D printed housings, a Nema 8 stepper, an  Arduino starter kit, and various hardware. A full parts list is available as a part of this package.

### Purpose
Stereotaxic surgeries are a very common procedure in modern neuroscience. They are minimally invasive procedures that use focused radiation beams to treat tumors and do research. The University of Dayton Biology Department uses a GCaMP6f Ca2+ sensor to image the firing of neurons after injecting a virus into the brain of a living specimen. This virus causes the calcium indicators (GCaMP6f Ca2+) to be present, which can then be imaged using a specific wavelength laser. The injection of the virus however must be a very controlled process to avoid harming the specimen. Commonly, labs will buy syringe pumps with controllers that run around $3,000. The University of Dayton Biology department has assigned the team the task of designing this open-source alternative solution using readily available, cost-efficient components and an Arduino microcontroller.

### Repository Layout
...

### Programming Arduino Using the Arduino IDE
...

### Simply Executing Serial Communication in VS Code 
Reilly Donwing - September 2024

This documentation will detail how to install Microsoft Visual Studio Code and a Serial Monitor extension to simply establish serial communication with the injector.

1. Retrieve the appropriate installer for your operating system from [https://code.visualstudio.com/download](https://code.visualstudio.com/download)   
2. Run the installer, the default location is fine. Changing the location will not affect this process.  
3. Open VS Code  
4. Open the extensions menu ( ctrl \+ shift \+ x ) and search for Serial Monitor 
5. Select the extension marked as produced by Microsoft, then select install
6. At the top of the window, select Terminal > New Terminal
7. In the new partition at the bottom of the window, select SERIAL MONITOR
8. With the device plugged in, ensure that the Port drop down has the correct port selected, ensure a buad rate of 115200, and select Start Monitoring.

If a message from the device is not displayed, press the reset button (physical button in corner of expansion board) on the Arduino controller.  
![](/readme_images/terminal_view.png)  
To send messages and interact with the device, click into the bar at the bottom of the Serial Monitor window. It should read Type in a message to send to the serial port. while waiting for an input.


### Programming Arduino in VS Code
Reilly Downing \- April 2024

This documentation will detail how to install Microsoft Visual Studio Code, and how to use the Arduino extension to establish serial communication with the injector.

1. Retrieve the appropriate installer for your operating system from [https://code.visualstudio.com/download](https://code.visualstudio.com/download)   
2. Run the installer, the default location is fine. Changing the location will not affect this process.  
3. Open VS Code  
4. Open the extensions menu ( ctrl \+ shift \+ x ) and search for Arduino  
5. Select the extension marked as produced by Microsoft, then select install  
6. If prompted, select to use Arduino CLI  
   1. If not prompted, open settings ( ctrl \+ , ), type ‘arduino’ into the search bar  
   2. Scroll down to see Arduino: **Use Arduino Cli**  
   3. Select the box to enable this

![](/readme_images/command_path.png)

7. Restart the IDE (VS Code)  
8. Ensure the device is plugged into the computer using USB  
   1. Open the command search ( ctrl \+ shift \+ p )  
   2. Begin to type Arduino: Select Serial Port, click the command  
   3. Select the port with Uno R3 or USB Serial Device from the list  
      1. For Error regarding Serial Monitor Extension, return to step 6, double check the CLI use in settings, and restart the IDE, ensuring it has been stopped in task manager before reopening  
      2. if the error pops up after this, simply ignore it and run Arduino: Select Serial Port again  
9. The arduino.json configuration file should be created inside the .vscode directory of the current workspace, ensure this file exists or create it.
   1. Edit the json object to point to the injector control program, it should have the following form:
   ```json
   {
    "board": "arduino:avr:uno",
    "sketch": "injector_control_stepper\\injector_control.ino",
    "port": "COM4"
   }
   ```
10. Open injector_control.ino and select the verify then upload options from the arduino extensions up in the top right of the editor. On errors regarding missing libraries, ensure that the accel_stepper library is included in your arduino libraries path.