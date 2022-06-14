#include <joypad.h>

//handle all joypad related input and outputs


typedef struct {
    bool select_buttons;
    bool select_movement;
    joypad_context controller_state;
} joypad_infos;

static joypad_infos jp = {0};

//buttons mode selected
bool joypad_select_buttons() {
    return jp.select_buttons;
}
//movement mode selected 
bool joypad_select_movement() {
    return jp.select_movement;
}

//select if button or movement mode
void joypad_select_mode(u8 value) {
    jp.select_movement = value & 0x10;
    jp.select_buttons = value & 0x20;
}

//get joypad output
u8 joypad_output() {
    //define ooutput variable
    //at start everything set -> which means not pressed
    u8 jp_output = 0xCF;

    //if buttons mode
    if(!joypad_select_buttons()) {
        if(joypad_get_context()->a) {
            jp_output &= ~(1 << 0);
        }
        if(joypad_get_context()->b) {
            jp_output &= ~(1 << 1);
        }
        if(joypad_get_context()->start) {
            jp_output &= ~(1 << 2);
        }
        if(joypad_get_context()->select) {
            jp_output &= ~(1 << 3);
        }
    }

    //if movement mode
    if(!joypad_select_movement()) {
        if(joypad_get_context()->right) {
            jp_output &= ~(1 << 0);
        }
        if(joypad_get_context()->left) {
            jp_output &= ~(1 << 1);
        }
        if(joypad_get_context()->up) {
            jp_output &= ~(1 << 2);
        }
        if(joypad_get_context()->down) {
            jp_output &= ~(1 << 3);
        }
    }
    return jp_output;
}
//get joypad infos
joypad_context *joypad_get_context() {
    return &jp.controller_state;
}