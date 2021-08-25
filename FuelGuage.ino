#include <Arduino_GFX_Library.h>
#include <SPI.h>

Arduino_DataBus *bus = new Arduino_HWSPI(7 /* DC */, 3 /* CS */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 8 /* RST */, 0 /* rotation */, true /* IPS */);

#define BACKGROUND BLACK
#define TANKSIZE 3.3

static uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    return (10 * (*p - '0')) + (*++p - '0');
}

static int16_t w, h, center;

static uint8_t c_mpg=30, a_mpg=44;
static uint16_t fuel_max = TANKSIZE * 40;

//TODO: read fuel level from CAN
static uint16_t fuel_level_start = (fuel_max - (TANKSIZE - 1.65) * 40);

static unsigned long currentTime;
static unsigned long deltaTime;
static unsigned long targetTime; // next action time

static uint16_t x0, y0;

void setup(void)
{
    gfx->begin();
    gfx->fillScreen(BACKGROUND);
    fuel_level_start = map(fuel_level_start, 0, fuel_max, 40, 320);

#ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
#endif

    Serial.begin(9600);

    // init LCD constant
    w = gfx->width();
    h = gfx->height();

    x0 = w / 2;
    y0 = h / 2;

    if (w < h)
    {
        center = w / 2;
    }
    else
    {
        center = h / 2;
    }

    calculate_fuel_level();
    calculate_fuel_mpg();
    display_fuel_guage();

    //gfx->setTextSize(2, 2);

    gfx->setCursor(x0 - 35, y0 - 15);
    gfx->setTextSize(3, 3);
    //gfx->print("100%");
    gfx->setRotation(1);

    //targetTime = ((millis() / 1000) + 1) * 1000;

    delay(5);
    // gfx->setCursor(x0-15, y0+15);
    // gfx->setTextSize(2,2);

    gfx->setTextColor(WHITE);
    gfx->setTextSize(2, 2);
    gfx->setRotation(0);

    display_fuel_mpg();

    /* for (float i = 310; i >= 40; i -= 5)
    {
        float percent = (i - 40) / (310 - 40) * 100;
        gfx->fillArc(x0, y0, 118, 88, i, i + 10, BLACK);
        // if(percent<20 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, RED);
        // if(percent<50 && percent>20 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, ORANGE);
        // if(percent>50 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, GREEN);
        //gfx->fillArc(x0, y0, 118, 88, 40, 320, 0xf1a2);
        //gfx->fillArc(x0, y0, 118, 88, i, i + 10, BLACK);
        gfx->setRotation(0);
        //gfx->fillRect(x0 - 55, y0 - 15, 110, 25, BLACK);
        gfx->fillRect(x0 - 55, h - 30, 110, 20, BLACK);
        //gfx->setCursor(x0 - 35, y0 - 15);

        String str = String(percent, 0); //+"%";
        gfx->setTextSize(3, 3);
        //if (percent < 10)
        //{
        //    draw_center_txt(str, x0 - 10, y0 - 15);
        //}
        //else
        //{
        //    draw_center_txt(str, x0, y0 - 15);
        //}

        gfx->setTextSize(1, 1);

        display_fuel_mpg();
        // gfx->print(percent,0);
        // gfx->print("%");
        gfx->setRotation(1);
    } */
}

void calculate_fuel_level()
{

    //fuel_level_start = fuel_level_start;
}

void calculate_fuel_mpg()
{
    c_mpg = 30;
    a_mpg = 44;
}
void display_fuel_mpg()
{
    //gfx->drawRect(60, h / 2 - 30, 30, 60, YELLOW);
    //gfx->drawRect(w - 90, h / 2 - 30, 30, 60, YELLOW);
    gfx->fillRect(x0 - 55, h - 30, 110, 20, BLACK);
    draw_center_txt(String(map(fuel_level_start,40,320,0,fuel_max)), x0, h - 30);
    draw_txt(String(c_mpg), 65, 160);
    draw_txt(String(a_mpg), 155, 160);

    float percent = (fuel_level_start - 40) / (320 - 40) * 100;
    gfx->setRotation(1);
    if (fuel_level_start > 40)
    {
        gfx->fillArc(x0, y0, 118, 88, fuel_level_start, 320 + 10, BLACK);
        fuel_level_start -= 1;
    }

    gfx->setRotation(0);

    /* for (float i = 310; i >= 40; i -= 5)
    {
        float percent = (i - 40) / (310 - 40) * 100;
        gfx->fillArc(x0, y0, 118, 88, i, i + 10, BLACK);
        // if(percent<20 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, RED);
        // if(percent<50 && percent>20 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, ORANGE);
        // if(percent>50 ) gfx->fillArc(x0, y0, 118, 88, i, i + 10, GREEN);
        //gfx->fillArc(x0, y0, 118, 88, 40, 320, 0xf1a2);
        //gfx->fillArc(x0, y0, 118, 88, i, i + 10, BLACK);
        gfx->setRotation(0);
        //gfx->fillRect(x0 - 55, y0 - 15, 110, 25, BLACK);
        gfx->fillRect(x0 - 55, h - 30, 110, 20, BLACK);
        //gfx->setCursor(x0 - 35, y0 - 15);

        String str = String(percent, 0); //+"%";
        gfx->setTextSize(3, 3);
        //if (percent < 10)
        //{
        //    draw_center_txt(str, x0 - 10, y0 - 15);
        //}
        //else
        //{
        //    draw_center_txt(str, x0, y0 - 15);
        //}

        gfx->setTextSize(1, 1);

        display_fuel_mpg();
        // gfx->print(percent,0);
        // gfx->print("%");
        gfx->setRotation(1);
    } */

    //draw_center_txt(String(c_mpg), 50, h/2-30+60);
    //draw_center_txt(String(a_mpg), x0, h - 20);
}
void display_fuel_guage()
{
    gfx->setRotation(1);
    //gfx->fillArc(x0, y0, 118, 88, 40, 320, 0x0352fc);
    gfx->fillArc(x0, y0, 118, 88, 226, 320, GREEN);
    gfx->fillArc(x0, y0, 118, 88, 133, 226, ORANGE);
    gfx->fillArc(x0, y0, 118, 88, 40, 133, RED);
    gfx->drawArc(x0, y0, 118, 88, 40, 320, BLACK);
    gfx->fillArc(x0, y0, 88, 83, 40, 320, WHITE);
    gfx->drawArc(x0, y0, 88, 83, 40, 320, BLACK);

    gfx->setRotation(0);
    gfx->fillRect(60, h / 2 - 5, 30, 35, YELLOW);
    gfx->fillRect(w - 90, h / 2 - 30, 30, 60, YELLOW);
}

void draw_center_txt(const String &buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    //String padding="000";
    gfx->getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    gfx->setCursor(x - w / 2, y);
    gfx->print(buf);
    //Serial.println(x - w / 2);
}

void draw_txt(const String &buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    //String padding="000";
    gfx->getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    gfx->setCursor(x, y);
    gfx->print(buf);
    //Serial.println(x);
}

void loop()
{
    targetTime = millis();
    if (targetTime % 5000 == 0 && targetTime > 5000)
    {
        Serial.println(((millis() / 1000) + 1) * 1000);
        calculate_fuel_mpg();
        display_fuel_mpg();
    }
}
