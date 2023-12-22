if (pin3clicked == false)
        {
            savedBrightness = matrix.getBrightness();
            pin3clicked = true;
            pin3canceled = false;
            for (size_t i = savedBrightness; i > 0; i -= 1)
            {
                matrix.setBrightness(i);
                delay(3);
                matrix.show();
            }
            matrix.setBrightness(1);
        }
        else
        {
            pin3clicked = false;
            pin3canceled = false;
            for (size_t i = 1; i < savedBrightness; i += 1)
            {
                matrix.setBrightness(i);
                delay(3);
                matrix.show();
            }
            matrix.setBrightness(savedBrightness);
            pin3canceled = true;
        }


           void initialize(){};
    void redraw(){};
    void fillScreen(uint16_t color){};
    void print(const char* text){};
    void print(String& text){};
    void setCursor(int16_t x, int16_t y){};
    void setTextWrap(bool wrap){};
    void show(){};
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h){};
    void setBrightness(uint8_t){};
    void clear(){};
    void setFont(const GFXfont *f = NULL){};
    void attachNeomatrixFrontend(Adafruit_NeoMatrix* frontend){};
    void initNeoMatrix(){};
    void attachSerialFrontend(std::function<void(const char*)> frontend){};
    void serialRender(){};
    void drawPixel(int16_t x, int16_t y, uint16_t color){};
    int width(){return 0;};
    int height(){return 0;};
    uint16_t* getBuffer(){return NULL;};
    void setTextColor(uint16_t color){};


    const uint16_t colors[] = {
    wsMatrix.Color(255, 255, 0),
    wsMatrix.Color(255, 255, 0),
    wsMatrix.Color(0, 0, 255),
    wsMatrix.Color(255, 255, 255),
    wsMatrix.Color(255, 0, 255)
};