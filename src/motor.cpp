#include <Arduino.h>

class Motor {
    private:
        bool reversed = false;
    public:
        Motor(uint8_t pinIN1, uint8_t pinIN2, uint8_t pinEN) : pinIN1(pinIN1), pinIN2(pinIN2), pinEN(pinEN) {
            pinMode(pinIN1, OUTPUT);
            pinMode(pinIN2, OUTPUT);
            pinMode(pinEN, OUTPUT);

            digitalWrite(pinEN, LOW);
        };
        // power is from -255 to 255
        // used int instead of int8_t due to range
        void setPower(int power) {
            if (reversed) {
                power = -power;
            }
            if (power >= 0) {
                digitalWrite(pinIN1, HIGH);
                digitalWrite(pinIN2, LOW);

                analogWrite(pinEN, power);
            } else {
                digitalWrite(pinIN1, LOW);
                digitalWrite(pinIN2, HIGH);

                analogWrite(pinEN, -power);
            }
        };
        void brake() {
            digitalWrite(pinIN1, LOW);
            digitalWrite(pinIN2, LOW);
            digitalWrite(pinEN, HIGH);
        };
        void freeRun() {
            digitalWrite(pinEN, LOW);
        };
        void setReversed(bool r) {
            reversed = r;
        }
    private:
        uint8_t pinIN1, pinIN2, pinEN;
};