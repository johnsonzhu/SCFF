INCLUDE = -I../src \
      -I$(WEBFRAMEWORK_INC_EXPORT_PATH) \
	  -I$(FCGI_INC_EXPORT_PATH) \
	  -I$(BOOST_INC_EXPORT_PATH) \
	  -I$(CTEMPLATE_INC_EXPORT_PATH) \

LIB = -L$(FCGI_LIB_EXPORT_PATH) -lfastcgipp \
	  -L$(BOOST_LIB_EXPORT_PATH) -lboost_thread -pthread -lboost_system -lboost_date_time \
	  -L$(WEBFRAMEWORK_LIB_EXPORT_PATH) -lwebframework \
	  -L$(CTEMPLATE_LIB_EXPORT_PATH) -lctemplate_nothreads -L/usr/lib64/mysql/ -lmysqlclient\
          -L/lib64/ -lrt
	  
LD_LIBRARY = -Wl,-R$(FCGI_LIB_EXPORT_PATH) \
		  -Wl,-R$(BOOST_LIB_EXPORT_PATH) \
		  -Wl,-R$(CTEMPLATE_LIB_EXPORT_PATH) \

VPATH=../lib:../src	

OUTPUT  = <? echo $category_lower; ?>.fcgi

SOURCES = <? echo $category_lower; ?>_adaptor.cpp \
		  <? echo $category_lower; ?>.cpp \

OBJECTS=$(SOURCES:.cpp=.o)
		  
all:$(OUTPUT) 

CFLAGS=-static

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $(filter %.cpp, $^) 

.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CFLAGS)  -o $(OUTPUT) $^ $(LIB) $(LD_LIBRARY)


clean:
	rm -f *.o *.~ *.bak *.fcgi
	rm -f $(OUTPUT)

dep:
	mkdep -p$(VPATH) $(INCLUDE) $(SOURCES) > .depend

ifneq ($(wildcard .depend),)
include .depend
endif
