#include "KeyboardDriver.h"
#include "config.h"

#define KBD_MASK_KEY_UP (1 << 7)

extern const char* ScanCodeMap[];

volatile uint8_t KeyboardDriver::running_; // atomic?? 1-byte

void KeyboardDriver::init() {
    pinMode(KBD_DATA_PIN, INPUT);
    DIDR0 = 0x01; // Digital Input Disable Register
    ADCSRA = (ADCSRA & 0xf8) | 0x04; // ADC 1/16 frequency divisor
    running_ = false;
}

uint8_t KeyboardDriver::readPulseSeries(uint8_t* widths, uint8_t max_pulses) {
    uint8_t pulse = 0;
    bool was_high = true;
    while (pulse < max_pulses) {
        uint8_t iter = 0;
        while ((iter < KBD_TIMEOUT_PROBES) &&
            ((analogRead(KBD_DATA_PIN) < KBD_SIG_THRESH) == was_high))
        {
            delayMicroseconds(KBD_PROBE_US);
            ++iter;
        }
        if (iter < KBD_TIMEOUT_PROBES) {
            was_high = !was_high;
            widths[pulse++] = iter;
        } else {
            break;
        }
    }
    if (pulse == 0) {
        return 0;
    }
    // Serial.print("Series:");
    for (int i = 0; i < pulse; ++i) {
        widths[i] = round(widths[i] / KBD_DURATION_DIV_RATIO);
    //     Serial.print(" ");
    //     Serial.print(widths[i]);
    }
    // Serial.println();
    return pulse;
}

// Returns 0 if no result
inline
uint16_t KeyboardDriver::readScanCode() {
    uint8_t durations[16] = {};
    uint8_t pulse_cnt = readPulseSeries(durations, 16);
    uint16_t scan_code = 0;
    return toScanCode16(durations, pulse_cnt);
}

// MSB = first bit read
// Low = 1, high = 0
inline
uint16_t KeyboardDriver::toScanCode16(uint8_t* durs, uint8_t pulse_cnt) {
    uint16_t scan_code = 0;
    uint16_t pulse_left = 16;
    bool high = false;
    for (uint8_t pulse = 0; pulse < pulse_cnt; ++pulse) {
        scan_code <<= durs[pulse]; // shl dur bits
        if (!high) {
            // set dur lowest bits
            scan_code |= (((uint16_t)1 << durs[pulse]) - (uint16_t)1);
        }
        high = !high;
        pulse_left -= durs[pulse];
    }
    scan_code <<= pulse_left;
    return scan_code;
}

const char* KeyboardDriver::toChar(uint8_t scan_code) {
    return ScanCodeMap[scan_code];
}

// Returns 0 if no result
// MSB = first bit read
uint16_t KeyboardDriver::loopOnce() {
    micros();
    if (analogRead(KBD_DATA_PIN) < KBD_SIG_THRESH) {
        return readScanCode();
    } else {
        return 0;
    }
}

// No call if no result
// MSB = first bit read
void KeyboardDriver::loopOnce(Callback cb) {
    uint16_t sc16 = loopOnce();
    if (sc16) {
        cb(sc16 & KBD_MASK_KEY_UP, sc16 >> 8);
    }
}
