/* #############################################################

timed2812.cpp version 0.2

        Purpose(s) of this version
        	Multiple LEDs support
                Increase stability
        Changelog
                N.A.
        Requirements
                Raspberry PI 5
                24 WS2812B chip connected to Raspberry PI

c0bra software - 22-09-2024

############################################################# */

#include <wiringPi.h>
#include <iostream>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

#define LED_PIN 1  // WiringPi pin 1 (GPIO18)
#define T1H 680    // Ideal time 800
#define T1L 310    // Ideal time 450
#define T0H 300    // Ideal time 400
#define T0L 750    // Ideal time 850
#define TCOMP 5000 // RP1 wake up time in nanoseconds
#define RESET_TIME 50  // Reset time in microseconds
#define LED_NUMBER 24  // Number of LEDs
#define RAINBOW_DELAY 500 // 50ms delay between updates


uint32_t color_matrix[LED_NUMBER];
uint32_t colorWheel(uint8_t pos);

class Timer {
public:
    void start() {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }

    long elapsed() {
        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        return (end_time.tv_sec - start_time.tv_sec) * 1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    }
private:
    struct timespec start_time;
};

Timer timer;
void sendBit(bool bit);
void sendColors(uint32_t* colors);
uint32_t colorWheel(uint8_t pos);


int main() {
    // Initialize WiringPi and set the pin mode
    if (wiringPiSetup() == -1) {
        std::cerr << "WiringPi setup failed!" << std::endl;
        return 1;
    }
    pinMode(LED_PIN, OUTPUT);


    uint8_t colorPos = 0;
    while (true) {
        for (int i = 0; i < LED_NUMBER; i++) {
            color_matrix[i] = colorWheel((colorPos + (i * 256 / LED_NUMBER)) & 255);
        }
        sendColors(color_matrix);
        colorPos++;
        usleep(RAINBOW_DELAY); // Delay for the rainbow effect
    }
    return 0;
}

void sendBit(bool bit) {

    if (bit) {
        // Send '1'
        digitalWrite(LED_PIN, HIGH);
        timer.start();
        while (timer.elapsed() < T1H); // High time for '1'
        digitalWrite(LED_PIN, LOW);
        timer.start();
        while (timer.elapsed() < T1L); // Low time for '1'
    } else {
        // Send '0'
        digitalWrite(LED_PIN, HIGH);
        timer.start();
        while (timer.elapsed() < T0H); // High time for '0'
        digitalWrite(LED_PIN, LOW);
        timer.start();
        while (timer.elapsed() < T0L); // Low time for '0'
    }
}


void sendColors(uint32_t *colors) {
    digitalWrite(LED_PIN, HIGH);     // Changed the way WAKEUP occurs. Now a single 20ns pulse get generated, then we wait. 
    digitalWrite(LED_PIN, LOW);	     //  This pulse is so fast it gets ignored by the WS2812B and ensure accurate timings.
    timer.start();
    while (timer.elapsed() < TCOMP); // Wait for the RP1 to wake up before sending data

    for (int j = 0; j < LED_NUMBER; j++) {
        for (int i = 23; i >= 0; i--) {
            sendBit((colors[j] >> i) & 1);
        }
    }

    // Ensure reset time
    usleep(RESET_TIME);
}

uint32_t colorWheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        return ((255 - pos * 3) << 16) | (0 << 8) | (pos * 3);
    }
    if (pos < 170) {
        pos -= 85;
        return ((0 << 16) | ((pos * 3) << 8) | (255 - pos * 3));
    }
    pos -= 170;
    return (((pos * 3) << 16) | ((255 - pos * 3) << 8) | 0);
}
