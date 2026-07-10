#include "model_viewer.h++"


int main() {

    ModelSettings settings = {
        "Sponza",
        "./models/sponza.obj",
        "",

        0.0f,
        -1.0f,
        10.0f,

        0.03f,
        0.03f,
        0.03f,

        0.0f,
        0.15f,
        0.0f
    };

    return run_model_viewer(
        settings
    );
}

