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
	CKString.o \
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

CKUUID.o: CKUUID.h CKString.h CKException.h
CKSocket.o: CKSocket.h CKString.h CKException.h
CKSocket.o: CKErrNoException.h
CKBufferedSocket.o: CKBufferedSocket.h CKSocket.h
CKBufferedSocket.o: CKString.h CKException.h
CKTCPConnection.o: CKTCPConnection.h CKBufferedSocket.h
CKTCPConnection.o: CKSocket.h CKString.h
CKTelnetConnection.o: CKTelnetConnection.h CKTCPConnection.h
CKTelnetConnection.o: CKBufferedSocket.h CKSocket.h
CKTelnetConnection.o: CKString.h CKException.h
CKFTPConnection.o: CKFTPConnection.h CKSocket.h
CKFTPConnection.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
CKFTPConnection.o: CKString.h CKException.h
CKMailDelivery.o: CKMailDelivery.h CKMailMessage.h CKString.h CKException.h
CKMailDeliverySystem.o: CKMailDeliverySystem.h CKMailDelivery.h
CKMailDeliverySystem.o: CKMailMessage.h CKString.h
CKMailMessage.o: CKMailMessage.h CKString.h CKException.h CKUUID.h
CKSMTPConnection.o: CKSMTPConnection.h CKSocket.h CKString.h
CKSMTPConnection.o: CKTCPConnection.h CKBufferedSocket.h CKException.h
CKSMTPDelivery.o: CKSMTPDelivery.h CKMailDelivery.h CKMailMessage.h
CKSMTPDelivery.o: CKString.h CKSMTPConnection.h CKTCPConnection.h
CKSMTPDelivery.o: CKBufferedSocket.h CKSocket.h CKException.h
CKIRCProtocol.o: CKIRCProtocol.h CKTelnetConnection.h CKTCPConnection.h
CKIRCProtocol.o: CKBufferedSocket.h CKSocket.h CKString.h CKFWMutex.h
CKIRCProtocol.o: CKIRCProtocolListener.h CKFWThread.h CKIRCResponder.h
CKIRCProtocol.o: CKException.h
CKIRCProtocolListener.o: CKIRCProtocolListener.h CKFWThread.h
CKIRCProtocolListener.o: CKIRCProtocol.h CKTelnetConnection.h
CKIRCProtocolListener.o: CKTCPConnection.h CKBufferedSocket.h
CKIRCProtocolListener.o: CKString.h CKFWMutex.h CKIRCProtocolExec.h
CKIRCProtocolListener.o: CKSocket.h CKException.h
CKIRCProtocolExec.o: CKException.h CKString.h CKIRCProtocol.h
CKIRCProtocolExec.o: CKTelnetConnection.h CKTCPConnection.h
CKIRCProtocolExec.o: CKBufferedSocket.h CKSocket.h
CKIRCProtocolExec.o: CKFWMutex.h CKIRCProtocolExec.h CKFWThread.h
CKIRCResponder.o: CKIRCResponder.h CKIRCProtocol.h CKTelnetConnection.h
CKIRCResponder.o: CKTCPConnection.h CKBufferedSocket.h CKSocket.h
CKIRCResponder.o: CKString.h CKFWMutex.h CKFWConditional.h
CKFWConditional.o: CKFWMutex.h CKErrNoException.h CKException.h CKString.h
CKFWMutex.o: CKFWMutex.h CKErrNoException.h CKException.h CKString.h
CKStackLocker.o: CKException.h CKString.h CKStackLocker.h CKFWMutex.h
CKFWSemaphore.o: CKFWSemaphore.h CKErrNoException.h CKException.h
CKFWSemaphore.o: CKString.h
CKFWTimedSemaphore.o: CKFWTimedSemaphore.h CKFWMutex.h CKFWConditional.h
CKFWThread.o: ../include/SQLAPI.h CKFWThread.h CKErrNoException.h 
CKFWThread.o: CKException.h CKString.h
CKFWThreadLocal.o: CKFWThreadLocal.h
CKFWTime.o: CKFWTime.h CKErrNoException.h CKException.h CKString.h
CKFWTimer.o: CKFWTimer.h CKErrNoException.h CKException.h CKString.h
CKString.o: CKString.h CKException.h
CKVariant.o: CKException.h CKString.h CKVariant.h
CKVariant.o: CKTimeSeries.h CKFWMutex.h CKTable.h
CKTable.o: CKException.h CKString.h CKTable.h 
CKTable.o: CKVariant.h CKTimeSeries.h CKFWMutex.h
CKTimeSeries.o: CKException.h CKString.h CKTable.h CKVariant.h CKTimeSeries.h
CKTimeSeries.o: CKFWMutex.h 
CKDataNode.o: CKDataNode.h CKVariant.h CKTimeSeries.h CKFWMutex.h
CKDataNode.o: CKString.h CKException.h
CKStopwatch.o: CKException.h CKString.h CKStopwatch.h 
CKException.o: CKException.h CKString.h
CKErrNoException.o: CKErrNoException.h CKException.h CKString.h
