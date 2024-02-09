#include "nfc.h"
MFRC522 mfrc522(0x28);

void initNfc() {
    //Wire.begin();
    mfrc522.PCD_Init();
}

bool isTagPresence() {
    if (!mfrc522.PICC_IsNewCardPresent() ||
        !mfrc522.PICC_ReadCardSerial()) {
        delay(200);
        return false;
    }

    return true;
}

String getTagUuid() {
    String uuid = "";
    for (byte i = 0; i < mfrc522.uid.size;
         i++) {
        if(i != 0) {
            uuid += "_";
        }

        if(mfrc522.uid.uidByte[i] < 0x10) {
            uuid += "0";
        }

        uuid += String(mfrc522.uid.uidByte[i], HEX);
        uuid.toUpperCase();
    }
    return uuid;
}
