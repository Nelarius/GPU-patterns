#include "ImGuiRenderer.h"
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "Window.h"
#include "common/Clock.h"
#include "common/File.h"
#include "common/Log.h"
#include "math/Vector.h"
#include "json11/json11.hpp"
#include <SDL_events.h>

//void ui(float& width, float& height, float& near, float& far, narwhal::Vec3f& view) {
//    ImGui::Begin("Test window");
//    if (ImGui::TreeNode("Render volume settings")) {
//        ImGui::SliderFloat("width", &width, 0.001f, 1.f);
//        ImGui::SliderFloat("height", &height, 0.001f, 1.f);
//        ImGui::SliderFloat("near", &near, 0.0001f, 0.1f);
//        ImGui::SliderFloat("far", &far, 0.01f, 10.f);
//        ImGui::SliderFloat("x", &view.x, -0.1f, 0.1f);
//        ImGui::SliderFloat("y", &view.y, -0.1f, 0.1f);
//        ImGui::SliderFloat("z", &view.z, -0.1f, 0.1f);
//        ImGui::TreePop();
//    }
//    ImGui::End();
//}

void ui() {
    ImGui::Begin("Test window");
    if (ImGui::TreeNode("Settings")) {
        // asdf asdf
        ImGui::TreePop();
    }
    ImGui::End();
}

int main() {
    auto config = narwhal::fileToString("config.json");
    std::string error{ "" };
    auto json = json11::Json::parse(config, error).object_items();

    narwhal::WindowSettings settings;
    settings.glMajor = int(json["opengl_major"].number_value());
    settings.glMinor = int(json["opengl_minor"].number_value());
    settings.width = int(json["window_width"].number_value());
    settings.height = int(json["window_height"].number_value());
    settings.name = json["window_name"].string_value();

    narwhal::Window window{ settings };
    narwhal::ImGuiRenderer imgui{ window };

    narwhal::MouseManager mouse;
    mouse.setPressCallback(narwhal::MouseButton::Left, [&]() -> void {
        imgui.mouseButtonPressed(int(narwhal::MouseButton::Left));
    });
    mouse.setReleaseCallback(narwhal::MouseButton::Left, [&]() -> void {
        imgui.mouseButtonReleased(int(narwhal::MouseButton::Left));
    });

    /***
    *       ______           __
    *      / __/ /  ___ ____/ /__ _______
    *     _\ \/ _ \/ _ `/ _  / -_) __(_-<
    *    /___/_//_/\_,_/\_,_/\__/_/ /___/
    *
    */

    /***
    *       ___       ______
    *      / _ )__ __/ _/ _/__ _______
    *     / _  / // / _/ _/ -_) __(_-<
    *    /____/\_,_/_//_/ \__/_/ /___/
    *
    */

    bool showGui = false;

    narwhal::KeyboardManager keyboard;
    keyboard.registerKeyUpCallback(narwhal::KeyF1, [&]() -> void {
        if (showGui)
            showGui = false;
        else
            showGui = true;
    });

    bool running = true;
    narwhal::Clock clock;
    while (running) {
        clock.update();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            keyboard.handleEvent(event);
        }
        mouse.update();
        imgui.newFrame(clock.deltaSeconds(), mouse.coords().x, mouse.coords().y);

        if (showGui) {
            ui();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        imgui.render();
        window.display();
    }

    return 0;
}
