#!/usr/bin/env bash

set -e

echo "Select a model:"
echo
echo "1) Chess King (textured)"
echo "2) Dragon"
echo "3) Elephant"
echo "4) Monkey"
echo "5) Sponza"
echo "6) Teapot"
echo "7) Three Objects"
echo "8) Venus"
echo

read -rp "Choice: " choice

case "$choice" in
    1)
        source_file="test_chessking.c++"
        ;;
    2)
        source_file="test_dragon.c++"
        ;;
    3)
        source_file="test_elephant.c++"
        ;;
    4)
        source_file="test_monkey.c++"
        ;;
    5)
        source_file="test_sponza.c++"
        ;;
    6)
        source_file="test_teapot.c++"
        ;;
    7)
        source_file="test_three_objects.c++"
        ;;
    8)
        source_file="test_venus.c++"
        ;;
    *)
        echo "Invalid choice."
        exit 1
        ;;
esac

echo
echo "Compiling $source_file..."

g++ "$source_file" \
    -std=c++17 \
    -O3 \
    -o ../bin/model_test \
    $(sdl2-config --cflags --libs) \
    -lGL

echo "Running..."
echo

../bin/model_test



