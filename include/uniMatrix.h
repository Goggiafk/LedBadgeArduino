
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <sstream>

class UniMatrix : public GFXcanvas16 {
public:
    UniMatrix(uint16_t w, uint16_t h, const GFXfont *f = (const GFXfont *)__null) : GFXcanvas16(w, h) {
        this->f = f;
        this->w = w;
        this->h = h;
    }

    void initialize(){
        clear();
        if (f != (const GFXfont *)__null) {
            setFont(f);
        }
        setCursor(0, 0);
        doubleBuffer = new uint16_t[w * h];
    }

    void attachNeomatrixFrontend(Adafruit_NeoMatrix* frontend) {
        this->neomatrixFrontend = frontend;
        initNeoMatrix();
    }

    void initNeoMatrix() {
        neomatrixFrontend->begin();
        neomatrixFrontend->clear();
        neomatrixFrontend->setBrightness(20);
        neomatrixFrontend->fillScreen(0);
        neomatrixFrontend->show();
    }

    void attachSerialFrontend(std::function<void(const char*)> frontend) {
        this->serialFrontendCb = frontend;
    }

    void clear() {
        fillScreen(0);
    }

    void setFont(const GFXfont *f = NULL) {
        GFXcanvas16::setFont(f);
    }

    void setCursor(int16_t x, int16_t y) {
        GFXcanvas16::setCursor(x, y);
    }

    void redraw(bool force = false){
        if (doubleBuffer == NULL) {
            return;
        }
        // memcpy(doubleBuffer, getBuffer(), sizeof(*doubleBuffer));
        memcpy(doubleBuffer, getBuffer(), 2*w*h);
        
        if (neomatrixFrontend != NULL) {
            neomatrixFrontend->drawRGBBitmap(0, 0, doubleBuffer, width(), height());
            neomatrixFrontend->show();
        }

        if (serialFrontendCb != NULL) {
            serialRender();
        }
    }

    void serialRender() {
        static int skip = 0;
        if (skip++ % 50 != 0) { return; }
        std::stringstream ss;
        for (int y = 0; y < height(); y++) {
            ss << "| ";
            for (int x = 0; x < width(); x++) {
                if (doubleBuffer[y * width() + x] == 0) { ss << ". ";}
                else { ss << "# ";}
            }
            ss << "|\n";
        }
        serialFrontendCb(ss.str().c_str());        
    }

    void setTextColor(uint16_t c) {
        GFXcanvas16::setTextColor(c);
    }

    void setTextColor(uint8_t r, uint8_t g, uint8_t b) {
        uint16_t color = ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
        GFXcanvas16::setTextColor(color);
    }

    int w,h;
    const GFXfont *f = NULL;
    Adafruit_NeoMatrix* neomatrixFrontend;
    std::function<void(const char*)> serialFrontendCb = NULL;
    uint16_t* doubleBuffer;
};