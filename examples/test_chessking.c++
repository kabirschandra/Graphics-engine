#include "model_viewer.h++"


int main() {

    ModelSettings settings = {
        "Chess King",
        "./models/ChessKing.obj",
        "./models/ChessKing.tga",

        0.0f,
        -1.0f,
        6.0f,

        25.0f,
        25.0f,
        25.0f,

        0.0f,
        0.5f,
        0.0f
    };

    return run_model_viewer(
        settings
    );
}

