#include "model_viewer.h++"


int main() {

    ModelSettings settings = {
        "Elephant",
        "./models/elephant.obj",
        "",

        0.0f,
        -1.0f,
        7.0f,

        1.0f,
        1.0f,
        1.0f,

        0.0f,
        0.4f,
        0.0f
    };

    return run_model_viewer(
        settings
    );
}
