#include <Arduino.h>
#include <M5Stack.h>
#include "servo.h"
#include "nfc.h"
#include "connexion.h"
#include "request.h"
#include <Module_GRBL_13.2.h>
#include <Wire.h>
#include "queue"
#include "UUID.h"

#define STEPMOTOR_I2C_ADDR 0x70

Module_GRBL _GRBL = Module_GRBL(STEPMOTOR_I2C_ADDR);
int revolution = 20;
unsigned long motorMilli = millis();
unsigned long stockMilli = millis();
unsigned long lastCheck = millis();
int speed = 180;

struct Coli {
    String uuid;
    unsigned long coliMillis;
    int wareHouseId;
};

struct StockMovement{
    int productId;
    int wareHouseId;
};

std::deque<Coli> coliQueue;
std::deque<StockMovement> stockMovementQueue;

void pauseMotor() {
    _GRBL.sendGcode("!");
}

void resumeMotor() {
    _GRBL.sendGcode("~");
}


UUID uid;


void setup() {
    M5.begin();
    M5.Power.begin();
    M5.Lcd.setTextSize(2);
    Wire.begin();
    _GRBL.Init(&Wire);
    Serial.begin(115200);
    SetServoToA();
    initNfc();
    initWifi();
    _GRBL.setMode("absolute");
    _GRBL.setMotor(revolution, 0, 0, speed);
    int pb = 0;
    M5.Lcd.clear();
    while (pb <= 5) {
        delay(1000);
        M5.Lcd.progressBar(50,100,200,40, 20 * pb);
        pb++;
    }
    M5.Lcd.clear();
}

void loop() {

    if(isTagPresence()) {

        unsigned long currentLastCheck = millis() - lastCheck;

        if(currentLastCheck < 2000) {
            unsigned long before = millis();
            pauseMotor();
            M5.Lcd.clear();
            M5.Lcd.println("LastCheck: " + String(currentLastCheck));
            M5.Lcd.setCursor(0,0);
            M5.Lcd.println("Veuillez retirer le coli et appuyez sur le boutton A pour continuer!");
            while (M5.BtnA.isReleased()) {
                M5.update();
            }
            unsigned long after = millis();
            unsigned long diff = after - before;
            for (auto & i : coliQueue) {
                i.coliMillis = i.coliMillis + diff;
            }
            resumeMotor();
            lastCheck = millis() - currentLastCheck;
            M5.Lcd.clear();
        } else {
            Coli newColi;
            String uuid = getTagUuid();
            newColi.uuid = uuid;
            JSONVar object = getProductByRef(uuid);

            int id;
            int wareHouseId;

            if(object.hasOwnProperty("error")) {
                if((int)object["error"]["code"] == 404) {
                    unsigned long before = millis();
                    pauseMotor();
                    bool noChoice = true;
                    M5.Lcd.clear();
                    M5.Lcd.setCursor(0,0);
                    M5.Lcd.println("Ce produit n'existe pas encore. Ajoutez le avec les bouttons suivants:");
                    M5.Lcd.println("A => WareHouse 1");
                    M5.Lcd.println("B => WareHouse 2");
                    M5.Lcd.println("C => WareHouse 3");
                    uid.generate();
                    while (noChoice) {
                        if(M5.BtnA.isPressed())
                        {
                            JSONVar response = createProduct(uuid, uid.toCharArray(), 1);
                            if(!response.hasOwnProperty("error")) {
                                id= int(response);
                                wareHouseId = 1;
                            }
                            noChoice = false;
                            resumeMotor();
                        } else if (M5.BtnB.isPressed()) {
                            JSONVar response = createProduct(uuid, uid.toCharArray(), 2);
                            if(!response.hasOwnProperty("error")) {
                                id= int(response);
                                wareHouseId = 2;
                            }
                            noChoice = false;
                            resumeMotor();
                        } else if (M5.BtnC.isPressed()) {
                            JSONVar response = createProduct(uuid, uid.toCharArray(), 3);
                            if(!response.hasOwnProperty("error")) {
                                id= int(response);
                                wareHouseId = 3;
                            }
                            noChoice = false;
                            resumeMotor();
                        }
                        M5.update();
                    }
                    unsigned long after = millis();
                    unsigned long diff = after - before;
                    for (auto & i : coliQueue) {
                        i.coliMillis = i.coliMillis + diff;
                    }
                }
            } else {
                id = String((const char*)object["id"]).toInt();
                wareHouseId = String((const char*)object["fk_default_warehouse"]).toInt();
            }

            newColi.coliMillis = millis() + 4000;

            newColi.wareHouseId = wareHouseId;
            coliQueue.push_back(newColi);
            M5.Lcd.clear();
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.println("Coli added: ");
            M5.Lcd.println((String)id + " - wid=" + newColi.wareHouseId);


            StockMovement stockMovement;
            stockMovement.productId = id;
            stockMovement.wareHouseId = wareHouseId;
            stockMovementQueue.push_back(stockMovement);
            M5.Lcd.println("**************");
            M5.Lcd.println("Stock movement added!");

            lastCheck = millis();
        }
    }

    if(!coliQueue.empty()) {
        Coli toStock = coliQueue.front();
        if(millis() >= toStock.coliMillis) {
            SetServoByWareHouse(toStock.wareHouseId);
            coliQueue.pop_front();
            M5.Lcd.clear();
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.println("Servo switch!");
        }
    }

    if(millis() - motorMilli > 8000) {
        revolution += revolution;
        _GRBL.setMotor(revolution, 0, 0, speed);
        motorMilli = millis();
    }

    if(millis() - stockMilli > 20000) {
        if(!stockMovementQueue.empty()) {
            StockMovement toMove = stockMovementQueue.front();
            String stockMovementId = createStockMovements(toMove.productId, toMove.wareHouseId);
            stockMovementQueue.pop_front();
            stockMilli = millis();
            M5.Lcd.clear();
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.println("Stock movement pushed: " + stockMovementId);
        }
        stockMilli = millis();
    }

    M5.update();
}