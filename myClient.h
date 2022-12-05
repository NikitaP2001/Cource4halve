#include <Ethernet.h>

const char serverName[] = "192.168.1.5";

class myClient {

public:
        myClient();
        ~myClient();
        int read(String &str);
        int write(String &str);

        void connect();        
private:

        const int k_mMaxMsgLen = 100;

        const byte m_mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

        const uint16_t m_port = 1337;

        EthernetClient m_client;

};
