#include <Arduino.h>
#include <Wire.h> // Required for I2C communication
#include <LiquidCrystal_I2C.h> 
#include <Servo.h>             
 
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define BUZZER_PIN 9 
#define SERVO_PIN 10
#define SERVO_PIN2 11  

#define greenLed 3
#define yellowLed 12
#define redLed 13

Servo servo , servo2;

//Notes pins [4:8] belongs to TCS3200 color sensor.Servo must be connected to one of the PWM(Pulse width modulation) pins which are {3,5,6,9,10,11}

#define I2C_ADDR    0x27 
#define LCD_COLUMNS 16  
#define LCD_ROWS    2    
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);


//Initial servo angles.

int upperServoAngle = 180;
int lowerServoAngle = 45;  

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

int greenCount = 0;
int redCount = 0;
int blueCount = 0;
int orangeCount = 0;
int brownCount = 0;
int yellowCount = 0;
int totalCandy = 0;


void setup() {
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  pinMode(greenLed , OUTPUT);
  pinMode(yellowLed , OUTPUT);
  pinMode(redLed , OUTPUT);


  lcd.init(); // Initializing LCD
  lcd.backlight(); // For turning on the backlight of the LCD

  pinMode(BUZZER_PIN, OUTPUT);


  delay(500); // Wait servo to return 
  servo.attach(SERVO_PIN);
  servo2.attach(SERVO_PIN2);  


  // Set Frequency scale as 20% (HIGH, LOW)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  delay(100); 
  lcd.clear(); 

}

void loop() {

  
  servo.write(upperServoAngle);
  servo2.write(lowerServoAngle);
  delay(1000);
  upperServoAngle = 133;
  digitalWrite(yellowLed , HIGH);
  servo.write(upperServoAngle);
  delay(1500);
  

  redValue = readColor('r');
  greenValue = readColor('g');
  blueValue = readColor('b');

  /* //Use only before color calibration.After calibration delete it.
  lcd.setCursor(0,0);
  lcd.print("R: ");
  lcd.print(redValue);
  lcd.print(" G: ");
  lcd.print(greenValue);
  lcd.setCursor(0,1);
  lcd.print("B: ");
  lcd.print(blueValue); */


  delay(2000); //Must be adjusted.
  digitalWrite(yellowLed , LOW);
  
  String detectedColor = determineColor(redValue, greenValue, blueValue);

  lcd.setCursor(0, 0); 
  lcd.print("Color: ");
  lcd.println(detectedColor); 
  lcd.setCursor(0, 1); 


  if (!detectedColor.equals("unknown")){
    digitalWrite(greenLed , HIGH);
  } else {
    digitalWrite(redLed , HIGH);
  }
  
  uint8_t lowerServoAngles[6] = {0,25,45,65,95,110};
  

  if (detectedColor.equals("red")) {
    redCount++;
    lcd.println("Total: " + String(redCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else if (detectedColor.equals("orange")) {
    orangeCount++;
    lcd.println("Total: " + String(orangeCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else if (detectedColor.equals("yellow")) {
    yellowCount++;
    lcd.println("Total: " + String(yellowCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else if (detectedColor.equals("green")) {
    greenCount++;
    lcd.println("Total: " + String(greenCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else if (detectedColor.equals("blue")) {
    blueCount++;
    lcd.println("Total: " + String(blueCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else if (detectedColor.equals("brown")) {
    brownCount++;
    lcd.println("Total: " + String(brownCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
  } else { // When detectedColor == "unknown";
    lcd.println("Total: " + String(brownCount) + " " + detectedColor);
    lowerServoAngle = lowerServoAngles[4];
    digitalWrite(BUZZER_PIN , HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN,  LOW);
  }
  
  servo2.write(lowerServoAngle);

  // Throwing process
  upperServoAngle = 80;
  servo.write(upperServoAngle);
  delay(500);
  upperServoAngle = 180; //back to default position
  delay(1000);
  lowerServoAngle = 45; //back to default position
  servo2.write(lowerServoAngle);
  digitalWrite(greenLed , LOW);
  digitalWrite(redLed , LOW);
}


int readColor(char color) {
  switch (color) {
    case 'r': // Red filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      break;
    case 'g': // Green filter
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      break;
    case 'b': // Blue filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      break;
    default: // No filter
      digitalWrite(S2, HIGH);
      digitalWrite(S3, LOW);
      break;
  }

  delayMicroseconds(150); //Time to wait for the adaptation of the filter.

  // Read LOW count as microseconds from the signal of OUT pin
  // Lower value = Higher Frequency = More Aggressive Color
  long int pulseDuration = pulseIn(sensorOut, LOW);

  // Set upper bound as 50000 in case for timeout(return 0) or too high.
  if (pulseDuration == 0 || pulseDuration > 50000) { 
     return 50000; 
  }

  return pulseDuration;
}

String determineColor(int r, int g, int b) {
  
  // Note: Lower pulseIn value = More aggresive color.

  if ((r >= 70 && r <= 100) && (g >= 100 && g <= 125) && (b >= 85 && b <= 100)) { return "red";}
  else if ((r >= 70 && r <= 80) && (g >= 75 && g <= 100) && (b >= 75 && b <= 85)) { return "orange"; }
  else if ((r >= 80 && r <= 95) && (g >= 85 && g <= 105) && (b >= 70 && b <= 85)) { return "brown"; }
  else if ((r >= 75 && r <= 90) && (g >= 80 && g <= 95) && (b >= 70 && b <= 85)) { return "green"; }
  else if ((r >= 85 && r <= 105) && (g >= 80 && g <= 100) && (b >= 60 && b <= 80)) { return "blue"; }
  else if ((r >= 60 && r <= 75) && (g >= 65 && g <= 86) && (b >= 70 && b <= 80)) { return "yellow"; }
  else { return "unknown"; }
}
         
 
