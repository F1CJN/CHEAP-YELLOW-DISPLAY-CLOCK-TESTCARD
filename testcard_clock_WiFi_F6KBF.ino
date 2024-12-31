/*
 * Génère une "testcard" similaire au BMP avec horloge Internet temps réel
 * Adapté pour CYD par Hamradio F1CJN
 * Cheap Yellow Display ESP32-2432S028 (touch screen resistif)
 */

#include <TFT_eSPI.h>
#include <WiFi.h>

const char* ssid = "nom box"; // Nom de la box  Wi-Fi
const char* password = "mot de passe"; // Mot de passe de la box Wi-Fi
const char* call = "F6KBF"; // Indicatif

const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600 * 1; // 1 pour heure d'hiver et 2 pour heure d'été
const int   daylightOffset_sec = 0;

TFT_eSPI tft = TFT_eSPI();   

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define GREY      RGB(127, 127, 127)
#define DARKGREY  RGB(64, 64, 64)
#define TURQUOISE RGB(0, 128, 128)
#define PINK      RGB(255, 128, 192)
#define OLIVE     RGB(128, 128, 0)
#define PURPLE    RGB(128, 0, 128)
#define AZURE     RGB(0, 128, 255)
#define ORANGE    RGB(255,128,64)

uint16_t ID;
uint8_t hh, mm, ss; // heures minutes secondes

void setup(void)
{
    Serial.begin(9600);
    WiFi.mode(WIFI_STA); // Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnexion en cours");

    tft.init();
    tft.setRotation(1);
    tft.invertDisplay(0); 
    tft.fillScreen(BLACK);

    while (WiFi.status() != WL_CONNECTED) {  // Attente de connexion
    Serial.print(".");
    delay(100); }

    Serial.println("\nConnecté au réseau WiFi");
    Serial.print("CYD IP Locale: ");
    Serial.println(WiFi.localIP()); 

    //Configure le serveur NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop(void)
{
    int16_t x, y, dx, dy, radius = 108, idx;
    uint16_t w, h, len, mask;
    uint16_t colors[8] = { BLACK, WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE };
    uint16_t height, width;
    width = tft.width();
    height = tft.height();
    tft.fillRect(0, 0, 7, 3, WHITE);
    tft.fillRect(313, 0, 7, 3, WHITE);
    tft.fillRect(0, 237, 7, 3, WHITE);
    tft.fillRect(313, 237, 7, 3, WHITE);
    for (y = 0, w = 18, h = 3; y < 240; y += 13 * w + h) {
        for (x = 25; x < 320 - 18; x += 2 * w) {
            tft.fillRect(x, y, w, h, WHITE);
        }
    }
    for (x = 0, w = 7, h = 18; x < 320; x += 17 * h + w) {
        for (y = 21; y < 240 - 18; y += 2 * h) {
            tft.fillRect(x, y, w, h, WHITE);
        }
    }
    tft.fillRect(7, 3, 17 * 18, 13 * 18, GREY);
    for (x = 7, y = 0, w = 1, h = 240; x < 320; x += 18) {
        tft.fillRect(x, y, w, h, WHITE);
    }
    for (x = 0, y = 3, w = 320, h = 1; y < 240; y += 18) {
        tft.fillRect(x, y, w, h, WHITE);
    }
    tft.fillRect(26, 22, 17, 99, TURQUOISE);
    tft.fillRect(26, 120, 17, 99, PINK);
    tft.fillRect(44, 22, 17, 35, AZURE);
    tft.fillRect(44, 184, 17, 35, ORANGE);
    tft.fillRect(260, 22, 17, 35, AZURE);
    tft.fillRect(260, 184, 17, 35, ORANGE);
    tft.fillRect(278, 22, 17, 99, OLIVE);
    tft.fillRect(278, 120, 17, 99, PURPLE);

    for (dx = radius; dx > -radius; dx--) {
        w = sqrt(radius * radius - dx * dx);
        y = 120 - dx;
        dy = (y - 3) / 18;
        mask = 7;
        colors[0] = (dy == 3) ? DARKGREY : BLACK;
        switch (dy) {
            case 0:
            case 1: idx = 1; len = 0; break;
            case 2: idx = 0; len = 0; break;
            case 3: idx = 0; len = 13; mask = 1; break;
            case 4:
            case 5: idx = 2; len = 38; break;
            case 6:
            case 7:
            case 8: idx = 0; len = 0; break;
            case 9: for (idx = 2; idx < 8; idx++) {
                    //dy = 0xFF >> (7 - idx);
                    dy = (idx - 2) * 51;
                    colors[idx] = tft.color565(dy, dy, dy);
                }
                idx = 2; len = 38; break;
            case 10: idx = 1; len = 0; break;
            case 11:
            case 12: colors[2] = YELLOW; idx = 2; len = 0; break;
        }
        if (len == 0)
            tft.fillRect(160 - w, y, w * 2, 1, colors[idx]);

        else {
            if (mask == 1) idx = 1 + (w) / len;
            dy = w % len;
            for (x = 160 - w; x < 160 + w; idx++) {
                tft.fillRect(x, y, dy, 1, colors[idx & mask]);
                x += dy;
                if (x + len > 160 + w) dy = w % len;
                else dy = len;
            }
        }

    }
    for (x = 72, y = 129, dx = 5, dy = 0; dx > 0; x += 2 * dx) {
        tft.fillRect(x, y, dx, 36, WHITE);
        dy += dx * 2;
        if (dy >= 36) {
            dy = 0;
            dx--;
        }
    }
    tft.fillRect(160 - 8, 5 * 18 + 3, 17, 3 * 18, BLACK);
    for (x = 3 * 18 + 7, y = 6 * 18 + 3, w = 1, h = 18; x < 160 + 108; x += 18) {
        tft.fillRect(x, y, w, h, WHITE);
    }
    tft.fillRect(160 - 108, 120, 108 * 2, 1, WHITE);
    tft.fillRect(160, 5 * 18 + 3, 1, 3 * 18, WHITE);
    tft.fillRect(108, 2 * 18 + 3, 6 * 18, 18, WHITE);
    tft.fillRect(160 - 8, 11 * 18 + 3, 17, radius - 18*9/2, RED);

    tft.setTextSize(2);
    tft.fillRect(120,22,84,17,BLACK);
    tft.setTextColor(WHITE);
    tft.drawCentreString(call, 162, 24,1); 		
   
    while (1) {
        if (++ss > 59) {
            ss = 0;
            mm++;
            if (mm > 59) {
                mm = 0;
                hh++;
                if (hh > 23) hh = 0;
            }
        }
        
        struct tm timeinfo;
         if (!getLocalTime(&timeinfo)) {
         Serial.println("Failed to obtain time");
         }
        
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        tft.fillRect(108, 10 * 18 + 3, 6 * 18, 18, BLACK);
        tft.setCursor(114, 185);
        tft.print(&timeinfo,"%H:%M:%S");
        delay(1000);
    }
}
