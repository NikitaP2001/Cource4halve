#include <SPI.h>

#include "myClient.h"


int myClient::read(String &str)
{
        int bytesRead = 0;
        char c = 0;
        str = "";
        if (m_client.connected()) {

                if (m_client.available())
                        while ((c = m_client.read()) != 0) {
                                bytesRead += 1;
                                str += c;
                                if (str.length() >= k_mMaxMsgLen)
                                        break;
                        }
                          
        } else {
                Serial.println("read: not connected, reconect");
                connect();
        }
        return bytesRead;
}

int myClient::write(String &str)
{
        bool bytesWritten = 0;

        if (m_client.connected()) {
                bytesWritten = m_client.write(str.c_str(), str.length() + 1);
        } else  {
                Serial.println("write: not connected, reconect");
                connect();
        }
        return bytesWritten;
}

void myClient::connect()
{
        m_client.stop();
        while (!m_client.connect(serverName, m_port)) {
                Serial.println("Connecting");
                delay(3000);
        }
        m_client.write("Device", strlen("Device") + 1);        
}

myClient::myClient()
{
        if (Ethernet.begin(m_mac) == 0) {
                // Check for Ethernet hardware present
                if (Ethernet.hardwareStatus() == EthernetNoHardware) {
                        Serial.println("[-] ethernet shield was not found");
                        while (true)
                                delay(1);
                } else if (Ethernet.linkStatus() == LinkOFF) {
                        Serial.println("[w] ethernet cable is not connected.");
                        delay(500);
                }  
        }  
        connect();
        
        Serial.print("[+] connected to ");
        m_client.remoteIP().printTo(Serial); 
        Serial.println();
}

myClient::~myClient()
{
        m_client.stop();
}
