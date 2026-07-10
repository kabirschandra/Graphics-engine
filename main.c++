#include <chrono>
#include <thread>

#include "./src/framebuffer/framebuffer.h++"
#include "./src/display/ncdisplay.h++"
#include "./src/render3D/render3D.h++"
#include "./src/objects/volumes.h++"

int main() {

    FrameBuffer framebuffer(300, 100);
    NCDisplay display;
    Render3D renderer;
    Cube cube;

    float angle = 0.0f;

    while(true) {

        renderer.draw_cube_standard(
            cube,
            framebuffer,
            angle
        );

        display.present(framebuffer);

        angle += 0.02f;
    }

    return 0;
}
