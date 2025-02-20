/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * 
 * Blinker is a cross-hardware, cross-platform solution for the IoT. 
 * It provides APP, device and server support, 
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields 
 * to help users build Internet of Things projects better and faster.
 * 
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 * 
 * Make sure installed 1.0.5 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * Docs: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************
 * 
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 * 
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 * 
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.5 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * 文档: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************/

#define BLINKER_WIFI
//#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include <HardwareSerial.h>
#include "ESP32_CAM_SERVER.h"
//#include "WiFi.h"


#define DATA_SIZE 26    // 26 bytes is a lower than RX FIFO size (127 bytes)
#define BAUD      9600  // Any baudrate from 300 to 115200
#define TEST_UART 1     // Serial1 will be used for the loopback testing with different RX FIFO FULL values
#define RXPIN     4     // GPIO 4 => RX for Serial1
#define TXPIN     5     // GPIO 5 => TX for Serial1

char auth[] = "439dd30522d3";
char ssid[]="Roxy";//WiFi名称
char pswd[]="153153153";//wifi密码
bool setup_camera = false;

unsigned char msg[10]={'A',0,0,0,0,0,0,0,'B',0};

BlinkerButton Button_forward("btn-forward");
BlinkerButton Button_backward("btn-backward");
BlinkerButton Button_stop("btn-stop");
BlinkerText Txt_ip("txt-ip");

BlinkerSlider Slider1("ran-1");
BlinkerSlider Slider2("ran-2");
BlinkerSlider Slider3("ran-3");

HardwareSerial SerialPort(1);

unsigned int g1=50, g2=50, g3=50;
unsigned char s = 0;

int i = 0;

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}

void button_forward_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, 1);
    s = 1;
}

void button_backward_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, 1);
    s = 2;
}

void button_stop_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, 0);
    s = 0;
}

void slider1_callback(int32_t value)
{
  g1 = value;
}

void slider2_callback(int32_t value)
{
  g2 = value;
}

void slider3_callback(int32_t value)
{
  g3 = value;
}




void setup()
{
    Serial.begin(115200);
    SerialPort.begin(57600, SERIAL_8N1, 13, 15); 
    BLINKER_DEBUG.stream(Serial);

    Button_forward.attach(button_forward_callback);
    Button_backward.attach(button_backward_callback);
    Button_stop.attach(button_stop_callback);

    Slider1.attach(slider1_callback);
    Slider2.attach(slider2_callback);
    Slider3.attach(slider3_callback);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Blinker.begin(auth,ssid,pswd);
    Blinker.attachData(dataRead);
}

void loop()
{
    Blinker.run();
  
    msg[1] = g1 / 256;
    msg[2] = g1 % 256;
    msg[3] = g2 / 256;
    msg[4] = g2 % 256;
    msg[5] = g3 / 256;
    msg[6] = g3 % 256;
    msg[7] = s;

    for(i = 0; i < 10; i ++) {
      SerialPort.write(msg[i]);
    }

    if (Blinker.connected() && !setup_camera)
    {
        setupCamera();
        setup_camera = true;
//        String ip = String()+ WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
//        Serial.println(Txt_ip, ip);
        Txt_ip.print(WiFi.localIP().toString());
        Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
    }
}
