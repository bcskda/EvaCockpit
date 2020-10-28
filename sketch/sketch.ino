#define VERSION ("0.0.1+0")
#define BAUD_RATE (9600)

#define PROTOCOL_VERSION (1)
#define HOST_CONFIRM_MAGIC ('O')
#define INIT_DELAY_MS (500)

#define BTN_BEGIN (2)
#define BTN_CNT (7)
#define LOOP_DELAY_MS (1000)

static bool gButtonState[BTN_CNT] = {};

void proto_init() {
    bool host_confirm = false;
    while (!host_confirm) {
        Serial.print("protocol ");
        Serial.println(PROTOCOL_VERSION);
        while (Serial.available() < 1) {
            delay(INIT_DELAY_MS);
        }
        host_confirm = (Serial.read() == HOST_CONFIRM_MAGIC);
    }
    Serial.print("Hello version=");
    Serial.println(VERSION);
}

void setup() {
    Serial.begin(BAUD_RATE);
    proto_init();
}

void loop() {
    delay(LOOP_DELAY_MS);
    Serial.println("Empty loop");
}
