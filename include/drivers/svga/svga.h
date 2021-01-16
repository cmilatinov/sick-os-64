#pragma once

#include "hardware/interrupts.h"
#include "hardware/pci.h"

#include "modules/drivers.h"

#include "drivers/svga/svga_def.h"

#define SVGA_OK 0
#define ERR_SVGA_DEVICE_NOT_FOUND 1 

#define SVGA_FIFO_START             0xFFFFFFFF00000000
#define SVGA_FRAMEBUFFER_START      0xFFFFFFFF10000000

using PCI::PCIDevice;
using PCI::PCIAddress;
using PCI::PCIBaseAddressRegister;

struct SVGADevice : public PCIDevice {
        
    // Address information
    uint32_t    ioBase;
    uint32_t *  fifoMemory;
    uint32_t    fifoSize;
    uint8_t *   framebufferMemory;
    uint32_t    framebufferSize;
    uint32_t    vramSize;

    // Device version and capabilities
    uint32_t    deviceVersionID;
    uint32_t    capabilities;

    // Video mode information
    uint32_t    width;
    uint32_t    height;
    uint32_t    bpp;
    uint32_t    pitch;

    struct {
        uint32_t    reservedSize;
        bool        usingBounceBuffer;
        uint8_t     bounceBuffer[1 << 20];
        uint32_t    nextFence;
    } fifo;

    struct {
        uint32_t            pending;
        uint32_t            switchContext;
        InterruptContext    oldContext;
        InterruptContext    newContext;
        uint32_t            count;
    } irq;


};

struct SVGAFifoCmdFence {
    uint32_t id;
    uint32_t fence;
} PACKED;

class SVGA : public Driver, public InterruptHandler {

    public:
    // SVGA device information
    SVGADevice svga;

    // Reads an SVGA register at the given index
    uint32_t ReadRegister(uint32_t index);

    // Writes to the SVGA register at the given index
    void WriteRegister(uint32_t index, uint32_t value);

    // Clears any pending stored IRQs
    // Any interrupts that occurred before a call to this function
    // will be ignored by the next WaitForIRQ call.
    // Returns the IRQ flags set before the clear.
    uint32_t ClearIRQ();

    // Waits for the next interrupt request and returns any flags raised by it
    uint32_t WaitForIRQ();

    // Wakes the device if it is not processing any commands currently
    void RingDoorbell();

    // Check if FIFO has a certain capability
    inline bool FIFO_HasCapability(uint32_t cap) {
        return (svga.fifoMemory[SVGA_FIFO_CAPABILITIES] & cap) != 0;
    }
    
    // Check if a certain FIFO register exists
    inline bool FIFO_IsRegisterValid(uint32_t reg) {
        return svga.fifoMemory[SVGA_FIFO_MIN] > (reg << 2);    
    }

    // Called whenever the FIFO is full and the system 
    // must wait for the device to process commands in order to send more
    void FIFO_Full();
    
    // Reserve at least *bytes* number of bytes in the FIFO memory
    // Used to store one or more commands
    // Must be accompanied by exactly one FIFO_Commit call
    void * FIFO_Reserve(uint32_t bytes);
    void * FIFO_Reserve(uint32_t type, uint32_t bytes);
    
    // Commit *bytes* number of bytes to the FIFO
    void FIFO_Commit(uint32_t bytes);

    // Commit all reserved FIFO space
    void FIFO_CommitAll();

    // Insert a fence in the FIFO
    uint32_t InsertFence();

    public:
    virtual const char * DisplayName() final { return "VMWare SVGA II"; };

    virtual void Load() final {};
    virtual void Reset() final {};
    virtual void Destroy() final {};

    virtual void HandleInterrupt(uint8_t vector) final;

    // Initializes the SVGA device and enables it
    uint32_t Init();

    // Enables the SVGA device
    void Enable();

    // Disables the SVGA device
    void Disable();

    // Set the SVGA display mode
    void SetMode(uint32_t width, uint32_t height, uint32_t bpp);

};