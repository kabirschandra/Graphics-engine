#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <algorithm>

#include "../framebuffer/framebuffer.h++"


/**
 * @brief Texture container storing pixel data for sampling.
 *
 * The Texture struct contains the image size and pixel array used for
 * texture mapping during rendering.
 */
typedef struct Texture {

    int width = 0;
    int height = 0;

    std::vector<Colour> pixels;


    /**
     * @brief Sample the texture at normalized coordinates.
     *
     * Coordinates are clamped to [0, 1], and the V axis is flipped to match
     * the conventional texture coordinate origin at the bottom-left.
     *
     * @param u :: Horizontal texture coordinate in [0, 1].
     * @param v :: Vertical texture coordinate in [0, 1].
     * @return Colour :: The sampled texture colour.
     */
    Colour sample(float u, float v) {

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

        int x =
            int(u * float(width - 1));

        int y =
            int((1.0f - v) * float(height - 1));

        return pixels[
            y * width + x
        ];
    }

} Texture;


#endif

