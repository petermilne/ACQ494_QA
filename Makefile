
CXXFLAGS += -std=c++11
LDLIBS += -lpopt
APPS := gpx2_simulate gpx2_decode gpx2_format_unit_test

all: $(APPS)

clean:
	rm $(APPS)
	
