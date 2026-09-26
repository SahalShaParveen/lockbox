#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5
#define RST_PIN 22
#define LED_PIN  2

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  Serial.println("RFID reader ready.");
}

void loop() {

  // Wait for a card/fob
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID
  Serial.print("UID:");

  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(" ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }

  Serial.println();

  // Blink LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(150);

    digitalWrite(LED_PIN, LOW);
    delay(150);
  }

  // Stop communicating with the fob
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(500);
}