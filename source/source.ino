#include <Wire.h>                                                 //include the I2C library to communicate with the sensor
#include "Adafruit_TCS34725.h"                                    //include the sensor library
 
 
 
#define commonAnode false                                         // set to false if using a common cathode LED                    
int step_count;
int distance = 0;
int color = 0;
boolean dir = true; //rotation based on color of M&M 
 
 
byte gammatable[256];                                             // our RGB -> eye-recognized gamma color
 
                                                                  //Create an instance of the TCS34725 Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
 
void setup() {

  Serial.begin(9600);  //Sart serial comms @ 9600 (you can change this)
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.println("Color View Test");                              //Title info            
 
  if (tcs.begin()) {                                              //if the sensor starts correctly
    Serial.println("Found sensor");                               //print the happy message
  } else {                                                        //if the sensor starts incorrectly
    Serial.println("No TCS34725 found ... check your connections");//print the not so happy message
    while (1); // halt!
  }
                                                                   // gamma table helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
     
    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;      
    }
    //Serial.println(gammatable[i]);
  }
}

void loop() {
  color = readColor();  //readColor function sets value of color to either 0, 1 , or 2
  if(color == 1)
  {
    dir = true;     //dir true turns motor clockwise for green M&M
    Serial.print("GREEN M&M");    
    Serial.println();
  }
  else if(color == 2)
  {
    dir = false;  //dir false turns motor counter clockwise for Blue M&M
    Serial.print("BLUE M&M");
    Serial.println();
  }
  else    //for color not recognized as blue or green
  {
    Serial.print("Error not blue or green");
    Serial.println();
  }
  
  //Code for Stepper Motor
  
  while(distance < 700 && distance > -1 && color > 0){    //distance = amount of rotation
                                                          //color initialized at 0 in the sketch, 
                                                         //while loop starts once a color is assigned value accorind to readColor function
  switch(step_count){           //using switch and based on step_count we can perform each motor step
    case 0:                     //as you can see we are shifting high state every step
     digitalWrite(2, HIGH);
     digitalWrite(3, LOW);
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
    break;
 
    case 1:
     digitalWrite(2, LOW);
     digitalWrite(3, HIGH);
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
    break;
 
    case 2:
     digitalWrite(2, LOW);
     digitalWrite(3, LOW);
     digitalWrite(4, HIGH);
     digitalWrite(5, LOW);
    break;
 
    case 3:
     digitalWrite(2, LOW);
     digitalWrite(3, LOW);
     digitalWrite(4, LOW);
     digitalWrite(5, HIGH);
    break;
  }
  if(dir){
                        //incrementing step_count value 
    step_count++;
  }
  else{
                           //decrementing step_count value (the other direction)
    step_count --;
  }
                             //if bigger than 3 while incrementing should set it to 0
  if(step_count > 3){
    step_count = 0; 
  }
                          //if less than 0 while decrementing should set it to 3
  if(step_count < 0){
    step_count = 3;
  }
              //change the delay to change the speed for this motor 0.002 second is the smallest possible
  delay(3);
  distance = distance +1;
                          //distance increments until 700, point at which M&M is dropped
  }

 while(distance > 0){ 
                          //stepper motor code again to set motor to initial position
  switch(step_count){
    case 0:
     digitalWrite(2, HIGH);
     digitalWrite(3, LOW);
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
    break;
 
    case 1:
     digitalWrite(2, LOW);
     digitalWrite(3, HIGH);
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
    break;
 
    case 2:
     digitalWrite(2, LOW);
     digitalWrite(3, LOW);
     digitalWrite(4, HIGH);
     digitalWrite(5, LOW);
    break;
 
    case 3:
     digitalWrite(2, LOW);
     digitalWrite(3, LOW);
     digitalWrite(4, LOW);
     digitalWrite(5, HIGH);
    break;
  }
  if(dir){
  step_count--;
  }
  else{
    step_count++;
  }
  if(step_count < 0){
    step_count = 3; 
  }
  if(step_count > 3){
    step_count = 0;
  }
  delay(3);
  distance = distance -1; 
                          //distance decrements until 0 (back to initial position
  }
  
}
 
 
int readColor() {
  uint16_t clear, red, green, blue;                                 //declare variables for the colors
 
//  tcs.setInterrupt(false);                                          // turn on LED (not used)
 
  delay(5000);                                                        // takes 50ms to read
 
  tcs.getRawData(&red, &green, &blue, &clear);                      //read the sensor
 
 // tcs.setInterrupt(true);                                           // turn off LED (not used)
 
  Serial.print("C:\t"); Serial.print(clear);                        //print color values
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
 
                                                                    // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  Serial.print("\t");
 // Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);  //prints hex values for rgb
  Serial.println();
                                            //following code decides color 
  if(green>600 && green>blue && green>red)   //green mnm if, > blue, > red, and greater than 600 threshold for setting
  {
    color = 1;
  }
  else if(blue>600 && blue>green && blue>red)   //blue mnm if, > green, > red, and setting threshold > 600
  {
    color = 2;
  }
  else
  {
    color = 0;        //else set color to 0 to stop while loop
  }
 return color;
                                                                    
}
