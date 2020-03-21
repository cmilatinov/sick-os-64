#include "hardware/interrupts.h"

#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/vga.h"

#include "modules/lib.h"
#include "modules/memory.h"

#include "common/keycodes.h"

extern "C" void kernelMain(void * heapStart){

	clear();

	void * heapEnd = reinterpret_cast<uint8_t*>(heapStart) + (1 << 24);
	MemoryManager memManager(heapStart, heapEnd);

	InterruptManager iManager(IRQ_OFFSET_X86_64);
	KeyboardDriver kb;
	MouseDriver mb;
	VGADriver vga;

	// kb.OnKeyDown(onKeyDown);
	// kb.OnKeyUp(onKeyUp);

	// mb.OnMouseButtonDown(onMouseDown);
	// mb.OnMouseButtonUp(onMouseUp);
	// mb.OnMouseMove(onMouseMove);

	iManager.SetInterruptHandler(0x21, &kb);
	iManager.SetInterruptHandler(0x2C, &mb);

	kb.Activate();
	mb.Activate();
	
	iManager.Activate();

	vga.SetGraphicsMode(VGA_MODE_320_200_256);

	uint8_t * VRAM = reinterpret_cast<uint8_t*>(0xA0000);
	uint32_t x = 500, c = 0;	

	while(1) {
		vga.Clear();

		vga.SetColor(COLOR_LIGHT_GREEN);
		for(uint16_t i = 0; i < 320; i += 5)
			for(uint8_t j = 0; j < 25; j += 5)
				vga.PutPixel(i, j);

		//vga.FillRect(0, 0, x, 1);

		// c = (c + 1) % 10;
		// if(c == 0)
		// 	x = (x + 1) % 8000;

		vga.Draw();
	}

}