#ifndef JOYPAD_H
#define JOYPAD_H

// structure and function for joypad handling

#include <define.h>

typedef struct {
//buttons
  bool a;
  bool b;
  bool start;
  bool select;

//Movement
  bool down;
  bool up;
  bool left;
  bool right;
} joypad_context;


//buttons mode selected
bool joypad_select_buttons();
//movement mode selected 
bool joypad_select_movement();
//get joypad output
u8 joypad_output();
//select if button or movement mode
void joypad_select_mode(u8 value);
//get joypad infos
joypad_context *joypad_get_context();
#endif