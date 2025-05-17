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
#define SERVO_PIN2 11  // İkinci servo için pin tanımı

#define greenLed 3
#define yellowLed 12
#define redLed 13

Servo servo;
Servo servo2;  // İkinci servo tanımı

//Notes pins [4:8] belongs to TCS3200 color sensor.Servo must be connected to one of the PWM(Pulse width modulation) pins which are {3,5,6,9,10,11}

#define I2C_ADDR    0x27 
#define LCD_COLUMNS 16  
#define LCD_ROWS    2    
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);


int servoAngle = 180;
int servo2Angle = 0;  // İkinci servo için başlangıç açısı

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


  lcd.init(); // Initialize the LCD
  lcd.backlight(); // For turning on the backlight

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);


  delay(500); // Servo'nun başlangıç pozisyonuna gitmesi için kısa bir süre bekle
  servo.attach(SERVO_PIN);
  servo2.attach(SERVO_PIN2);  // İkinci servoyu bağla
  Serial.begin(9600);

  Serial.begin(9600);
  Serial.println("TCS3200 Renk Sensörü Hazır");

  // Frekans ölçeklendirmeyi %20 olarak ayarla (HIGH, LOW)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  delay(100); // Küçük bir bekleme
  lcd.clear(); // LCD ekranını temizle

}

void loop() {

  
  servo.write(servoAngle);
  servo2.write(servo2Angle);  // İkinci servoyu başlangıç pozisyonuna getir
  delay(1000);
  servoAngle = 133;
  digitalWrite(yellowLed , HIGH);
  servo.write(servoAngle);
  delay(1500);
  digitalWrite(yellowLed , LOW);
  
  
  redValue = readColor('r');
  greenValue = readColor('g');
  blueValue = readColor('b');

  delay(2000); //Must be adjusted.

  
  String detectedColor = determineColor(redValue, greenValue, blueValue);
  Serial.print("Detected color: ");
  Serial.println(detectedColor);
  Serial.println("--------------------");

  Serial.print("Kaydirak acisi: ");
  Serial.println(servo2Angle);

  
  if(detectedColor.equals("unknown")){
    digitalWrite(redLed , HIGH);
    digitalWrite(BUZZER_PIN , HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN,  LOW);
    digitalWrite(redLed , LOW);
  } else {
    digitalWrite(greenLed , HIGH);
  }

   // Önceki yazıyı temizle (titreşime neden olabilir, alternatif aşağıda)
  lcd.setCursor(0, 0); // İmleci başa al (ilk satır)
  lcd.print("Color: ");
  lcd.println(detectedColor); 
  lcd.setCursor(0, 1); 
  //Buralara kaydırağın açısı yazılacak
  
  if (detectedColor.equals("red")) {
    redCount++;
    lcd.println("Total: " + String(redCount) + " " + detectedColor);
    servo2Angle = 130; 
    servo2.write(servo2Angle);
  } else if (detectedColor.equals("orange")) {
    orangeCount++;
    lcd.println("Total: " + String(orangeCount) + " " + detectedColor);
    servo2Angle = 110; 
    servo2.write(servo2Angle);
  } else if (detectedColor.equals("yellow")) {
    yellowCount++;
    lcd.println("Total: " + String(yellowCount) + " " + detectedColor);
    servo2Angle = 90; 
    servo2.write(servo2Angle);
  } else if (detectedColor.equals("green")) {
    greenCount++;
    lcd.println("Total: " + String(greenCount) + " " + detectedColor);
    servo2Angle = 70; 
    servo2.write(servo2Angle);
  } else if (detectedColor.equals("blue")) {
    blueCount++;
    lcd.println("Total: " + String(blueCount) + " " + detectedColor);
    servo2Angle = 50; 
    servo2.write(servo2Angle);
  } else if (detectedColor.equals("brown")) {
    brownCount++;
    lcd.println("Total: " + String(brownCount) + " " + detectedColor);
    servo2Angle = 30; 
    servo2.write(servo2Angle);
  }
  
  //Atış işlemine başlama
  servoAngle = 80;
  servo.write(servoAngle);
  delay(500);
  servoAngle = 180; //back to default position
  delay(1000);
  servo2Angle = 0; //back to default position
  servo2.write(servo2Angle);
  Serial.print("Kaydirak acisi: ");
  Serial.println(servo2Angle);
  
  if(detectedColor.equals("unknown")){
    lcd.setCursor(0, 1);
    lcd.print("Buzzer activated");
  }
  digitalWrite(greenLed , LOW);
}


// Belirtilen renk filtresini seçip frekansı (pulseIn değeri) okuyan fonksiyon
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

  // OUT pinindeki sinyalin LOW süresini mikrosaniye cinsinden oku
  // Düşük değer = Yüksek Frekans = Yoğun Renk
  unsigned long pulseDuration = pulseIn(sensorOut, LOW);

  // Eğer pulseIn zaman aşımına uğrarsa (0 dönerse) veya çok yüksekse, makul bir üst sınır ver.
  if (pulseDuration == 0 || pulseDuration > 50000) { // 50000us sınırı deneyerek ayarlanabilir
     return 50000; // Veya başka bir hata/maksimum değer
  }

  return pulseDuration;
}

// Ham R, G, B değerlerine göre rengi belirleyen fonksiyon (KALİBRASYON GEREKLİ!)
String determineColor(int r, int g, int b) {
  
  // Note: Lower pulseIn value = More aggresive color.

 if ((r >= 70 && r <= 90) && (g >= 100 && g <= 115) && (b >= 70 && b <= 100)) { return "red";}
else if ((r >= 65 && r <= 85) && (g >= 90 && g <= 105) && (b >= 80 && b <= 100)) { return "orange"; }
else if ((r >= 80 && r <= 105) && (g >= 95 && g <= 110) && (b >= 75 && b <= 90)) { return "brown"; }
else if ((r >= 80 && r <= 100) && (g >= 75 && g <= 100) && (b >= 70 && b <= 90)) { return "green"; }
else if ((r >= 80 && r <= 105) && (g >= 80 && g <= 105) && (b >= 50 && b <= 85)) { return "blue"; }
else if ((r >= 50 && r <= 75) && (g >= 60 && g <= 95) && (b >= 65 && b <= 81)) { return "yellow"; }
else { return "unknown"; }
}

