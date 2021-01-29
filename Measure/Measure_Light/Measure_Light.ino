/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 *   Attiny85 PINS (i2c)
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *
 *   Nano/Atmega328 PINS: connect LCD to A4/A5 (i2c)
 *   ESP8266: GPIO4(SDA) / GPIO5( SCL )
 *   STM32: B7(SDA), B6(SCL)
 */

#include "ssd1306.h"
#include "nano_gfx.h"

void setup()
{
    /* Select the font to use with menu and all font functions */
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    sh1106_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    analogReference(DEFAULT);

    ssd1306_flipHorizontal(1);  /* rotate screen in X */
    ssd1306_flipVertical(1);    /* rotate screen in Y */
}

void loop()
{
    char buffer[15];
    //ssd1306_fillScreen( 0x00 );
    char str_temp[8];
    long average = 0;

    for(int i=0; i<16; i++){
      average += analogRead(A2);
      delay(5);
    }

    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    dtostrf(3.054e-4 * (float) average, 5, 2, str_temp);

    ssd1306_printFixedN(35, 10, "LED V", STYLE_NORMAL, FONT_SIZE_2X);
    ssd1306_printFixedN(95, 20, "f", STYLE_NORMAL, FONT_SIZE_NORMAL);
    sprintf(buffer, "%s V ", str_temp);
    ssd1306_printFixedN(20, 35, buffer, STYLE_NORMAL, FONT_SIZE_2X);
    delay(100);
}
