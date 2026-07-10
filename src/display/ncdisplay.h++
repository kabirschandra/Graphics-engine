#ifndef NCDISPLAY
#define NCDISPLAY
#define XSCALE 2.0f
#define YSCALE 1.0f



#include <ncurses.h>
#include <locale.h>
#include "../framebuffer/framebuffer.h++"

class NCDisplay {

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

        if(r > 200 && g > 200 && b > 200) {
            return 8;
        }

        if(r > 128 && g > 128 && b < 128) {
            return 4;
        }

        if(r > 128 && b > 128 && g < 128) {
            return 6;
        }

        if(g > 128 && b > 128 && r < 128) {
            return 7;
        }

        if(r >= g && r >= b) {
            return 2;
        }

        if(g >= r && g >= b) {
            return 3;
        }

        if(b >= r && b >= g) {
            return 5;
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
     * @brief Initialize the ncurses Display
     *
     * @return void :: None
     */
    NCDisplay() {
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
     * @brief Shut down the ncurses Display
     *
     * @return void :: None
     */
    ~NCDisplay() {
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
                mvaddnwstr(y * YSCALE, x * XSCALE, &block, 1);
                attroff(COLOR_PAIR(pair));
            }
        }

        refresh();
    }


};





#endif
