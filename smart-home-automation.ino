#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// 1. Network & Firebase Credentials
#define WIFI_SSID "Nothing Phone (2a)_1940"
#define WIFI_PASSWORD "1w3r5y7i9p"
#define FIREBASE_HOST "https://smart-home-automation-79320-default-rtdb.asia-southeast1.firebasedatabase.app" // [cite: 15]
#define FIREBASE_AUTH "WSujxUfJpDIRrSswryaHp2D5VAsuaA1ARJER770m"

// 2. Hardware Layer Definitions [cite: 17, 19]
#define LED_PIN 4       // Lighting Node: Standard illumination [cite: 21]
#define RELAY_PIN 14     // Appliance Node: Relay + Fan [cite: 22]
#define BUZZER_PIN 12    // Alert Node: Acoustic feedback [cite: 24]

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  Serial.begin(115200);
  
  // Initialize Hardware [cite: 17]
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Communication Layer: Wi-Fi Heartbeat [cite: 16]
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Cloud Layer Configuration [cite: 15]
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true); 
}

void loop() {
  // 3. Bi-Directional Communication: Syncing UI with Hardware [cite: 8, 32]
  
  // Control Lighting Node (device1State) [cite: 20, 28]
  if (Firebase.getInt(firebaseData, "/device1State")) {
    int ledState = firebaseData.intData();
    digitalWrite(LED_PIN, ledState == 1 ? HIGH : LOW);
  }

  // Control Appliance Node (device2State) [cite: 20, 29]
  if (Firebase.getInt(firebaseData, "/device2State")) {
    int applianceState = firebaseData.intData();
    digitalWrite(RELAY_PIN, applianceState == 0 ? HIGH : LOW);
    
    // Alert Node: Acoustic feedback for specific events (e.g., Appliance ON) [cite: 24]
    if (applianceState == 1) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
  
  delay(1000); // Polling interval for real-time synchronization [cite: 26]
}