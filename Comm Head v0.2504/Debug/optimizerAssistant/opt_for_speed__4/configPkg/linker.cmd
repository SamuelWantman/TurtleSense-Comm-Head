
/*
 * Do not modify this file; it is automatically generated from the template
 * linkcmd.xdt in the ti.platforms.msp430 package and will be overwritten.
 */

/*
 * put '"'s around paths because, without this, the linker
 * considers '-' as minus operator, not a file name character.
 */


-l"C:\Users\sam_000\Dropbox\Turtle Sense\TI workspace\Comm Head v0.2502\Debug__opt_for_speed__4\configPkg\package\cfg\main_pe430X.oe430X"
-l"C:\ti\grace_2_20_02_32\packages\ti\targets\msp430\rts430\lib\ti.targets.msp430.rts430.ae430X"
-l"C:\ti\grace_2_20_02_32\packages\ti\catalog\msp430\init\lib\ti.catalog.msp430.init.ae430X"

--retain="*(xdc.meta)"
--retain="*(xdc.noload)"


/* Elf symbols */
--symbol_map __TI_STACK_BASE=_stack
--symbol_map __TI_STACK_SIZE=_STACK_SIZE
--symbol_map __TI_STATIC_BASE=__bss__
--symbol_map __c_int00=_c_int00
--symbol_map __TI_cleanup_ptr=_cleanup_ptr
--symbol_map __TI_dtors_ptr=_dtors_ptr


--args 0x0
-heap  0x0
-stack 0x40

/*
 * Linker command file contributions from all loaded packages:
 */

/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.msp430.peripherals.interrupt (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from ti.catalog.msp430.init (null): */

/* Content from ti.catalog.msp430.peripherals.special_function (null): */

/* Content from ti.catalog.msp430.peripherals.clock (null): */

/* Content from ti.catalog.msp430.peripherals.watchdog (null): */

/* Content from ti.catalog.msp430.peripherals.pmm (null): */

/* Content from ti.catalog.msp430.peripherals.ref (null): */

/* Content from ti.catalog.msp430.peripherals.comparator (null): */

/* Content from ti.catalog.msp430.peripherals.communication (null): */

/* Content from ti.catalog.msp430.peripherals.timer (null): */

/* Content from ti.catalog.msp430.peripherals.adc (null): */

/* Content from ti.catalog.msp430.peripherals.rtc (null): */

/* Content from ti.catalog.msp430.peripherals.frctl (null): */

/* Content from ti.catalog.msp430.peripherals.dma (null): */

/* Content from ti.catalog.msp430.peripherals.svs (null): */

/* Content from ti.catalog.msp430.peripherals.dac (null): */

/* Content from ti.catalog.msp430.peripherals.gpio (null): */

/* Content from ti.catalog.msp430 (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from ti.targets (null): */

/* Content from ti.targets.msp430.elf (null): */

/* Content from ti.platforms.msp430 (null): */

/* Content from xdc.services.getset (null): */

/* Content from ti.targets.msp430.rts430 (null): */

/* Content from ti.mcu.msp430.csl.interrupt_vectors (null): */

/* Content from ti.mcu.msp430.csl.gpio (null): */

/* Content from ti.mcu.msp430.csl2.system (null): */

/* Content from ti.mcu.msp430.csl (ti/mcu/msp430/csl/linker_cmds.xdt): */
/* link with the library generated by MSP430 CSL (C:/ti/grace_2_20_02_32/packages/ti/mcu/msp430/csl/) */
-l"C:\Users\sam_000\Dropbox\TURTLE~1\TIWORK~1\COMMHE~1.250\src\grace/grace.lib"

/* Content from ti.mcu.msp430.csl2.adc (null): */

/* Content from ti.mcu.msp430.csl2.timer (null): */

/* Content from ti.mcu.msp430.csl2.ref (null): */

/* Content from ti.mcu.msp430.csl2.communication (null): */

/* Content from ti.mcu.msp430.csl.system (null): */

/* Content from ti.mcu.msp430.csl2.clock (null): */

/* Content from ti.mcu.msp430.csl2.watchdog (null): */

/* Content from xdc.services.io (null): */

/* Content from configPkg (null): */

/* Content from ti.mcu.msp430.csl2 (null): */



/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__RESETFXN__C = 1;
xdc_runtime_Startup__EXECFXN__C = 0;
xdc_runtime_Startup_exec__E = 0;


SECTIONS
{



    xdc.meta: type = COPY
}

