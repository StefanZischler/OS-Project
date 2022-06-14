#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <define.h>

//set window size
static const int WINDOW_WIDTH =  800;
static const int WINDOW_HEIGHT =  600;

//inizialize and update game window
void render_game_windnow();
void update_window();

//handle events inputs
void handel_events();

#endif