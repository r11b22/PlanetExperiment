#include <iostream>

#include "App.h"
#include "MainScene.hpp"

int main() {
    App app{"Sphere Experiment"};

    MainScene scene = {};
    app.loadScene(&scene);
    app.run();

    return 0;
}
