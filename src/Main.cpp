#include "ImGuiRenderer.h"
#include "MouseManager.h"
#include "KeyboardManager.h"
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

using narwhal::Vec2f;
using narwhal::Vec3f;
using narwhal::Vec3i;
using narwhal::Vec4f;
using narwhal::Mat4f;

struct Triangle {
    GLuint i, j, k;
};

struct GridVisShader {
    static const GLuint a_vertex{ 2u };
};

struct Vertex {
    narwhal::Vec3f position;
};

struct Parameters {
    float timeFactor{0.01f};
    float diffusivity{0.0015f};
    float inhibitorCoupling{0.1f};
    float activatorCoupling{0.1f};
    float inhibitorBaseProduction{0.1f};
    float activatorBaseProduction{0.1f};
};

void ui(Parameters& params) {
    ImGui::Begin("Test window");

    ImGui::SliderFloat("time factor", &params.timeFactor, 0.01f, 5.0f);
    ImGui::SliderFloat("diffusivity", &params.diffusivity, 0.01f, 5.0f);
    ImGui::SliderFloat("activator base production", &params.activatorBaseProduction, 0.01f, 5.f);

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
    // these must be divisible by four
    const unsigned int resx = 900;
    const unsigned int resy = 1200;
    const float diffusivity = 0.0016f;
    const float h = 0.01f;
    const float inhCoupling = 0.1f;
    const float actCoupling = 0.1f;
    const float inhBaseProduction = 0.1f;
    const float actBaseProduction = 0.1f;
    const float timeFactor = 0.01f;
    /***
    *       ______           __
    *      / __/ /  ___ ____/ /__ _______
    *     _\ \/ _ \/ _ `/ _  / -_) __(_-<
    *    /___/_//_/\_,_/\_,_/\__/_/ /___/
    *
    */
    narwhal::Program gridVisShader, clearShader, bulkShader, synchronizeShader;
    {
        narwhal::DynamicArray<narwhal::Shader> shaders;
        shaders.emplaceBack(narwhal::fileToString("data/grid.vert.glsl"), GL_VERTEX_SHADER);
        shaders.emplaceBack(narwhal::fileToString("data/grid.frag.glsl"), GL_FRAGMENT_SHADER);
        gridVisShader.link(shaders);
        shaders.clear();

        shaders.emplaceBack(narwhal::fileToString("data/bulk.comp.glsl"), GL_COMPUTE_SHADER);
        bulkShader.link(shaders);
        shaders.clear();

        shaders.emplaceBack(narwhal::fileToString("data/synchronize.comp.glsl"), GL_COMPUTE_SHADER);
        synchronizeShader.link(shaders);
        shaders.clear();
    }

    /***
    *       ___       ______
    *      / _ )__ __/ _/ _/__ _______
    *     / _  / // / _/ _/ -_) __(_-<
    *    /____/\_,_/_//_/ \__/_/ /___/
    *
    */
    narwhal::BufferObject indexBuffer{ GL_ELEMENT_ARRAY_BUFFER };
    narwhal::BufferObject rdBuffer{ GL_ARRAY_BUFFER };
    narwhal::BufferObject rdBufferPrevious{ GL_ARRAY_BUFFER };
    narwhal::Texture rdTexture{ GL_TEXTURE_BUFFER };
    narwhal::Texture rdTexturePrevious{ GL_TEXTURE_BUFFER };
    GLuint gridArray = 0u;
    {
        const GLuint m = resx;
        const GLuint n = resy;
        narwhal::DynamicArray<Triangle> indices(resx*resy);

        for (GLuint i = 0; i < n - 1u; ++i) {
            for (GLuint j = 0; j < m - 1u; ++j) {
                indices.pushBack(Triangle{ m*i + j,      m*i + j + 1u,  m*(i+1u) + j });
                indices.pushBack(Triangle{ m*(i+1u) + j, m*i + j + 1u,  m*(i+1u) + j + 1u });
            }
        }
        indexBuffer.dataStore(indices.size()*3, sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        float max = float(m*n - 1);
        narwhal::DynamicArray<Vec4f> rdGrid(m*n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                float scalar = float(i*m + j);
                rdGrid.pushBack(Vec4f{h*j, h*i, scalar / max, scalar / max });
            }
        }
        rdBuffer.dataStore(rdGrid.size(), sizeof(Vec4f), rdGrid.data(), GL_DYNAMIC_COPY);
        rdBufferPrevious.dataStore(rdGrid.size(), sizeof(Vec4f), rdGrid.data(), GL_DYNAMIC_COPY);
        rdTexture.setStore(GL_RGBA32F, rdBuffer);
        rdTexturePrevious.setStore(GL_RGBA32F, rdBufferPrevious);

        rdBuffer.bind();
        indexBuffer.bind();
        glGenVertexArrays(1, &gridArray);
        NARWHAL_ASSERT(gridArray != 0u);
        glBindVertexArray(gridArray);
        glVertexAttribPointer(GridVisShader::a_vertex, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(GridVisShader::a_vertex);
        glBindVertexArray(0u);
        indexBuffer.unbind();
        rdBuffer.unbind();
    }

    /***
    *       __  ___     __      _
    *      /  |/  /__ _/ /_____(_)______ ___
    *     / /|_/ / _ `/ __/ __/ / __/ -_|_-<
    *    /_/  /_/\_,_/\__/_/ /_/\__/\__/___/
    *
    */
    Mat4f ortho = Mat4f::orthographic(h*(resx-1), h*(resy-1), 0.f, 2.f);
    Mat4f model = Mat4f::translation(Vec3f(-0.5f*h*resx + 0.5f*h, -0.5f*h*resy + 0.5f*h, 0.f));

    /***
    *       __
    *      / /  ___  ___  ___
    *     / /__/ _ \/ _ \/ _ \
    *    /____/\___/\___/ .__/
    *                  /_/
    */
    bool running = true;
    Parameters params{};
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
            ui(params);
        }

        // computation magic goes here
        const unsigned int localGroupSize = 4u;
        const unsigned int numLocalGroupsX = resx / localGroupSize;
        const unsigned int numLocalGroupsY = resy / localGroupSize;
        bulkShader.use();
        glBindImageTexture(1, rdTexture.object(),         0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindImageTexture(2, rdTexturePrevious.object(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
        glUniform1i(3, resx);
        glUniform1i(4, resy);
        glUniform1f(5, params.diffusivity);
        glUniform1f(6, h);
        glUniform1f(7, 0.1f);
        glUniform1f(8, inhCoupling);
        glUniform1f(9, actCoupling);
        glUniform1f(10, inhBaseProduction);
        glUniform1f(11, params.activatorBaseProduction);
        glUniform1f(12, params.timeFactor);

        glDispatchCompute(numLocalGroupsX, numLocalGroupsY, 1);
        bulkShader.stopUsing();

        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        synchronizeShader.use();
        glBindImageTexture(1, rdTexture.object(),         0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(2, rdTexturePrevious.object(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glUniform1i(3, resx);
        glDispatchCompute(numLocalGroupsX, numLocalGroupsY, 1);
        synchronizeShader.stopUsing();

        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering magic goes here
        glBindVertexArray(gridArray);
        indexBuffer.bind();
        gridVisShader.use();
        gridVisShader.setUniform("u_PV", ortho);
        gridVisShader.setUniform("u_M", model);

        glDrawElements(GL_TRIANGLES, indexBuffer.count(), GL_UNSIGNED_INT, 0);

        indexBuffer.unbind();
        glBindVertexArray(0u);
        gridVisShader.stopUsing();

        imgui.render();
        window.display();
    }

    glDeleteVertexArrays(1, &gridArray);

    return 0;
}
