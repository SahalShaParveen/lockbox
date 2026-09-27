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

bool readCard();
void printUID();
bool authorise();

void grantAccess();
void denyAccess();

void unlock();
void lock();

void beep(int duration);
void stopCardCommunication();


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
  if (!readCard()){
    return;
  }

  printUID();
  bool authorised = authorise();

  if (authorised) {
    grantAccess(); 
  }
  else {
    denyAccess();
  }

  stopCardCommunication();
  delay(1000);
}


void grantAccess(){
  Serial.println("ACCESS GRANTED");  
  unlock(); 
  delay(3000); 
  lock();
}


void denyAccess(){
  Serial.println("ACCESS DENIED");

  for (int i = 0; i < 3; i++) {
    beep(100);
    delay(50);
  }

  lock();
}


void unlock(){
  digitalWrite(LED_PIN, HIGH);
  beep(500);

  lockServo.write(90); 
}  


void lock(){
  lockServo.write(0);

  beep(100);
  digitalWrite(LED_PIN, LOW);
}


bool readCard(){
    if (!rfid.PICC_IsNewCardPresent()){
        return false;
    }

    if (!rfid.PICC_ReadCardSerial()){
        return false;
    }

    return true;
}


void printUID(){
    Serial.print("UID:");

    for (byte i = 0; i < rfid.uid.size; i++){
        Serial.print(" ");

        if (rfid.uid.uidByte[i] < 0x10){
            Serial.print("0");
        }

        Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
}


bool authorise(){
    if (rfid.uid.size != sizeof(KEY)){
        return false;
    }

    for (byte i = 0; i < rfid.uid.size; i++){
        if (rfid.uid.uidByte[i] != KEY[i]){
            return false;
        }
    }

    return true;
}


void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}


void stopCardCommunication(){
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
} 