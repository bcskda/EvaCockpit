#pragma once

class KeyboardDriver {
  public:
    // void callback(bool is_press, uint8_t scan_code)
    /// press or release
    using Callback = void(*)(bool, uint16_t) noexcept;

  public:
    static void init();

    // static bool isRunning();

    // static void requestStop();

    // static void requestReset();

    // Returns 0 if no result
    // MSB = first bit read
    static uint16_t loopOnce();

    // No call if no result
    // MSB = first bit read
    static void loopOnce(Callback cb);

    // static void loopUntilStopped(Callback cb);

    static const char* toChar(uint8_t scan_code);

  private:
    static inline bool isReadHigh();

    static inline uint8_t readPulseSeries(uint8_t* durs, uint8_t max_pulses);

    // Returns 0 if no result
    // MSB = first bit read
    static inline uint16_t readScanCode();

    // MSB = first bit read
    static inline uint16_t toScanCode16(uint8_t* durs, uint8_t pulse_cnt);

  private:
    static volatile uint8_t running_; // atomic?? 1-byte
};
