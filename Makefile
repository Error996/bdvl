WARNING_FLAGS+=-Wall
OPTIMIZATION_FLAGS+=-g0 -O0
OPTION_FLAGS=-fomit-frame-pointer -fPIC
R_LFLAGS+=-lc -ldl -lcrypt -lpcap
PLATFORM+=$(shell uname -m)
SONAME+=bdvl.so
NEW_INC+=new_inc

all: setup kit

setup:
	rm -rf $(NEW_INC)
	python setup.py

kit: $(NEW_INC)/bedevil.c
	$(CC) -std=gnu99 $(WARNING_FLAGS) -I$(NEW_INC) $(OPTIMIZATION_FLAGS) -shared -Wl,-soname,$(SONAME).$(PLATFORM) -fPIC $(NEW_INC)/bedevil.c $(R_LFLAGS) -o $(SONAME).$(PLATFORM)
	-$(CC) -m32 -std=gnu99 $(WARNING_FLAGS) -I$(NEW_INC) $(OPTIMIZATION_FLAGS) -shared -Wl,-soname,$(SONAME).i686 -fPIC $(NEW_INC)/bedevil.c $(R_LFLAGS) -o $(SONAME).i686 2>/dev/null
	strip $(SONAME)*

clean:
	rm -rf $(SONAME)* $(NEW_INC)* *.b64
