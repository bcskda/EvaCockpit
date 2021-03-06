#include "rc522.h"
#include "KeyboardDriver.h"

#define VERSION ("0.0.1+2")
#define PROTOCOL_VERSION (1)
#define BAUD_RATE (115200)
#define WAIT_DELAY_US (10)

#define MAGIC_CMD   ('C')
#define MAGIC_INIT  ('I')
#define MAGIC_MESG  ('M')
#define MAGIC_PING  ('P')
#define MAGIC_RESET ('R')

#define BTN_BEGIN (2)
#define BTN_CNT (7)
#define BTN_TIMEOUT_LOOPS (20)
#define LOOP_DELAY_MS (100)

//////////
// Util
//////////

char read_blocking() { // TODO timeout in read_blocking
    while (Serial.available() < 1) {
        delayMicroseconds(WAIT_DELAY_US);
    }
    return Serial.read();
}

//////////
// Protocol
//////////

// true ~ ok, false ~ reset
// timeout = iteration count, 0 ~ wait forever
bool proto_wait(char confirm_magic, void(*before_try)(), int timeout = 0) {
    bool host_confirm = false;
    int iter = 0;
    while (!host_confirm && ((timeout == 0) || (iter < timeout))) {
        before_try();
        char c = read_blocking(); // TODO timeout in read_blocking
        if (c == MAGIC_RESET) {
            return false;
        }
        host_confirm = (c == confirm_magic);
        ++iter;
    }
    return true;
}

void print_motd(bool is_reset) {
    Serial.print("Hello version=");
    Serial.print(VERSION);
    if (is_reset) {
        Serial.print(" after reset");
    }
    Serial.print("; MFRC522 info: ");
    rc522_dump_version();
}

void _proto_init_before_try() {
    Serial.print(MAGIC_INIT);
    Serial.println(PROTOCOL_VERSION);
}

void proto_init(bool is_reset) {
    while (!proto_wait(MAGIC_INIT, _proto_init_before_try)) {
        /* received reset; retry */
    }
    print_motd(is_reset);
}

//////////
// Buttons
//////////

static bool gButtonState[BTN_CNT] = {};

// true ~ ok, false ~ error
bool update_buttons() {
    for (int i = 0; i < BTN_CNT; ++i) {
        bool pressed = (digitalRead(BTN_BEGIN + i) == HIGH);
        if (pressed && !gButtonState[i]) {
            Serial.print(MAGIC_CMD);
            Serial.println(i);
            if (!proto_wait(MAGIC_PING, _loop_nop, BTN_TIMEOUT_LOOPS)) {
                return false;
            }
        }
        gButtonState[i] = pressed;
    }
    return true;
}

//////////
// RFID
//////////

bool update_rfid() {
    if (rc522_ready()) {
        Serial.print(MAGIC_MESG);
        rc522_dump_uid();
        return proto_wait(MAGIC_PING, _loop_nop, BTN_TIMEOUT_LOOPS);
    } else {
        return true;
    }
}

//////////
// Keyboard
//////////

void kbd_callback(bool is_pressed, uint8_t scan_code) noexcept {
    Serial.print(MAGIC_MESG);
    if (is_pressed) {
        Serial.print("Scan code up: ");
    } else {
        Serial.print("Scan code down: ");
    }
    Serial.println(KeyboardDriver::toChar(scan_code));
    if (!proto_wait(MAGIC_PING, _loop_nop, BTN_TIMEOUT_LOOPS)) {
        reset();
    }
}

//////////
// Main
//////////

void reset_buttons() {
    for (int i = 0; i < BTN_CNT; ++i) {
        gButtonState[i] = 0;
    }
}

void setup() {
    KeyboardDriver::init();
    Serial.begin(BAUD_RATE);
    rc522_setup();
    proto_init(false);
    reset_buttons();
}

void reset() {
    Serial.end();
    Serial.begin(BAUD_RATE);
    proto_init(true);
    reset_buttons();
    KeyboardDriver::init();
}

// true ~ reset requested
bool check_reset() {
    if (Serial.available() > 0) {
        return Serial.peek() == MAGIC_RESET;
    }
    else {
        return false;
    }
}

void _loop_nop() {
    /* nop */
}

void loop() {
    if (check_reset()) {
        reset();
    }
    if (!update_buttons()) {
        reset();
    }
    if (!update_rfid()) {
        reset();
    }*/
    KeyboardDriver::loopOnce(kbd_callback);
    // delay(LOOP_DELAY_MS);
    // delayMicroseconds(50);
}
