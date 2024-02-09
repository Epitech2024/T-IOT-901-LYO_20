#ifndef THE_CONVOYER_NFC_H
#define THE_CONVOYER_NFC_H
#include "MFRC522_I2C.h"

void initNfc();

bool isTagPresence();

String getTagUuid();

#endif //THE_CONVOYER_NFC_H
