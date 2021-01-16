#include "drivers/svga/svga.h"

#include "modules/lib.h"
#include "modules/memory.h"
#include "modules/paging.h"

uint32_t SVGA::ReadRegister(uint32_t index) {
    Port32::Write(svga.ioBase + SVGA_INDEX_PORT, index);
    return Port32::Read(svga.ioBase + SVGA_VALUE_PORT);
}

void SVGA::WriteRegister(uint32_t index, uint32_t value) {
    Port32::Write(svga.ioBase + SVGA_INDEX_PORT, index);
    Port32::Write(svga.ioBase + SVGA_VALUE_PORT, value);
}

uint32_t SVGA::ClearIRQ() {
    uint32_t flags = 0;
    Atomic_Exchange(svga.irq.pending, flags);
    return flags;
}

uint32_t SVGA::WaitForIRQ() {

    uint32_t flags;

    /*
     * Store this CPU context, and tell the IRQ handler to return
     * here if an IRQ occurs. We use IntrContext here just like a
     * setjmp/longjmp that works in ISRs.
     *
     * This must be above the irq.pending check, so that we re-test
     * irq.pending if an IRQ occurs after testing irq.pending but
     * before we halt. Without this, we could deadlock if an IRQ
     * comes in before we actually halt but after we've decided to
     * halt.
     */
    Interrupts::SaveContext(&svga.irq.newContext);

    do {
        
        flags = 0;

        /*
        * switchContext must be set to TRUE before we halt, or we'll deadlock.
        * This variable is marked as volatile, plus we use a compiler memory barrier.
        */
        svga.irq.switchContext = true;
        asm volatile ("" ::: "memory");

        Atomic_Exchange(svga.irq.pending, flags);

        if (!flags)
            Interrupts::Halt();

        /*
         * Must set switchContext to FALSE before this stack frame is deallocated.
         */
        svga.irq.switchContext = false;
        asm volatile ("" ::: "memory");

    } while (!flags);

    return flags;

}

void SVGA::RingDoorbell() {

    // If the device isn't already busy processing any commands, wake it
    if (FIFO_IsRegisterValid(SVGA_FIFO_BUSY) && !svga.fifoMemory[SVGA_FIFO_BUSY]) {

      /* Remember that we already rang the doorbell. */
      svga.fifoMemory[SVGA_FIFO_BUSY] = true;

      /*
       * Asynchronously wake up the SVGA3D device.  The second
       * parameter is an arbitrary nonzero 'sync reason' which can be
       * used for debugging, but which isn't part of the SVGA3D
       * protocol proper and which isn't used by release builds of
       * VMware products.
       */
      WriteRegister(SVGA_REG_SYNC, 1);

   }

}

void SVGA::FIFO_Full() {

    if (FIFO_IsRegisterValid(SVGA_FIFO_FENCE_GOAL) && (svga.capabilities & SVGA_CAP_IRQMASK)) {

      /*
       * On hosts which support interrupts, we can sleep until the
       * FIFO_PROGRESS interrupt occurs. This is the most efficient
       * thing to do when the FIFO fills up.
       *
       * As with the IRQ-based SVGA_SyncToFence(), this will only work
       * on Workstation 6.5 virtual machines and later.
       */

      WriteRegister(SVGA_REG_IRQMASK, SVGA_IRQFLAG_FIFO_PROGRESS);
      ClearIRQ();
      RingDoorbell();
      WaitForIRQ();
      WriteRegister(SVGA_REG_IRQMASK, 0);

   } else {

      /*
       * Fallback implementation: Perform one iteration of the
       * legacy-style sync. This synchronously processes FIFO commands
       * for an arbitrary amount of time, then returns control back to
       * the guest CPU.
       */

      WriteRegister(SVGA_REG_SYNC, 1);
      ReadRegister(SVGA_REG_BUSY);

   }

}

void * SVGA::FIFO_Reserve(uint32_t bytes) {
    
    uint32_t * fifo = svga.fifoMemory;
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    uint32_t nextCmd = fifo[SVGA_FIFO_NEXT_CMD];
    bool reserveable = FIFO_HasCapability(SVGA_FIFO_CAP_RESERVE);

    /*
     * This example implementation uses only a statically allocated
     * buffer.  If you want to support arbitrarily large commands,
     * dynamically allocate a buffer if and only if it's necessary.
     */

    if (bytes > sizeof svga.fifo.bounceBuffer || bytes > (max - min)) {
        // FIFO command too large
        return nullptr;
    }

    if (bytes % sizeof(uint32_t)) {
        // FIFO command length not 32-bit aligned
        return nullptr;
    }

    if (svga.fifo.reservedSize != 0) {
        // FIFOReserve before FIFOCommit
        return nullptr;
    }

    svga.fifo.reservedSize = bytes;

    while (1) {
        uint32_t stop = fifo[SVGA_FIFO_STOP];
        bool reserveInPlace = false;
        bool needBounce = false;

        /*
        * Find a strategy for dealing with "bytes" of data:
        * - reserve in place, if there's room and the FIFO supports it
        * - reserve in bounce buffer, if there's room in FIFO but not
        *   contiguous or FIFO can't safely handle reservations
        * - otherwise, sync the FIFO and try again.
        */

        if (nextCmd >= stop) {
            /* There is no valid FIFO data between nextCmd and max */

            if (nextCmd + bytes < max ||
                (nextCmd + bytes == max && stop > min)) {
                /*
                * Fastest path 1: There is already enough contiguous space
                * between nextCmd and max (the end of the buffer).
                *
                * Note the edge case: If the "<" path succeeds, we can
                * quickly return without performing any other tests. If
                * we end up on the "==" path, we're writing exactly up to
                * the top of the FIFO and we still need to make sure that
                * there is at least one unused DWORD at the bottom, in
                * order to be sure we don't fill the FIFO entirely.
                *
                * If the "==" test succeeds, but stop <= min (the FIFO
                * would be completely full if we were to reserve this
                * much space) we'll end up hitting the FIFOFull path below.
                */
                reserveInPlace = true;
            } else if ((max - nextCmd) + (stop - min) <= bytes) {
                /*
                * We have to split the FIFO command into two pieces,
                * but there still isn't enough total free space in
                * the FIFO to store it.
                *
                * Note the "<=". We need to keep at least one DWORD
                * of the FIFO free at all times, or we won't be able
                * to tell the difference between full and empty.
                */
                FIFO_Full();
            } else {
                /*
                * Data fits in FIFO but only if we split it.
                * Need to bounce to guarantee contiguous buffer.
                */
                needBounce = true;
            }

        } else {
            /* There is FIFO data between nextCmd and max */

            if (nextCmd + bytes < stop) {
                /*
                * Fastest path 2: There is already enough contiguous space
                * between nextCmd and stop.
                */
                reserveInPlace = true;
            } else {
                /*
                * There isn't enough room between nextCmd and stop.
                * The FIFO is too full to accept this command.
                */
                FIFO_Full();
            }
        }

        /*
        * If we decided we can write directly to the FIFO, make sure
        * the VMX can safely support this.
        */
        if (reserveInPlace) {
            if (reserveable || bytes <= sizeof(uint32_t)) {
                svga.fifo.usingBounceBuffer = false;
                if (reserveable) {
                    fifo[SVGA_FIFO_RESERVED] = bytes;
                }
                return reinterpret_cast<uint8_t*>(fifo) + nextCmd;
            } else {
                /*
                * Need to bounce because we can't trust the VMX to safely
                * handle uncommitted data in FIFO.
                */
                needBounce = true;
            }
        }

        /*
        * If we reach here, either we found a full FIFO, called
        * SVGAFIFOFull to make more room, and want to try again, or we
        * decided to use a bounce buffer instead.
        */
        if (needBounce) {
            svga.fifo.usingBounceBuffer = true;
            return svga.fifo.bounceBuffer;
        }
    } /* while (1) */
}

void * SVGA::FIFO_Reserve(uint32_t type, uint32_t bytes) {
    uint32_t * cmd = reinterpret_cast<uint32_t*>(FIFO_Reserve(bytes + sizeof(type)));
    cmd[0] = type;
    return cmd + 1;
}

void SVGA::FIFO_Commit(uint32_t bytes) {

    uint32_t * fifo = svga.fifoMemory;
    uint32_t nextCmd = fifo[SVGA_FIFO_NEXT_CMD];
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    bool reserveable = FIFO_HasCapability(SVGA_FIFO_CAP_RESERVE);

    if (svga.fifo.reservedSize == 0) {
        // FIFOCommit before FIFOReserve
        return;
    }

    svga.fifo.reservedSize = 0;

    if (svga.fifo.usingBounceBuffer) {

        /*
        * Slow paths: copy out of a bounce buffer.
        */
        uint8_t * buffer = svga.fifo.bounceBuffer;

        if (reserveable) {

            /*
            * Slow path: bulk copy out of a bounce buffer in two chunks.
            *
            * Note that the second chunk may be zero-length if the reserved
            * size was large enough to wrap around but the commit size was
            * small enough that everything fit contiguously into the FIFO.
            *
            * Note also that we didn't need to tell the FIFO about the
            * reservation in the bounce buffer, but we do need to tell it
            * about the data we're bouncing from there into the FIFO.
            */
            uint32_t chunkSize = MIN(bytes, max - nextCmd);
            fifo[SVGA_FIFO_RESERVED] = bytes;
            memcpy(reinterpret_cast<uint8_t*>(fifo) + nextCmd, buffer, chunkSize);
            memcpy(reinterpret_cast<uint8_t*>(fifo) + min, buffer + chunkSize, bytes - chunkSize);

        } else {
            
            /*
            * Slowest path: copy one dword at a time, updating NEXT_CMD as
            * we go, so that we bound how much data the guest has written
            * and the host doesn't know to checkpoint.
            */
            uint32_t * dword = reinterpret_cast<uint32_t*>(buffer);

            while (bytes > 0) {
                fifo[nextCmd / sizeof(*dword)] = *dword++;
                nextCmd += sizeof(*dword);
                if (nextCmd == max) {
                    nextCmd = min;
                }
                fifo[SVGA_FIFO_NEXT_CMD] = nextCmd;
                bytes -= sizeof(*dword);
            }
        }
    }

    /*
     * Atomically update NEXT_CMD, if we didn't already
     */
    if (!svga.fifo.usingBounceBuffer || reserveable) {
        nextCmd += bytes;
        if (nextCmd >= max) {
            nextCmd -= max - min;
        }
        fifo[SVGA_FIFO_NEXT_CMD] = nextCmd;
    }

    /*
     * Clear the reservation in the FIFO.
     */
    if (reserveable) {
        fifo[SVGA_FIFO_RESERVED] = 0;
    }
}

void SVGA::FIFO_CommitAll() {
    FIFO_Commit(svga.fifo.reservedSize);
}

uint32_t SVGA::InsertFence() {
    if (!FIFO_HasCapability(SVGA_FIFO_CAP_FENCE))
        return 1;

    if (svga.fifo.nextFence == 0)
        svga.fifo.nextFence = 1;

    SVGAFifoCmdFence * cmd;
    uint32_t fence = svga.fifo.nextFence++;

    cmd = reinterpret_cast<SVGAFifoCmdFence*>(FIFO_Reserve(sizeof(SVGAFifoCmdFence)));
    cmd->id = SVGA_CMD_FENCE;
    cmd->fence = fence;
    FIFO_CommitAll();
    
    return fence;
}

void SVGA::HandleInterrupt(uint8_t vector) {

    register long rsp asm ("rsp");
    InterruptContext * context = reinterpret_cast<InterruptContext*>(rsp);

    /*
     * The SVGA_IRQSTATUS_PORT is a separate I/O port, not a register.
     * Reading from it gives us the set of IRQ flags which are
     * set. Writing a '1' to any bit in this register will clear the
     * corresponding flag.
     *
     * Here, we read then clear all flags.
     */
    uint16_t port = svga.ioBase + SVGA_IRQSTATUS_PORT;
    uint32_t irqFlags = Port32::Read(port);
    Port32::Write(port, irqFlags);

    svga.irq.count++;

    Atomic_Or(svga.irq.pending, irqFlags);

    if (svga.irq.switchContext) {
        memcpy(&svga.irq.oldContext, context, sizeof(*context));
        memcpy(context, &svga.irq.newContext, sizeof(*context));
        svga.irq.switchContext = false;
    }
}

uint32_t SVGA::Init() {

    // Find the device on the PCI bus
    if (!PCI::FindDevice(PCI_VENDOR_ID_VMWARE, PCI_DEVICE_ID_VMWARE_SVGA2, &svga))
        return ERR_SVGA_DEVICE_NOT_FOUND;

    // Enable it
    PCI::SetDeviceEnabled(&svga.addr, true);

    PCIBaseAddressRegister bar;
    PCI::GetDeviceBAR(&svga.addr, 0, &bar);
    svga.ioBase = bar.address & UINT32_MAX;

    PCI::GetDeviceBAR(&svga.addr, 1, &bar);
    svga.framebufferMemory = reinterpret_cast<uint8_t*>(bar.address);

    PCI::GetDeviceBAR(&svga.addr, 2, &bar);
    svga.fifoMemory = reinterpret_cast<uint32_t*>(bar.address);

    // Version negotiation
    svga.deviceVersionID = SVGA_ID_2;
    do {
        
        // Write version
        WriteRegister(SVGA_REG_ID, svga.deviceVersionID);

        // Version matches, break
        if (ReadRegister(SVGA_REG_ID) == svga.deviceVersionID)
            break;

        // Version too high, decrement
        else
            svga.deviceVersionID--;

    } while (svga.deviceVersionID >= SVGA_ID_0);

    // Get VRAM, Framebuffer, and FIFO sizes
    svga.vramSize = ReadRegister(SVGA_REG_VRAM_SIZE);
    svga.framebufferSize = ReadRegister(SVGA_REG_FB_SIZE);
    svga.fifoSize = ReadRegister(SVGA_REG_MEM_SIZE);
    
    // Get capabilities if device supports it
    if (svga.deviceVersionID >= SVGA_ID_1)
        svga.capabilities = ReadRegister(SVGA_REG_CAPABILITIES);

    // Interrupt setup
    //if (svga.capabilities & SVGA_CAP_IRQMASK) {

        /* Start out with all SVGA IRQs masked */
        WriteRegister(SVGA_REG_IRQMASK, 0);

        /* Clear all pending IRQs stored by the device */
        Port32::Write(svga.ioBase + SVGA_IRQSTATUS_PORT, 0xFF);

        /* Clear all pending IRQs stored by us */
        ClearIRQ();

        /* Enable the IRQ */
        InterruptManager::RequestIRQ(svga.interruptLine, this);
        InterruptManager::SetMask(svga.interruptLine, true);

    //}

    Enable();

    return SVGA_OK;

}

void SVGA::Enable() {
    
    /*
    * Initialize the command FIFO. The beginning of FIFO memory is
    * used for an additional set of registers, the "FIFO registers".
    * These are higher-performance memory mapped registers which
    * happen to live in the same space as the FIFO. The driver is
    * responsible for allocating space for these registers, according
    * to the maximum number of registers supported by this driver
    * release.
    */
    Paging::Map(reinterpret_cast<uint64_t>(svga.fifoMemory), SVGA_FIFO_START, (svga.fifoSize + PAGE_SIZE - 1) / PAGE_SIZE);
    svga.fifoMemory = reinterpret_cast<uint32_t*>(SVGA_FIFO_START);
    svga.fifoMemory[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(uint32_t);
    svga.fifoMemory[SVGA_FIFO_MAX] = svga.fifoSize;
    svga.fifoMemory[SVGA_FIFO_NEXT_CMD] = svga.fifoMemory[SVGA_FIFO_MIN];
    svga.fifoMemory[SVGA_FIFO_STOP] = svga.fifoMemory[SVGA_FIFO_MIN];

    /*
    * Prep work for 3D version negotiation. See SVGA3D_Init for
    * details, but we have to give the host our 3D protocol version
    * before enabling the FIFO.
    */
    if (FIFO_HasCapability(SVGA_CAP_EXTENDED_FIFO) && FIFO_IsRegisterValid(SVGA_FIFO_GUEST_3D_HWVERSION))
        svga.fifoMemory[SVGA_FIFO_GUEST_3D_HWVERSION] = SVGA3D_HWVERSION_CURRENT;

    /*
    * Enable the SVGA device and FIFO.
    */
    WriteRegister(SVGA_REG_ENABLE, true);
    WriteRegister(SVGA_REG_CONFIG_DONE, true);

    if (svga.capabilities & SVGA_CAP_IRQMASK) {

        WriteRegister(SVGA_REG_IRQMASK, SVGA_IRQFLAG_ANY_FENCE);
        ClearIRQ();

        InsertFence();
        
        WriteRegister(SVGA_REG_SYNC, 1);
        while (ReadRegister(SVGA_REG_BUSY) != false);

        WriteRegister(SVGA_REG_IRQMASK, 0);
        WaitForIRQ();

    }

}

void SVGA::Disable() {
    WriteRegister(SVGA_REG_ENABLE, false);
}

void SVGA::SetMode(uint32_t width, uint32_t height, uint32_t bpp) {
    svga.width = width;
    svga.height = height;
    svga.bpp = bpp;

    WriteRegister(SVGA_REG_WIDTH, width);
    WriteRegister(SVGA_REG_HEIGHT, height);
    WriteRegister(SVGA_REG_BITS_PER_PIXEL, bpp);
    WriteRegister(SVGA_REG_ENABLE, true);
    svga.pitch = ReadRegister(SVGA_REG_BYTES_PER_LINE);
}