/*
   Serial debugging support

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <libraries/BelaArduino/Arduino.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

class Debugger {

    public:

        static void printf(const char * fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            char buf[200];
            vsnprintf(buf, 200, fmt, ap); 
            Serial.print(buf);
            va_end(ap);
        }

        static void reportForever(const char * fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            char buf[200];
            vsnprintf(buf, 200, fmt, ap); 
            va_end(ap);

            strcat(buf, "\n");

            while (true) {
                Serial.print(buf);
                delay(500);
            }
        }

        // for boards that do not support floating-point vnsprintf
        static void printfloat(float val, uint8_t prec=3)
        {
            uint16_t mul = 1;
            for (uint8_t k=0; k<prec; ++k) {
                mul *= 10;
            }
            char sgn = '+';
            if (val < 0) {
                val = -val;
                sgn = '-';
            }
            uint32_t bigval = (uint32_t)(val*mul);
            Debugger::printf("%c%d.%d", sgn, bigval/mul, bigval % mul);
        }

        static void printlnfloat(float val, uint8_t prec=3)
        {
            printfloat(val, prec);
            printf("\n");
        }

        static void checkStatus(uint8_t error, const char * fmt)
        {
            if (error) {
                reportForever(fmt, error);
            }
        }

}; // class Debugger
