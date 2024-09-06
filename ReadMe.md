# **UD Automated Stereotaxic Injector**

This project has been developed by Owen Beer, Reilly Downing, and Megan LaBelle in support of Dr. Aaron Sathyanesan and Dr. Krishna Kidambi with the University of Dayton Biology and Mechanical Engineering departments.

The Automated Injector is constructed with 3D printed housings, a Nema 8 stepper, an  Arduino starter kit, and various hardware. A full parts list is available as a part of this package.

### Purpose
Stereotaxic surgeries are a very common procedure in modern neuroscience. They are minimally invasive procedures that use focused radiation beams to treat tumors and do research. The University of Dayton Biology Department uses a GCaMP6f Ca2+ sensor to image the firing of neurons after injecting a virus into the brain of a living specimen. This virus causes the calcium indicators (GCaMP6f Ca2+) to be present, which can then be imaged using a specific wavelength laser. The injection of the virus however must be a very controlled process to avoid harming the specimen. Commonly, labs will buy syringe pumps with controllers that run around $3,000. The University of Dayton Biology department has assigned the team the task of designing this open-source alternative solution using readily available, cost-efficient components and an Arduino microcontroller.

### Repository Layout
...

### Programming Arduino in VS Code 
...

### **How to establish serial communication with device through VS Code**
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
9. Open the Serial Monitor  
   1. Open the command search ( ctrl \+ shift \+ p )  
   2. Begin to type Arduino: Open Serial Monitor, click the command  
   3. Select the baud rate, 115200 is used for the injector

The serial terminal should open at the bottom of the window. You may need to select Start Monitoring to open the COM port. If a message from the device is not displayed, press the reset button (physical button in corner of expansion board) on the Arduino controller.  
![](/readme_images/terminal_view.png)  
To send messages and interact with the device, click into the bar at the bottom of the Serial Monitor window. It should read Type in a message to send to the serial port. while waiting for an input.