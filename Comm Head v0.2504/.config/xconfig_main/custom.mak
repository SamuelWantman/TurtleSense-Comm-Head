## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e430X linker.cmd package/cfg/main_pe430X.oe430X

linker.cmd: package/cfg/main_pe430X.xdl
	$(SED) 's"^\"\(package/cfg/main_pe430Xcfg.cmd\)\"$""\"C:/Users/sam_000/Dropbox/Turtle Sense/TI workspace/Comm Head v0.2502/.config/xconfig_main/\1\""' package/cfg/main_pe430X.xdl > $@
