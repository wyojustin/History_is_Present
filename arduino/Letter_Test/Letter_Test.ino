#include <FastLED.h>
#include "MatrixMap.h"
#include "words.h"

#define CLOCKIOT

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    4
#define CLK_PIN     16
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS    384
CRGB leds[NUM_LEDS];
uint8_t mask[NUM_LEDS];
int wordcount = 0;


#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

bool flip_display = true;
uint16_t XY( uint8_t x, uint8_t y){
  uint16_t out = 0;
  if(flip_display){
    x = MatrixWidth - x - 1;
    y = MatrixHeight - y - 1;
  }
  if(x < MatrixWidth && y < MatrixHeight){
    out = MatrixMap[y][x];
  }
  return out;
}

void set_absolute(int p, const struct CRGB & color){
  if(0 <= p && p < NUM_LEDS){
    leds[p] = color;
  }
}
void mask_absolute(uint8_t * mask, int p, uint8_t val){
  if(0 <= p && p < NUM_LEDS){
    mask[p] = val;
  }
}

void mask_letter(uint8_t* mask, int row, int col, uint8_t val){
  int p;
  for(int ii = 0; ii < 2; ii++){
    for(int jj = 0; jj < 2; jj++){
      p = XY(2 * col + ii, 2 * row + jj);
      mask_absolute(mask, p, val);
    }
  }  
}

void set_letter(int row, int col, const struct CRGB & color){
  int p;
  for(int ii = 0; ii < 2; ii++){
    for(int jj = 0; jj < 2; jj++){
      p = XY(2 * col + ii, 2 * row + jj);
      set_absolute(p, color);
    }
  }
}

void mask_word(uint8_t* mask, uint8_t *word, uint8_t val){
  for(int j = 0; j < word[2]; j++){
    mask_letter(mask, word[0], word[1] + j, val);
  }
}
void set_word(uint8_t *word, const struct CRGB & color){
  for(int j = 0; j < word[2]; j++){
    set_letter(word[0], word[1] + j, color);
  }
}

void setup() {
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(6000); // 3 second delay for recovery
  Serial.begin(115200);
  Serial.println("WyoLum.com");

}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {solid, bpm, rainbowWithGlitter, juggle};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void update_mask(){
  if(wordcount <= 3){
    increment_mask(mask, 2);
  }
  else{
    //fadeToBlackBy(leds, NUM_LEDS, 255);
    decrement_mask(mask, 1.2);
  }
}
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  
  // send the 'leds' array out to the actual LED strip
  apply_mask(mask);
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 40 ) {update_mask();} // slowly cycle the "base color" through the rainbow
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 3 ) { add_word(); } // change patterns periodically
  EVERY_N_SECONDS( 3 * 5 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  //gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  uint8_t i = random(ARRAY_SIZE( gPatterns));
  gCurrentPatternNumber = i;
}

void off(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
}

void seq_words(){
  uint8_t *subj, *pred;
  for(int ii = 0; ii < 4; ii++){
    subj = SUBJECTS[ii];
    Serial.println(ii);
    for(int jj = 0; jj < 4; jj++){
      Serial.print("    ");
      Serial.println(jj);
      pred = PREDS[jj];

      off();
      FastLED.show();
      delay(1000);
      set_word(subj, CRGB::Green);FastLED.show();delay(1000);
      set_word(IS, CRGB::Red);FastLED.show();    delay(1000);
      set_word(pred, CRGB::Blue);FastLED.show(); delay(1000);
    }
  }
}

void old_rand_words(){
  uint8_t *subj, *pred;
  int ii = random(4);
  int jj = random(4);
  subj = SUBJECTS[ii];
  Serial.println(ii);
  Serial.print("    ");
  Serial.println(jj);
  pred = PREDS[jj];

  off();
  FastLED.show();
  delay(1000);
  set_word(subj, CRGB::Green);FastLED.show();delay(1000);
  set_word(IS, CRGB::Red);FastLED.show();    delay(1000);
  set_word(pred, CRGB::Blue);FastLED.show(); delay(1000);
}

void apply_mask(uint8_t *mask){
  for(int i = 0; i < NUM_LEDS; i++){
    fadeToBlackBy(leds + i, 1, (255 - mask[i]));
  }
}
void increment_mask(uint8_t *mask, uint8_t val){
  int new_val;
  for(int i = 0; i < NUM_LEDS; i++){
    if(mask[i] > 0){
      new_val = (int)val * (int)mask[i];
      if(new_val > 255){
	new_val = 255;
      }
      mask[i] = new_val;
    }
  }  
}

void decrement_mask(uint8_t *mask, float val){
  uint8_t new_val;
  for(int i = 0; i < NUM_LEDS; i++){
    new_val = mask[i] / val;
    if(new_val > mask[i]){
      Serial.print(new_val);
      Serial.print(">");
      Serial.println(mask[i]);
    }
    mask[i] = new_val;
  }  
}
void clear_mask(uint8_t *mask){
  for(int i = 0; i < NUM_LEDS; i++){
    mask[i] = 0;
  }
}

void show(){
  apply_mask(mask);
  FastLED.show();
}
void add_word(){
  uint8_t *word;
  int i = random(4);

  if(wordcount == 0){
    mask_word(mask, SUBJECTS[i], 1);
    wordcount++;
  }
  else if(wordcount == 1){
    mask_word(mask, IS, 1);
    wordcount++;
  }
  else if(wordcount == 2){
    mask_word(mask, PREDS[i], 1);
    wordcount++;
  }
  else if(wordcount == 3){
    wordcount++;
    //wait for it...
  }
  else{
    wordcount = 0;
  }
}

void letters(){
  off();
  Serial.println("letters()");
  for(int i = 0; i < 1; i++){
    for(int j = 0; j < 4; j++){
      set_letter(i, j, CRGB::Green);
      FastLED.show();
      delay(100);
    }
  }
}
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 20;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void solid(){
  CHSV hsv;
  hsv.hue = gHue;
  hsv.val = 255;
  hsv.sat = 240;
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = hsv;
    //hsv.hue += deltahue;
  }
}

