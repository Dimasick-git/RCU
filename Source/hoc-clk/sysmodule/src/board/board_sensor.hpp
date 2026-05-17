#pragma once

#include "common.hpp"

namespace board {

class BoardSensor {
public:
    BoardSensor();
    ~BoardSensor();

    bool initialize();
    void update();

    float getTemperature() const;
    float getVoltage() const;
    float getCurrent() const;

private:
    float m_temperature;
    float m_voltage;
    float m_current;
};

} // namespace board