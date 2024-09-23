#ifndef IOP_LIB_SENSORS_WATER_PUMP_HPP
#define IOP_LIB_SENSORS_WATER_PUMP_HPP

#include <relay.hpp>
#include <Arduino.h>

namespace relay {
class WaterPump {
    relay::Relay relay;
    std::unordered_map<iop_hal::Moment, iop::time::seconds> states;
    std::optional<iop_hal::Moment> current;
    std::optional<iop_hal::Moment> next;
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

    auto setTime(const iop_hal::Moment moment, const iop::time::seconds time) noexcept -> void {
        this->states.insert_or_assign(moment, time);
    }

    auto turnOnFor(const iop::time::seconds time) const noexcept -> void {
        Serial.println("ONNNNNN");
        this->relay.setHighFor(time);
        Serial.println("OFFFFFF");
    }

    auto actIfNeeded() noexcept -> bool {
        auto now = iop_hal::Moment::now();
        now.setSecond(0);

        if (!this->next) {
          for (const auto & [moment, _]: this->states) {
            if (moment >= now && (!this->next || this->next > moment)) {
              this->next = moment;
            }
          }

          if (!this->next) return false;
        }

        if (now < this->next && !(this->current && this->isNextTomorrow && now < this->current)) return false;
        this->current = now;
        this->isNextTomorrow = false;

        std::optional<iop_hal::Moment> next;
        std::optional<iop_hal::Moment> earliest;
        for (const auto & [moment, time]: this->states) {
            if (!earliest || earliest > moment) earliest = moment;

            if (moment > now && (!next || next > moment)) {
                next = moment;
            } else if (this->next == moment) {
                this->turnOnFor(time);
            }
        }

        if (!next && earliest) {
            next = earliest;
            this->isNextTomorrow = true;
        }
        this->next = next;

        return true;
    }
};
}

#endif

