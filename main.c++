#include <chrono>
#include <thread>

#include "./src/framebuffer/engine.h++"
#include "./src/renderer/ncrenderer.h++"


int main()
{
    FrameBuffer buffer(80, 40);
    NCRenderer renderer;

    Colour black  = {  0,   0,   0, 255};
    Colour green  = {  0, 255,   0, 255};
    Colour yellow = {255, 255,   0, 255};

    int frame = 0;

    while (true) {
        buffer.fill_screen(black);

        int x = 20 + (frame % 30);

        buffer.fill_triangle( x, 5, x - 8, 25, x + 8, 25, green);

        buffer.draw_triangle( x, 5, x - 8, 25, x + 8, 25, yellow);

        renderer.present(buffer);

        frame++;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}


