WARNING_FLAGS+=-Wall
OPTION_FLAGS+=-fPIC
R_LFLAGS+=-lc -ldl -lcrypt -lpcap
PLATFORM+=$(shell uname -m)
SONAME+=bdvl.so
NEW_INC+=new_inc

all: setup kit

setup:
	rm -rf $(NEW_INC)
	python setup.py

kit: $(NEW_INC)/bedevil.c
	$(CC) -std=gnu99 -g $(OPTION_FLAGS) $(WARNING_FLAGS) -I$(NEW_INC) -shared -Wl,--build-id=none $(NEW_INC)/bedevil.c $(R_LFLAGS) -o build/$(SONAME).$(PLATFORM)
	-$(CC) -m32 -std=gnu99 -g $(OPTION_FLAGS) $(WARNING_FLAGS) -I$(NEW_INC) -shared -Wl,--build-id=none $(NEW_INC)/bedevil.c $(R_LFLAGS) -o build/$(SONAME).i686 2>/dev/null
	strip build/$(SONAME)*

clean:
	rm -rf build/$(SONAME)* $(NEW_INC)*
