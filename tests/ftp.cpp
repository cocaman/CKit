/*
 * This is a test program that exercises the FTP Connection in CKit to
 * confirm that it's working and that things are all OK. It connects to
 * a test account, does a little directory changing and transfers a file
 * before quitting.
 */

#include <iostream>
#include <stdio.h>
#include <CKString.h>

#include "CKFTPConnection.h"

int main(int argc, char *argv[]) {
	bool	error = false;

	// establish a connection to the FTP server
	std::cout << "Connecting to xchi6100dwk" << std::endl;
	CKFTPConnection	ftp("xchi6100dwk");
	ftp.setTraceControlCommunications(true);
	
	// try to login as a known user
	std::cout << "Attempting to login to the FTP server" << std::endl;
	if (!ftp.loginToHost("anonymous", "robert.beaty@ubs.com")) {
		error = true;
		std::cerr << "Could not log into the FTP server" << std::endl;
	}
	
	// get the starting directory
	std::cout << "Getting the starting directory" << std::endl;
	CKString		loginDir = ftp.currentDirectoryPath();
	std::cout << "CWD = " << loginDir << std::endl;

	// change directory and get new directory
	std::cout << "Changing Directory" << std::endl;
	ftp.changeCurrentDirectoryPath("pub");
	ftp.changeCurrentDirectoryPath("files");
	ftp.changeCurrentDirectoryPath("tools");
	std::cout << "CD to: " << ftp.currentDirectoryPath() << std::endl;
	
	// see if I can get the file there
	std::cout << "Getting remote file contents" << std::endl;
	CKString		test1 = ftp.getContents("README");
	std::cout << "----- README -----" << std::endl << test1 <<
				 "------------------" << std::endl;
	
	//now see if I can copy this guy
	std::cout << "Copying remote file to local disk" << std::endl;
	ftp.copyHostToLocal("README", "/tmp/README");

	// close the connection down
	std::cout << "Disconnecting" << std::endl;
	ftp.disconnect();
	
	if (error) {
		std::cout << "FAILURE" << std::endl;
	} else {
		std::cout << "SUCCESS" << std::endl;
	}
}
