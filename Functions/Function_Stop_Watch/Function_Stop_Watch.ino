/*
  Required libraries:
    ssd1306   by Alexey Dynda
    TimerOne  by Jesse Tane
*/


#include "ssd1306.h"
#include <stdio.h>
#include "tones.h"
#include "TimerOne.h"

// notes in the melody:
int melody[] = {

  NOTE_C4, NOTE_G3
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {

  4, 8
};

#define BTN_MENU 5
#define BTN_SET 6

int timerExpired = 0;

/*
 *   call timer every 100ms 
 */
 
void timerAction(){
  ++timerExpired;
}

void setup()
{
    /* Select the font to use with menu and all font functions */
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    sh1106_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    analogReference(DEFAULT);

    ssd1306_flipHorizontal(1);  /* rotate screen in X */
    ssd1306_flipVertical(1);    /* rotate screen in Y */

    pinMode(BTN_MENU, INPUT_PULLUP);
    pinMode(BTN_SET, INPUT_PULLUP);

    // preset timer period every 100 ms and call timerAction function when time expire
    pinMode(10, OUTPUT);
    Timer1.initialize(100000);         // initialize timer1, and set a 1/10 second period
    Timer1.attachInterrupt(timerAction);  // attaches callback() as a timer overflow interrupt
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

// Measure voltage to get number
int getVoltage(){
    long average = 0;
    for(int i=0; i<16; i++){
      average += analogRead(A3);
      delay(5);
    }
    average >>= 4;
    return (int) average;
}

// states of the set and menu
enum menu_e{
  MENU_SHOW,
  MENU_SET_CLOCK_HOUR,
  MENU_SET_CLOCK_MIN,
  MENU_SET_CLOCK_SEC,
  MENU_SET_ALARM_HOUR,
  MENU_SET_ALARM_MIN,
  MENU_SET_ALARM_ON_OFF
};

void loop() {
    char buffer1[15], buffer2[15];
    char str_temp[8];
    static char lastStatus1 = 0, lastStatus2 = 0;
    static char timer_tenth_sec = 0;
    static int clock_hour = 0, clock_min = 0, clock_sec = 0;
    static int alarm_hour = 0, alarm_min = 0, alarm_sec = 0;
    static char alarmOn = 0;
    static char menuChoose = MENU_SHOW;
    static char buzzerOn = 0;
    static char previousAlarmOn = 0;

    if(buzzerOn){
      playTone();
    }

    if(!digitalRead(BTN_MENU)){
      if(lastStatus1 == 0){
        lastStatus1 = 1;
        alarmOn = 0;
        ssd1306_fillScreen( 0x00 );
        if(menuChoose == MENU_SHOW) 
          menuChoose = MENU_SET_CLOCK_HOUR;
        else
          menuChoose = MENU_SHOW;
      }
    }
    else{
      lastStatus1 = 0;
    }

    if(!digitalRead(BTN_SET)){
      if(lastStatus2 == 0){
        lastStatus2 = 1;
        ssd1306_fillScreen( 0x00 );
        if(menuChoose != MENU_SHOW){
          if(++menuChoose > MENU_SET_CLOCK_SEC) 
            menuChoose = MENU_SET_CLOCK_HOUR;
        }
        else{
          if(buzzerOn) {
            buzzerOn = 0;
          }
          else{
            alarmOn = !alarmOn;
          }
        }
      }
    }
    else{
      lastStatus2 = 0;
    }

    if(timerExpired){
      timer_tenth_sec += timerExpired;
      timerExpired = 0;
      if(timer_tenth_sec >= 10){
        timer_tenth_sec -= 10;
        if((menuChoose == MENU_SHOW) && (buzzerOn == 0) && (alarmOn == 1)){
          if(--alarm_sec < 0){
            alarm_sec = 59;
            if(--alarm_min < 0){
              alarm_min = 59;
              if(--alarm_hour < 0){
                alarm_hour = 23;
              }
            }
          } 
          if((alarm_sec == 0) && (alarm_min == 0) && (alarm_hour == 0)){
            buzzerOn = 1;
            alarmOn = 0;
          }
        }
      }
    }
    switch(menuChoose){
      case MENU_SHOW:
        
        if(alarmOn){
          sprintf(buffer1, "%02d:%02d:%02d", alarm_hour, alarm_min, alarm_sec);
          ssd1306_printFixedN(15, 10, buffer1, STYLE_NORMAL, FONT_SIZE_2X);
        }
        else{
          alarm_hour = clock_hour;
          alarm_min = clock_min;
          alarm_sec = clock_sec;
          sprintf(buffer1, "%02d:%02d:%02d", clock_hour, clock_min, clock_sec);
          ssd1306_printFixedN(15, 10, buffer1, STYLE_NORMAL, FONT_SIZE_2X);
        }
        break;
        
      case MENU_SET_CLOCK_HOUR:
        clock_hour = getVoltage() >> 3;
        if(clock_hour > 23) clock_hour = 23;
        
        if((timer_tenth_sec%5) < 2)
          sprintf(buffer1, "%02d:%02d:%02d", clock_hour, clock_min, clock_sec);
        else
          sprintf(buffer1, "  :%02d:%02d", clock_min, clock_sec);
          ssd1306_printFixedN(15, 35, buffer1, STYLE_NORMAL, FONT_SIZE_2X);

        ssd1306_printFixedN(10, 10, "Set Clock", STYLE_NORMAL, FONT_SIZE_2X);
        break;
        
      case MENU_SET_CLOCK_MIN:
        clock_min = getVoltage() >> 3;
        if(clock_min > 59) clock_min = 59;
        
        if((timer_tenth_sec%5) < 2)
          sprintf(buffer1, "%02d:%02d:%02d", clock_hour, clock_min, clock_sec);
        else
          sprintf(buffer1, "%02d:  :%02d", clock_hour, clock_sec);
        ssd1306_printFixedN(15, 35, buffer1, STYLE_NORMAL, FONT_SIZE_2X);
          
        ssd1306_printFixedN(10, 10, "Set Clock", STYLE_NORMAL, FONT_SIZE_2X);
          
        break;
        
      case MENU_SET_CLOCK_SEC:
        clock_sec = getVoltage() >> 3;
        if(clock_sec > 59) clock_sec = 59;
        
        if((timer_tenth_sec%5) < 2)
          sprintf(buffer1, "%02d:%02d:%02d", clock_hour, clock_min, clock_sec);
        else
          sprintf(buffer1, "%02d:%02d:  ", clock_hour, clock_min);
        ssd1306_printFixedN(15, 35, buffer1, STYLE_NORMAL, FONT_SIZE_2X);
          
        ssd1306_printFixedN(10, 10, "Set Clock", STYLE_NORMAL, FONT_SIZE_2X);
        break;
    }
    delay(20);
}
