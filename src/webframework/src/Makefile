INSTALL_PATH = ../lib/

INCLUDE = -I../src \
	  -I../include \
	  -I$(FCGI_INC_EXPORT_PATH) \
	  -I$(BOOST_INC_EXPORT_PATH) \
	  -I$(CTEMPLATE_INC_EXPORT_PATH) \
	  
LIB = -L$(FCGI_LIB_EXPORT_PATH) -lfastcgipp \
	  -L$(BOOST_LIB_EXPORT_PATH) -lboost_thread -pthread -lboost_system -lboost_date_time -lmysqlclient \
	  -L$(CTEMPLATE_LIB_EXPORT_PATH) -lctemplate_nothreads \

LD_LIBRARY = -Wl,-R$(FCGI_LIB_EXPORT_PATH) \
		  -Wl,-R$(BOOST_LIB_EXPORT_PATH) \
		  -Wl,-R$(CTEMPLATE_LIB_EXPORT_PATH) \
		  
VPATH=../include	
SOURCES = $(wildcard *.cpp)  
OBJS = $(SOURCES:.cpp=.o)

OUTPUT_LIB = libwebframework.a 

.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $(filter %.cpp, $^)

.c.o:
	$(GCC) $(CFLAGS) $(INCLUDE) -c $(filter %.c, $^)

.o:
	$(CXX) $(CFLAGS) -o $@ $^        

all: $(OUTPUT_LIB)

$(OUTPUT_LIB): $(OBJS) 
	ar -rs $@ $^ 
	cp -f $(OUTPUT_LIB) $(INSTALL_PATH)

clean:
	rm -f *.o *.~ *.bak *.a *.so

install:
	mkdir -p $(WEBFRAMEWORK_LIB_EXPORT_PATH)
	mkdir -p $(WEBFRAMEWORK_INC_EXPORT_PATH)
	cp -f ../lib/* $(WEBFRAMEWORK_LIB_EXPORT_PATH)
	cp -f ../include/* $(WEBFRAMEWORK_INC_EXPORT_PATH)

dep:
	mkdep $(INCLUDE) $(SOURCES) > .depend

ifneq ($(wildcard .depend),)
include .depend
endif

