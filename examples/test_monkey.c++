#include "model_viewer.h++"


int main() {

    ModelSettings settings = {
        "Monkey",
        "./models/monkey.obj",
        "",

        0.0f,
        0.0f,
        5.0f,

        1.0f,
        1.0f,
        1.0f,

        0.0f,
        0.5f,
        0.0f
    };

    return run_model_viewer(
        settings
    );
}
