#include <pixeng.h>
#include <vector>
#include <random>

#define CELL_SIZE 8
#define WIDTH (600 / CELL_SIZE)
#define HEIGHT (400 / CELL_SIZE)
#define MOVE_TIMER_STEP 0.025f
#define MIN_MOVE_TIMER 0.03f

template <typename T>
struct Point {
    T x;
    T y;
};

class SnakeGame : public PixelEngine {
protected:
    void on_initialize() override {
        srand(static_cast<unsigned int>(time(nullptr)));

        snake_body.push_back({
                WIDTH / 2.0f,
                HEIGHT / 2.0f
                });

        snake_dir = { 1, 0 };
        move_timer = 0.25f;
        current_timer = 0.0f;

        for (int i = 0; i < WIDTH; ++i) {
            walls.push_back({i, 0});
            walls.push_back({i, HEIGHT - 1});
        }

        for (int i = 0; i < HEIGHT; ++i) {
            walls.push_back({0, i});
            walls.push_back({WIDTH - 1, i});
        }

        reposition_apple();
    }

    bool on_update(float delta_time) override {
        current_timer += delta_time;

        if (!handle_input()) {
            return false;
        }

        if (snake_collide_with_self() ||
            snake_collide_with_wall()) {
            return false;
        }

        if (snake_collide_with_apple()) {
            reposition_apple();
            add_part_to_snake();
            
            move_timer -= MOVE_TIMER_STEP;
            if (move_timer < MIN_MOVE_TIMER)
                move_timer = MIN_MOVE_TIMER;
        }

        move_snake();
        wrap_around_position();

        plot_pixel(apple_pos.x, apple_pos.y, COLOR_GREEN);
        for (auto& wall : walls) {
            plot_pixel((int)wall.x, (int)wall.y, COLOR_BLUE);
        }
        for (auto& part : snake_body) {
            plot_pixel((int)part.x, (int)part.y, COLOR_RED);
        }

        return true;
    }

    bool snake_collide_with_self() {
        const auto& head = snake_body[0];

        // There is no need to check the first 4 parts since they
        // cant collide.
        for (int i = 4; i < snake_body.size(); ++i) {
            const auto& part = snake_body[i];

            if ((int)head.x == (int)part.x &&
                (int)head.y == (int)part.y) {
                return true;
            }
        }

        return false;
    }

    bool snake_collide_with_wall() {
        const auto& head = snake_body[0];

        for (const auto& wall : walls) {
            if ((int)head.x == wall.x &&
                (int)head.y == wall.y) {
                return true;
            }
        }

        return false;
    }

    void reposition_apple() {
        do {
            apple_pos = { rand() % WIDTH, rand() % HEIGHT };
        } while (snake_collide_with_apple());
    }

    void move_snake() {
        if (current_timer >= move_timer) {
            current_timer -= move_timer;

            for (int i = snake_body.size() - 1; i > 0; --i) {
                snake_body[i].x = snake_body[i-1].x;
                snake_body[i].y = snake_body[i-1].y;
            }

            snake_body[0].x += snake_dir.x;
            snake_body[0].y += snake_dir.y;
        }
    }

    void add_part_to_snake() {
        snake_body.push_back({
                snake_body.back().x,
                snake_body.back().y
                });
    }

    bool handle_input() {
        if (get_key_state(Keyboard::KEY_LEFT).pressed) {
            snake_dir.y = 0;

            if (snake_dir.x == 0) {
                snake_dir.x = -1;
                current_timer = move_timer;
            }
        }

        if (get_key_state(Keyboard::KEY_RIGHT).pressed) {
            snake_dir.y = 0;

            if (snake_dir.x == 0) {
                snake_dir.x = 1;
                current_timer = move_timer;
            }
        }

        if (get_key_state(Keyboard::KEY_DOWN).pressed) {
            snake_dir.x = 0;

            if (snake_dir.y == 0) {
                snake_dir.y = 1;
                current_timer = move_timer;
            }
        }

        if (get_key_state(Keyboard::KEY_UP).pressed) {
            snake_dir.x = 0;

            if (snake_dir.y == 0) {
                snake_dir.y = -1;
                current_timer = move_timer;
            }
        }

        if (get_key_state(Keyboard::KEY_Q).pressed) {
            return false;
        }

        return true;
    }

    bool snake_collide_with_apple() {
        for (auto& part : snake_body) {
            if (apple_pos.x == (int)part.x &&
                apple_pos.y == (int)part.y) {
                return true;
            }
        }

        return false;
    }

    void wrap_around_position() {
        auto& snake_head = snake_body[0];

        if ((int)snake_head.x < 0)       snake_head.x = WIDTH - 1;
        if ((int)snake_head.x >= WIDTH)   snake_head.x = 0;
        if ((int)snake_head.y < 0)       snake_head.y = HEIGHT - 1;
        if ((int)snake_head.y >= HEIGHT)  snake_head.y = 0;
    }

private:
    std::vector<Point<float>> snake_body;
    std::vector<Point<int>> walls;
    Point<int> apple_pos;
    Point<int> snake_dir;

    float move_timer;
    float current_timer;
};

int main() {
    SnakeGame app;

    if (!app.create(WIDTH, HEIGHT, CELL_SIZE, CELL_SIZE)) {
        return 1;
    }

    return app.run();
}
