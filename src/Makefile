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
ifeq ($(shell gcc --version),2.96)
CXX = g++ -DGPP2 -D_GNU_SOURCE
else
CXX = g++
endif
CXX_DEFS = -D_REENTRANT -O2
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
	CKFWRWMutex.o \
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
	CKPrice.o \
	CKDataNode.o \
	CKIndexNode.o \
	CKStopwatch.o \
	CKException.o \
	CKErrNoException.o \
	muParserBase.o \
	muParser.o \
	muParserTest.o
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

CKUUID.o: CKUUID.h CKString.h CKFWMutex.h CKException.h
CKSocket.o: CKSocket.h CKString.h CKFWMutex.h CKException.h CKErrNoException.h
CKBufferedSocket.o: CKBufferedSocket.h CKSocket.h CKString.h CKFWMutex.h 
CKBufferedSocket.o: CKException.h
CKTCPConnection.o: CKTCPConnection.h CKBufferedSocket.h CKSocket.h 
CKTCPConnection.o: CKString.h CKFWMutex.h 
CKTelnetConnection.o: CKTelnetConnection.h CKTCPConnection.h CKException.h
CKTelnetConnection.o: CKBufferedSocket.h CKSocket.h CKString.h CKFWMutex.h 
CKFTPConnection.o: CKFTPConnection.h CKTelnetConnection.h CKTCPConnection.h
CKFTPConnection.o: CKBufferedSocket.h CKSocket.h CKString.h CKFWMutex.h
CKFTPConnection.o: CKException.h
CKMailDelivery.o: CKMailDelivery.h CKMailMessage.h CKString.h CKFWMutex.h
CKMailDelivery.o: CKException.h
CKMailDeliverySystem.o: CKMailDeliverySystem.h CKMailDelivery.h
CKMailDeliverySystem.o: CKMailMessage.h CKString.h CKFWMutex.h
CKMailMessage.o: CKMailMessage.h CKString.h CKFWMutex.h
CKMailMessage.o: CKException.h CKUUID.h 
CKSMTPConnection.o: CKSMTPConnection.h CKTCPConnection.h CKBufferedSocket.h
CKSMTPConnection.o: CKSocket.h CKString.h CKFWMutex.h CKException.h
CKSMTPDelivery.o: CKSMTPDelivery.h CKMailDelivery.h CKMailMessage.h
CKSMTPDelivery.o: CKString.h CKFWMutex.h CKSocket.h CKException.h
CKSMTPDelivery.o: CKSMTPConnection.h CKTCPConnection.h CKBufferedSocket.h
CKIRCProtocol.o: CKIRCProtocol.h CKSocket.h CKString.h CKFWMutex.h
CKIRCProtocol.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
CKIRCProtocol.o: CKVector.h CKStackLocker.h CKException.h CKStopwatch.h 
CKIRCProtocol.o: CKIRCProtocolListener.h CKFWThread.h CKIRCResponder.h
CKIRCProtocolListener.o: CKIRCProtocolListener.h CKFWThread.h CKIRCProtocol.h
CKIRCProtocolListener.o: CKTelnetConnection.h CKTCPConnection.h
CKIRCProtocolListener.o: CKBufferedSocket.h CKSocket.h
CKIRCProtocolListener.o: CKString.h CKFWMutex.h CKVector.h CKStackLocker.h
CKIRCProtocolListener.o: CKException.h CKIRCProtocolExec.h
CKIRCProtocolExec.o: CKException.h CKString.h CKFWMutex.h
CKIRCProtocolExec.o: CKIRCProtocol.h CKTelnetConnection.h CKTCPConnection.h
CKIRCProtocolExec.o: CKBufferedSocket.h CKSocket.h CKFWThread.h
CKIRCProtocolExec.o: CKVector.h CKStackLocker.h CKIRCProtocolExec.h
CKIRCResponder.o: CKIRCResponder.h CKIRCProtocol.h CKTelnetConnection.h
CKIRCResponder.o: CKTCPConnection.h CKBufferedSocket.h 
CKIRCResponder.o: CKSocket.h CKString.h CKFWMutex.h 
CKIRCResponder.o: CKVector.h CKStackLocker.h CKException.h
CKFWConditional.o: CKFWConditional.h 
CKFWConditional.o: ./CKFWMutex.h CKErrNoException.h CKException.h CKString.h
CKFWMutex.o: CKFWMutex.h CKErrNoException.h CKException.h CKString.h
CKFWRWMutex.o: CKFWRWMutex.h CKErrNoException.h CKException.h CKString.h
CKStackLocker.o: CKException.h CKString.h CKFWMutex.h CKStackLocker.h
CKFWSemaphore.o: CKFWSemaphore.h CKErrNoException.h CKException.h
CKFWSemaphore.o: CKString.h CKFWMutex.h 
CKFWTimedSemaphore.o: CKFWTimedSemaphore.h CKFWMutex.h CKFWConditional.h
CKFWThread.o: ../include/SQLAPI.h CKFWThread.h
CKFWThread.o: CKErrNoException.h CKException.h CKString.h CKFWMutex.h
CKFWThreadLocal.o: CKFWThreadLocal.h 
CKFWTime.o: CKFWTime.h CKErrNoException.h CKException.h CKString.h CKFWMutex.h 
CKFWTimer.o: CKFWTimer.h CKErrNoException.h CKException.h CKString.h CKFWMutex.h 
CKString.o: CKString.h CKFWMutex.h 
CKVariant.o: CKException.h CKString.h CKFWMutex.h 
CKVariant.o: CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h CKTable.h
CKTable.o: CKException.h CKString.h CKFWMutex.h
CKTable.o: CKTable.h CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h
CKTimeSeries.o: CKException.h CKString.h CKFWMutex.h CKTable.h 
CKTimeSeries.o: CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h
CKPrice.o: CKPrice.h CKException.h CKString.h CKFWMutex.h CKTable.h 
CKPrice.o: CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h
CKDataNode.o: CKDataNode.h CKVariant.h CKTimeSeries.h CKFWMutex.h
CKDataNode.o: CKString.h CKVector.h CKStackLocker.h CKException.h
CKIndexNode.o: CKIndexNode.h CKDataNode.h CKVariant.h CKTimeSeries.h CKFWMutex.h
CKIndexNode.o: CKString.h CKVector.h CKStackLocker.h CKException.h
CKStopwatch.o: CKException.h CKString.h CKFWMutex.h 
CKStopwatch.o: CKStopwatch.h CKVector.h CKStackLocker.h
CKException.o: CKException.h CKString.h CKFWMutex.h
CKErrNoException.o: CKErrNoException.h 
CKErrNoException.o: CKException.h CKString.h CKFWMutex.h
muParserBase.o: muParser.h muParserBase.h muParserDef.h muParserGenerics.h
muParserBase.o: muParserException.h
muParser.o: muParser.h muParserBase.h muParserDef.h muParserGenerics.h
muParser.o: muParserException.h
muParserTest.o: muParserTest.h muParser.h muParserBase.h muParserDef.h
muParserTest.o: muParserGenerics.h muParserException.h
