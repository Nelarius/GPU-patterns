#include "Clock.h"
#include <SDL_timer.h>

namespace narwhal {

Clock::Clock()
    : timeDeltaMilli_{ 16u },
    currentTimeMilli_{ 0u },
    elapsedTimeMilli_{ 0u },
    scale_{ 1.f },
    ToSeconds_{ 1.f / 1000.f },
    ToMinutes_{ 1.f / 60000.f },
    ToHours_{ 1.f / 3600000.f } {

    currentTimeMilli_ = SDL_GetTicks();
}

void Clock::scale(float s) {
    scale_ = s;
}

float Clock::deltaSeconds() const {
    return ToSeconds_ * timeDeltaMilli_;
}

float Clock::elapsedSeconds() const {
    return ToSeconds_ * SDL_GetTicks();
}

float Clock::elapsedMinutes() const {
    return ToMinutes_ * SDL_GetTicks();
}

float Clock::elapsedHours() const {
    return ToHours_ * SDL_GetTicks();
}

void Clock::update() {
    timeDeltaMilli_ = float(SDL_GetTicks() - currentTimeMilli_);
    currentTimeMilli_ = SDL_GetTicks();
}

}
