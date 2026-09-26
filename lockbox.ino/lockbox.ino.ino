#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define SS_PIN    5
#define RST_PIN   22
#define LED_PIN   2
#define SERVO_PIN 13

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Set up servo
  lockServo.attach(SERVO_PIN);
  lockServo.write(0);       // Locked position

  // Set up RC522
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  Serial.println("RFID lock ready.");
}

void loop() {

  // No card detected
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Card detected, but couldn't read it
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID
  Serial.print("Card detected! UID:");

  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(" ");
    
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
  }

  Serial.println();

  // Blink LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }

  // Unlock
  Serial.println("Unlocking...");
  lockServo.write(90);

  // Stay unlocked for 3 seconds
  delay(3000);

  // Lock again
  Serial.println("Locking...");
  lockServo.write(0);

  // Finish communication with card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Prevent the same card from immediately triggering again
  delay(1000);
}