#include <stdint.h>
#include <stdbool.h>

// Define necessary register addresses
// XIP
#define XIP_BASE                    (0x10000000)
// SSI
#define SSI_BASE                    (0x18000000)
#define SSI_CTRLR0                  (*(volatile uint32_t *) (SSI_BASE + 0x000))
#define SSI_SSIENR                  (*(volatile uint32_t *) (SSI_BASE + 0x008))
#define SSI_BAUDR                   (*(volatile uint32_t *) (SSI_BASE + 0x014))
#define SSI_SPI_CTRLR0              (*(volatile uint32_t *) (SSI_BASE + 0x0f4))
// M0PLUS
#define M0PLUS_BASE                 (0xe0000000)
#define M0PLUS_VTOR                 (*(volatile uint32_t *) (M0PLUS_BASE + 0xed08))

// GPIO access for APP switching
#define RESETS_RESET                *(volatile uint32_t *) (0x4000c000)
#define RESETS_RESET_DONE           *(volatile uint32_t *) (0x4000c008)

#define GPIO_10_STATUS              (*(volatile uint32_t *) (0x40014000 + 0x050))

// Boot stage 2 entry point
__attribute__((naked, noreturn, section(".boot2"))) void bootStage2(void)
{
    // 2. Setup SSI interface
    SSI_SSIENR = 0; // Disable SSI to configure it
    SSI_BAUDR = 4; // Set clock divider to 4
    SSI_CTRLR0 = (3 << 8) | (31 << 16); // Set EEPROM mode and 32 clocks per data frame
    SSI_SPI_CTRLR0 = (6 << 2) | (2 << 8) | (0x03 << 24); // Set address length to 24-bits, instruction length to 8-bits and command to Read Data (03h)
    SSI_SSIENR = 1; // Enable SSI

    // 3. Enable XIP Cache
    {
        RESETS_RESET &= ~(1 << 5); // Bring IO_BANK0 out of reset state
        while (!(RESETS_RESET_DONE & (1 << 5))); // Wait for peripheral to respond

        uint8_t gpioUP = ((GPIO_10_STATUS >> 17) & 0x01);
        bool useAppA = (gpioUP) ? true : false; 

        uint32_t APP_OFFSET = (useAppA) ? 0x100: 0x400;

        // Mimic non-rp2040 arm microcontroller behavior
        // 1. Set correct VTOR value
        M0PLUS_VTOR = XIP_BASE + APP_OFFSET; // Start of flash + boot stage 2 size
    }

    // 2. Load the stack pointer from the first entry in the vector table
    asm("msr msp, %0" :: "r"(*(uint32_t *)(M0PLUS_VTOR + 0x0)));

    // 3. Call the resetHandler using the second entry in the vector table
    asm("bx %0" :: "r"(*(uint32_t *)(M0PLUS_VTOR + 0x4)));

    // This area is unreachable
    while(true);
}