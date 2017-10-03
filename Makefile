CXX             = g++
CXXFLAGS        = -g -std=gnu++11 -Wall -Iinclude #-DNDEBUG=1
LD              = g++
LDFLAGS         = -Llib
LNKFLAGS        = -lpthread

# These are macros for linking to already compiled gtest libraries.
GTEST_INC_DIR   = contrib/gtest/googletest/include
GTEST_LNK_PATH  = build/googlemock/gtest
GTEST_LNK_FLAGS = -lgtest -lgtest_main
GTEST		= build/googlemock/gtest

# These are macros to build the client static library.
CLIENT_SOURCE   = $(wildcard src/client/*.cpp)
CLIENT_OBJECTS  = $(CLIENT_SOURCE:.cpp=.o)
CLIENT          = lib/libps_client.a

# These are macros to build the functional test. This test is just the ability
# to send a message to the server, and for it to "echo" it back to the client.
ECHO_LNK_FLAGS  = $(LNKFLAGS) -lps_client
ECHO_SOURCE     = $(wildcard src/tests/*.cpp)
ECHO_OBJECTS    = $(ECHO_SOURCE:.cpp=.o)
ECHO            = bin/echo_test

# These are macros for making unit tests.
UNIT_FLAGS      = -I$(GTEST_INC_DIR)
UNIT_LNK_FLAGS  = -L$(GTEST_LNK_PATH) $(GTEST_LNK_FLAGS) $(LNKFLAGS) -lps_client
UNIT_SOURCE     = $(wildcard src/unit/*.cpp)
UNIT_HEADERS    = $(wildcard src/unit/*.h)
UNIT_OBJECTS    = $(UNIT_SOURCE:.cpp=.o)
UNIT            = bin/client_unit

TARGETS        	= $(CLIENT) $(ECHO) $(GTEST) $(UNIT)

all:            $(TARGETS)

$(CLIENT):      $(CLIENT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(UNIT):    	$(UNIT_OBJECTS)
	$(LD) $(UNIT_FLAGS) -o $@ $^ $(LDFLAGS) $(UNIT_LNK_FLAGS)

$(ECHO):        $(ECHO_OBJECTS)
	$(LD) $(ECHO_FLAGS) -o $@ $^ $(LDFLAGS) $(ECHO_LNK_FLAGS)

$(GTEST):	contrib/gtest
	(mkdir build; cd build; cmake ../contrib/gtest; make)

%.o:            %.cpp   $(wildcard include/*/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -fr build $(TARGETS) $(CLIENT_OBJECTS) $(ECHO_OBJECTS) $(UNIT_OBJECTS)

.PHONY: all clean
