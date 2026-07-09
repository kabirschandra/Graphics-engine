#ifndef NCRENDERER
#define NCRENDERER

#include <ncurses.h>
#include <locale.h>
#include "../framebuffer/framebuffer.h++"

class NCRenderer {

    private:

    /**
     * @brief Map a colour to an ncurses colour pair
     *
     * @param colour :: Colour to convert
     * @return short :: Ncurses colour pair index
     */
    short colour_to_pair(Colour& colour) {
        int r = colour.red;
        int g = colour.green;
        int b = colour.blue;

        if(r < 40 && g < 40 && b < 40) {
            return 1;
        }

        if(r >= g && r >= b) {
            if (g > 128) return 4; // yellow
            if (b > 128) return 6; // magenta
            return 2; // red
        }

        if(g >= r && g >= b) {
            if (r > 128) return 4; // yellow
            if (b > 128) return 7; // cyan
            return 3; // green
        }

        if(b >= r && b >= g) {
            if (r > 128) return 6; // magenta
            if (g > 128) return 7; // cyan
            return 5; // blue
        }

        return 8;
    }

    /**
     * @brief Convert a colour into a block character for display
     *
     * @param colour :: Colour to convert
     * @return wchar_t :: Character representing the colour intensity
     */
    wchar_t colour_to_block(Colour& colour) {
        int grey = int(colour.red   * 0.2126f + colour.green * 0.7152f + colour.blue  * 0.0722f);

        grey = (grey * colour.alpha) / 255;
        grey = std::clamp(grey, 0, 255);

        if(grey < 32)  return L' ';
        if(grey < 96)  return L'░';
        if(grey < 160) return L'▒';
        if(grey < 224) return L'▓';
        return L'█';
    }


    public:
    /**
     * @brief Initialize the ncurses renderer
     *
     * @return void :: None
     */
    NCRenderer() {
        setlocale(LC_ALL, "");
        initscr();
        noecho();
        cbreak();
        curs_set(0);

        start_color();
        use_default_colors();

        init_pair(1, COLOR_BLACK, -1);
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_YELLOW, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);
        init_pair(7, COLOR_CYAN, -1);
        init_pair(8, COLOR_WHITE, -1);        

        nodelay(stdscr, TRUE);
        keypad(stdscr, TRUE);
    }


    /**
     * @brief Shut down the ncurses renderer
     *
     * @return void :: None
     */
    ~NCRenderer() {
        endwin();
    }



    /**
     * @brief Render a framebuffer to the terminal
     *
     * @param framebuffer :: Framebuffer to display
     *
     * @return void :: None
     */
    void present(FrameBuffer& framebuffer) {

        erase();

        for(int y = 0; y < framebuffer.get_height(); y++) {
            for(int x = 0; x < framebuffer.get_width(); x++) {
                Colour pixel = framebuffer.at(x, y);

                wchar_t block = colour_to_block(pixel);
                short pair = colour_to_pair(pixel);

                attron(COLOR_PAIR(pair));
                mvaddnwstr(y, x, &block, 1);
                attroff(COLOR_PAIR(pair));
            }
        }

        refresh();
    }


};





#endif
