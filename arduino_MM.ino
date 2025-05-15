#include <Arduino.h>
#include <Wire.h> // I2C iletişimi için gerekli
#include <LiquidCrystal_I2C.h> // I2C LCD kütüphanesi
#include <Servo.h>             // Servo motor kütüphanesi


#define I2C_ADDR    0x27 // LCD I2C Adresi (KENDİNİZİNKİNİ KONTROL EDİN!)
#define LCD_COLUMNS 16   // LCD Sütun Sayısı
#define LCD_ROWS    2    // LCD Satır Sayısı  

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

#define BUZZER_PIN 9 // Buzzer'ın bağlandığı dijital pin
#define SERVO_PIN 10 // Servo sinyal kablosunun bağlandığı PWM pini (~)

Servo servo;
int servoAngle = 180;

// TCS3200 Pin Tanımlamaları
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// Renk okumaları için değişkenler
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup() {
  // Pin modlarını ayarla
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  lcd.init(); // LCD'yi başlat (bazı kütüphanelerde lcd.begin() olabilir)
  lcd.backlight(); // Arka ışığı aç
  lcd.setCursor(0, 0); // İmleci ilk satır, ilk sütuna ayarla
  lcd.print("Renk Sensoru");
  lcd.setCursor(0, 1); // İmleci ikinci satır, ilk sütuna ayarla
  lcd.print("Basliyor...");
  Serial.println("TCS3200 Renk Sensörü, LCD, Buzzer & Servo Hazır");

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Başlangıçta buzzer kapalı olsun

  delay(500); // Servo'nun başlangıç pozisyonuna gitmesi için kısa bir süre bekle
  servo.attach(SERVO_PIN);
  Serial.begin(9600);

  // Seri haberleşmeyi başlat (Değerleri görmek için)
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
  delay(1000);
  servoAngle = 131;
  servo.write(servoAngle);
  delay(1500);
  
  // Kırmızı rengi oku
  redValue = readColor('r');


  // Yeşil rengi oku
  greenValue = readColor('g');


  // Mavi rengi oku
  blueValue = readColor('b');

  // Okunan Ham Değerleri Yazdır (KALİBRASYON İÇİN ÖNEMLİ!)
  Serial.print("Ham Degerler -> R: ");
  Serial.print(redValue);
  Serial.print(" G: ");
  Serial.print(greenValue);
  Serial.print(" B: ");
  Serial.println(blueValue);
  delay(10000);

  // Rengi belirle ve yazdır
  String detectedColor = determineColor(redValue, greenValue, blueValue);
  Serial.print("Tespit Edilen Renk: ");
  Serial.println(detectedColor);
  Serial.println("--------------------");

  if (detectedColor.equals("KIRMIZI")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if (detectedColor.equals("TURUNCU")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if (detectedColor.equals("SARI")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if (detectedColor.equals("YESIL")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if (detectedColor.equals("MAVI")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if (detectedColor.equals("KAHVERENGI")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  } else if(detectedColor.equals("BILINMEYEN")) {
    servoAngle = 100;
  servo.write(servoAngle);
  delay(500);
  }
  servoAngle = 180;
  

  
  lcd.clear(); // Önceki yazıyı temizle (titreşime neden olabilir, alternatif aşağıda)
  lcd.setCursor(0, 0); // İmleci başa al (ilk satır)
  lcd.print("Renk: ");
  lcd.print(detectedColor);

  if (detectedColor.equals("BILINMEYEN")) {
    // Bilinmeyen renk ise kısa bir bip sesi çal
    Serial.println("Bilinmeyen renk algılandı! Buzzer çalıyor."); // Hata ayıklama için
    digitalWrite(BUZZER_PIN, HIGH); // 1000 Hz frekansında ses üretmeye başla
    delay(150);           // 200 milisaniye bekle (bip süresi)
    digitalWrite(BUZZER_PIN, LOW); // 1000 Hz frekansında ses üretmeye başla
    delay(800); // Toplam gecikmeyi yaklaşık 1 saniyeye tamamlamak için
                // (200ms bip + 800ms bekleme)
  } else {
    // Bilinen renk ise normal bekleme süresi
    delay(1000);
  }
}

// Belirtilen renk filtresini seçip frekansı (pulseIn değeri) okuyan fonksiyon
int readColor(char color) {
  switch (color) {
    case 'r': // Kırmızı Filtre
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      break;
    case 'g': // Yeşil Filtre
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      break;
    case 'b': // Mavi Filtre
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      break;
    default: // Clear (Filtresiz) - İsteğe bağlı
      digitalWrite(S2, HIGH);
      digitalWrite(S3, LOW);
      break;
  }

  // Sensörün seçilen filtreye adapte olması için kısa bekleme
  delayMicroseconds(150);

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
  // --- BURASI KALİBRE EDİLMELİ ---
  // Aşağıdaki değerler SADECE ÖRNEKTİR ve sizin ortamınızda çalışmayabilir!
  // Kalibrasyon yaparken, her renk için R, G, B değerlerini not alın ve
  // bu değerlere göre mantıksal karşılaştırmalar yapın.
  // Unutmayın: Düşük pulseIn değeri = Daha yoğun renk.

  // Örnek Kalibrasyon Değerleri (TAMAMEN TAHMİNİ - KENDİNİZ YAPMALISINIZ!)
  // Kırmızı Örnek: R=30, G=80, B=90
  // Yeşil Örnek:   R=90, G=40, B=85
  // Mavi Örnek:    R=95, G=85, B=50
  // Sarı Örnek:    R=40, G=50, B=95 (R ve G düşük, B yüksek)
  // Turuncu Örnek: R=35, G=60, B=90 (Kırmızıya yakın ama G biraz daha düşük)
  // Kahve Örnek:   R=60, G=75, B=85 (Koyu, tüm değerler orta seviyede olabilir)

  // Örnek Karar Mantığı (Çok Basit - Geliştirilebilir)
  // En düşük değere sahip kanal genellikle en baskın renktir.

   // Kırmızı en baskın (en düşük pulseIn)
   /* if (r < 85 && g > 100 && b > 90) return "KIRMIZI"; // Kırmızı için örnek eşik
    if (r < 80 && g < 98 && b > 80) return "TURUNCU"; // Turuncu için örnek eşik
    // Kahverengi için daha karmaşık bir kontrol gerekebilir (belki R, G, B birbirine yakın ve orta değerlerde?)
    if (r > 80 && r < 110 && g > 80 && g < 103 && b > 80) return "KAHVERENGI"; */
  
     if ((r <= 95 && r >= 85) && (g >= 90 && g <= 100) &&  (b >= 90 && b <= 100)) return "YESIL"; // Yeşil için örnek eşik

  if (b < r && b < g) { // Mavi en baskın
     if (b < 65 && r > 85 && g > 75) return "MAVI"; // Mavi için örnek eşik
  }

   // Sarı (Kırmızı ve Yeşil baskın, Mavi zayıf)
  if (r < 60 && g < 65 && b > 85) {
      return "SARI";
  }



  return "BILINMEYEN"; // Hiçbir koşul eşleşmezse
}