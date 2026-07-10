#ifndef TGALOADER_H
#define TGALOADER_H

#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include "../framebuffer/framebuffer.h++"


/**
 * @brief Loader for uncompressed and RLE-compressed TGA textures.
 *
 * TGALoader supports 24-bit and 32-bit true-color TGA images in both
 * uncompressed (type 2) and RLE-compressed (type 10) formats.
 */
class TGALoader {

    private:

    /**
     * @brief Read one pixel from a TGA file stream.
     *
     * @param file :: Input stream for the open TGA file.
     * @param bytesPerPixel :: Number of bytes per pixel (3 or 4).
     * @return Colour :: Converted BGRA pixel value.
     * @throws std::runtime_error if the file ends unexpectedly.
     */
    static Colour read_pixel(
        std::ifstream &file,
        int bytesPerPixel
    ) {

        uint8_t data[4] = {
            0,
            0,
            0,
            255
        };

        file.read(
            reinterpret_cast<char *>(data),
            bytesPerPixel
        );

        if(!file) {
            throw std::runtime_error(
                "FATAL: Unexpected end of TGA pixel data."
            );
        }

        return {
            data[2],
            data[1],
            data[0],
            bytesPerPixel == 4 ?
                data[3] :
                uint8_t(255)
        };
    }


    public:

    /**
     * @brief Load a TGA image into a texture.
     *
     * This method supports true-color TGA files with 24-bit and 32-bit pixel
     * formats. It also handles both raw and RLE-compressed images, while
     * rejecting paletted or unsupported image types.
     *
     * @param filename :: Path to the TGA file.
     * @return Texture :: Loaded texture with pixel data.
     * @throws std::runtime_error if the file cannot be opened, if the header
     *         is invalid, or if the image uses unsupported features.
     */
    static Texture load(const std::string &filename) {

        std::ifstream file(
            filename,
            std::ios::binary
        );

        if(!file.is_open()) {
            throw std::runtime_error(
                "FATAL: Could not open TGA file: " +
                filename
            );
        }

        uint8_t header[18] = {};

        file.read(
            reinterpret_cast<char *>(header),
            18
        );

        if(!file) {
            throw std::runtime_error(
                "FATAL: Invalid TGA header."
            );
        }

        int idLength =
            int(header[0]);

        int colourMapType =
            int(header[1]);

        int imageType =
            int(header[2]);

        int width =
            int(header[12]) |
            (int(header[13]) << 8);

        int height =
            int(header[14]) |
            (int(header[15]) << 8);

        int bitsPerPixel =
            int(header[16]);

        int descriptor =
            int(header[17]);

        if(colourMapType != 0) {
            throw std::runtime_error(
                "FATAL: Colour-mapped TGA files are not supported."
            );
        }

        if(imageType != 2 && imageType != 10) {
            throw std::runtime_error(
                "FATAL: Only true-colour TGA types 2 and 10 are supported."
            );
        }

        if(
            bitsPerPixel != 24 &&
            bitsPerPixel != 32
        ) {
            throw std::runtime_error(
                "FATAL: Only 24-bit and 32-bit TGA files are supported."
            );
        }

        if(width <= 0 || height <= 0) {
            throw std::runtime_error(
                "FATAL: Invalid TGA dimensions."
            );
        }

        file.seekg(
            idLength,
            std::ios::cur
        );

        int bytesPerPixel =
            bitsPerPixel / 8;

        size_t pixelCount =
            size_t(width) *
            size_t(height);

        std::vector<Colour> sourcePixels;

        sourcePixels.reserve(
            pixelCount
        );

        if(imageType == 2) {

            for(size_t i = 0; i < pixelCount; i++) {

                sourcePixels.push_back(
                    read_pixel(
                        file,
                        bytesPerPixel
                    )
                );
            }

        } else {

            while(sourcePixels.size() < pixelCount) {

                uint8_t packetHeader = 0;

                file.read(
                    reinterpret_cast<char *>(&packetHeader),
                    1
                );

                if(!file) {
                    throw std::runtime_error(
                        "FATAL: Invalid TGA RLE packet."
                    );
                }

                int packetLength =
                    int(packetHeader & 0x7F) + 1;

                if((packetHeader & 0x80) != 0) {

                    Colour colour =
                        read_pixel(
                            file,
                            bytesPerPixel
                        );

                    for(int i = 0; i < packetLength; i++) {

                        if(sourcePixels.size() >= pixelCount) {
                            break;
                        }

                        sourcePixels.push_back(
                            colour
                        );
                    }

                } else {

                    for(int i = 0; i < packetLength; i++) {

                        if(sourcePixels.size() >= pixelCount) {
                            break;
                        }

                        sourcePixels.push_back(
                            read_pixel(
                                file,
                                bytesPerPixel
                            )
                        );
                    }
                }
            }
        }

        bool topOrigin =
            (descriptor & 0x20) != 0;

        bool rightOrigin =
            (descriptor & 0x10) != 0;

        Texture texture;

        texture.width =
            width;

        texture.height =
            height;

        texture.pixels.resize(
            pixelCount
        );

        for(int sourceY = 0; sourceY < height; sourceY++) {

            for(int sourceX = 0; sourceX < width; sourceX++) {

                int destinationX =
                    rightOrigin ?
                    width - 1 - sourceX :
                    sourceX;

                int destinationY =
                    topOrigin ?
                    sourceY :
                    height - 1 - sourceY;

                texture.pixels[
                    destinationY * width +
                    destinationX
                ] =
                    sourcePixels[
                        sourceY * width +
                        sourceX
                    ];
            }
        }

        return texture;
    }
};


#endif