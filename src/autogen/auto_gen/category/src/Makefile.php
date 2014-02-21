include $(C2C_BASE_PATH)/comm/makefile.comm

INCLUDE = -I./ -I$(C2C_BASE_PATH)/comm/export_include

LIB = -L$(C2C_BASE_PATH)/comm/export_lib -lwebframework_s -L/usr/local/ctemplate/lib -lctemplate_nothreads

VPATH=../lib

OUTPUT  = <? echo $category_lower; ?>.so

OBJS    = <? echo $category_lower; ?>_adaptor.o \
		  <? echo $category_lower; ?>.o 
		  
all:$(OUTPUT) 

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $^ 

.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^

$(OUTPUT): $(OBJS)
	$(CXX) $(CFLAGS) $(MY_INC) -shared  -o $(OUTPUT) $(OBJS) $(LIB)

clean:
	rm -f *.o *.~ *.bak
	rm -f $(OUTPUT)

install:
        cp -f $(OUTPUT) $(CGI_INSTALL_PATH)
