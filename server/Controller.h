#pragma once

class Controller {
friend class Connection;
public:

        Controller() = default;

        void setTargetTemp(int temp);
        void setTargetHum(int hum);

        int getTemp();
        int getHum();

private:
        int m_temp = 0;
        int m_hum = 0;

        int m_targetTemp = 0;
        int m_targetHum = 0;

};