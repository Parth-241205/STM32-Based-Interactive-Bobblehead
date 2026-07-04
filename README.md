# STM32 Based Interactive Deskbot Bobblehead
# Overview
This project uses a Nucleo-64 STM32F072RB with a 2.4" LCD TFT Display a IMU, and multiple touch Sensors mounted on a custom 3d printed Bobble Head body.
The STM controller uses custom functions to perform various tasks such as STOPWATCH, Clock Display, Basic Calculations (Multiplication, Division, Addition, Subtraction, Exponential) and an Interactive mode where The eyes of the Deskbot reacts and rolls on tapping the bobble head. <br><br><br>



<img width="350" height="350" alt="Screenshot 2026-04-22 193437" src="https://github.com/user-attachments/assets/c9256b5d-2b2c-405a-b77f-886389b587aa" />
 &emsp;
 &emsp;
&emsp;

<img width="350" height="350" alt="bot" src="https://github.com/user-attachments/assets/74328ead-a845-4217-967c-69376faa31b1" />



## Wiring Guide
____
#### Display : LCD to Nucleo-64
SPI Interface :-<br> <br>
  1)PA5 (SCK) <br>
  2)PA7 (MOSI) <br>
  3)PB12 (CS) <br>
  4)PC5 (DC)<br>
  5)PC4 (Reset)<br>
____
#### MPU to Nucleo
I2C Interface :-<br> <br>
  1)PB6 (SCL) <br>
  2)PB7 (SDA)<br>
____
#### Controls : Touch sensor to Nucleo
GPIO Interface :-<br> <br>
  1)PA4 (Menu - jumps to app list) <br>
  2)PA0 (Up/Increment) <br>
  3)PA1 (Down/Decrement) <br>
  4)PB0 (Select/Confirm) <br>
  5)PB1 (Home button back to the Face)

# Next Tasks
1) To include a RTC clock module connected via an LDO, electrically isolated from the main circuit to keep the clock running for better portability.
2) To include Arcade Games like Pong and Brickbuster.
