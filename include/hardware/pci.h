#pragma once

#include "common/types.h"
#include "modules/drivers.h"
#include "hardware/ports.h"

#define PCI_NUM_BUSES       256
#define PCI_NUM_DEVICES     32
#define PCI_NUM_FUNCTIONS   8

#define PCI_CLASS_UNCLASSIFIED                  0x00
#define PCI_CLASS_MASS_STORAGE                  0x01
#define PCI_CLASS_NETWORK                       0x02
#define PCI_CLASS_DISPLAY                       0x03
#define PCI_CLASS_MULTIMEDIA                    0x04
#define PCI_CLASS_MEMORY                        0x05
#define PCI_CLASS_BRIDGE                        0x06
#define PCI_CLASS_COMMUNICATION                 0x07
#define PCI_CLASS_GENERIC_PERIPHERAL            0x08
#define PCI_CLASS_INPUT_DEVICE                  0x09
#define PCI_CLASS_DOCKING_STATION               0x0a
#define PCI_CLASS_PROCESSOR                     0x0b
#define PCI_CLASS_SERIAL_BUS                    0x0c
#define PCI_CLASS_WIRELESS                      0x0d
#define PCI_CLASS_INTELLIGENT                   0x0e
#define PCI_CLASS_SATELLITE_COMMUNICATIONS      0x0f
#define PCI_CLASS_ENCRYPTION                    0x10
#define PCI_CLASS_SIGNAL_PROCESSING             0x11
#define PCI_CLASS_PROCESSING_ACCELERATORS       0x12
#define PCI_CLASS_NON_ESSENTIAL                 0x13
#define PCI_CLASS_COPROCESSOR                   0x40

#define PCI_DISPLAY_VGA         0x00
#define PCI_DISPLAY_XGA         0x01
#define PCI_DISPLAY_3D          0x02
#define PCI_DISPLAY_DISPLAY     0x80

#define PCI_BAR_MEMORY_SPACE    0
#define PCI_BAR_IO_SPACE        1

#define PCI_BAR_MEMORY_TYPE_32  0x0
#define PCI_BAR_MEMORY_TYPE_16  0x1
#define PCI_BAR_MEMORY_TYPE_64  0x2

#define PCI_DATA_PORT           0xCFC
#define PCI_COMMAND_PORT        0xCF8

#define PCI_VENDOR_ID_NONE      0xFFFF

namespace PCI {

    struct PCIAddress {
        uint8_t bus, device, function;
        inline uint32_t Pack(uint16_t offset) const {
            return (1 << 31) |       // Enable bit
            ((bus & 0xFF) << 16) |      // Bus number (8 bit)
            ((device & 0x1F) << 11) |   // Device number (5 bit)
            ((function & 0x07) << 8) |  // Function number (3 bit)
            (offset & 0xFF);            // Register offset (8 bit)
        }
    };

    struct PCIDevice {
        PCIAddress addr;
        uint16_t vendorID;
        uint16_t deviceID;
        uint8_t revisionID;
        uint8_t interface;
        uint8_t subclassID;
        uint8_t classID;
        uint8_t headerType;
        bool multifunction;
        uint8_t interruptLine;
    };

    struct PCIBaseAddressRegister {
        uint64_t address;
        bool prefetchable;
        uint8_t type;
        uint8_t space;
    };

    // Read/Write configuration registers
    uint32_t ReadConfig32(const PCIAddress * addr, uint8_t offset);
    uint16_t ReadConfig16(const PCIAddress * addr, uint8_t offset);
    uint8_t ReadConfig8(const PCIAddress * addr, uint8_t offset);
    void WriteConfig32(const PCIAddress * addr, uint8_t offset, uint32_t value);
    void WriteConfig16(const PCIAddress * addr, uint8_t offset, uint16_t value);
    void WriteConfig8(const PCIAddress * addr, uint8_t offset, uint8_t value);

    // Scan the bus for any devices present.
    // Returns the number of devices found.
    uint32_t ScanBus();

    // Returns true if a device matching the supplied vendorID and deviceID is found on the bus.
    // The device configuration is stored in the struct pointed to by device
    bool FindDevice(uint16_t vendorID, uint16_t deviceID, PCIDevice * device);

    // Allows/Prevents a device to respond to I/O or memory space accesses
    // Also enables/disables bus mastering used for DMA devices
    void SetDeviceEnabled(const PCIAddress * addr, bool enable);

    // Returns true if the requested BAR exists, false otherwise.
    // The BAR information is stored in the struct pointed to by bar.
    bool GetDeviceBAR(const PCIAddress * addr, uint8_t index, PCIBaseAddressRegister * bar);

    

};
