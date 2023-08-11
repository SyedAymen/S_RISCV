OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS {
  . = 0x80000000;

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trapHandlerStart = .;
    *(trapHandler)
    . = ALIGN(0x1000);
    ASSERT(. - _trapHandlerStart == 0x1000, "error: trapHandler larger than one page");
    PROVIDE(endText = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*)
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*)
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*)
    . = ALIGN(16);
    *(.bss .bss.*)
  }
  PROVIDE(kernelEnd = .);
}
