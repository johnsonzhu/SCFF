include $(C2C_BASE_PATH)/comm/makefile.comm.v2
all: subdirs

SUBDIRS = \
	websvc \
	cgi 

subdirs:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir clean; \
	done


