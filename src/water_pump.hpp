#ifndef IOP_LIB_SENSORS_WATER_PUMP_HPP
#define IOP_LIB_SENSORS_WATER_PUMP_HPP

#include <relay.hpp>

namespace relay {
class WaterPump {
    relay::Relay relay;
    std::unordered_map<Moment, iop::time::seconds> states;
    std::optional<Moment> next;
public:
    WaterPump(const iop_hal::PinRaw pin) noexcept: relay(pin) {}
    WaterPump(relay::Relay relay) noexcept: relay(std::move(relay)) {}
    
    auto begin() const noexcept -> void {
        this->relay.begin();
    }

    auto clear() noexcept -> void {
        this->states.clear();
        this->next = std::nullopt;
    }

    auto setTime(const Moment moment, const iop::time::seconds time) noexcept -> void {
        if (Moment::now() < moment && (!this->next || *this->next > moment)) {
            this->next = moment;
        }

        this->states.insert_or_assign(moment, time);
    }

    auto turnOnFor(const iop::time::seconds time) const noexcept -> void {
        this->relay.setHighFor(time);
    }

    auto actIfNeeded() noexcept -> bool;
};
}

#endif