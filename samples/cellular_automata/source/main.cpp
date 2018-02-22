#include <pixeng.h>
#include <string.h>
#include <random>
#include <time.h>
#include <iostream>

#define CELL_SIZE 4
#define WIDTH (600 / CELL_SIZE)
#define HEIGHT (400 / CELL_SIZE)

class CellularAutomata : public PixelEngine {
protected:
    void on_initialize() override {
        srand(static_cast<unsigned int>(time(nullptr)));

        _state = new int[WIDTH * HEIGHT];
        _output = new int[WIDTH * HEIGHT];

        memset(_state, 0, WIDTH * HEIGHT * sizeof(int));
        memset(_output, 0, WIDTH * HEIGHT * sizeof(int));

        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            _state[i] = rand() % 2;
        }
    }

    void on_destruct() override {
        delete [] _state;
        delete [] _output;
    }

    bool on_update(float delta_time) override {
        auto cell = [&](int x, int y) {
            return _output[y * WIDTH + x];
        };

        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            _output[i] = _state[i];
        }

        for (int x = 1; x < WIDTH - 1; ++x) {
            for (int y = 1; y < HEIGHT - 1; ++y) {
                int num_neighbours = cell(x-1, y-1) + cell(x, y-1) + cell(x+1, y-1)
                                   + cell(x-1, y  )                + cell(x+1, y  )
                                   + cell(x-1, y+1) + cell(x, y+1) + cell(x+1, y+1);

                if (cell(x, y) == 1) {
                    _state[y * WIDTH + x] = num_neighbours == 2 || num_neighbours == 3;
                } else {
                    _state[y * WIDTH + x] = num_neighbours == 3;
                }

                if (cell(x, y) == 1) {
                    plot_pixel(x, y, COLOR_WHITE);
                } else {
                    plot_pixel(x, y, COLOR_BLACK);
                }
            }
        }

        return true;
    }

private:
    int* _state;
    int* _output;
};

int main() {
    CellularAutomata app;

    if (!app.create(WIDTH, HEIGHT, CELL_SIZE, CELL_SIZE)) {
        return 1;
    }

    return app.run();
}
