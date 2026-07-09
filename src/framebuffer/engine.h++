#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>

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
            std::cerr << "WARNING: Pixel (" << x << ", " << y << ") is not within bounds.\n";
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
    void fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Colour colour)
    {
        // Sort by y: y0 <= y1 <= y2
        if (y0 > y1) {
            std::swap(y0, y1); std::swap(x0, x1); 
        }
        if (y0 > y2) { 
            std::swap(y0, y2); std::swap(x0, x2); 
        }
        if (y1 > y2) { 
            std::swap(y1, y2); std::swap(x1, x2); 
        }

        if (y1 == y2) {
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

