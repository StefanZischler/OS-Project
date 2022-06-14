#include <game_window.h>
#include <emulator.h>
#include <ppu.h>
#include <bus.h>
#include <joypad.h>


//SDL Library for window creation and handle io events
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//define SDL objects
SDL_Window *SDL_window;
SDL_Renderer *SDL_renderer;
SDL_Texture *SDL_texture;
SDL_Surface *SDL_surface;

//initialize and render window
void render_game_windnow() {
    //intialize SDL video
    SDL_Init(SDL_INIT_VIDEO);
    //TTF_Init();
    //initiate window and renderer
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &SDL_window, &SDL_renderer);
    //create surface (SDL_CreateRGBSurface(flag, width, height, depth, red mask, green, mask, blue mask, alpha mask))
    SDL_surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32,
     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
     //create texture
    SDL_texture = SDL_CreateTexture(SDL_renderer, SDL_PIXELFORMAT_ARGB8888,
     SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
     //initialize window position
     int x_position, y_position;
     SDL_GetWindowPosition(SDL_window, &x_position, &y_position);
     SDL_SetWindowPosition(SDL_window, x_position, y_position);
}

//update window with infromation from PPU
void update_window() {
    //create rectangle to setup update screen
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = rect.h = 2048;
    int scalar = 4;

    u32 *display_buffer = ppu_get_context()->display_buffer;

    //loop through screen and fill rectangle
    for (int line_number = 0; line_number < Y_RESOLUTION; line_number++) {
        for (int dots = 0; dots < X_RESOLUTION; dots++) {
            rect.x = dots * scalar;
            rect.y = line_number * scalar;
            rect.w = scalar;
            rect.h = scalar;
            SDL_FillRect(SDL_surface, &rect, display_buffer[dots + (line_number * X_RESOLUTION)]);
        }
    }
    //Update Texture and Renderer
    SDL_UpdateTexture(SDL_texture, NULL, SDL_surface->pixels, SDL_surface->pitch);
    SDL_RenderClear(SDL_renderer);
    SDL_RenderCopy(SDL_renderer, SDL_texture, NULL, NULL);
    SDL_RenderPresent(SDL_renderer);
}

//define keyboard inputs
void keyboard_layout(bool is_pressed, u32 key) {
    switch(key) {
        case SDLK_a: joypad_get_context()->b = is_pressed; break;
        case SDLK_s: joypad_get_context()->a = is_pressed; break;
        case SDLK_RETURN: joypad_get_context()->start = is_pressed; break;
        case SDLK_RSHIFT: joypad_get_context()->select = is_pressed; break;
        case SDLK_UP: joypad_get_context()->up = is_pressed; break;
        case SDLK_DOWN: joypad_get_context()->down = is_pressed; break;
        case SDLK_RIGHT: joypad_get_context()->right = is_pressed; break;
        case SDLK_LEFT: joypad_get_context()->left = is_pressed; break;
    }

}
//handle keyboard inputs
void handel_events() {
    SDL_Event event;
    //while event happend, see if key is pressed or not 
    //or if window was closed
    while(SDL_PollEvent(&event) > 0) {
        //key is pressed
        if (event.type == SDL_KEYDOWN) {
            keyboard_layout(true, event.key.keysym.sym);
        }
        //key not pressed
        if (event.type == SDL_KEYUP) {
            keyboard_layout(false, event.key.keysym.sym);
        }
        //window was closed -> exit emulator
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            emulator_get_context()->exit = true;
        } 
    }
}
