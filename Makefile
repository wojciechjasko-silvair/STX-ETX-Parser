MK := mkdir
RM := rm -rf
MV := mv -f
GIT := git
7Zip := 7z

SOURCE_PATH         = "$(shell pwd)\source"
GENERATOR           ?= Eclipse CDT4 - Unix Makefiles
TOOLCHAIN_FILE_PATH ?= toolchains/generic.cmake

UNITTEST_PARAMS = 

.PHONY: default unittest clean

default: unittest

unittest:
	$(RM) build/unittest && $(MK) build/unittest
	cd build/unittest && cmake -G"$(GENERATOR)" -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE_PATH) $(SOURCE_PATH)

clean:
	$(foreach dir,$(wildcard build/*),     $(RM) $(dir))
	$(foreach dir,$(wildcard artifacts/*), $(RM) $(dir))
