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
CXX_DEFS = -D_REENTRANT -O2 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386
LIB_EXT = dylib
OS_LIBS = -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
SHARED = -dynamiclib
endif
ifeq ($(shell uname),Linux)
ifeq ($(shell gcc --version),2.96)
CXX = g++ -DGPP2 -D_GNU_SOURCE
else
CXX = g++
endif
CXX_DEFS = -O2
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
LDFLAGS =

#
# These are the main targets that we'll be making
#
APPS = uuid smtp ftp chat nan node stringTest vectorTest queueTest ParserTest \
		series base64 mindalign numberTest table

all: $(APPS)

clean:
	rm -f $(APPS)

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCS)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) $< -o $@

uuid: uuid.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) uuid.cpp -o uuid $(LIBS) $(LDFLAGS)

smtp: smtp.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) smtp.cpp -o smtp $(LIBS) $(LDFLAGS)

ftp: ftp.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) ftp.cpp -o ftp $(LIBS) $(LDFLAGS)

chat: chat.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) chat.cpp -o chat $(LIBS) $(LDFLAGS)

mindalign: mindalign.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) mindalign.cpp -o mindalign $(LIBS) $(LDFLAGS)

nan: nan.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) nan.cpp -o nan $(LIBS) $(LDFLAGS)

table: table.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) table.cpp -o table $(LIBS) $(LDFLAGS)

node: node.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) node.cpp -o node $(LIBS) $(LDFLAGS)

stringTest: stringTest.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) stringTest.cpp -o stringTest $(LIBS) $(LDFLAGS)

vectorTest: vectorTest.cpp ../src/CKVector.h $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) vectorTest.cpp -o vectorTest $(LIBS) $(LDFLAGS)

queueTest: queueTest.cpp ../src/CKFIFOQueue.h $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) queueTest.cpp -o queueTest $(LIBS) $(LDFLAGS)

ParserTest: ParserTest.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) ParserTest.cpp -o ParserTest $(LIBS) $(LDFLAGS)

series: series.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) series.cpp -o series $(LIBS) $(LDFLAGS)

base64: base64.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) base64.cpp -o base64 $(LIBS) $(LDFLAGS)

numberTest: numberTest.cpp $(LIB_FILE)
	$(CXX) -m32 $(CXXFLAGS) $(DEBUG) numberTest.cpp -o numberTest $(LIBS) $(LDFLAGS)

# DO NOT DELETE
