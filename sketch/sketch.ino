#define VERSION ("0.0.1+0")
#define PROTOCOL_VERSION (1)
#define BAUD_RATE (9600)
#define WAIT_DELAY_MS (100)

#define MAGIC_INIT  ('I')
#define MAGIC_MESG  ('M')
#define MAGIC_PING  ('P')
#define MAGIC_RESET ('R')

#define BTN_BEGIN (2)
#define BTN_CNT (7)
#define LOOP_DELAY_MS (1000)

static bool gButtonState[BTN_CNT] = {};

//////////
// Util
//////////

char read_blocking() {
    while (Serial.available() < 1) {
        delay(WAIT_DELAY_MS);
    }
    return Serial.read();
}

//////////
// Protocol
//////////

// true ~ ok, false ~ reset
bool proto_wait(char confirm_magic, void(*before_try)()) {
    bool host_confirm = false;
    while (!host_confirm) {
        before_try();
        char c = read_blocking();
        if (c == MAGIC_RESET) {
            return false;
        }
        host_confirm = (c == confirm_magic);
    }
    return true;
}

void print_motd(bool is_reset) {
    Serial.print("Hello version=");
    Serial.print(VERSION);
    if (is_reset) {
        Serial.print(" after reset");
    }
    Serial.println("");
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
// Main
//////////

void setup() {
    Serial.begin(BAUD_RATE);
    proto_init(false);
}

void _loop_before_try() {
    /* nop */
}

void loop() {
  loop_begin:
    Serial.write(MAGIC_MESG);
    Serial.println("Looped message with delivery confirmation");
    if (!proto_wait(MAGIC_PING, _loop_before_try)) {
        Serial.end();
        Serial.begin(BAUD_RATE);
        proto_init(true);
        goto loop_begin;
    }
    delay(LOOP_DELAY_MS);
}
