#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Btn actions ------------------------------------------------------------------
#define PREV '<'
#define NEXT '>'
#define BACK '<'
#define ENTER '0'
// ------------------------------------------------------------------------------

// Keypad init ------------------------------------------------------------------
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'<','0','>'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5};

Keypad kpd = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// ------------------------------------------------------------------------------

// LCD init ---------------------------------------------------------------------
LiquidCrystal_I2C lcd (0x27,16,2);
// ------------------------------------------------------------------------------

enum states {
    HOME_SEL_PAY, HOME_SEL_DEP, HOME_SEL_BAL, HOME_SEL_CH_PIN, PAY, DEP, BAL, CH_PIN, READ_RESP
};
enum states STATE, NEXT_STATE;

bool readCard(char key, char* card, int* currLen) {
    if (key == NEXT && *currLen == 16) {
        return true;
    } else if (*currLen == 16) {
        lcd.setCursor(5, 0);
        lcd.print(" *too long*");
    } else if (key == NEXT) {
        lcd.setCursor(5, 0);
        lcd.print("*too short*");
    } else {
        card[(*currLen)++] = key;
    }

    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
        if (i < *currLen) lcd.print(card[i]);
        else lcd.print(' ');
    }

    return false;
}

char* getCard(const char* msg = "CARD:") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);

    char* card = (char*) malloc(17 * sizeof(char));
    card[16] = '\0';
    int currLen = 0;

    while (1) {
        char innerKey = kpd.getKey();
        if (innerKey == BACK) return nullptr;
        if (innerKey && readCard(innerKey, card, &currLen)) return card;
    }
}

bool readPin(char key, char* pin, int* currLen) {
  if (key == NEXT && *currLen == 4) {
        return true;
    } else if (*currLen == 4) {
        lcd.setCursor(5, 0);
        lcd.print(" *too long*");
    } else if (key == NEXT) {
        lcd.setCursor(5, 0);
        lcd.print("*too short*");
    } else {
        pin[(*currLen)++] = key;
    }

    lcd.setCursor(0, 1);
    for (int i = 0; i < 4; i++) {
        if (i < *currLen) lcd.print('*');
        else lcd.print(' ');
    }

    return false;
}

char* getPin(const char* msg = "PIN:") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);

    char* pin = (char*) malloc(5 * sizeof(char));
    pin[4] = '\0';
    int currLen = 0;

    while (1) {
        char innerKey = kpd.getKey();
        if (innerKey == BACK) return nullptr;
        if (innerKey && readPin(innerKey, pin, &currLen)) return pin;
    }
}

bool readAmount(char key, char* amount, int* currLen, const char* prefix) {
    if (key == NEXT) {
        return true;
    } else if (*currLen == 6) {
        lcd.setCursor(6, 0);
        lcd.print("*too long*");
    } else {
        if (*currLen != 0 || key != '0') amount[(*currLen)++] = key;
    }

    lcd.setCursor(0, 1);
    lcd.print(prefix);
    for (int i = 0; i < 6; i++) {
        if (i < *currLen) lcd.print(amount[i]);
        else lcd.print(' ');
    }

    return false;
}

char* getAmount(const char* msg = "Amnt:", const char* prefix = "CZK ") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lcd.setCursor(0, 1);
    lcd.print(prefix);

    char* amount = (char*) malloc(7 * sizeof(char));
    int currLen = 0;

    while (1) {
        char innerKey = kpd.getKey();
        if (innerKey == BACK) return nullptr;
        if (innerKey && readAmount(innerKey, amount, &currLen, prefix) && currLen != 0) {
            amount[currLen] = '\0';
            return amount;
        }
    }
}

bool sendDataToSerial(const char* type, bool withNewPin = false, bool withAmount = false) {
    char* c = getCard();
    if (!c) return false;

    char* p = getPin();
    if (!p) return false;

    char* np = nullptr;
    if (withNewPin) {
        np = getPin("NewP:");
        if (!np) return false;
    }

    char* a = nullptr;
    if (withAmount) {
        a = getAmount();
        if (!a) return false;
    }

    Serial.print(type);
    Serial.print(",");
    Serial.print(c);
    Serial.print(",");
    Serial.print(p);
    if (withNewPin) {
        Serial.print(",");
        Serial.print(np);
        free(np);
    }
    if (withAmount) {
        Serial.print(",");
        Serial.print(a);
        free(a);
    }
    Serial.println();

    free(c);
    free(p);

    return true;
}

void menu(int choice) {
  switch (choice) {
    case 0:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Waiting for");
        lcd.setCursor(0,1);
        lcd.print("response ...");
        break;
    case 1:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("> Pay");
        lcd.setCursor(0,1);
        lcd.print("Deposit");
        break;
    case 2:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("> Deposit");
        lcd.setCursor(0,1);
        lcd.print("Balance");
        break;
    case 3:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("> Balance");
        lcd.setCursor(0,1);
        lcd.print("Change pin");
        break;
    case 4:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("> Change pin");
        lcd.setCursor(0,1);
        lcd.print("Pay");
        break;
  }
}

void changeState(char key) {
    switch (STATE) {
        case HOME_SEL_PAY:
            if (key == NEXT) {
                menu(2);
                NEXT_STATE = HOME_SEL_DEP;
            } else if (key == PREV) {
                menu(4);
                NEXT_STATE = HOME_SEL_CH_PIN;
            } else if (key == ENTER) {
                NEXT_STATE = PAY;
            }
            break;

        case HOME_SEL_DEP:
            if (key == NEXT) {
                menu(3);
                NEXT_STATE = HOME_SEL_BAL;
            } else if (key == PREV) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else if (key == ENTER) {
                NEXT_STATE = DEP;
            }
            break;

        case HOME_SEL_BAL:
            if (key == NEXT) {
                menu(4);
                NEXT_STATE = HOME_SEL_CH_PIN;
            } else if (key == PREV) {
                menu(2);
                NEXT_STATE = HOME_SEL_DEP;
            } else if (key == ENTER) {
                NEXT_STATE = BAL;
            }
            break;

        case HOME_SEL_CH_PIN:
            if (key == NEXT) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else if (key == PREV) {
                menu(3);
                NEXT_STATE = HOME_SEL_BAL;
            } else if (key == ENTER) {
                NEXT_STATE = CH_PIN;
            }
            break;

        case PAY:
            if(key == BACK) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else {
                if (!sendDataToSerial("pay", false, true)) {
                    menu(1);
                    NEXT_STATE = HOME_SEL_PAY;
                    break;
                }

                menu(0);
                NEXT_STATE = READ_RESP;
            }
            break;

        case DEP:
            if (key == BACK) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else {
                if (!sendDataToSerial("dep", false, true)) {
                    menu(1);
                    NEXT_STATE = HOME_SEL_PAY;
                    break;
                }

                menu(0);
                NEXT_STATE = READ_RESP;
            }
            break;

        case BAL:
            if (key == BACK) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else {
                if (!sendDataToSerial("bal")) {
                    menu(1);
                    NEXT_STATE = HOME_SEL_PAY;
                    break;
                }

                menu(0);
                NEXT_STATE = READ_RESP;
            }
            break;

        case CH_PIN:
            if (key == BACK) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else {
                if (!sendDataToSerial("ch_pin", true)) {
                    menu(1);
                    NEXT_STATE = HOME_SEL_PAY;
                    break;
                }

                menu(0);
                NEXT_STATE = READ_RESP;
            }
            break;
        case READ_RESP:
            if (key == BACK) {
                menu(1);
                NEXT_STATE = HOME_SEL_PAY;
            } else {
                // TODO: read serial
            }
            break;
    }
    STATE = NEXT_STATE;
}

void setup() {
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    menu(1);
    STATE = HOME_SEL_PAY;
}

void loop(){
    char key = kpd.getKey();
    changeState(key);
}
