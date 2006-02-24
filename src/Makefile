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
	CKTimeTable.o \
	CKPrice.o \
	CKDataNode.o \
	CKIndexNode.o \
	CKMultiIndexNode.o \
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
CKFTPConnection.o: CKFTPConnection.h CKSocket.h CKString.h CKFWMutex.h
CKFTPConnection.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
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
CKIRCProtocol.o: CKIRCProtocol.h
CKIRCProtocol.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h
CKIRCProtocol.o: CKSocket.h CKString.h CKFWMutex.h CKVector.h CKStackLocker.h
CKIRCProtocol.o: CKFWRWMutex.h CKFWSemaphore.h CKIRCResponder.h CKStopwatch.h
CKIRCProtocol.o: CKException.h CKIRCProtocolListener.h CKFWThread.h
CKIRCProtocolListener.o: CKIRCProtocolListener.h CKFWThread.h 
CKIRCProtocolListener.o: CKIRCProtocol.h CKTelnetConnection.h CKTCPConnection.h
CKIRCProtocolListener.o: CKBufferedSocket.h CKSocket.h CKString.h CKFWMutex.h
CKIRCProtocolListener.o: CKVector.h CKStackLocker.h CKFWRWMutex.h
CKIRCProtocolListener.o: CKFWSemaphore.h CKException.h CKIRCProtocolExec.h
CKIRCProtocolExec.o: CKException.h CKString.h CKFWMutex.h CKIRCProtocol.h
CKIRCProtocolExec.o: CKTelnetConnection.h CKTCPConnection.h CKBufferedSocket.h 
CKIRCProtocolExec.o: CKSocket.h CKVector.h CKStackLocker.h CKFWRWMutex.h
CKIRCProtocolExec.o: CKFWSemaphore.h CKIRCProtocolExec.h CKFWThread.h
CKIRCResponder.o: CKIRCResponder.h CKIRCProtocol.h CKTelnetConnection.h
CKIRCResponder.o: CKTCPConnection.h CKBufferedSocket.h CKSocket.h 
CKIRCResponder.o: CKString.h CKFWMutex.h CKVector.h CKStackLocker.h
CKIRCResponder.o: CKFWRWMutex.h CKFWSemaphore.h CKException.h
CKFWConditional.o: CKFWConditional.h CKFWMutex.h CKErrNoException.h 
CKFWConditional.o: CKException.h CKString.h
CKFWMutex.o: CKFWMutex.h CKErrNoException.h CKException.h CKString.h
CKFWRWMutex.o: CKFWRWMutex.h CKErrNoException.h CKException.h
CKFWRWMutex.o: CKString.h CKFWMutex.h
CKStackLocker.o: CKException.h CKString.h CKFWMutex.h
CKStackLocker.o: CKStackLocker.h CKFWRWMutex.h CKFWSemaphore.h
CKFWSemaphore.o: CKFWSemaphore.h CKErrNoException.h CKException.h
CKFWSemaphore.o: CKString.h CKFWMutex.h
CKFWTimedSemaphore.o: CKFWTimedSemaphore.h CKFWMutex.h CKFWConditional.h
CKFWThread.o: ../include/SQLAPI.h CKFWThread.h CKErrNoException.h
CKFWThread.o: CKException.h CKString.h CKFWMutex.h
CKFWThreadLocal.o: CKFWThreadLocal.h 
CKFWTime.o: CKFWTime.h CKErrNoException.h CKException.h CKString.h CKFWMutex.h
CKFWTimer.o: CKFWTimer.h CKErrNoException.h CKException.h CKString.h 
CKFWTimer.o: CKFWMutex.h
CKString.o: CKException.h CKString.h CKFWMutex.h 
CKVariant.o: CKVariant.h CKTimeSeries.h CKFWMutex.h
CKVariant.o: CKString.h CKVector.h CKStackLocker.h CKFWRWMutex.h
CKVariant.o: CKFWSemaphore.h CKException.h CKTable.h
CKVariant.o: CKTimeTable.h muParser.h muParserBase.h muParserDef.h
CKVariant.o: muParserGenerics.h muParserException.h CKPrice.h
CKTable.o: CKException.h CKString.h CKFWMutex.h
CKTable.o: CKTable.h CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h
CKTable.o: CKFWRWMutex.h CKFWSemaphore.h 
CKTimeSeries.o: CKException.h CKString.h CKFWMutex.h 
CKTimeSeries.o: CKTable.h CKVariant.h CKFWSemaphore.h 
CKTimeSeries.o: CKTimeSeries.h CKVector.h CKStackLocker.h CKFWRWMutex.h
CKTimeTable.o: CKTimeTable.h CKFWMutex.h CKTable.h 
CKTimeTable.o: CKVariant.h CKFWRWMutex.h CKFWSemaphore.h 
CKTimeTable.o: CKTimeSeries.h CKString.h CKVector.h CKStackLocker.h
CKTimeTable.o: CKException.h muParser.h muParserBase.h muParserDef.h
CKTimeTable.o: muParserGenerics.h muParserException.h
CKPrice.o: CKPrice.h CKException.h CKString.h CKFWMutex.h
CKPrice.o: CKTable.h CKVariant.h CKTimeSeries.h CKVector.h CKStackLocker.h
CKPrice.o: CKFWRWMutex.h CKFWSemaphore.h 
CKDataNode.o: CKDataNode.h CKVariant.h CKTimeSeries.h CKFWMutex.h
CKDataNode.o: CKString.h CKVector.h CKStackLocker.h CKFWRWMutex.h
CKDataNode.o: CKFWSemaphore.h CKException.h
CKIndexNode.o: CKIndexNode.h CKDataNode.h CKVariant.h CKTimeSeries.h
CKIndexNode.o: CKFWMutex.h CKString.h CKVector.h CKException.h
CKIndexNode.o: CKStackLocker.h CKFWRWMutex.h CKFWSemaphore.h
CKMultiIndexNode.o: CKMultiIndexNode.h CKVector.h CKString.h CKFWMutex.h
CKMultiIndexNode.o: CKStackLocker.h CKFWRWMutex.h CKFWSemaphore.h
CKMultiIndexNode.o: CKException.h CKDataNode.h CKVariant.h CKTimeSeries.h
CKStopwatch.o: CKException.h CKString.h CKFWMutex.h CKStopwatch.h 
CKStopwatch.o: CKVector.h CKStackLocker.h CKFWRWMutex.h CKFWSemaphore.h 
CKException.o: CKException.h CKString.h CKFWMutex.h
CKErrNoException.o: CKErrNoException.h CKException.h CKString.h CKFWMutex.h
muParserBase.o: muParser.h muParserBase.h muParserDef.h muParserGenerics.h
muParserBase.o: muParserException.h CKVariant.h CKTimeSeries.h CKFWMutex.h 
muParserBase.o: CKString.h CKVector.h CKException.h 
muParserBase.o: CKStackLocker.h CKFWRWMutex.h CKFWSemaphore.h
muParser.o: muParser.h muParserBase.h muParserDef.h muParserGenerics.h
muParser.o: muParserException.h CKVariant.h CKTimeSeries.h CKFWMutex.h 
muParser.o: CKString.h CKVector.h CKStackLocker.h CKTimeTable.h
muParser.o: CKFWRWMutex.h CKFWSemaphore.h CKException.h CKTable.h CKPrice.h
muParserTest.o: muParserTest.h muParser.h muParserBase.h muParserDef.h
muParserTest.o: muParserGenerics.h muParserException.h CKVariant.h
muParserTest.o: CKTimeSeries.h CKFWMutex.h 
muParserTest.o: CKString.h CKVector.h CKStackLocker.h CKFWRWMutex.h
muParserTest.o: CKFWSemaphore.h CKException.h
