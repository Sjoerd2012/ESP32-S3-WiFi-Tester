#include <WiFi.h>
#include <LiquidCrystal.h>

// ============================================
// LCD pin mapping for parallel 16x2 LCD
// Aangepast voor beschikbare GPIO's op ESP32-S3
// ============================================
const int rs = 6;      // RS pin
const int en = 7;      // Enable pin
const int d4 = 15;     // D4
const int d5 = 16;     // D5
const int d6 = 17;     // D6
const int d7 = 8;      // D7

// ============================================
// WiFi credentials - AANPASSEN!
// ============================================
const char* wifi_ssid = "JOUW_WIFI_NAAM";
const char* wifi_password = "JOUW_WIFI_WACHTWOORD";

// LCD object (16 kolommen, 2 rijen, 4-bit mode)
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ============================================
// Functie: Tekst afkorten op maximale lengte
// ============================================
String truncateText(String text, int maxLen) {
  if (text.length() <= maxLen) {
    return text;
  }
  return text.substring(0, maxLen);
}

// ============================================
// Functie: Signaalsterkte status bepalen
// ============================================
String getSignalStatus(int rssi) {
  if (rssi >= -55) return "Strong";
  if (rssi >= -67) return "Good";
  if (rssi >= -80) return "Weak";
  return "Poor";
}

// ============================================
// Functie: Signaalkwaliteit in % berekenen
// RSSI range: -100 dBm (zwak) tot -50 dBm (sterk)
// ============================================
int getSignalQuality(int rssi) {
  int quality = map(rssi, -100, -50, 0, 100);
  quality = constrain(quality, 0, 100);
  return quality;
}

// ============================================
// Functie: Regel op LCD schrijven (clear + print)
// ============================================
void printLine(int row, String text) {
  lcd.setCursor(0, row);
  // Clear de hele rij
  lcd.print("                ");
  // Print de nieuwe tekst
  lcd.setCursor(0, row);
  lcd.print(truncateText(text, 16));
}

// ============================================
// Functie: WiFi status weergeven op LCD
// ============================================
void showWifiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    int rssi = WiFi.RSSI();
    int quality = getSignalQuality(rssi);
    String ssid = WiFi.SSID();
    String status = getSignalStatus(rssi);

    // Regel 1: SSID
    String line1 = "WiFi: " + ssid;
    
    // Regel 2: RSSI dBm en kwaliteit
    String line2 = String(rssi) + "dBm " + String(quality) + "% " + status;

    printLine(0, line1);
    printLine(1, line2);
    
    // Debug output naar Serial
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.print(" | RSSI: ");
    Serial.print(rssi);
    Serial.print(" dBm | Quality: ");
    Serial.print(quality);
    Serial.print("% | Status: ");
    Serial.println(status);
  } else {
    // Niet verbonden
    printLine(0, "Searching WiFi...");
    printLine(1, "Waiting...");
    Serial.println("WiFi status: Disconnected");
  }
}

// ============================================
// Setup: Initialisatie
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== ESP32 WiFi Tester ===");
  Serial.println("Initializing LCD...");

  // LCD initialiseren (16 kolommen, 2 rijen)
  lcd.begin(16, 2);
  lcd.clear();
  
  // Splash screen
  lcd.setCursor(0, 0);
  lcd.print("ESP32 WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Tester");
  
  Serial.println("LCD initialized. Starting WiFi...");
  delay(2000);

  // WiFi verbinding starten
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  Serial.println("Connecting to WiFi...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Wachten op verbinding (max 30 seconden)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("\n✗ WiFi connection failed. Check SSID and password.");
  }

  delay(1000);
  showWifiStatus();
}

// ============================================
// Loop: Elke 2 seconden WiFi status updaten
// ============================================
void loop() {
  showWifiStatus();
  delay(2000);
}
