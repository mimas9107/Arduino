#include <string.h>

// 感測器和繼電器腳位定義
const int WATER_LEVEL_PIN = A0;      // 水位感測器類比輸入腳位
const int SOIL_MOISTURE_PIN = A1;    // 土壤濕度感測器類比輸入腳位
const int RELAY_PIN = 9;             // 繼電器數位輸出腳位

// 澆水系統設定參數
const int WATERING_TIME = 5;         // 澆水時間 (秒)
const int SOIL_MOISTURE_THRESHOLD = 45; // 土壤濕度閾值 (%)
const int WATER_LEVEL_THRESHOLD = 35;  // 水位閾值 (%)
const unsigned long DETECTION_INTERVAL = 180000; // 檢測間隔 (毫秒)
const unsigned long STANDBY_INTERVAL = DETECTION_INTERVAL; // 待機間隔 (毫秒)
const unsigned long MAX_PUMP_RUN_TIME = 30000; // 水泵最大連續運轉時間 (毫秒)
const int MAX_PUMPING_TIMES = 6;      // 水泵最大運轉次數
const int LIMITED_PUMPING_TIMES = 12;  // 水泵極限運轉次數

// 變數宣告
int waterLevel;                     // 水位百分比
int soilMoisture;                   // 土壤濕度百分比
String outputMessage;                // 序列埠輸出訊息
unsigned long pumpStartTime = 0;     // 水泵開始運轉時間
int pumpingTimes = 0;                // 水泵運轉次數

// setup() 函式：初始化設定
void setup() {
  Serial.begin(9600);                // 初始化序列埠通訊
  pinMode(RELAY_PIN, OUTPUT);        // 設定繼電器腳位為輸出模式
  digitalWrite(RELAY_PIN, LOW);       // 初始化時關閉繼電器
  Serial.println("Arduino Nano Planting System Starting!"); // 輸出啟動訊息
}

// loop() 函式：主程式迴圈
void loop() {
  readSensors();                     // 讀取感測器數值
  printSensorData();                 // 序列埠輸出感測器數值
  controlWaterPump();                // 控制水泵
  delay(100);                        // 延遲 100 毫秒
  outputMessage = "";                // 清空輸出訊息
}

// readSensors() 函式：讀取感測器數值
void readSensors() {
  waterLevel = map(analogRead(WATER_LEVEL_PIN), 0, 670, 0, 100); // 讀取水位並映射到 0-100
  soilMoisture = map(analogRead(SOIL_MOISTURE_PIN), 0, 670, 0, 100); // 讀取土壤濕度並映射到 0-100
}

// printSensorData() 函式：序列埠輸出感測器數值
void printSensorData() {
  outputMessage = "水位: " + String(waterLevel) + "%, 土壤濕度: " + String(soilMoisture) + "%";
  Serial.println(outputMessage);
}

// controlWaterPump() 函式：控制水泵
void controlWaterPump() {
  if (waterLevel > WATER_LEVEL_THRESHOLD) { // 如果水位高於閾值
    if (soilMoisture < SOIL_MOISTURE_THRESHOLD) { // 如果土壤濕度低於閾值
      if (digitalRead(RELAY_PIN) == LOW) { // 如果水泵關閉，則啟動
        digitalWrite(RELAY_PIN, HIGH);
        pumpStartTime = millis(); // 記錄水泵開始運轉時間
        Serial.println("正在澆水...");
      }

      if (millis() - pumpStartTime >= MAX_PUMP_RUN_TIME) { // 檢查水泵連續運轉時間
        digitalWrite(RELAY_PIN, LOW); // 強制關閉水泵
        Serial.println("水泵連續運轉時間過長，已關閉！");
      } else {
        delay(WATERING_TIME * 1000); // 澆水 wateringtime 秒
        pumpingTimes++; // 累加水泵運轉次數

        if (pumpingTimes > MAX_PUMPING_TIMES && pumpingTimes < LIMITED_PUMPING_TIMES) {
          digitalWrite(RELAY_PIN, LOW); // 關閉水泵
          delay(10000); // 延長延遲時間，防止水泵過度運轉
          Serial.println("水泵運轉次數過多，暫停澆水！");
        } else if (pumpingTimes >= LIMITED_PUMPING_TIMES) {
          digitalWrite(RELAY_PIN, LOW); // 強制關閉水泵
          Serial.println("水泵運轉次數達到上限，系統停止澆水！");
          pumpingTimes = 0; // 重置水泵運轉次數
          // 加入更強硬的保護機制，例如停止系統運作
        }
      }
    } else { // 如果土壤濕度高於閾值
      digitalWrite(RELAY_PIN, LOW); // 關閉水泵
      Serial.println("土壤濕度足夠，待機 " + String(STANDBY_INTERVAL / 1000) + " 秒");
      delay(STANDBY_INTERVAL);
      pumpingTimes = 0; // 重置水泵運轉次數
    }
  } else { // 如果水位低於閾值
    digitalWrite(RELAY_PIN, LOW); // 關閉水泵
    Serial.println("水位過低，請加水！");
    delay(DETECTION_INTERVAL / 30); // 延遲一段時間
  }
}