#include <Arduino.h>

#include "controller.h"

LastTempQueue::LastTempQueue(int size)
        : m_queueLength(size)
{
        m_temps = new temp_t[m_queueLength];
        m_hums = new hum_t[m_queueLength];
}


LastTempQueue::~LastTempQueue()
{
        delete[] m_temps;
        delete[] m_hums;
}

void LastTempQueue::incrementPos(int &pos)
{
        pos = (pos + 1 >= m_queueLength) ? 0 : pos + 1;
}

LastTempQueue::AddEntry(temp_t temp, hum_t hum)
{
        m_temps[m_queueEnd] = temp;
        m_hums[m_queueEnd] = hum;
        incrementPos(m_queueEnd);  
        if (m_queueEnd == m_queuePos)
                incrementPos(m_queuePos);

}

temp_t LastTempQueue::GetAverageTemp()
{
        int beg = m_queuePos;
        int end = m_queueEnd;
        int length = 0;
        float sum = 0;
        while (beg != end) {
                sum += m_temps[beg];
                length += 1;
                incrementPos(beg);
        }
        if (length != 0)
                return (int)(sum / length);
        else
                return 0;
}


hum_t LastTempQueue::GetAverageHum()
{
        int beg = m_queuePos;
        int end = m_queueEnd;
        int length = 0;
        float sum = 0;
        while (beg != end) {
                sum += m_hums[beg];
                length += 1;
                incrementPos(beg);
        }
        if (length != 0)
                return (int)(sum / length);
        else
                return 0;

}

Controller::Controller()
        : m_dht(DHTPIN, DHTTYPE),
        m_queue(10)
{
        pinMode(Relay1, OUTPUT);
        pinMode(Relay2, OUTPUT);

        m_dht.begin();
        m_millisLast = millis();
}


hum_t Controller::getHum()
{
        return m_queue.GetAverageHum();
}


temp_t Controller::getTemp()
{
        return m_queue.GetAverageTemp();
}


void Controller::setTarget(int temp, int hum)
{
        m_targetTemp = temp;
        m_targeHumm = hum;
}


bool Controller::state()
{
        return m_noError;
}

void Controller::wait()
{
        unsigned long millisCurr = millis();
        if (millisCurr - m_millisLast <= m_kMillisInVsec)
                delay(m_kMillisInVsec - millisCurr + m_millisLast);
        else                
                delay(m_kMillisInVsec);                        
        m_vTime = (m_vTime >= m_kVPeriodMax) ? 0 : m_vTime + 1;

        float humi = m_dht.readHumidity();
        float temp = m_dht.readTemperature();
        if (!isnan(humi) && !isnan(temp)) {
                m_queue.AddEntry((int)temp, (int)humi);
        } else
                m_noError = false;

        if (getHum() < m_targeHumm)
                digitalWrite(Relay2, HIGH);
        else if (getHum() > m_targeHumm)
                digitalWrite(Relay2, LOW);

        if (getTemp() < m_targetTemp)
                digitalWrite(Relay1, HIGH);
        else if (getHum() > m_targetTemp)
                digitalWrite(Relay1, LOW);

        m_millisLast = millis();
}
