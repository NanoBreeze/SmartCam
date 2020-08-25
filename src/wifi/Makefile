
.PHONY: clean all
SRCS = main.cpp
CXXFLAGS+= -g
CXXFLAGS+= -I../../wpa_supplicant-2.9/src/common 
CXXFLAGS+= -I../../wpa_supplicant-2.9/src/utils
LDFLAGS+= -L${CURDIR} -lwpa_client

all: $(SRCS)
	$(CROSS_COMPILE)g++ $(SRCS) $(CXXFLAGS) $(LDFLAGS) -o wifi.o 

prep: $(SRCS)
	$(CROSS_COMPILE)g++ $(SRCS) $(CXXFLAGS) -DCONFIG_CTRL_IFACE -E  


clean:
	-rm *.o