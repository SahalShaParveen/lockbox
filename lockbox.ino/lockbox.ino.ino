#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define SS_PIN    5
#define RST_PIN   22
#define LED_PIN   2
#define SERVO_PIN 13

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

// Authorised card UID
byte authorisedUID[] = {0x7E, 0xF6, 0x51, 0x05};

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Servo
  lockServo.attach(SERVO_PIN);
  lockServo.write(0);

  // RC522
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  Serial.println("RFID lock ready.");
}

void loop() {

  // Wait for a card
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Try to read the card
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

  // Check UID
  bool authorised = true;

  if (rfid.uid.size != sizeof(authorisedUID)) {
    authorised = false;
  } else {
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] != authorisedUID[i]) {
        authorised = false;
        break;
      }
    }
  }

  if (authorised) {

    Serial.println("ACCESS GRANTED");

    // Blink LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(150);
      digitalWrite(LED_PIN, LOW);
      delay(150);
    }

    // Unlock
    Serial.println("Unlocking...");
    lockServo.write(90);

    // Stay unlocked for 3 seconds
    delay(3000);

    // Lock again
    Serial.println("Locking...");
    lockServo.write(0);

  } else {

    Serial.println("ACCESS DENIED");

  }

  // Stop communicating with the card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Prevent immediate retriggering
  delay(1000);
}