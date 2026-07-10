#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>
#include "../matrix/matrix.h++"
#include <array>

#include "../objects/mesh.h++"

typedef struct Colour {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    uint8_t alpha = 255;
} Colour;

typedef struct Sprite {

    int width = 0;
    int height = 0;
    std::vector<Colour> pixels = {};
} Sprite;

/**
 * @brief RGBA texture stored in CPU memory.
 */
typedef struct Texture {

    int width = 0;
    int height = 0;

    std::vector<Colour> pixels;


    Colour sample(float u, float v) const {

        if(width <= 0 || height <= 0 || pixels.empty()) {
            return {
                255,
                0,
                255,
                255
            };
        }

        u = std::clamp(
            u,
            0.0f,
            1.0f
        );

        v = std::clamp(
            v,
            0.0f,
            1.0f
        );

        int x = int(
            u * float(width - 1)
        );

        int y = int(
            (1.0f - v) *
            float(height - 1)
        );

        return pixels[
            y * width + x
        ];
    }

} Texture;

class FrameBuffer {

    private:
    int width = 0;
    int height = 0;
    std::vector<Colour> pixels = {};


    public:

    /**
     * @brief Construct a framebuffer
     *
     * @param width :: Width of the framebuffer in pixels
     * @param height :: Height of the framebuffer in pixels
     */
    FrameBuffer(int width, int height) {
        if(1 > width || 1 > height) {
            throw std::invalid_argument("FATAL: Expect width and height of frame buffer to be positive integers.");
        }

        this->width = width;
        this->height = height;
        pixels.resize(width * height);
    }

    /**
     * @brief Get the framebuffer width
     *
     * @return int :: Width in pixels
     */
    int get_width(void) {
        return this->width;
    }

    /**
     * @brief Get the framebuffer height
     *
     * @return int :: Height in pixels
     */
    int get_height(void) {
        return this->height;
    }

    /**
     * @brief Get the framebuffer pixel buffer
     *
     * @return std::vector<Colour>& :: Reference to the pixel data
     */
    std::vector<Colour> &get_pixels(void) {
        return this->pixels;
    }

    /**
     * @brief Fill the whole framebuffer with one colour
     *
     * @param colour :: Colour to paint across the buffer
     *
     * @return void :: None
     */
    void fill_screen(Colour colour) {
        std::fill(this->pixels.begin(), this->pixels.end(), colour);
        return;
    }

    /**
     * @brief Check whether a pixel is inside the framebuffer
     *
     * @param x :: X coordinate to test
     * @param y :: Y coordinate to test
     * @return bool :: True if the pixel is within bounds
     */
    bool in_bounds(int x, int y) {
        if(x >= 0 &&
            x < this->width &&
            y >= 0 &&
            y < this->height) {
            return true;
        } else {
            //std::cerr << "WARNING: Pixel (" << x << ", " << y << ") is not within bounds.\n";
            return false;
        }
        return false;
    }

    /**
     * @brief Access a pixel by coordinate
     *
     * @param x :: X coordinate of the pixel
     * @param y :: Y coordinate of the pixel
     * @return Colour& :: Reference to the requested pixel
     */
    Colour &at(int x, int y) {
        return this->pixels[y * this->width + x];
    }

    /**
     * @brief Draw a single pixel to the framebuffer
     *
     * @param x :: X coordinate of the pixel
     * @param y :: Y coordinate of the pixel
     * @param colour :: Colour to draw
     *
     * @return void :: None
     */
    void colour_pixel(int x, int y, Colour colour) {
        if(!this->in_bounds(x,y)) {
            return;
        }

        this->at(x,y) = colour;
        return;
    }

    /**
     * @brief Draw a line between two points
     *
     * @param xStart :: Starting X coordinate
     * @param yStart :: Starting Y coordinate
     * @param xEnd :: Ending X coordinate
     * @param yEnd :: Ending Y coordinate
     * @param colour :: Colour to draw the line
     *
     * @return void :: None
     */
    void draw_line(int xStart, int yStart, int xEnd, int yEnd, Colour colour) {
        //Modified from Wikipedia
        int dx = std::abs(xEnd - xStart);
        int dy = std::abs(yEnd - yStart);

        int sx = (xStart < xEnd) ? 1 : -1;
        int sy = (yStart < yEnd) ? 1 : -1;

        int error = dx - dy;

        while(true) {
            this->colour_pixel(xStart, yStart, colour);

            if(xStart == xEnd && yStart == yEnd) {
                break;
            }

            int error2 = error * 2;

            if(error2 > -dy) {
                error -= dy;
                xStart += sx;
            }

            if(error2 < dx) {
                error += dx;
                yStart += sy;
            }
        }

        return;
    }

    /**
     * @brief Draw the outline of a triangle
     *
     * @param x0 :: First X coordinate
     * @param y0 :: First Y coordinate
     * @param x1 :: Second X coordinate
     * @param y1 :: Second Y coordinate
     * @param x2 :: Third X coordinate
     * @param y2 :: Third Y coordinate
     * @param colour :: Colour to draw the triangle
     *
     * @return void :: None
     */
    void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Colour colour) {
        this->draw_line(x0, y0, x1, y1, colour);
        this->draw_line(x1, y1, x2, y2, colour);
        this->draw_line(x2, y2, x0, y0, colour);

        return;
    }


    /**
     * @brief Draw the outline of a triangle from a 3x4 matrix
     *
     * Each row is one vertex:
     *
     * [x0, y0, z0, w0]
     * [x1, y1, z1, w1]
     * [x2, y2, z2, w2]
     *
     * @param triangle :: Triangle matrix containing three vertices
     * @param colour :: Colour to draw the triangle
     *
     * @return void :: None
     */
    void draw_triangle(Matrix &triangle, Colour colour) {

        if(triangle.get_rows() != 3 || triangle.get_cols() < 2) {
            throw std::invalid_argument(
                "FATAL: Triangle matrix must have 3 rows and at least 2 columns.\n"
            );
        }

        int x0 = int(triangle.at(0, 0));
        int y0 = int(triangle.at(0, 1));

        int x1 = int(triangle.at(1, 0));
        int y1 = int(triangle.at(1, 1));

        int x2 = int(triangle.at(2, 0));
        int y2 = int(triangle.at(2, 1));

        this->draw_line(x0, y0, x1, y1, colour);
        this->draw_line(x1, y1, x2, y2, colour);
        this->draw_line(x2, y2, x0, y0, colour);

        return;
    }



    /**
     * @brief Draw a horizontal line segment
     *
     * @param xStart :: Starting X coordinate
     * @param xEnd :: Ending X coordinate
     * @param y :: Y coordinate of the line
     * @param colour :: Colour to draw the line
     *
     * @return void :: None
     */
    void draw_scanline(int xStart, int xEnd, int y, Colour colour)
    {
        if (xStart > xEnd) {
            std::swap(xStart, xEnd);
        }

        for (int x = xStart; x <= xEnd; x++) {
            colour_pixel(x, y, colour);
        }

        return;
    }

    /**
     * @brief Fill a triangle with a flat bottom
     *
     * @param x0 :: First X coordinate
     * @param y0 :: First Y coordinate
     * @param x1 :: Second X coordinate
     * @param y1 :: Second Y coordinate
     * @param x2 :: Third X coordinate
     * @param y2 :: Third Y coordinate
     * @param colour :: Colour to fill the triangle
     *
     * @return void :: None
     */
    void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Colour colour) {
        float invSlope1 = float(x1 - x0) / float(y1 - y0);
        float invSlope2 = float(x2 - x0) / float(y2 - y0);

        float curX1 = x0;
        float curX2 = x0;

        for (int y = y0; y <= y1; y++) {
            draw_scanline(int(std::round(curX1)), int(std::round(curX2)), y, colour);
            curX1 += invSlope1;
            curX2 += invSlope2;
        }
    }

    /**
     * @brief Fill a triangle with a flat top
     *
     * @param x0 :: First X coordinate
     * @param y0 :: First Y coordinate
     * @param x1 :: Second X coordinate
     * @param y1 :: Second Y coordinate
     * @param x2 :: Third X coordinate
     * @param y2 :: Third Y coordinate
     * @param colour :: Colour to fill the triangle
     *
     * @return void :: None
     */
    void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Colour colour) {
        float invSlope1 = float(x2 - x0) / float(y2 - y0);
        float invSlope2 = float(x2 - x1) / float(y2 - y1);

        float curX1 = x2;
        float curX2 = x2;

        for (int y = y2; y >= y0; y--) {
            draw_scanline(int(std::round(curX1)), int(std::round(curX2)), y, colour);

            curX1 -= invSlope1;
            curX2 -= invSlope2;
        }
        return;
    }

    /**
     * @brief Fill a triangle of any shape
     *
     * @param x0 :: First X coordinate
     * @param y0 :: First Y coordinate
     * @param x1 :: Second X coordinate
     * @param y1 :: Second Y coordinate
     * @param x2 :: Third X coordinate
     * @param y2 :: Third Y coordinate
     * @param colour :: Colour to fill the triangle
     *
     * @return void :: None
     */
    void fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Colour colour) {
        // Sort by y: y0 <= y1 <= y2
        if(y0 > y1) {
            std::swap(y0, y1); std::swap(x0, x1); 
        }
        if(y0 > y2) { 
            std::swap(y0, y2); std::swap(x0, x2); 
        }
        if(y1 > y2) { 
            std::swap(y1, y2); std::swap(x1, x2); 
        }

        if(y1 == y2) {
            fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, colour);
        } else if (y0 == y1) {
            fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, colour);
        } else {
            int x3 = int(std::round(x0 + float(y1 - y0) / float(y2 - y0) * float(x2 - x0) ));

            int y3 = y1;

            fill_flat_bottom_triangle(x0, y0, x1, y1, x3, y3, colour);
            fill_flat_top_triangle(x1, y1, x3, y3, x2, y2, colour);
        }
        return;
    }


    /**
     * @brief Fill a triangle from a 3x4 matrix
     *
     * Each row represents one vertex:
     *
     * [x0, y0, z0, w0]
     * [x1, y1, z1, w1]
     * [x2, y2, z2, w2]
     *
     * @param triangle :: Triangle matrix containing three vertices
     * @param colour :: Colour used to fill the triangle
     *
     * @return void :: None
     */
    void fill_triangle(Matrix &triangle, Colour colour) {

        if(triangle.get_rows() != 3 || triangle.get_cols() < 2) {
            throw std::invalid_argument(
                "FATAL: Triangle matrix must have 3 rows and at least 2 columns.\n"
            );
        }

        this->fill_triangle(
            int(triangle.at(0, 0)),
            int(triangle.at(0, 1)),

            int(triangle.at(1, 0)),
            int(triangle.at(1, 1)),

            int(triangle.at(2, 0)),
            int(triangle.at(2, 1)),

            colour
        );

        return;
    }
    /**
     * @brief Fill a screen-space triangle with a texture.
     *
     * The triangle must already have been projected, perspective-divided,
     * and converted to framebuffer coordinates.
     *
     * The triangle's W values must still contain their clip-space values
     * so UV interpolation can remain perspective-correct.
     *
     * @param triangle :: 3x4 screen-space triangle matrix
     * @param textureCoordinates :: UV coordinate for each vertex
     * @param texture :: Texture to sample
     *
     * @return void :: None
     */
    void fill_textured_triangle(
        Matrix &triangle,
        const std::array<UV, 3> &textureCoordinates,
        const Texture &texture
    ) {

        if(
            triangle.get_rows() != 3 ||
            triangle.get_cols() < 4
        ) {
            throw std::invalid_argument(
                "FATAL: A textured triangle must be a 3x4 matrix.\n"
            );
        }

        float x0 = triangle.at(0, 0);
        float y0 = triangle.at(0, 1);

        float x1 = triangle.at(1, 0);
        float y1 = triangle.at(1, 1);

        float x2 = triangle.at(2, 0);
        float y2 = triangle.at(2, 1);

        float w0 = triangle.at(0, 3);
        float w1 = triangle.at(1, 3);
        float w2 = triangle.at(2, 3);

        if(
            std::fabs(w0) < 0.000001f ||
            std::fabs(w1) < 0.000001f ||
            std::fabs(w2) < 0.000001f
        ) {
            return;
        }

        float inverseW0 = 1.0f / w0;
        float inverseW1 = 1.0f / w1;
        float inverseW2 = 1.0f / w2;

        float minimumX = std::min({
            x0,
            x1,
            x2
        });

        float maximumX = std::max({
            x0,
            x1,
            x2
        });

        float minimumY = std::min({
            y0,
            y1,
            y2
        });

        float maximumY = std::max({
            y0,
            y1,
            y2
        });

        int startX = std::max(
            0,
            int(std::floor(minimumX))
        );

        int endX = std::min(
            this->width - 1,
            int(std::ceil(maximumX))
        );

        int startY = std::max(
            0,
            int(std::floor(minimumY))
        );

        int endY = std::min(
            this->height - 1,
            int(std::ceil(maximumY))
        );

        auto edge = [](
            float ax,
            float ay,
            float bx,
            float by,
            float px,
            float py
        ) {

            return
                (px - ax) * (by - ay) -
                (py - ay) * (bx - ax);
        };

        float area = edge(
            x0,
            y0,
            x1,
            y1,
            x2,
            y2
        );

        if(std::fabs(area) < 0.000001f) {
            return;
        }

        for(int y = startY; y <= endY; y++) {

            for(int x = startX; x <= endX; x++) {

                float pixelX =
                    float(x) + 0.5f;

                float pixelY =
                    float(y) + 0.5f;

                float barycentric0 =
                    edge(
                        x1,
                        y1,
                        x2,
                        y2,
                        pixelX,
                        pixelY
                    ) / area;

                float barycentric1 =
                    edge(
                        x2,
                        y2,
                        x0,
                        y0,
                        pixelX,
                        pixelY
                    ) / area;

                float barycentric2 =
                    edge(
                        x0,
                        y0,
                        x1,
                        y1,
                        pixelX,
                        pixelY
                    ) / area;

                if(
                    barycentric0 < 0.0f ||
                    barycentric1 < 0.0f ||
                    barycentric2 < 0.0f
                ) {
                    continue;
                }

                float interpolatedInverseW =
                    barycentric0 * inverseW0 +
                    barycentric1 * inverseW1 +
                    barycentric2 * inverseW2;

                if(
                    std::fabs(interpolatedInverseW) <
                    0.000001f
                ) {
                    continue;
                }

                float u =
                    (
                        barycentric0 *
                        textureCoordinates[0].u *
                        inverseW0 +

                        barycentric1 *
                        textureCoordinates[1].u *
                        inverseW1 +

                        barycentric2 *
                        textureCoordinates[2].u *
                        inverseW2
                    ) /
                    interpolatedInverseW;

                float v =
                    (
                        barycentric0 *
                        textureCoordinates[0].v *
                        inverseW0 +

                        barycentric1 *
                        textureCoordinates[1].v *
                        inverseW1 +

                        barycentric2 *
                        textureCoordinates[2].v *
                        inverseW2
                    ) /
                    interpolatedInverseW;

                this->colour_pixel(
                    x,
                    y,
                    texture.sample(
                        u,
                        v
                    )
                );
            }
        }

        return;
    }

    /**
     * @brief Draw a sprite onto the framebuffer
     *
     * @param sprite :: Sprite to draw
     * @param xStart :: Starting X coordinate
     * @param yStart :: Starting Y coordinate
     * @param scaleX :: Horizontal scale factor
     * @param scaleY :: Vertical scale factor
     *
     * @return void :: None
     */
    void draw_sprite(Sprite &sprite, int xStart, int yStart, float scaleX, float scaleY) {
        if(scaleX <= 0 || scaleY <= 0) {
            throw std::invalid_argument("FATAL: Sprite scale factor/s are not within bounds.\n");
        }

        int drawWidth = int(sprite.width * scaleX);
        int drawHeight = int(sprite.height * scaleY);

        for(int y = 0; y < drawHeight; y++) {
            int sourceY = int(y / scaleY);

            for(int x = 0; x < drawWidth; x++) {
                int sourceX = int(x / scaleX);

                Colour colour = sprite.pixels[sourceY * sprite.width + sourceX];

                colour_pixel(xStart + x, yStart + y, colour);
            }
        }

        return;
    }

    /**
     * @brief Print the framebuffer as ASCII art
     *
     * @return void :: None
     */
    void print(void) {
        const char ramp[] = " .:-=+*#%@"; //Black terminal
        //const char ramp[] = "@%#*+=-:. "; //White terminal

        for(int y = 0; y < this->height; y++) {
            for(int x = 0; x < this->width; x++) {
                Colour pixel = this->at(x,y);
                int greyScale = (pixel.alpha * (pixel.red * 0.2126 + pixel.green * 0.7152 + pixel.blue * 0.0722))/255;
                char character = ramp[(greyScale * (sizeof(ramp) - 1)) / 255];

                std::cout << character;
            }
            std::cout << "\n";
        }
    }
};






#endif

