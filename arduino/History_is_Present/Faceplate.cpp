#include "Faceplate.h"
#include <Arduino.h>

Faceplate::Faceplate(){
}
Faceplate::Faceplate(uint8_t *_words,
		     uint8_t *_displays,
		     uint32_t *_minute_leds,
		     uint32_t *_minutes_hack,
		     char *_name){
  words = _words;
  displays = _displays;
  minute_leds = _minute_leds;
  minutes_hack = _minutes_hack;
  name = _name;
  n_minute_hack = minute_leds[0];
  n_minute_led = minute_leds[1];
}

void Faceplate::setup(uint8_t _width, uint8_t _height, uint16_t (*_XY_p)(uint8_t col, uint8_t row)){
  width = _width;
  height = _height;
  num_leds = width * height;
  XY_p = _XY_p;
}
void Faceplate::setMask(uint8_t* mask, uint8_t row, uint8_t col, uint8_t val){
  if(row >= height){
    Serial.println("Faceplate::setMask: too tall");
  }
  else if(col >= width){
    Serial.println("Faceplate::setMask: too wide");
  }
  else{
    uint16_t pos = XY_p(col, row);
    if(pos < num_leds){
      mask[pos] = val;
    }
    else{
      Serial.print("huh??? setMask::pos");Serial.println(pos);
    }
  }
}

void Faceplate::maskTime(uint32_t tm, uint8_t* mask){
  uint8_t bits;     // holds the on off state for 8 words at a time
  uint8_t word[3];  // start columm, start row, length of the current word
  int time_inc, min_hack_inc;
  uint32_t min_hack;
  uint8_t row, col;
  
  uint8_t n_byte_per_display = displays[0];
  //Serial.print("n_byte_per_display:");Serial.println(n_byte_per_display);
  time_inc = ((tm % 86400) / 300) % 288;
  //Serial.print("time_inc:");Serial.println(time_inc);
  for(uint8_t j = 0; j < n_byte_per_display; j++){ // j is a byte index 
    
    // read the state for the next set of 8 words
    bits = displays[1 + (time_inc * n_byte_per_display) + j];
    //Serial.print("j");Serial.println(j);
    //Serial.print("bits");Serial.println(bits, BIN);
    //Serial.println(bits, BIN);; delay(100);
    for(uint8_t k = 0; k < 8; k++){                     // k is a bit index
      if((bits >> k) & 1){                              // check to see if word is on or off
	getword(j * 8 + k, word);                       // if on, read location and length
	/*
	Serial.print("display word[0]:");
	Serial.print(word[0]); Serial.print(",");
	Serial.print(word[1]); Serial.print(",");
	Serial.print(word[2]); Serial.println();
	*/
	for(int m=word[0]; m < word[0] + word[2]; m++){ // and display it
	  setMask(mask, word[1], m, 255);
	}
      }
    }
  }
  //delay(1000);
  // check for minutes hack
  if(n_minute_hack > 0){
    min_hack_inc = (int)((tm % 300 ) / (300. / n_minute_hack));
    min_hack = minutes_hack[min_hack_inc];
    for(int i=0; i < n_minute_led; i++){
      if((min_hack >> i) & 1){ // turn on this LED!
	row = minute_leds[i + 2] >> 4;
	col = minute_leds[i + 2] & 0b00001111;
	setMask(mask, row, col, 255);
      }
    }
  }
}

void Faceplate::getword(int i, uint8_t* out){
  out[0] = words[3 * i + 1];
  out[1] = words[3 * i + 2];
  out[2] = words[3 * i + 3];
}
