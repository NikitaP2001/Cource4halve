#include <SPI.h>
#include <Ethernet.h>

#include "myClient.h"
#include "controller.h"

myClient *cli;
Controller *cnt;

void setup()
{
        // Open serial communications and wait for port to open:
        Serial.begin(9600);
        while (!Serial) {
                delay(200);
        }

        cli = new myClient;
        cnt = new Controller();
}

static int getHum(String &msg)
{
        int i = 0;
        while (msg[i] != ' ')
                i++;
        while (msg[i] == ' ')
                i++;
        String strHum = msg.substring(i, (int)(msg.end() - msg.begin()));
        return atoi(strHum.c_str());
}

static int getTemp(String &msg)
{
        int i = 0;
        while (msg[i] != ' ')
                i++;
        String strTemp = msg.substring(0, i);
        return atoi(strTemp.c_str());
}

void loop()
{
        String str;
        int hum = 0;
        int temp = 0;
        
        while (true) {
                Ethernet.maintain();                                
                if (cli->read(str)) {
                        hum = getHum(str);
                        temp = getTemp(str);
                        
                }                              
                
                str = cnt->getTemp();
                str += " ";
                str += cnt->getHum();
                cli->write(str);
                Serial.println(str);

                if (!cnt->state()) {
                        Serial.println("error detected, wait restart");
                        break;
                }

                cnt->wait();                      
        }
        
        delete cli;
        delete cnt;
        while (true)
                delay(200);
}
