/* #############################################################

timed2812.cpp version 0.1

        Purpose(s) of this version
				Drive WS2812B LED with Raspberry PI 5
                Test I_O speed and capabilities 
        Changelog
                N.A.
        Requirements
                Raspberry PI 5
                WS2812B chip connected to Raspberry PI

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
#define TCOMP 3500 // RP1 wake up time in nanoseconds
#define RESET_TIME 50000  // Reset time in microseconds

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
void sendColor(uint32_t color);


int main() {
    // Initialize WiringPi and set the pin mode
    if (wiringPiSetup() == -1) {
        std::cerr << "WiringPi setup failed!" << std::endl;
        return 1;
    }
    pinMode(LED_PIN, OUTPUT);

   while (true) {
        // Send color data to WS2812B (for example, red color)
        sendColor(0xFF0000);  // Red
        usleep(500000);  // 500ms delay

        sendColor(0x00FF00);  // Green
        usleep(500000);  // 500ms delay

        sendColor(0x0000FF);  // Blue
        usleep(500000);  // 500ms delay

        sendColor(0x000000);  // Off
        usleep(500000);  // 500ms delay

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



void sendColor(uint32_t color) {

    digitalWrite(LED_PIN, HIGH);
    timer.start();
    while (timer.elapsed() < TCOMP); // Wait for the RP1 to wake up before sending data

    for (int i = 23; i >= 0; i--) {
        sendBit((color >> i) & 1);
    }

    // Ensure reset time
    usleep(RESET_TIME);
}
