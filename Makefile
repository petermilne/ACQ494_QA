
CXXFLAGS += -std=c++11
ifneq ($(CROSS_COMPILE),)
	LDLIBS += -L../ACQ400/lib
endif
LDLIBS += -lpopt
APPS := gpx2_simulate gpx2_decode gpx2_format_unit_test

all: $(APPS)

clean:
	rm -f $(APPS)
	
