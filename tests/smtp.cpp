/*
 * This is a test program that uses SMTP and mails a little test message
 * to a user. It's nothing special, but it shows that it's working and
 * how to use the classes.
 */

#include <iostream>
#include <stdio.h>

#include "CKMailMessage.h"
#include "CKMailDeliverySystem.h"
#include "CKSMTPDelivery.h"

int main(int argc, char *argv[]) {
	bool	error = false;

	std::cout << "Creating an SMTP delivery channel as the default" << std::endl;
	CKSMTPDelivery	dc("smtp-us.swissbank.com", "SMTP Test");
	CKMailDeliverySystem::setDefaultDelivery(&dc);

	std::cout << "Creating the message" << std::endl;
	CKMailMessage	msg;
	msg.setSubject("This is a test");
	msg.setMessageBody("Hi, Bob! This is a nice test.");
	msg.addToRecipients("robert.beaty@ubs.com");

	std::cout << "Sending the message" << std::endl;
	if (!CKMailDeliverySystem::deliver(msg)) {
		error = true;
	}

	if (error) {
		std::cout << "FAILURE" << std::endl;
	} else {
		std::cout << "SUCCESS" << std::endl;
	}
}
