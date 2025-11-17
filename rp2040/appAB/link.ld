ENTRY(bootStage2);

MEMORY
{
    flash(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    sram(rwx) : ORIGIN = 0x20000000, LENGTH = 256k
}

SECTIONS
{
    .boot2 :
    {
        _sboot2 = .;
        *(.boot2*)
        _eboot2 = .;
        . = . + (252 - (_eboot2 - _sboot2));
        *(.crc*)
    } > flash
    
    .app_A :
    {
        _sappA = .;
        *(.vector_A*)
        *(.app_A*)
        _eappA = .;
        . = . + (768 - (_eappA - _sappA));
    } > flash

    .app_B :
    {
        _sappB = .;
        *(.vector_B*)
        *(.app_B*)
        _eappB = .;
        . = . + (768 - (_eappB - _sappB));
    } > flash

    .data :
    {
        *(.data*)
    } > sram AT > flash     /* "> sram" is the VMA, "> flash" is the LMA */

    .bss (NOLOAD) :
    {
        *(.bss*)
    } > sram

    .stack (NOLOAD) :
    {
        . = ORIGIN(sram) + LENGTH(sram);
        __stack = .;
    } > sram

    /* Get LMA and VMA for .data section */
    _sdata = ADDR(.data);               /* Get starting LMA */
    _edata = _sdata + SIZEOF(.data);    /* Get ending LMA */
    _sdataf = LOADADDR(.data);          /* Get starting VMA */

    /* Get start and end of .bss section */
    __bss_start__ = ADDR(.bss);                 /* Get starting LMA */
    __bss_end__ = __bss_start__ + SIZEOF(.bss);       /* Get ending LMA */
}