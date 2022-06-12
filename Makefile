
LDLIBS += -lpopt
APPS := gpx2_simulate gpx2_decode

all: $(APPS)

clean: $(APPS)
	rm $(APPS)
	
