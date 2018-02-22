#include "pixeng.h"
#include <iostream>
#include <string.h>
#include <math.h>
#include <SDL.h>

template <typename T>
int sign(T val) {
    return ((val < 0) ? -1 : ((val > 0) ? 1 : 0));
}

template <typename T>
T clamp(const T& val, const T& low, const T& high) {
    if (val < low) return low;
    if (val > high) return high;
    return val;
}

namespace globals {
    SDL_Window* sdl_window      = nullptr;
    SDL_Renderer* sdl_renderer  = nullptr;
    SDL_Texture* back_buffer    = nullptr;
    Color* pixels               = nullptr;
}

PixelEngine::PixelEngine()
: _num_w_pixels(0)
, _num_h_pixels(0)
, _pixel_width(0)
, _pixel_height(0)
{
}

PixelEngine::~PixelEngine() {
    delete [] globals::pixels;

    if (globals::sdl_renderer)
        SDL_DestroyRenderer(globals::sdl_renderer);

    if (globals::sdl_window)
        SDL_DestroyWindow(globals::sdl_window);

    SDL_Quit();
}

bool PixelEngine::create(int num_w_pixels,
                         int num_h_pixels,
                         int pixel_width,
                         int pixel_height) {

    _num_w_pixels = num_w_pixels;
    _num_h_pixels = num_h_pixels;
    _pixel_width = pixel_width;
    _pixel_height = pixel_height;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    globals::sdl_window = SDL_CreateWindow(
            "pixeng",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            real_window_width(),
            real_window_height(),
            SDL_WINDOW_SHOWN);

    if (globals::sdl_window == nullptr) {
        return false;
    }

    globals::sdl_renderer = SDL_CreateRenderer(
            globals::sdl_window,
            -1,
            0);

    if (globals::sdl_renderer == nullptr) {
        return false;
    }

    // Create streaming render texture
    globals::back_buffer = SDL_CreateTexture(
            globals::sdl_renderer,
            SDL_PIXELFORMAT_RGB24,
            SDL_TEXTUREACCESS_STREAMING,
            real_window_width(),
            real_window_height());

    globals::pixels = new Color[real_window_width() * real_window_height()];

    memset(_keys, 0x00, sizeof(KeyState) * 256);
    memset(_mouse, 0x00, sizeof(KeyState) * 5);
    _mouse_x = 0;
    _mouse_y = 0;

    on_initialize();

    return true;
}

int PixelEngine::run() {
    bool quit = false;
    unsigned int prev_time = 0;
    unsigned int curr_time = 0;
    float delta_time = 0;
    SDL_Event e;

    while (!quit) {
        prev_time = curr_time;
        curr_time = SDL_GetTicks();
        delta_time = (curr_time - prev_time) / 1000.0f;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        auto current_state = SDL_GetKeyboardState(nullptr);
        auto mouse_state = SDL_GetMouseState(&_mouse_x, &_mouse_y);

        for (int i = 0; i < 256; ++i) {
            _keys[i].pressed = false;
            _keys[i].released = false;

            if (current_state[i]) {
                _keys[i].pressed = !_keys[i].held;
                _keys[i].held = true;
            } else {
                _keys[i].released = true;
                _keys[i].held = false;
            }
        }

        for (int i = 0; i < 5; ++i) {
            _mouse[i].pressed = false;
            _mouse[i].released = false;

            if (mouse_state & SDL_BUTTON(i + 1)) {
                _mouse[i].pressed = _mouse[i].held;
                _mouse[i].held = true;
            } else {
                _mouse[i].released = false;
                _mouse[i].held = false;
            }
        }

        // Begin the frame by clering out the pixel-array (to the color black)
        // Note: This could be nice to let the user modify
        memset(globals::pixels, 0x00, sizeof(Color) * real_window_width() * real_window_height());

        if (!on_update(delta_time)) {
            quit = true;
        }

        SDL_UpdateTexture(
                globals::back_buffer,
                nullptr,
                globals::pixels,
                real_window_width() * sizeof(Color));

        SDL_RenderCopy(
                globals::sdl_renderer,
                globals::back_buffer,
                nullptr,
                nullptr);

        SDL_RenderPresent(globals::sdl_renderer);
    }

    return 0;
}

void PixelEngine::plot_pixel(int x, int y, Color color) {
    if (x >= 0 && x < _num_w_pixels && y >= 0 && y < _num_h_pixels) {
        int x1 = (x * _pixel_width) % real_window_width();
        int y1 = (y * _pixel_height) % real_window_height();
        int x2 = x1 + _pixel_width;
        int y2 = y1 + _pixel_height;

        for (int px = x1; px < x2; ++px) {
            for (int py = y1; py < y2; ++py) {
                globals::pixels[py * real_window_width() + px] = color;
            }
        }
    }
}

void PixelEngine::plot_line(int x1, int y1, int x2, int y2, Color color) {
    clamp_x_coord(x1);
    clamp_x_coord(x2);
    clamp_y_coord(y1);
    clamp_y_coord(y2);

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int delta_x_abs = abs(delta_x);
    int delta_y_abs = abs(delta_y);
    int delta_x_sign = sign(delta_x);
    int delta_y_sign = sign(delta_y);

    int x = delta_x_abs >> 1;
    int y = delta_y_abs >> 1;
    int px = x1;
    int py = y1;

    plot_pixel(px, py, color);

    if (delta_x_abs >= delta_y_abs) {
        for (int i = 0; i < delta_x_abs; ++i) {
            y += delta_y_abs;

            if (y >= delta_x_abs) {
                y -= delta_x_abs;
                py += delta_y_sign;
            }

            px += delta_x_sign;
            plot_pixel(px, py, color);
        }
    } else {
        for (int i = 0; i < delta_y_abs; ++i) {
            x += delta_x_abs;

            if (x >= delta_y_abs) {
                x -= delta_y_abs;
                px += delta_x_sign;
            }

            py += delta_y_sign;
            plot_pixel(px, py, color);
        }
    }
}

void PixelEngine::plot_line_horiz(int x1, int x2, int y, Color color) {
    clamp_x_coord(x1);
    clamp_x_coord(x2);
    clamp_y_coord(y);

    for (int x = x1; x <= x2; ++x) {
        plot_pixel(x, y, color);
    }
}

void PixelEngine::plot_line_vert(int y1, int y2, int x, Color color) {
    clamp_y_coord(y1);
    clamp_y_coord(y2);
    clamp_x_coord(x);

    for (int y = y1; y <= y2; ++y) {
        plot_pixel(x, y, color);
    }
}

void PixelEngine::plot_rectangle(int x1, int y1, int x2, int y2, Color color) {
    plot_line_vert(y1, y2, x2, color);
    plot_line_horiz(x1, x2, y1, color);
    plot_line_horiz(x1, x2, y2, color);
    plot_line_vert(y1, y2, x1, color);
}

const KeyState& PixelEngine::get_key_state(Keyboard key) const {
    return _keys[static_cast<int>(key)];
}

int PixelEngine::real_window_width() const {
    return _num_w_pixels * _pixel_width;
}

int PixelEngine::real_window_height() const {
    return _num_h_pixels * _pixel_height;
}

void PixelEngine::clamp_x_coord(int& x) {
    x = clamp(x, 0, _num_w_pixels - 1);
}

void PixelEngine::clamp_y_coord(int& y) {
    y = clamp(y, 0, _num_h_pixels - 1);
}

const KeyState& PixelEngine::get_mouse_state(Mouse key) const {
}

int PixelEngine::get_mouse_x() const {
    return _mouse_x;
}

int PixelEngine::get_mouse_y() const {
    return _mouse_y;
}
