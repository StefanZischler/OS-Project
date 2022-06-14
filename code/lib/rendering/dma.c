#include <dma.h>
#include <bus.h>
#include <ppu.h>

// DMA handles communication between CPU and OAM

typedef struct {
    bool running;
    u8 current_byte;
    u8 value;
    u8 delay_on_start;
} dma_context;

static dma_context dma;
//set beginning variables for DMA
void dma_beginning(u8 start_value) {
    dma.running = true;
    dma.value = start_value;
    dma.current_byte = 0;
    //two emulator cycles before DMA can start
    dma.delay_on_start = 2;
}

//bool for testing if DMA is currently transferring data
bool is_dma_transferring() {
    return dma.running;
}

//test if DMA is active and when ready start OAM write from PPU
void dma_ticks() {
    //test if DMA is running, only continue if its running
    if (!dma.running) {
        return;
    }

    //test if delay reached Zero
    if (dma.delay_on_start) {
        dma.delay_on_start--;
        return;
    }

    //if thats the case start OAM write
    //adjust value value is divided by $100 (https://gbdev.io/pandocs/OAM_DMA_Transfer.html)
    ppu_oam_write(dma.current_byte, bus_read((dma.value * 0x100) + dma.current_byte));

    dma.current_byte++;

    //let DMA run until maximum of 9F bytes
    dma.running = dma.current_byte < 0xA0;
}

