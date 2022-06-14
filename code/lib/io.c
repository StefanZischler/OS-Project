#include <io.h>
#include <timer.h>
#include <joypad.h>
#include <lcd.h>
#include <cpu.h>


// deal with i/o in the specific memory range
/* Source: https://gbdev.io/pandocs/Memory_Map.html#io-ranges
I/O Ranges
The Game Boy uses the following I/O ranges:

Start	End	First appeared	Purpose
$FF00		    DMG	Joypad input
$FF01	$FF02	DMG	Serial transfer
$FF04	$FF07	DMG	Timer and divider
$FF10	$FF26	DMG	Sound
$FF30	$FF3F	DMG	Wave pattern
$FF40	$FF4B	DMG	LCD Control, Status, Position, Scrolling, and Palettes
$FF4F		    CGB	VRAM Bank Select
$FF50		    DMG	Set to non-zero to disable boot ROM
$FF51	$FF55	CGB	VRAM DMA
$FF68	$FF69	CGB	BG / OBJ Palettes
$FF70		    CGB	WRAM Bank Select
*/

//read from the IO
u8 io_read(u16 address) {
    //joypad inputs
    if (address == 0xFF00) {
        return joypad_output();
    }
    //timer inputs
    if (0xFF04 <= address <= 0xFF07) {
        return timer_read(address);
    }

    //get interrupts flags
    if (address == 0xFF0f) {
        return cpu_get_ie_register();
    }
    //LCD Display Status
    if (0xFF40 <= address <= 0xFF4B) {
        // return lcd_status();
        return lcd_read(address);
    }
    printf("Unsupported io_read(%04X)\n", address);
    return 0;
}


//read to the IO
void io_write(u16 address, u8 value) {
    //joypad inputs
    if (address == 0xFF00) {
        joypad_select_mode(value);
        return;
    }
    //timer inputs
    if (0xFF04 <= address <= 0xFF07) {
        timer_write(address, value);
        return; 
    }
    //set interrupt flag
    if (address == 0xFF0F) {
        cpu_set_ie_register(value);
        return;
    }
    //LCD Display Status
    if (0xFF40 <= address <= 0xFF4B) {
        lcd_write(address, value);
        return;
    }
    printf("Unsupported io_write(%04X)\n", address);
}
