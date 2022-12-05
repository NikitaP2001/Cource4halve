#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

#define Relay1 8
#define Relay2 7

typedef int temp_t ;
typedef int hum_t ;

class LastTempQueue {

public:
        LastTempQueue(int size);
        ~LastTempQueue();

        AddEntry(temp_t temp, hum_t hum);

        temp_t GetAverageTemp();
        hum_t GetAverageHum();
private:
        void incrementPos(int &pos);

private:
        int      m_queueLength = 0;
        int      m_queuePos = 0;
        int      m_queueEnd = 0;
        temp_t   *m_temps = nullptr;
        hum_t    *m_hums = nullptr;
};

class Controller {

public:

        Controller();
        ~Controller() = default;

        void setTarget(int temp, int hum);

        int getNewHum();
        int getNetTemp();

        void wait();        

        bool state();

        int getHum();
        int getTemp();

private:
        bool m_noError            = true;

        DHT m_dht;

        temp_t m_targetTemp          = 25;
        hum_t m_targeHumm           = 35;

        LastTempQueue m_queue;

        unsigned long m_vTime      = 0;
        unsigned long m_millisLast = 0;
        
        static const int m_kVPeriodMax   = 10;

        static const int m_kMillisInVsec = 5000;

};
