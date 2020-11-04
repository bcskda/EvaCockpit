#define VERSION ("0.0.1+1")
#define PROTOCOL_VERSION (1)
#define BAUD_RATE (9600)
#define WAIT_DELAY_MS (10)

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
        delay(WAIT_DELAY_MS);
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
// Main
//////////

void reset_buttons() {
    for (int i = 0; i < BTN_CNT; ++i) {
        gButtonState[i] = 0;
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    proto_init(false);
    reset_buttons();
}

void reset() {
    Serial.end();
    Serial.begin(BAUD_RATE);
    proto_init(true);
    reset_buttons();
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
    delay(LOOP_DELAY_MS);
}
