#ifndef IOP_LIB_SENSORS_WATER_PUMP_NOOP_HPP
#define IOP_LIB_SENSORS_WATER_PUMP_NOOP_HPP

#include <water_pump.hpp>

namespace relay {
auto WaterPump::actIfNeeded() noexcept -> bool { return false; }
}

#endif