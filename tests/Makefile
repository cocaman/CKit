#
# Makefile for CKit tests
#
# We need to have a few system-level dependencies to make things cleanly
# compile on a few different systems.
#
ifeq ($(shell uname),SunOS)
CXX = g++ -g
CXX_DEFS =
LIB_EXT = so
ifeq ($(shell uname -r),5.6)
OS_LIBS = -lsocket -lnsl -lposix4
else
OS_LIBS = -lsocket -lnsl -lrt
endif
SHARED = -shared
endif
ifeq ($(shell uname),Darwin)
CXX = g++
CXX_DEFS =
LIB_EXT = dylib
OS_LIBS =
SHARED = -dynamiclib
endif
ifeq ($(shell uname),Linux)
CXX = g++
CXX_DEFS = -DGPP2
LIB_EXT = so
OS_LIBS =
SHARED = -shared
endif

#
# These are the locations of certain directories in the package
#
INCLUDE_DIR = ../include
LIB_DIR = ../lib
SRC_DIR = ../src

#
# This is the ultimate target we're making - the library with the entire
# CKit contained within it.
#
LIB_FILE = $(LIB_DIR)/libCKit.$(LIB_EXT)

#
# These are the pretty standard C++-style defines for a makefile - the
# includes, the libs, the compiler flags, etc.
#
INCLUDES = -I. -I$(INCLUDE_DIR) -I$(SRC_DIR)
DEFINES = $(CXX_DEFS)
CXXFLAGS = -fPIC -g -Wall $(INCLUDES) $(DEFINES)

LIBS = -L$(LIB_DIR) $(OS_LIBS) -lstdc++ -lsqlapi -lCKit -lpthread
LDFLAGS = -fPIC

#
# These are the main targets that we'll be making
#
APPS = uuid smtp ftp chat nan node stringTest vectorTest queueTest
all: $(APPS)

clean:
	rm -f $(APPS)

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCS)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) $< -o $@

uuid: uuid.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) uuid.cpp -o uuid $(LIBS) $(LDFLAGS)

smtp: smtp.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) smtp.cpp -o smtp $(LIBS) $(LDFLAGS)

ftp: ftp.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) ftp.cpp -o ftp $(LIBS) $(LDFLAGS)

chat: chat.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) chat.cpp -o chat $(LIBS) $(LDFLAGS)

nan: nan.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) nan.cpp -o nan $(LIBS) $(LDFLAGS)

node: node.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) node.cpp -o node $(LIBS) $(LDFLAGS)

stringTest: stringTest.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) stringTest.cpp -o stringTest $(LIBS) $(LDFLAGS)

vectorTest: vectorTest.cpp ../src/CKVector.h $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) vectorTest.cpp -o vectorTest $(LIBS) $(LDFLAGS)

queueTest: queueTest.cpp ../src/CKFIFOQueue.h $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $(DEBUG) queueTest.cpp -o queueTest $(LIBS) $(LDFLAGS)

# DO NOT DELETE
