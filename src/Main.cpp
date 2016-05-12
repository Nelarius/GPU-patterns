#include "ImGuiRenderer.h"
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "Window.h"
#include "common/Array.h"
#include "common/Clock.h"
#include "common/File.h"
#include "common/Log.h"
#include "math/Vector.h"
#include "opengl/Program.h"
#include "opengl/Shader.h"
#include "opengl/VertexArrayObject.h"
#include "opengl/VertexArrayObjectFactory.h"
#include "json11/json11.hpp"
#include <SDL_events.h>
#include <GL/glew.h>

#include <cstdlib>

using narwhal::Vec3f;
using narwhal::Mat4f;

void ui() {
    ImGui::Begin("Test window");
    if (ImGui::TreeNode("Settings")) {
        // asdf asdf
        ImGui::TreePop();
    }
    ImGui::End();
}

struct Triangle {
    GLuint i, j, k;
};

struct GridVisShader {
    static const GLuint a_vertex{ 1u };
};

struct Vertex {
    narwhal::Vec3f position;
    float          scalar;
};

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

    bool showGui = false;
    narwhal::KeyboardManager keyboard;
    keyboard.registerKeyUpCallback(narwhal::KeyF1, [&]() -> void {
        if (showGui)
            showGui = false;
        else
            showGui = true;
    });

    narwhal::Window window{ settings };
    narwhal::ImGuiRenderer imgui{ window };

    narwhal::MouseManager mouse;
    mouse.setPressCallback(narwhal::MouseButton::Left, [&]() -> void {
        imgui.mouseButtonPressed(int(narwhal::MouseButton::Left));
    });
    mouse.setReleaseCallback(narwhal::MouseButton::Left, [&]() -> void {
        imgui.mouseButtonReleased(int(narwhal::MouseButton::Left));
    });

    //TODO: get rid of hard-coded constants
    const int resx = 300;
    const int resy = 225;
    const float h = 0.1f;
    /***
    *       ______           __
    *      / __/ /  ___ ____/ /__ _______
    *     _\ \/ _ \/ _ `/ _  / -_) __(_-<
    *    /___/_//_/\_,_/\_,_/\__/_/ /___/
    *
    */
    narwhal::Program gridVisShader, clearShader, computeShader;
    {
        narwhal::DynamicArray<narwhal::Shader> shaders;
        shaders.emplaceBack(narwhal::fileToString("data/grid.vert.glsl"), GL_VERTEX_SHADER);
        shaders.emplaceBack(narwhal::fileToString("data/grid.frag.glsl"), GL_FRAGMENT_SHADER);
        gridVisShader.link(shaders);
        shaders.clear();

        shaders.emplaceBack(narwhal::fileToString("data/clear.comp.glsl"), GL_COMPUTE_SHADER);
        clearShader.link(shaders);
        shaders.clear();
    }

    /***
    *       ___       ______
    *      / _ )__ __/ _/ _/__ _______
    *     / _  / // / _/ _/ -_) __(_-<
    *    /____/\_,_/_//_/ \__/_/ /___/
    *
    */
    narwhal::DynamicArray<Triangle> indices(resx*resy);
    narwhal::BufferObject indexBuffer{ GL_ELEMENT_ARRAY_BUFFER };
    narwhal::BufferObject vertexBuffer{ GL_ARRAY_BUFFER };
    GLuint gridArray = 0u;
    {
        const GLuint m = resx;
        const GLuint n = resy;

        for (GLuint i = 0; i < n - 1u; ++i) {
            for (GLuint j = 0; j < m - 1u; ++j) {
                indices.pushBack(Triangle{ m*i + j,      m*i + j + 1u,  m*(i+1u) + j });
                indices.pushBack(Triangle{ m*(i+1u) + j, m*i + j + 1u,  m*(i+1u) + j + 1u });
            }
        }
        indexBuffer.dataStore(indices.size()*3, sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        narwhal::DynamicArray<Vertex> vertices(m*n);
        const float max = float(m + n);
        for (GLuint i = 0; i < n; ++i) {
            for (GLuint j = 0; j < m; ++j) {
                vertices.pushBack(Vertex{ Vec3f{h*j, h*i, -1.f}, (i + j) / max });
            }
        }
        NARWHAL_ASSERT(vertices.size() != 0u);
        vertexBuffer.dataStore(vertices.size(), sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        vertexBuffer.bind();
        glGenVertexArrays(1, &gridArray);
        NARWHAL_ASSERT(gridArray != 0u);
        glBindVertexArray(gridArray);
        glVertexAttribPointer(GridVisShader::a_vertex, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(GridVisShader::a_vertex);
        glBindVertexArray(0u);
        vertexBuffer.unbind();
    }

    /***
    *       __  ___     __      _
    *      /  |/  /__ _/ /_____(_)______ ___
    *     / /|_/ / _ `/ __/ __/ / __/ -_|_-<
    *    /_/  /_/\_,_/\__/_/ /_/\__/\__/___/
    *
    */
    Mat4f ortho = Mat4f::orthographic(h*resx-h, h*resy-h, 0.f, 2.f);
    Mat4f model = Mat4f::translation(Vec3f(-0.5f*h*resx, -0.5*h*resy, 0.f));

    /***
    *       __
    *      / /  ___  ___  ___
    *     / /__/ _ \/ _ \/ _ \
    *    /____/\___/\___/ .__/
    *                  /_/
    */
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

        // computation magic goes here

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        imgui.render();

        // rendering magic goes here
        glBindVertexArray(gridArray);

        gridVisShader.use();
        gridVisShader.setUniform("u_PV", ortho);
        gridVisShader.setUniform("u_M", model);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

        glBindVertexArray(0u);
        gridVisShader.stopUsing();

        window.display();
    }

    glDeleteVertexArrays(1, &gridArray);

    return 0;
}
