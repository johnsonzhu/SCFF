include $(C2C_BASE_PATH)/comm/makefile.comm.v2

INCLUDE = -I../include \
		  -I$(C2C_BASE_PATH)/comm/export_include \
		  -I$(C2C_BASE_PATH)/comm/export_include/libc2c 

LIB =
OUTPUT  = lib<? echo $category_lower; ?>_adaptor.a
OBJS = <? echo $category_lower; ?>_adaptor.o

all:$(OUTPUT)

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $^ 

.o:
	$(CXX) $(CPPFLAGS) $(INCLUDE) -o $@ $^

$(OUTPUT):$(OBJS)
	ar -rs $@ $^

clean:
	rm -f *.o *.~ *.bak
	rm -f $(OUTPUT)

dep:
	mkdep $(INCLUDE) $(SOURCES) > .depend

ifneq ($(wildcard .depend),)
include .depend
endif

