#
# Makefile for CKit including the FeatherWeight library
#
#
# We need to have a few system-level dependencies to make things cleanly
# compile on a few different systems.
#
ifeq ($(shell uname),SunOS)
CXX = g++
CXX_DEFS = -D_REENTRANT -O2
SO_EXT = so
SO_FLAG = -shared
endif
ifeq ($(shell uname),Linux)
CXX = g++
CXX_DEFS = -DGPP2 -D_REENTRANT -O2
SO_EXT = so
SO_FLAG = -shared
endif
ifeq ($(shell uname),Darwin)
CXX = g++
CXX_DEFS = -D_REENTRANT -O2
SO_EXT = dylib
SO_FLAG = -dynamiclib
endif

#
# These are the locations of the directories we'll use
#
SRC_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin
INCLUDE_DIR = ../include

#
# This is the ultimate target we're making - the library with the entire
# Bloomberg Server contained within it.
#
LIB_FILE = $(LIB_DIR)/libCKit.$(SO_EXT)

#
# These are the pretty standard C++-style defines for a makefile - the
# includes, the libs, the compiler flags, etc.
#
INCLUDES = -I. -I$(INCLUDE_DIR)
DEFINES = $(CXX_DEFS)
CXXFLAGS = -fPIC -g -Wall $(INCLUDES) $(DEFINES)
LIBS = -L$(LIB_DIR) -lstdc++ -lsqlapi
LDFLAGS = -fPIC $(LIBS)

#
# These are all the components of CKit
#
OBJS = CKUUID.o \
	CKSocket.o \
	CKBufferedSocket.o \
	CKTCPConnection.o \
	CKTelnetConnection.o \
	CKFTPConnection.o \
	CKMailDelivery.o \
	CKMailDeliverySystem.o \
	CKMailMessage.o \
	CKSMTPConnection.o \
	CKSMTPDelivery.o \
	CKIRCProtocol.o \
	CKIRCProtocolListener.o \
	CKIRCProtocolExec.o \
	CKIRCResponder.o \
	CKFWConditional.o \
	CKFWMutex.o \
	CKStackLocker.o \
	CKFWSemaphore.o \
	CKFWTimedSemaphore.o \
	CKFWThread.o \
	CKFWThreadLocal.o \
	CKFWTime.o \
	CKFWTimer.o \
	CKVariant.o \
	CKTable.o \
	CKTimeSeries.o \
	CKDataNode.o \
	CKStopwatch.o \
	CKException.o \
	CKErrNoException.o
SRCS = $(OBJS:%.o=%.cpp)

#
# These are the main targets that we'll be making
#
all: $(LIB_FILE)

clean:
	rm -f $(OBJS) $(LIB_FILE)

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCS)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) $< -o $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(LIB_FILE): $(LIB_DIR) $(OBJS)
	$(CXX) $(SO_FLAG) $(OBJS) -o $(LIB_FILE) $(LDFLAGS)

# DO NOT DELETE

CKUUID.o: CKUUID.h CKException.h
CKSocket.o: CKSocket.h CKException.h CKErrNoException.h
CKBufferedSocket.o: CKBufferedSocket.h CKSocket.h CKException.h
CKTCPConnection.o: CKTCPConnection.h CKBufferedSocket.h CKSocket.h
CKTelnetConnection.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
CKTelnetConnection.o: CKSocket.h CKException.h
CKFTPConnection.o: CKFTPConnection.h CKTelnetConnection.h CKTCPConnection.h
CKFTPConnection.o: CKBufferedSocket.h CKSocket.h CKException.h
CKMailDelivery.o: CKMailDelivery.h CKMailMessage.h CKException.h
CKMailDeliverySystem.o: CKMailDeliverySystem.h CKMailDelivery.h
CKMailDeliverySystem.o: CKMailMessage.h
CKMailMessage.o: CKMailMessage.h CKException.h CKUUID.h
CKSMTPConnection.o: CKSMTPConnection.h CKTCPConnection.h CKBufferedSocket.h
CKSMTPConnection.o: CKSocket.h CKException.h
CKSMTPDelivery.o: CKSMTPDelivery.h CKMailDelivery.h CKMailMessage.h
CKSMTPDelivery.o: CKSMTPConnection.h CKTCPConnection.h CKBufferedSocket.h
CKSMTPDelivery.o: CKSocket.h CKException.h
CKIRCProtocol.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
CKIRCProtocol.o: CKIRCProtocol.h CKSocket.h CKFWMutex.h CKException.h
CKIRCProtocol.o: CKIRCProtocolListener.h CKFWThread.h CKIRCResponder.h
CKIRCProtocolListener.o: CKIRCProtocolListener.h CKFWThread.h CKIRCProtocol.h
CKIRCProtocolListener.o: CKTelnetConnection.h CKTCPConnection.h CKFWMutex.h
CKIRCProtocolListener.o: CKBufferedSocket.h CKSocket.h CKException.h
CKIRCResponder.o: CKIRCResponder.h CKIRCProtocol.h CKTelnetConnection.h
CKIRCResponder.o: CKTCPConnection.h CKBufferedSocket.h CKSocket.h CKFWMutex.h
CKFWConditional.o: CKFWConditional.h CKFWMutex.h CKErrNoException.h CKException.h
CKFWMutex.o: CKFWMutex.h CKErrNoException.h CKException.h
CKStackLocker.o: CKStackLocker.h CKFWMutex.h CKErrNoException.h CKException.h
CKFWSemaphore.o: CKFWSemaphore.h CKErrNoException.h CKException.h
CKFWTimedSemaphore.o: CKFWTimedSemaphore.h CKFWMutex.h CKFWConditional.h
CKFWThread.o: CKFWThread.h CKErrNoException.h CKException.h
CKFWThreadLocal.o: CKFWThreadLocal.h
CKFWTime.o: CKFWTime.h CKException.h
CKFWTimer.o: CKFWTimer.h CKErrNoException.h CKException.h
CKVariant.o: CKException.h CKVariant.h CKTable.h CKTimeSeries.h CKFWMutex.h
CKTable.o: CKException.h CKTable.h CKVariant.h
CKTimeSeries.o: CKException.h CKTable.h CKVariant.h CKTimeSeries.h CKFWMutex.h
CKDataNode.o: CKException.h CKVariant.h CKTable.h CKTimeSeries.h CKFWMutex.h
CKDataNode.o: CKDataNode.h
CKStopwatch.o: CKException.h CKStopwatch.h
CKErrNoException.o: CKErrNoException.h CKException.h
