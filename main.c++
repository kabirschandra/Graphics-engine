#include <chrono>
#include <thread>

#include "./src/framebuffer/framebuffer.h++"
#include "./src/display/sdgldisplay.h++"
#include "./src/render3D/render3D.h++"


int main() {

    FrameBuffer framebuffer(
        1280,
        720
    );

    SDLGLDisplay display(
        "Graphics Engine",
        1280,
        720,
        framebuffer.get_width(),
        framebuffer.get_height()
    );

    Cube cube;
    Render3D renderer;

    float angle = 0.0f;

    while(display.process_events()) {

        renderer.draw_cube_standard(
            cube,
            framebuffer,
            angle
        );

        display.present(
            framebuffer
        );

        angle += 0.02f;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(8)
        );
    }

    return 0;
}