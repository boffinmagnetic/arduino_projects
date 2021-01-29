/*
  Required libraries:
    ssd1306   by Alexey Dynda
*/

#include "ssd1306.h"
#include <stdio.h>
#include "tones.h"

#define BTN_RESET 3
// notes in the melody:
int melody[] = {

  NOTE_C4, NOTE_G3
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8
};

void setup()
{
    /* Select the font to use with menu and all font functions */
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    sh1106_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    analogReference(DEFAULT);

    ssd1306_flipHorizontal(1);  /* rotate screen in X */
    ssd1306_flipVertical(1);    /* rotate screen in Y */

    pinMode(BTN_RESET, INPUT_PULLUP);
}

// based on Tone example
void playTone(void){
  for (int thisNote = 0; thisNote < 2; thisNote++) {
    int noteDuration = 600 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8);
  }
}

void loop()
{
    static int count = 0;
    char buffer[15];
    //ssd1306_fillScreen( 0x00 );
    char str_temp[8];
    long average = 0;
    static char lastStatus = 0;

    for(int i=0; i<16; i++){
      average += analogRead(A2);
      delay(5);
    }

    average >>= 4;

    if(average < 900){
      if(!lastStatus){
        ++count;
          playTone();
      }
      lastStatus = 1;
    }
    else{
      lastStatus = 0;
    }

    if(!digitalRead(BTN_RESET))
      count = 0;
      
    sprintf(buffer, "%6d", count);

    ssd1306_printFixedN(30, 10, "Counts", STYLE_NORMAL, FONT_SIZE_2X);
    ssd1306_printFixedN(30, 35, buffer, STYLE_NORMAL, FONT_SIZE_2X);
    delay(10);
}
