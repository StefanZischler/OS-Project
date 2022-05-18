#include <rom.h>
#include <string.h>

//ROM context information
typedef struct {
    char filename[1024];
    u8 *rom_data;
    u32 rom_size;
    rom_header *header;
} rom_context;

static rom_context r_ctx;

//Licence Codes from the different publishers, taken from: https://gbdev.io/pandocs/The_Cartridge_Header.html
static const char *LICENCE_CODE[0xA5] = {
    [0x00] =    "None",
    [0x01] =    "Nintendo R&D1",
    [0x08] =    "Capcom",
    [0x13] =    "Electronic Arts",
    [0x18] =    "Hudson Soft",
    [0x19] =    "b-ai",
    [0x20] =    "kss",
    [0x22] =    "pow",
    [0x24] =    "PCM Complete",
    [0x25] =    "san-x",
    [0x28] =    "Kemco Japan",
    [0x29] =    "seta",
    [0x30] =	"Viacom",
    [0x31] =	"Nintendo",
    [0x32] =	"Bandai",
    [0x33] =	"Ocean/Acclaim",
    [0x34] =	"Konami",
    [0x35] =	"Hector",
    [0x37] =	"Taito",
    [0x38] =	"Hudson",
    [0x39] =	"Banpresto",
    [0x41] =	"Ubi Soft",
    [0x42] =	"Atlus",
    [0x44] =	"Malibu",
    [0x46] =	"angel",
    [0x47] =	"Bullet-Proof",
    [0x49] =	"irem",
    [0x50] =	"Absolute",
    [0x51] =	"Acclaim",
    [0x52] =	"Activision",
    [0x53] =	"American sammy",
    [0x54] =	"Konami",
    [0x55] =	"Hi tech entertainment",
    [0x56] =	"LJN",
    [0x57] =	"Matchbox",
    [0x58] =	"Mattel",
    [0x59] =	"Milton Bradley",
    [0x60] =	"Titus",
    [0x61] =	"Virgin",
    [0x64] =	"LucasArts",
    [0x67] =	"Ocean",
    [0x69] =	"Electronic Arts",
    [0x70] =	"Infogrames",
    [0x71] =	"Interplay",
    [0x72] =	"Broderbund",
    [0x73] =	"sculptured",
    [0x75] =	"sci",
    [0x78] =	"THQ",
    [0x79] =    "Accolade",
    [0x80] =	"misawa",
    [0x83] =	"lozc",
    [0x86] =	"Tokuma Shoten Intermedia",
    [0x87] =	"Tsukuda Original",
    [0x91] =	"Chunsoft",
    [0x92] =	"Video system",
    [0x93] =	"Ocean/Acclaim",
    [0x95] =	"Varie",
    [0x96] =	"Yonezawa/s’pal",
    [0x97] =	"Kaneko",
    [0x99] =	"Pack in soft",
    [0xA4] =	"Konami (Yu-Gi-Oh!)"
};

//ROM types from the different publishers, taken from: https://gbdev.io/pandocs/The_Cartridge_Header.html
static const char *ROM_TYPES[] = {
   "ROM ONLY"
   "MBC1"
   "MBC1+RAM"
   "MBC1+RAM+BATTERY"
   "MBC2"
   "MBC2+BATTERY"
   "ROM+RAM 1"
   "ROM+RAM+BATTERY 1"
   "MMM01"
   "MMM01+RAM"
   "MMM01+RAM+BATTERY"
   "MBC3+TIMER+BATTERY"
   "MBC3+TIMER+RAM+BATTERY 2"
   "MBC3"
   "MBC3+RAM 2"
   "MBC3+RAM+BATTERY 2"
   "MBC5"
   "MBC5+RAM"
   "MBC5+RAM+BATTERY"
   "MBC5+RUMBLE"
   "MBC5+RUMBLE+RAM"
   "MBC5+RUMBLE+RAM+BATTERY"
   "MBC6"
   "MBC7+SENSOR+RUMBLE+RAM+BATTERY"
   "POCKET CAMERA"
   "BANDAI TAMA5"
   "HuC3"
   "HuC1+RAM+BATTERY"
};

//get licence name
const char *rom_licence_name(){
    if(r_ctx.header->new_licence_code <= 0xA4) {
        return LICENCE_CODE[r_ctx.header->old_licence_code];
    }
    return "Not known Licence Code!";
}

//get ROM type
const char *rom_type_name(){
    if(r_ctx.header->rom_type <= 0x22) {
        return ROM_TYPES[r_ctx.header->rom_type];
    }
    return "Not known Licence Code!";
}
//read and write function for the ROM/Cartridge
u8 rom_read(u16 address) {
    //test if Catridge is ROM only (e.g. Tetris)
    if( address < 0x4000) {
        return r_ctx.rom_data[address];
    }
}
void rom_write(u16 address, u8 value) {
    //if ROM only, doesnt need to write to ROM
    return;
}

//load rom data
bool rom_load(char *rom) {
    //TODO: implement load function and display ROM information
}