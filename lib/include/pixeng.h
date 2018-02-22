#pragma once

/*
 * TODO:
 *  - Add 'on_mouse_event'-method
 *  - Implement filled rect plotting
 *  - Implement circle plotting
 *  - Implement filled circle plotting
 *  - Add more predefined colors
 *
 *  - Start playing around with some sample projects
 *      - Visualize sorting
 *      - A* (and other pathfinding algorithms)
 *      - Maze generation
 */
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

static const Color COLOR_BLACK  = {0, 0, 0};
static const Color COLOR_WHITE  = {255, 255, 255};
static const Color COLOR_RED    = {255, 0, 0};
static const Color COLOR_GREEN  = {0, 255, 0};
static const Color COLOR_BLUE   = {0, 0, 255};

Color hex_to_color(unsigned int hex) {
    return {
        static_cast<unsigned char>((hex & 0xFF0000) >> 16),
        static_cast<unsigned char>((hex & 0x00FF00) >> 8),
        static_cast<unsigned char>((hex & 0x0000FF))
    };
}

struct KeyState {
    bool pressed;
    bool released;
    bool held;
};

enum class Keyboard {
    KEY_A = 4,
    KEY_B = 5,
    KEY_C = 6,
    KEY_D = 7,
    KEY_E = 8,
    KEY_F = 9,
    KEY_G = 10,
    KEY_H = 11,
    KEY_I = 12,
    KEY_J = 13,
    KEY_K = 14,
    KEY_L = 15,
    KEY_M = 16,
    KEY_N = 17,
    KEY_O = 18,
    KEY_P = 19,
    KEY_Q = 20,
    KEY_R = 21,
    KEY_S = 22,
    KEY_T = 23,
    KEY_U = 24,
    KEY_V = 25,
    KEY_W = 26,
    KEY_X = 27,
    KEY_Y = 28,
    KEY_Z = 29,

    KEY_1 = 30,
    KEY_2 = 31,
    KEY_3 = 32,
    KEY_4 = 33,
    KEY_5 = 34,
    KEY_6 = 35,
    KEY_7 = 36,
    KEY_8 = 37,
    KEY_9 = 38,
    KEY_0 = 39,

    KEY_RETURN = 40,
    KEY_ESCAPE = 41,
    KEY_BACKSPACE = 42,
    KEY_TAB = 43,
    KEY_SPACE = 44,

    KEY_RIGHT = 79,
    KEY_LEFT = 80,
    KEY_DOWN = 81,
    KEY_UP = 82,
};

enum class Mouse {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE = 2,
    MOUSE_RIGHT = 3,
    MOUSE_X1 = 4,
    MOUSE_X2 = 5,
};

class PixelEngine {
public:
    PixelEngine();
    ~PixelEngine();

    bool create(int num_w_pixels,
                int num_h_pixels,
                int pixel_width,
                int pixel_height);

    int run();

    // Rendering functionallity
    void plot_pixel(int x, int y, Color color);
    void plot_line(int x1, int y1, int x2, int y2, Color color);
    void plot_line_horiz(int x1, int x2, int y, Color color);
    void plot_line_vert(int y1, int y2, int x, Color color);
    void plot_rectangle(int x1, int y1, int x2, int y2, Color color);
    void plot_rectangle_filled(int x1, int y1, int x2, int y2, Color color);

    // Input functionallity
    const KeyState& get_key_state(Keyboard key) const;
    const KeyState& get_mouse_state(Mouse key) const;
    int get_mouse_x() const;
    int get_mouse_y() const;

protected:
    virtual void on_initialize() = 0;
    virtual bool on_update(float delta_time) = 0;

private:
    int real_window_width() const;
    int real_window_height() const;
    void clamp_x_coord(int& x);
    void clamp_y_coord(int& y);

    int _num_w_pixels;
    int _num_h_pixels;
    int _pixel_width;
    int _pixel_height;

    KeyState _keys[256];
    KeyState _mouse[5];
    int _mouse_x;
    int _mouse_y;
};
