g++ main.c++ -std=c++17 -O3 -o ./bin/graphics_engine \
    $(sdl2-config --cflags --libs) \
    -lGL

./bin/graphics_engine

