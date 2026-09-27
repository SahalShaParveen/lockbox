#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

const byte SS_PIN = 5;
const byte RST_PIN = 22;
const byte SCK_PIN = 18;
const byte MOSI_PIN = 23;
const byte MISO_PIN = 19; 


const byte LED_PIN    = 2;
const byte SERVO_PIN  = 13;
const byte BUZZER_PIN = 27;


MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

const byte KEY[] = {0x7E, 0xF6, 0x51, 0x05};


void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lockServo.attach(SERVO_PIN);
  lockServo.write(0);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
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

  if (rfid.uid.size != sizeof(KEY)) {
    authorised = false;
  } 
  else {
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] != KEY[i]) {
        authorised = false;
        break;
      }
    }
  }


  // =========================
  // AUTHORISED
  // =========================

  if (authorised) {

    Serial.println("ACCESS GRANTED");

    // One long beep
    beep(500);

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

    // Stay unlocked
    delay(3000);

    // Lock again
    Serial.println("Locking...");
    lockServo.write(0);
  }


  // =========================
  // UNAUTHORISED
  // =========================

  else {

    Serial.println("ACCESS DENIED");

    // Three short beeps
    for (int i = 0; i < 3; i++) {
      beep(150);
      delay(100);
    }
  }


  // Stop communication with card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Prevent immediate retriggering
  delay(1000);
}

void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}
