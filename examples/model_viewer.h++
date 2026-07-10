#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include <chrono>
#include <string>

#include "../src/framebuffer/framebuffer.h++"
#include "../src/display/sdgldisplay.h++"
#include "../src/objects/objloader.h++"
#include "../src/textures/tgaloader.h++"
#include "../src/render3D/render3D.h++"


typedef struct ModelSettings {

    std::string windowTitle;
    std::string modelPath;
    std::string texturePath;

    float positionX;
    float positionY;
    float positionZ;

    float scaleX;
    float scaleY;
    float scaleZ;

    float rotationSpeedX;
    float rotationSpeedY;
    float rotationSpeedZ;

} ModelSettings;


/**
 * @brief Run the standard OBJ model viewer.
 *
 * An empty texturePath renders the model as a wireframe.
 *
 * @param settings :: Model paths and transform settings
 * @return int :: Program exit status
 */
int run_model_viewer(const ModelSettings &settings) {

    FrameBuffer framebuffer(
        1280,
        720
    );

    SDLGLDisplay display(
        settings.windowTitle,
        1280,
        720,
        framebuffer.get_width(),
        framebuffer.get_height()
    );

    Mesh mesh =
        OBJLoader::load(
            settings.modelPath
        );

    mesh.set_position(
        settings.positionX,
        settings.positionY,
        settings.positionZ
    );

    mesh.set_scale(
        settings.scaleX,
        settings.scaleY,
        settings.scaleZ
    );

    Texture texture;
    Texture *texturePointer = nullptr;

    if(!settings.texturePath.empty()) {

        texture =
            TGALoader::load(
                settings.texturePath
            );

        texturePointer =
            &texture;
    }

    Render3D renderer;

    auto previousTime =
        std::chrono::steady_clock::now();

    int frameCount = 0;
    float elapsedForFPS = 0.0f;

    while(display.process_events()) {

        auto currentTime =
            std::chrono::steady_clock::now();

        float deltaTime =
            std::chrono::duration<float>(
                currentTime - previousTime
            ).count();

        previousTime =
            currentTime;

        frameCount++;
        elapsedForFPS += deltaTime;


        mesh.rotate(
            settings.rotationSpeedX * deltaTime,
            settings.rotationSpeedY * deltaTime,
            settings.rotationSpeedZ * deltaTime
        );

        /*
         * Clear once at the beginning of each frame.
         */
        framebuffer.fill_screen({
            0,
            0,
            0,
            255
        });

        renderer.draw_mesh(
            mesh,
            framebuffer,
            texturePointer
        );

        display.present(
            framebuffer
        );
    }

    return 0;
}


#endif


