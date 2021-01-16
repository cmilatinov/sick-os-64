#include "hardware/pci.h"
#include "modules/lib.h" 
#include "modules/memory.h"

using namespace PCI;

namespace {

    const Port32 data32 = Port32(PCI_DATA_PORT);
    const Port16 data16 = Port16(PCI_DATA_PORT);
    const Port8 data8 = Port8(PCI_DATA_PORT);
    const Port32 command = Port32(PCI_COMMAND_PORT);

    PCIDevice * devices = nullptr;
    uint32_t numDevices = 0;

    bool GetDevice(const PCIAddress * addr, PCIDevice * device) {
        
        uint32_t config00 = ReadConfig32(addr, 0x00);
        if ((config00 & 0xFFFF) == PCI_VENDOR_ID_NONE)
            return false;

        device->addr = *addr;

        device->vendorID = config00 & 0xFFFF;
        device->deviceID = (config00 >> 16) & 0xFFFF;

        uint32_t config08 = ReadConfig32(addr, 0x08);
        device->revisionID = config08 & 0xFF;
        device->interface = (config08 >> 8) & 0xFF;
        device->subclassID = (config08 >> 16) & 0xFF;
        device->classID = (config08 >> 24) & 0xFF;

        uint32_t config0C = ReadConfig32(addr, 0x0C);
        device->headerType = (config0C >> 16) & 0x7F;
        device->multifunction = ((config0C >> 16) & 0x80) > 0;

        uint32_t config3C = ReadConfig32(addr, 0x3C);
        device->interruptLine = config3C & 0xFF;

        return true;
        
    }

}

namespace PCI {

    uint32_t ReadConfig32(const PCIAddress * addr, uint8_t offset) {
        command.Write(addr->Pack(offset));
        return data32.Read();
    }

    uint16_t ReadConfig16(const PCIAddress * addr, uint8_t offset) {
        command.Write(addr->Pack(offset));
        return data16.Read();
    }

    uint8_t ReadConfig8(const PCIAddress * addr, uint8_t offset) {
        command.Write(addr->Pack(offset));
        return data8.Read();
    }

    void WriteConfig32(const PCIAddress * addr, uint8_t offset, uint32_t value) {
        command.Write(addr->Pack(offset));
        return data32.Write(value);
    }

    void WriteConfig16(const PCIAddress * addr, uint8_t offset, uint16_t value) {
        command.Write(addr->Pack(offset));
        return data16.Write(value);
    }

    void WriteConfig8(const PCIAddress * addr, uint8_t offset, uint8_t value) {
        command.Write(addr->Pack(offset));
        return data8.Write(value);
    }

    uint32_t ScanBus() {
        
        static const uint32_t MAX_DEVICES = 10;

        PCIDevice tempDevices[MAX_DEVICES];
        uint32_t devCount = 0;

        // Loop for each bus
        for (uint32_t bus = 0; bus < PCI_NUM_BUSES; bus++) {

            // Loop for each device
            for (uint32_t device = 0; device < PCI_NUM_DEVICES; device++) {
                
                // Create address struct
                PCIAddress addr;
                addr.bus = bus;
                addr.device = device;
                addr.function = 0;

                // Get first function, if device is multifunction, keep getting next function until max functions or no device.
                if (GetDevice(&addr, tempDevices + devCount) && ++devCount < MAX_DEVICES && tempDevices[devCount].multifunction) {

                    do {
                        addr.function++;
                    } while (addr.function < PCI_NUM_FUNCTIONS && GetDevice(&addr, tempDevices + devCount) && ++devCount < MAX_DEVICES);

                }

                if (devCount >= MAX_DEVICES)
                    goto maxDevicesReached;

            }
        }

        maxDevicesReached:

        // Free device list if exists
        if (devices != nullptr)
            free(devices);

        // // Allocate space for device list
        devices = reinterpret_cast<PCIDevice*>(malloc(devCount * sizeof(PCIDevice)));
        numDevices = devCount;

        // Copy device list
        for (uint32_t i = 0; i < numDevices; i++)
            devices[i] = tempDevices[i];

        return numDevices;

    }

    bool FindDevice(uint16_t vendorID, uint16_t deviceID, PCIDevice * device) {
        
        // Loop for every device found
        for (uint32_t i = 0; i < numDevices; i++) {
            
            // If device matches, copy device info and return true
            PCIDevice dev = devices[i];
            if (dev.vendorID == vendorID && dev.deviceID == deviceID) {
                *device = dev;
                return true;
            }

        }

        // No device found
        return false;

    }

    void SetDeviceEnabled(const PCIAddress * addr, bool enable) {

        uint16_t command = ReadConfig16(addr, 0x04);

        const uint16_t flags = 0x0007;

        if (enable)
            command |= flags;
        else
            command &= ~flags;

        WriteConfig16(addr, 0x04, command);

    }

    bool GetDeviceBAR(const PCIAddress * addr, uint8_t index, PCIBaseAddressRegister * bar) {
        
        // Check if device exists
        PCIDevice device;
        if (!GetDevice(addr, &device))
            return false;
        
        // Check if BAR exists
        uint32_t numBARs = 6 - 4 * (device.headerType & 0x7F);
        if (index >= numBARs)
            return false;

        uint32_t barContent = ReadConfig32(addr, 0x10 + (4 * index));
        
        if ((barContent & 0x1) == PCI_BAR_IO_SPACE) { 

            // I/O space BAR
            uint32_t address = barContent & 0xFFFFFFFC;
            
            bar->space = PCI_BAR_IO_SPACE;
            bar->address = address;

        } else {

            // Memory space BAR
            bar->space = PCI_BAR_MEMORY_SPACE;
            bar->type = (barContent >> 1) & 0x3;

            // Prefetchable Memory = reading causes no side effects, reading the same location always gives the same result, can be read in bursts
            // Non-Prefetchable Memory = reading causes side effects, cannot be read in bursts, only read when needed
            // Example: - Reading a FIFO register; its pointer changes after a read operation and so does the data inside.
            //          - Reading a register that is cleared upon being read.
            bar->prefetchable = (barContent & 0x8) > 0;

            // Find BAR address
            switch (bar->type) {
                case PCI_BAR_MEMORY_TYPE_16:
                    bar->address = barContent & 0xFFF0;
                    break;
                case PCI_BAR_MEMORY_TYPE_32:
                    bar->address = barContent & 0xFFFFFFF0;
                    break;
                case PCI_BAR_MEMORY_TYPE_64:
                    uint64_t bar2Content = ReadConfig32(addr, 0x10 + (4 * (index + 1)));
                    bar->address = ((bar2Content & 0xFFFFFFFF) << 32) | (barContent & 0xFFFFFFF0);
                    break;
            }

        }

        return true;
    }

}