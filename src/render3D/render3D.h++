#ifndef RENDER3D_H
#define RENDER3D_H

#include <cmath>

#include "../framebuffer/framebuffer.h++"
#include "../matrix/matrix.h++"
#include "../objects/volumes.h++"

class Render3D {

public:

    /**
     * @brief Draw a rotating cube
     *
     * @param cube :: Cube to draw
     * @param framebuffer :: Target framebuffer
     * @param angle :: Rotation angle in radians
     *
     * @return void :: None
     */
    void draw_cube_standard(
        Cube cube,
        FrameBuffer &framebuffer,
        float angle
    ) {

        framebuffer.fill_screen({
            0,
            0,
            0,
            255
        });

        float fov = 90.0f * M_PI / 180.0f;

        int screenWidth = framebuffer.get_width();
        int screenHeight = framebuffer.get_height();

        float aspect =
            float(screenWidth) /
            float(screenHeight);

        float near = 0.1f;
        float far = 1000.0f;

        Colour faceColours[6] = {
            {255,   0,   0, 255},
            {  0, 255,   0, 255},
            {  0,   0, 255, 255},
            {255, 255,   0, 255},
            {  0, 255, 255, 255},
            {255,   0, 255, 255}
        };

        Colour outline = {
            255,
            255,
            255,
            255
        };

        for(size_t i = 0; i < cube.tris.size(); i++) {

            Matrix triangle = cube.tris[i];

            triangle = triangle.translate(
                -0.5f,
                -0.5f,
                -0.5f
            );

            triangle = triangle.scale(
                2.0f,
                2.0f,
                2.0f
            );

            triangle = triangle.rotate_x(angle);
            triangle = triangle.rotate_y(angle);
            triangle = triangle.rotate_z(angle);

            triangle = triangle.translate(
                0.0f,
                0.0f,
                3.0f
            );

            triangle = triangle.project(
                fov,
                aspect,
                near,
                far
            );

            triangle = triangle.perspective_divide();

            triangle = triangle.viewport(
                screenWidth,
                screenHeight
            );

            Colour faceColour = faceColours[i / 2];

            framebuffer.fill_triangle(
                triangle,
                faceColour
            );

            framebuffer.draw_triangle(
                triangle,
                outline
            );
        }

        return;
    }

};

#endif


