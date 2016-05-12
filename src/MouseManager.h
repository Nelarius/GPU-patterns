#pragma once

#include "math/Vector.h"
#include <utility>
#include <unordered_map>
#include <functional>

namespace narwhal {

/*
* These enumerations correspond to SDL_BUTTON_* values,
* defined in http://hg.libsdl.org/SDL/file/default/include/SDL_mouse.h
*/
enum MouseButton {
    Left = 1,
    Middle,
    Right
};

/**
* @brief Bind commands to mouse events.
* Internally uses SDL. Mouse button ID's are denoted by integers. Get the corresponding value
* by doing SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT. Get the correct mask by
* writing SDL_BUTTON( SDL_BUTTON_LEFT ), etc.
*/
class MouseManager {
    using Command = std::function<void()>;
public:
    MouseManager() = default;
    ~MouseManager() = default;
    /*
    * @brief set a callback command for when a mouse button is pressed.
    * @param button The SDL button id defined in SDL_mouse.h
    * @param command The command
    **/
    void setPressCallback(MouseButton button, const Command& command);
    /*
    * @brief set a callback command for when a mouse button is released.
    * @param button The SDL button id defined in SDL_mouse.h
    * @param command The command
    **/
    void setReleaseCallback(MouseButton button, const Command& command);
    void update();

    Vec2i coords() const;

private:
    uint32_t    previousState_{ 0u }; // the mouse state mask
    Vec2i       previousCoords_{};
    Vec2i       currentCoords_{};
    std::unordered_map<int, Command>  pressed_{};
    std::unordered_map<int, Command>  released_{};
};

}
