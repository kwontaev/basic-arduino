#include <HardwareSerial.h>
#include "DHT.h"

#define DHTPIN 6     // DHT11 연결 핀
#define DHTTYPE DHT11 // DHT11 센서 타입
DHT dht(DHTPIN, DHTTYPE);

HardwareSerial &SerialAT = Serial1; // BG96 모뎀과의 통신을 위한 하드웨어 직렬 포트

// HardwareSerial &modemSerial = Serial1;
//const int BG96_TX = 17; // ESP32의 BG96 TX 핀
//const int BG96_RX = 16; // ESP32의 BG96 RX 핀



void setup() {
    Serial.begin(115200); // 디버그를 위한 시리얼 모니터
    SerialAT.begin(115200); // BG96 모뎀과의 통신 설정
    dht.begin();

    // BG96 모뎀 초기화 및 네트워크 연결 설정
    initializeModem();

    // 데이터 전송 및 수신 주기
    sendData();
    receiveData();
}

void loop() {
    delay(10000); // 10초 대기 후 데이터 재전송 및 재수신
    sendData();
    receiveData();
}

void initializeModem() {
    sendATCommand("AT", 2000);
    sendATCommand("AT+CPIN?", 5000); // SIM 카드 상태 확인
    sendATCommand("AT+CGATT=1", 10000); // 네트워크에 연결
    //sendATCommand("AT+QICSGP=1,1,\"lte.sktelecom.com\",\"\",\"\",1", 5000); // APN 설정
    sendATCommand("AT+CGDCONT?", 10000); // apn체크 
    //sendATCommand("AT+CGDCONT=1,\“IPV4V6\”,\”lte-internet.sktelecom.com\”", 5000); // APN 설정
    sendATCommand("AT+IPR?", 10000); // apn체크 
    //sendATCommand("AT+IPR=115200", 10000); // APN 설정
    //sendATCommand("AT+QIACT=1", 10000); // 데이터 연결 활성화
}

void sendData() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    
    String jsonBody = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    String httpRequest = "AT+HTTP_SEND=1,0,0,\"http://192.168.0.2/submit_json_data.php\",\"-H 'Content-Type: application/json'\",\"" + jsonBody + "\"";
    sendATCommand(httpRequest.c_str(), 5000); // HTTP POST 요청

  
//    String httpRequestData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
//    sendATCommand("AT+HTTP_SEND=1,0,0,\"http://192.168.0.2/submit_data.php\",\"\",\"" + httpRequestData + "\"", 5000); // HTTP POST 요청
    
    //String httpRequestData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
    //sendATCommand("AT+HTTP_SEND=1,0,0,http://192.168.0.2/submit_data.php", 5000); // URL 설정 모드 진입
    //sendATCommand("http://192.168.0.2/submit_data.php", 5000); // URL 전송
    //sendATCommand(",,,", 10000); // HTTP POST 요청 시작
/*  sendATCommand("AT+QHTTPURL=30,60", 5000); // URL 설정 모드 진입
    sendATCommand("http://192.168.0.2/submit_data.php", 5000); // URL 전송
    sendATCommand("AT+HTTP_SEND=60", 10000); // HTTP POST 요청 시작
*/    
}

void receiveData() {

    
    String httpRequest = "AT+HTTP_SEND=0,0,0,\"http://192.168.0.2/read_json_data.php\"";
    sendATCommand(httpRequest.c_str(), 5000); // HTTP GET 요청

    //sendATCommand("AT+HTTP_SEND=0,0,0,\"http://192.168.0.18/read_data.php\"", 5000); // HTTP GET 요청
    //sendATCommand("AT+HTTP_SEND=0,0,0,http://192.168.0.2/read_data.php", 5000); // URL 설정 모드 진입
    //sendATCommand("http://192.168.0.2/read_data.php", 5000); // URL 전송
    //sendATCommand(",,,", 10000); // HTTP GET 요청 시작
/*
    sendATCommand("AT+QHTTPURL=30,60", 5000); // URL 설정 모드 진입
    sendATCommand("http://192.168.0.2/read_data.php", 5000); // URL 전송
    sendATCommand("AT+QHTTPGET=60", 10000); // HTTP GET 요청 시작
*/
}

void sendATCommand(const char *command, int timeout) {
    SerialAT.println(command); // 모뎀에 AT 명령어 전송
    long int time = millis();

    while ((time + timeout) > millis()) {
        while (SerialAT.available()) {
            char c = SerialAT.read();
            Serial.write(c); // 모뎀으로부터의 응답을 시리얼 모니터에 출력
        }
    }
    Serial.println();
}
