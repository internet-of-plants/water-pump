#ifndef IOP_LIB_SENSORS_WATER_PUMP_ARDUINO_HPP
#define IOP_LIB_SENSORS_WATER_PUMP_ARDUINO_HPP

#include <water_pump.hpp>

namespace relay {
auto WaterPump::actIfNeeded() noexcept -> bool {
    if (!this->next) return false;
    if (Moment::now() < this->next) return false;

    std::optional<Moment> next;
    for (const auto & [moment, time]: this->states) {
        if (this->next == moment) {
            this->turnOnFor(time);
        }
        if (moment > *this->next && (!next || moment < *next)) {
            next = moment;
        }
    }
    this->next = next;

    return true;
}
}

#endif