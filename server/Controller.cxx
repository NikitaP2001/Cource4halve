#include "Controller.h"


void Controller::setTargetTemp(int temp)
{
        m_targetTemp = temp;
}


void Controller::setTargetHum(int hum)
{
        m_targetHum = hum;
}


int Controller::getTemp()
{
        return m_temp;
}


int Controller::getHum()
{
        return m_hum;
}
