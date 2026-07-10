#ifndef SDLGL_DISPLAY_H
#define SDLGL_DISPLAY_H

#include <stdexcept>
#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "../framebuffer/framebuffer.h++"


/**
 * @class SDLGLDisplay
 * @brief Presents a CPU framebuffer via SDL2 and legacy OpenGL.
 *
 * This class creates an SDL2 window with an OpenGL context, manages a
 * texture for the framebuffer, and provides event processing and presentation
 * helpers for the graphics engine.
 */
class SDLGLDisplay {

    private:

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;

    GLuint texture = 0;

    int textureWidth = 0;
    int textureHeight = 0;

    bool running = true;


    /**
     * @brief Allocate a new OpenGL texture for framebuffer upload.
     *
     * @param width :: Texture width in pixels
     * @param height :: Texture height in pixels
     * @return void :: None
     */
    void create_texture(int width, int height) {

        textureWidth = width;
        textureHeight = height;

        glGenTextures(
            1,
            &texture
        );

        glBindTexture(
            GL_TEXTURE_2D,
            texture
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glPixelStorei(
            GL_UNPACK_ALIGNMENT,
            1
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            textureWidth,
            textureHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        glBindTexture(
            GL_TEXTURE_2D,
            0
        );

        return;
    }


    /**
     * @brief Recreate the OpenGL texture if the framebuffer size changes.
     *
     * @param width :: New texture width in pixels
     * @param height :: New texture height in pixels
     * @return void :: None
     */
    void resize_texture(int width, int height) {

        if(texture != 0) {
            glDeleteTextures(
                1,
                &texture
            );

            texture = 0;
        }

        create_texture(
            width,
            height
        );

        return;
    }


    public:

    /**
     * @brief Create an SDL2 window with an OpenGL context
     *
     * @param title :: Window title
     * @param windowWidth :: Initial window width
     * @param windowHeight :: Initial window height
     * @param framebufferWidth :: Logical framebuffer width
     * @param framebufferHeight :: Logical framebuffer height
     */
    SDLGLDisplay(
        const std::string &title,
        int windowWidth,
        int windowHeight,
        int framebufferWidth,
        int framebufferHeight
    ) {

        static_assert(
            sizeof(Colour) == 4,
            "Colour must contain exactly four 8-bit RGBA values."
        );

        if(windowWidth <= 0 || windowHeight <= 0) {
            throw std::invalid_argument(
                "FATAL: SDL window dimensions must be positive."
            );
        }

        if(framebufferWidth <= 0 || framebufferHeight <= 0) {
            throw std::invalid_argument(
                "FATAL: Framebuffer dimensions must be positive."
            );
        }

        if(SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error(
                std::string("FATAL: SDL_Init failed: ") +
                SDL_GetError()
            );
        }

        SDL_GL_SetAttribute(
            SDL_GL_DOUBLEBUFFER,
            1
        );

        SDL_GL_SetAttribute(
            SDL_GL_RED_SIZE,
            8
        );

        SDL_GL_SetAttribute(
            SDL_GL_GREEN_SIZE,
            8
        );

        SDL_GL_SetAttribute(
            SDL_GL_BLUE_SIZE,
            8
        );

        SDL_GL_SetAttribute(
            SDL_GL_ALPHA_SIZE,
            8
        );

        /*
         * Use the legacy OpenGL 2.1-style pipeline so that this presenter
         * does not require GLAD, GLEW, shaders, VAOs or VBOs.
         */
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MAJOR_VERSION,
            2
        );

        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MINOR_VERSION,
            1
        );

        window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_OPENGL |
            SDL_WINDOW_RESIZABLE |
            SDL_WINDOW_ALLOW_HIGHDPI
        );

        if(window == nullptr) {

            std::string error = SDL_GetError();

            SDL_Quit();

            throw std::runtime_error(
                "FATAL: SDL_CreateWindow failed: " +
                error
            );
        }

        context = SDL_GL_CreateContext(
            window
        );

        if(context == nullptr) {

            std::string error = SDL_GetError();

            SDL_DestroyWindow(
                window
            );

            window = nullptr;

            SDL_Quit();

            throw std::runtime_error(
                "FATAL: SDL_GL_CreateContext failed: " +
                error
            );
        }

        if(SDL_GL_MakeCurrent(window, context) != 0) {
            throw std::runtime_error(
                std::string("FATAL: SDL_GL_MakeCurrent failed: ") +
                SDL_GetError()
            );
        }

        /*
         * Enable vertical synchronisation when supported.
         */
        SDL_GL_SetSwapInterval(
            1
        );

        glDisable(
            GL_DEPTH_TEST
        );

        glDisable(
            GL_CULL_FACE
        );

        glEnable(
            GL_TEXTURE_2D
        );

        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

        create_texture(
            framebufferWidth,
            framebufferHeight
        );
    }


    /**
     * @brief Destroy the OpenGL texture, context and SDL window
     */
    ~SDLGLDisplay() {

        if(texture != 0) {
            glDeleteTextures(
                1,
                &texture
            );

            texture = 0;
        }

        if(context != nullptr) {
            SDL_GL_DeleteContext(
                context
            );

            context = nullptr;
        }

        if(window != nullptr) {
            SDL_DestroyWindow(
                window
            );

            window = nullptr;
        }

        SDL_Quit();
    }


    SDLGLDisplay(const SDLGLDisplay &) = delete;
    SDLGLDisplay &operator=(const SDLGLDisplay &) = delete;


    /**
     * @brief Process SDL window events and update running state.
     *
     * @return bool :: True while the window is still open, false after close
     */
    bool process_events(void) {

        SDL_Event event;

        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_QUIT) {
                running = false;
            }

            if(event.type == SDL_KEYDOWN) {

                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        return running;
    }


    /**
     * @brief Upload and display a CPU framebuffer
     *
     * @param framebuffer :: Framebuffer to display
     */
    void present(FrameBuffer &framebuffer) {

        int framebufferWidth =
            framebuffer.get_width();

        int framebufferHeight =
            framebuffer.get_height();

        if(
            framebufferWidth != textureWidth ||
            framebufferHeight != textureHeight
        ) {
            resize_texture(
                framebufferWidth,
                framebufferHeight
            );
        }

        glBindTexture(
            GL_TEXTURE_2D,
            texture
        );

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            framebufferWidth,
            framebufferHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            framebuffer.get_pixels().data()
        );

        int drawableWidth = 0;
        int drawableHeight = 0;

        SDL_GL_GetDrawableSize(
            window,
            &drawableWidth,
            &drawableHeight
        );

        glViewport(
            0,
            0,
            drawableWidth,
            drawableHeight
        );

        glClear(
            GL_COLOR_BUFFER_BIT
        );

        glMatrixMode(
            GL_PROJECTION
        );

        glLoadIdentity();

        glMatrixMode(
            GL_MODELVIEW
        );

        glLoadIdentity();

        glColor4f(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

        /*
         * Draw the framebuffer texture across the whole window.
         *
         * Texture Y is reversed because framebuffer row zero represents
         * the top of the image.
         */
        glBegin(
            GL_QUADS
        );

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);

        glEnd();

        glBindTexture(
            GL_TEXTURE_2D,
            0
        );

        SDL_GL_SwapWindow(
            window
        );

        return;
    }
};


#endif