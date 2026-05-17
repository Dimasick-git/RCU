#pragma once

#include <cstdint>
#include <string>
#include "../../common/include/common.hpp"

namespace board {

class BoardSensor {
public:
    BoardSensor();
    ~BoardSensor();

    bool initialize();
    void shutdown();

    float getTemperature() const;
    float getVoltage() const;
    float getCurrent() const;

    std::string getBoardId() const;
    std::string getFirmwareVersion() const;

private:
    bool initialized_;
    float temperature_;
    float voltage_;
    float current_;
    std::string board_id_;
    std::string firmware_version_;
};

} // namespace board
