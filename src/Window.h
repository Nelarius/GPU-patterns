#pragma once

#include <SDL_video.h>
#include <string>

namespace narwhal {

struct WindowSettings {
    WindowSettings() = default;
    ~WindowSettings() = default;

    int width{ 800 };
    int height{ 600 };
    std::string name{ "Narwhal window" };
    int glMajor{ 4 };
    int glMinor{ 3 };
    int stencilBits{ 8 };
    int depthBits{ 24 };
    int multisampleBuffer{ 1 };
    int multisampleSamples{ 4 };
    typedef struct {
        float r, g, b, a;
    } Color;
    Color clearColor{ 0.1f, 0.1f, 0.1f, 1.0f };
};

/// @brief A platform-independent window.
class Window {
public:
    /// @brief Construct the window.
    /// The OpenGL context and window are ready for use after construction.
    Window(const WindowSettings& settings);
    ~Window();
    Window()                         = delete;
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&)                 = delete;
    Window& operator=(Window&&)      = delete;

    /// @brief Display the window and any changes made to the back buffer.
    void display();

    unsigned width() const;
    unsigned height() const;

    /**
     * @brief Get the underlying SDL window.
     * @return
     * This pointer is managed and should not be deleted!
     */
    SDL_Window* SDLwindow();

private:
    void initialize_();
    void initializeSDL_();
    void initializeOpenGL_();
    void initializeValues_(const WindowSettings& settings);

    SDL_Window*     window_;
    SDL_GLContext   glContext_;
    std::string     name_;
    int             width_;
    int             height_;
    int             glMajor_;
    int             glMinor_;
    int             stencilBits_;
    int             depthBits_;
    int             msBuffer_;  // multisample buffer
    int             msSamples_; // number of multisamples
    float           clearColorR_;
    float           clearColorB_;
    float           clearColorG_;
    float           clearColorA_;

};

}
