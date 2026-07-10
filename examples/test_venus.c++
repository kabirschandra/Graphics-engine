#include "model_viewer.h++"


int main() {

    ModelSettings settings = {
        "Venus",
        "./models/venus.obj",
        "",

        0.0f,
        -10.0f,
        50.0f,

        0.01f,
        0.01f,
        0.01f,

        0.0f,
        0.4f,
        0.0f
    };

    return run_model_viewer(
        settings
    );
}
