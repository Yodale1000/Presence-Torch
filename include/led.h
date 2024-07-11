#include <Adafruit_NeoPixel.h>

#define LED_PIN 23
#define LED_COUNT 16

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void showStrip() {
  strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < LED_COUNT; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void FadeIn(byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}

void FadeOut(byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}

void FadeInOut(byte red, byte green, byte blue){
  FadeIn(red, green, blue);
  FadeOut(red, green, blue);
}

void SetBrightness(int brightness) {
  strip.setBrightness(brightness);
}

void FadeInPixel(int pixel, byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setPixel(pixel,r,g,b);
    showStrip();
  }
}

void FadeOutPixel(int pixel,byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 255; k >= 0; k=k-1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setPixel(pixel,r,g,b);
    showStrip();
  }
}