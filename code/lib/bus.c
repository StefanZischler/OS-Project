#include <bus.h>
#include <rom.h>
#include <ram.h>
#include <cpu.h>

/* Memory Map for the GameBoy
0000	3FFF	ROM Bank 0
4000	7FFF	ROM Bank 1 - Switchable 
8000	9FFF	Video RAM (VRAM)
A000	BFFF	External RAM/Cartridge RAM
C000	CFFF	Work RAM (WRAM)/ROM Bank 0
D000	DFFF	Work RAM (WRAM)/ROM Bank 1-7 - switchable - only for GameBoy Color
E000	FDFF	Mirror of C000~DDFF (ECHO RAM) - Nintendo says use of this area is prohibited.
FE00	FE9F	Sprite attribute table (OAM)	
FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited
FF00	FF7F	I/O Registers	
FF80	FFFE	High RAM (HRAM)	
FFFF	FFFF	Interrupt Enable register (IE)
*/

//read function for 16-bit address and 8-bit value
u8 bus_read(u16 address) {
    //test address in increasing order and call expected read function
    if (address < 0x8000) {
        //ROM/Cartridge Data
        return rom_read(address);
    } else if (address < 0xA000) {
        //Video RAM
        //TODO: Implement ppu_read
        return 0;
    } else if (address < 0xC000) {
        //Cartridge RAM
        return rom_read(address);
    } else if (address < 0xE000) {
        //Working RAM (wram)
        return work_ram_read(address);
    } else if (address < 0xFE00) {
        //ECHO RAM (not usable)
        return 0;
    } else if (address < 0xFEA0) {
        //Sprite attribute table (OAM)
        //TODO: Implement oam_write
        return 0;
    } else if (address < 0xFF00) {
        //ECHO RAM (not usable)
        return 0;
    } else if (address < 0xFF80) {
        //I/O Registers
        //TODO: Implement io_write
        return 0;
    } else if (address < 0xFFFF) {
        //High RAM (HRAM)	
        return high_ram_read(address);
    } else if (address == 0xFFFF) {
        //Interrupt Enable register (IE)	
        return cpu_get_ie_register();
    }
}
//write function for 16-bit address and 8-bit value
void bus_write(u16 address, u8 value) {
    //test address in increasing order and call expected write function
    if (address < 0x8000) {
        //ROM/Cartridge Data
        return rom_write(address, value);
    } else if (address < 0xA000) {
        //Video RAM
        //TODO: Implement ppu_write
        return;
    } else if (address < 0xC000) {
        //Cartridge RAM
        return rom_write(address, value);
    } else if (address < 0xE000) {
        //Working RAM (wram)
        return work_ram_write(address, value);
    } else if (address < 0xFE00) {
        //ECHO RAM (not usable)
        return;
    } else if (address < 0xFEA0) {
        //Sprite attribute table (OAM)
        //TODO: Implement oam_read
        return;
    } else if (address < 0xFF00) {
        //ECHO RAM (not usable)
        return;
    } else if (address < 0xFF80) {
        //I/O Registers
        //TODO: Implement io_read
        return;
    } else if (address < 0xFFFF) {
        //High RAM (HRAM)	
        return high_ram_write(address, value);
    } else if (address == 0xFFFF) {
        //Interrupt Enable register (IE)	
        return cpu_set_ie_register(value);
    }
}

//read function for 16-bit address and 16-bit value
u16 bus_read_16(u16 address) {
    //get lower and higher bit form address
    u16 lower = bus_read(address);
    u16 higher = bus_read(address + 1);
    // return lower OR higher (moved bit 8bit to the left)
    return lower | (higher << 8);

}
//write function for 16-bit address and 16-bit value
void bus_write_16(u16 address, u16 value) {
    //call bus_write with adjusted address and value
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}

