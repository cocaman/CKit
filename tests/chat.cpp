/*
 * This is a test program that exercises the IRC Connection in CKit to
 * confirm that it's working and that things are all OK. It connects to
 * a chat server, does a little chatting and then bails out.
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "CKIRCProtocol.h"
#include "CKIRCResponder.h"

bool	cQuit;

class myResponder :
	public CKIRCResponder
{
	public:
		myResponder() {};
		virtual ~myResponder() {};
		virtual void respondToIRCMessage( CKIRCIncomingMessage & aMsg );
};

void myResponder::respondToIRCMessage( CKIRCIncomingMessage & aMsg )
{
	aMsg.response = "got: ";
	aMsg.response += aMsg.message;
	if (aMsg.message.substr(0,4) == "quit") {
		cQuit = true;
 	}
 	std::cout << "got: '" << aMsg.message << "'" << std::endl;
}




int main(int argc, char *argv[]) {
	bool	error = false;
	
	// set the flag to run
	cQuit = false;

	// establish a connection to the IRC server
	std::cout << "Connecting to chat:6667" << std::endl;
	CKIRCProtocol	irc("chat", 6667, "_xchi6019");
	// set some reasonable values...
	irc.setRealName("killer chat bot");
	std::cout << "Connected and ready to go" << std::endl;

	// create a responder
	myResponder	r;
	irc.addToResponders(&r);
	
	while (!cQuit) {
		std::cout << "chatting again..." << std::endl;
		irc.sendMessage("drbob", "Another trip through the loop");
		sleep(5);
	}

	irc.disconnect();

	if (error) {
		std::cout << "FAILURE" << std::endl;
	} else {
		std::cout << "SUCCESS" << std::endl;
	}
}
