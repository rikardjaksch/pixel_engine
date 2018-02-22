#include <pixeng.h>

#define CELL_SIZE 8
#define WIDTH (600 / CELL_SIZE)
#define HEIGHT (400 / CELL_SIZE)

class Base : public PixelEngine {
protected:
    void on_initialize() override {
    }

    bool on_update(float delta_time) override {
        return true;
    }
};

int main() {
    Base app;

    if (!app.create(WIDTH, HEIGHT, CELL_SIZE, CELL_SIZE)) {
        return 1;
    }

    return app.run();
}
