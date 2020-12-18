#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN (9)
#define SS_PIN  (10)
#define RC522_INIT_DELAY_MS (20)

MFRC522 g_mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void rc522_setup() {
    SPI.begin();                // Init SPI bus
    g_mfrc522.PCD_Init();       // Init MFRC522
    delay(RC522_INIT_DELAY_MS);
}

// Show details of PCD - MFRC522 Card Reader details
void rc522_dump_version() {
    g_mfrc522.PCD_DumpVersionToSerial();
}

bool rc522_ready() {
    if (! g_mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }
    return g_mfrc522.PICC_ReadCardSerial();
}

// Pre: rc522_ready() returned true
void rc522_dump_uid() {
    byte size = g_mfrc522.uid.size;
    for (byte i = 0; i < size; ++i) {
        Serial.print(g_mfrc522.uid.uidByte[i], HEX);
    }
}
