#ifndef IOP_LIB_SENSORS_WATER_PUMP_HPP
#define IOP_LIB_SENSORS_WATER_PUMP_HPP

#include <relay.hpp>
#include <Arduino.h>

namespace relay {
class WaterPump {
    relay::Relay relay;
    std::unordered_map<Moment, iop::time::seconds> states;
    std::optional<Moment> current;
    std::optional<Moment> next;
    bool isNextTomorrow;
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

    auto actIfNeeded() noexcept -> bool {
        if (!this->next) return false;

        const auto now = Moment::now();
        if (now < this->next && !(this->current && this->isNextTomorrow && now < this->current)) return false;
        this->current = now;
        this->isNextTomorrow = false;

        bool found;
        std::optional<Moment> next;
        std::optional<Moment> first;
        for (const auto & [moment, time]: this->states) {
            if (!first) first = moment;

            if (found) {
                next = moment;
                break;
            } else if (this->next == moment) {
                this->turnOnFor(time);
                found = true;
            }
        }

        if (!next.has_value() && first) {
            next = first;
            this->isNextTomorrow = true;
        }
        this->next = next;

        return true;
    }
};
}

#endif

