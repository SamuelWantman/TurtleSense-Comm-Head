#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/grace_2_20_02_32/packages;C:/ti/msp430/MSP430ware_1_60_02_09/driverlib/packages;C:/ti/ccsv5/ccs_base;C:/Users/sam_000/Dropbox/TURTLE~1/TIWORK~1/COMMHE~1.250/.config
override XDCROOT = C:/ti/xdctools_3_25_04_88
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/grace_2_20_02_32/packages;C:/ti/msp430/MSP430ware_1_60_02_09/driverlib/packages;C:/ti/ccsv5/ccs_base;C:/Users/sam_000/Dropbox/TURTLE~1/TIWORK~1/COMMHE~1.250/.config;C:/ti/xdctools_3_25_04_88/packages;..
HOSTOS = Windows
endif
