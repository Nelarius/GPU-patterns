#pragma once

#include <chrono>
#include <cstdint>

namespace narwhal {

// the first time delta after initialization will always be 16 ms.
class Clock {
public:
    Clock();
    ~Clock() = default;

    // scale the time delta by a scale factor
    // this will not affect the elapsed time of the clock
    void scale(float s);

    void update();
    float deltaSeconds() const;   // in seconds
    float elapsedSeconds() const;
    float elapsedMinutes() const;
    float elapsedHours() const;

private:
    float         timeDeltaMilli_;
    std::uint32_t currentTimeMilli_;
    // the number of milliseconds since the SDL library was initialized
    std::uint32_t elapsedTimeMilli_;

    float       scale_;
    const float ToSeconds_;
    const float ToMinutes_;
    const float ToHours_;
};

}
