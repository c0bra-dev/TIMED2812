# TIMED2812
Raspberry PI 5 very basic C++ code to interact with WS2812B LED on normal GPIO through RP1 chip and the use of basic timers.

Timers are adjusted through the use of an oscilloscope to match measured times.
Code seems more stable than I expected.


Requires wiringPi. Install with

sudo apt update
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
sudo ./build


Compile this code: 
g++ -o timed2812 timed2812.cpp -lwiringPi

and run:
sudo ./timed2812

