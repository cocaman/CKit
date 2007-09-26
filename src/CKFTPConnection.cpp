/*
 * CKFTPConnection.cpp - this file implements the class that can be used as a
 *                       rather advanced FTP-based file manipulation tool for
 *                       the user. The idea is that all local and remote file
 *                       access can be done through this class - which includes
 *                       moving files, copying them from one host to another,
 *                       creating and reading them - pretty much everything
 *                       you might want to do with files you can do with this
 *                       class.
 *
 *                       It exists because there was a need to have the ease
 *                       of FTP but keep everything in the process space and
 *                       not shell out to have the file copied and then have
 *                       to the read it in.
 *
 * $Id: CKFTPConnection.cpp,v 1.13 2007/09/26 19:33:45 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef GPP2
#include <algorithm>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKFTPConnection.h"
#include "CKException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that gets things started, but
 * wse still need to set the at least the host, login and password
 * before we can make a successful connection.
 */
CKFTPConnection::CKFTPConnection() :
	mHostname(),
	mUsername(),
	mPassword(),
	mState(-1),
	mControlPort(),
	mServerReplyLines(),
	mIsLoggedIn(false),
	mIncomingDataTimeout(DEFAULT_INCOMING_DATA_TIMEOUT)
{
}


/*
 * This constructor specifies the host, but wse still need to set
 * the at least the login and password before we can make a
 * successful connection.
 */
CKFTPConnection::CKFTPConnection( const CKString & aHost ) :
	mHostname(),
	mUsername(),
	mPassword(),
	mState(-1),
	mControlPort(),
	mServerReplyLines(),
	mIsLoggedIn(false),
	mIncomingDataTimeout(DEFAULT_INCOMING_DATA_TIMEOUT)
{
	// try to establish a control connection to the host
	if (!connectToHost(aHost)) {
		std::ostringstream	msg;
		msg << "CKFTPConnection::CKFTPConnection(const CKString &) - while "
			"trying to connect to the FTP server on '" << aHost << "' we were "
			"unable to talk to the control port. This is a serious problem that "
			"needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This constructor specifies everything that we need to get
 * the FTP connection up and running. The timeout defaults to
 * a reasonable value, so you shouldn't have to set it unless
 * you know that there's a serious problem with the FTP server.
 */
CKFTPConnection::CKFTPConnection( const CKString & aHost,
                                  const CKString & aUser,
                                  const CKString & aPassword,
                                  int anIncomingDataTimeout ) :
	mHostname(),
	mUsername(),
	mPassword(),
	mState(-1),
	mControlPort(),
	mServerReplyLines(),
	mIsLoggedIn(false),
	mIncomingDataTimeout(DEFAULT_INCOMING_DATA_TIMEOUT)
{
	// try to establish a control connection to the host
	if (!connectToHost(aHost)) {
		std::ostringstream	msg;
		msg << "CKFTPConnection::CKFTPConnection(const CKString &, const "
			"CKString &, const CKString &, int) - while trying to connect "
			"to the FTP server on '" << aHost << "' we were unable to talk to "
			"the control port. This is a serious problem that needs to be "
			"looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// try to log into the FTP server
		if (!loginToHost(aUser, aPassword)) {
			std::ostringstream	msg;
			msg << "CKFTPConnection::CKFTPConnection(const CKString &, const "
				"CKString &, const CKString &, int) - while trying to login "
				"to the FTP server on '" << aHost << "' we were not allowed "
				"access. Please make sure that you have provided a valid username "
				"and password for this FTP server. This is a serious problem that "
				"needs to be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKFTPConnection::CKFTPConnection( const CKFTPConnection & anOther ) :
	mHostname(),
	mUsername(),
	mPassword(),
	mState(-1),
	mControlPort(),
	mServerReplyLines(),
	mIsLoggedIn(false),
	mIncomingDataTimeout(DEFAULT_INCOMING_DATA_TIMEOUT)
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKFTPConnection::~CKFTPConnection()
{
	// disconnect from the FTP server
	disconnect();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKFTPConnection & CKFTPConnection::operator=( const CKFTPConnection & anOther )
{
	// make sure we don't to this to ourselves
	if (this != & anOther) {
		// set everything that the other one has
		mHostname = anOther.mHostname;
		mUsername = anOther.mUsername;
		mPassword = anOther.mPassword;
		mState = anOther.mState;
		mControlPort = anOther.mControlPort;
		mServerReplyLines = anOther.mServerReplyLines;
		mIsLoggedIn = anOther.mIsLoggedIn;
		mIncomingDataTimeout = anOther.mIncomingDataTimeout;
	}
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This accessor method simply sets the host name for use by
 * subsequent FTP connection(s). This distinction needs to be
 * made because once a connection to an FTP server on a remote
 * host is made, the host name is "locked" and cannot be
 * changed so long as that connection is established.
 */
void CKFTPConnection::setHostname( const CKString & aName )
{
	mHostname = aName;
}


/*
 * This accessor method simply sets the user name for use by
 * subsequent FTP connection(s). This distinction needs to be
 * made because once a user is logged into the FTP server on
 * the remote host, the user name is "locked" and cannot be
 * changed so long as that user is logged in.
 */
void CKFTPConnection::setUsername( const CKString & aName )
{
	mUsername = aName;
}


/*
 * This accessor method simply sets the user's password for
 * use by subsequent FTP connection(s). This distinction needs
 * to be made because once a user is logged into the FTP server
 * on the remote host, the user's password is "locked" and
 * cannot be changed so long as that user is logged in.
 */
void CKFTPConnection::setPassword( const CKString & aPassword )
{
	mPassword = aPassword;
}


/*
 * This method sets the tracing capabilities for the control
 * connection/port of the FTP session. This is useful as a
 * diagnostic tool in that all received and transmitted data
 * will be sent to the console for diagnosis of any problems.
 *
 * This method simply verifies that a control port is available
 * and then instructs that port to set or reset it's tracing
 * based on the passed-in value.
 */
void CKFTPConnection::setTraceControlCommunications( bool aFlag )
{
	mControlPort.setTraceData(aFlag);
}


/*
 * This method sets the timeout for all incoming data for the
 * FTP transfer socket connections.
 */
void CKFTPConnection::setIncomingDataTimeout( int aTimeoutInSecs )
{
	mIncomingDataTimeout = aTimeoutInSecs;
}


/*
 * This accessor method simply returns the name of the remote host
 * to use, or in use. This distinction needs to be made because once
 * a connection is made the hostname is "locked" and cannot be
 * changed so long as a connection exists.
 */
CKString CKFTPConnection::getHostname() const
{
	return mHostname;
}


/*
 * This accessor method simply returns the user name in use, or
 * ready for use by the connection. This distinction needs to be
 * made because once a user is logged into a remote host, the
 * user name is "locked" and cannot be changed so long as that
 * user is logged in.
 */
CKString CKFTPConnection::getUsername() const
{
	return mUsername;
}


/*
 * This accessor method simply returns the state of the FTP control
 * connection to the remote host. If the user has logged in
 * successfully, this method will return true, otherwise it will
 * return false.
 */
bool CKFTPConnection::isLoggedIn() const
{
	return mIsLoggedIn;
}


/*
 * This method simply returns the state of data tracing on the
 * FTP control port. It actually goes to the control port and
 * asks it it's status, and returns that. If the
 * CKTelnetConnection is logging all received and sent data, then
 * this method will return true, otherwise it returns false.
 */
bool CKFTPConnection::traceControlCommunications() const
{
	return mControlPort.traceData();
}


/*
 * This method returns the value (in seconds) of the incoming data
 * timeout for all FTP communications. This is important because
 * we need to be creating new transfer sockets and they need to
 * have a timeout that might be different from the control port.
 */
int CKFTPConnection::getIncomingDataTimeout() const
{
	return mIncomingDataTimeout;
}


/********************************************************
 *
 *                Connection Methods
 *
 ********************************************************/
/*
 * This method takes the hostname provided and attempts to open
 * the control port connection to the PI port on the FTP server.
 * If successful, the login procedure is done. If the login fails,
 * as evidenced by a return value of false; then another username and
 * password combination can be attempted to this same host.
 */
bool CKFTPConnection::connectToHost( const CKString & aHost )
{
	bool	error = false;

	// Now we need to open the socket connection to the server
	if (!error) {
		// save the hostname we're trying to connect to
		setHostname(aHost);

		// We need to connect to the FTP server on the TCP connection
		if (!mControlPort.connect(aHost, DEFAULT_FTP_PORT)) {
			error = true;
			// wipe out the hostname we previously set
			setHostname("");
			// ...and throw the exception
			std::ostringstream	msg;
			msg << "CKFTPConnection::connectToHost(const CKString &) - we "
				"were unable to establish a socket-level connection to the  host " <<
				aHost << ":" << DEFAULT_FTP_PORT << " for the FTP control port. "
				"This is a serious problem as we have to have this port established "
				"in order to initiate any FTP actions. Please check to see that the "
				"FTP server is running and available on that host.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			/*
			 * Now that we have a connection, we can make sure that the
			 * socket itself is in non-blocking mode regardless of the
			 * platform we're on.
			 */
			mControlPort.doNotBlockForTransferredData();
		}
	}

    /*
     * Finally, we need to receive the reply we get from the FTP
     * server when we make a connection to it. Because there are
	 * instances that this will take a while, let's up the timeout
	 * on the socket.
     */
    if (!error) {
		mControlPort.setReadTimeout(180);
        if (getReply() != CKFTPServiceReadyForNewUser) {
            /*
             * Undo what we've done by disconnecting and resetting the
             * hostname
             */
            mControlPort.disconnect();
            setHostname("");
            // record the error and raise the exception
            error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::connectToHost(const CKString &) - the "
				"FTP server on the  host " << aHost << " did not indicate that "
				"it was ready for users. This could mean that the server is "
				"overloaded, or it's not functioning properly. Please check these "
				"before trying again.";
			throw CKException(__FILE__, __LINE__, msg.str());
        }
    }

	return !error;
}


/*
 * This method simply goes through the FTP server login procedure
 * using the username and password already set up, and the control
 * port already connected to a remote host.
 *
 * If no remote host is connected to, this method obviously fails.
 */
bool CKFTPConnection::loginToHost()
{
	return loginToHost(getUsername(), getPassword());
}


/*
 * This method attempts to set the username and password for this
 * FTP connection, and if successful, tries to log into the
 * remote host already connected to. Due to the nature of the
 * login information, these changes will only be accepted if there
 * isn't already a user logged into the remote host.
 */
bool CKFTPConnection::loginToHost( const CKString & aUser,
								   const CKString & aPassword )
{
	bool	error = false;

	// see if the control port is connected to a remote host
	if (!error) {
		if (!mControlPort.isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::loginToHost(const CKString &, "
				"const CKString &) - there is no established connection to an "
				"FTP server so we can't possibly login to one. Please make sure to "
				"connect to a server with connectToHost() first, and then call "
				"this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// send the FTP login sequence
	if (!error) {
		if (ftpLoginUsername(aUser, aPassword) != CKFTPUserSuccessfullyLoggedIn) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::loginToHost(const CKString &, "
				"const CKString &) - the FTP server on " << getHostname() <<
				" did not successfully authenticate the user " << aUser << ". "
				"Please make sure that the FTP server and username match and that "
				"the password is correct.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

    // if we're successful, then set the logged in flag
    if (!error) {
        setIsLoggedIn(true);
        // don't forget to save the login parameters that worked
        setUsername(aUser);
        setPassword(aPassword);
    }

	return !error;
}


/*
 * This is the call to log out of the FTP server on the remote
 * host and close the command connection to that remote host.
 * If the user has not already logged-out this command will do it
 * for them.
 */
void CKFTPConnection::disconnect()
{
	if (mControlPort.isConnected()) {
		if (isLoggedIn()) {
			doQUIT();
			setIsLoggedIn(false);
		}
		mControlPort.disconnect();
	}
}


/********************************************************
 *
 *             Directory Operation Methods
 *
 ********************************************************/
/*
 * This method is the equivalent method call for OPENSTEP's
 * NSFileManager to change the current working directory on the
 * remote host. This is very important because for repeated copy
 * calls, it is advantageous to be in the directory so that it
 * doesn't have to be completly spelled out each time.
 *
 * If the change cannot be made, this method returns false. Otherwise
 * it returns true after making the change at the remote host.
 */
bool CKFTPConnection::changeCurrentDirectoryPath( const CKString & aPath )
{
	bool	error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::changeCurrentDirectoryPath(const CKString &) - "
				"this instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to loginToHost() "
				"before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// run the CWD command and check for a good reply
	if (!error) {
        if (!isPositiveCompletionReply(doCWD(aPath))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::changeCurrentDirectoryPath(const CKString &) - "
				"the FTP server at " << getHostname() << " did not reply with a "
				"successful return code. This could mean that you didn't have the "
				"necessary permissions to change to that directory, or it wasn't "
				"a directory at all. Please check with a standard FTP client.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method creates a directory on the remote host at the
 * path specified. It is similar to the OPENSTEP's NSFileManager
 * method, but executed on the remote host.
 *
 * If the directory cannot be created, this method returns false.
 * Otherwise it returns true after creating the directory on the
 * remote host.
 */
bool CKFTPConnection::createDirectoryAtPath( const CKString & aPath )
{
	bool	error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::createDirectoryAtPath(const CKString &) - "
				"this instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to loginToHost() "
				"before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// run the MKD command and check for a good reply
	if (!error) {
        if (!isPositiveCompletionReply(doMKD(aPath))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::createDirectoryAtPath(const CKString &) - "
				"the FTP server at " << getHostname() << " did not reply with a "
				"successful return code. This could mean that you didn't have the "
				"necessary permissions to create that directory. Please check "
				"with a standard FTP client.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method simply returns the current path on the remote
 * host, just like the OPENSTEP NSFileManager does for the local
 * machine. It is returned in an CKString. If there is no
 * current path, or there is no established connection, this
 * method returns the empty string.
 */
CKString CKFTPConnection::currentDirectoryPath()
{
	bool			error = false;
	CKString		retval;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::currentDirectoryPath() - this instance is "
				"not logged into a valid FTP server and so nothing can be done. "
				"Please login to the server with a call to loginToHost() before "
				"calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// run the PWD command and check for a good reply
	if (!error) {
        if (!isPositiveCompletionReply(doPWD())) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::currentDirectoryPath() - the FTP server "
				"at " << getHostname() << " did not reply with a successful "
				"return code. Please check the FTP server as to why.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to look at the reply from the server and pick
	 * out the directory name - it's the part between the double
	 * quotes in the first line of the reply.
	 */
	if (!error) {
		int		beg = mServerReplyLines[0].find("\"", 0);
		int		end = mServerReplyLines[0].find("\"", (beg+1));
		if ((beg == -1) || (end == -1) || (end <= beg)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::currentDirectoryPath() - the directory "
				"should have been placed between double quotes on the first "
				"line of the reply from the server. That line is this: '" <<
				mServerReplyLines[0] << "'. Please check into this problem as "
				"soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// grab the stuff between the double-quotes
			retval = mServerReplyLines[0].substr((beg+1), (end - beg - 1));
		}
	}

	return retval;
}


/********************************************************
 *
 *                File Operations Methods
 *
 ********************************************************/
/*
 * This method implements the traditional FTP role of moving a
 * file from the local host to the remote host. In FTP terms,
 * this is a 'PUT <filename>'. Of course, since a full path can
 * be used to describe the source and destination, it offers a
 * bit more flexibility than the traditional FTP command.
 *
 * If the file cannot be copied, this method returns false.
 * Otherwise it returns true after copying the file to the
 * remote host.
 */
bool CKFTPConnection::copyLocalToHost( const CKString & aLocalFile,
                                       const CKString & aRemoteFile )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::copyLocalToHost(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	if (!error) {
		// read the local file into a CKString
		CKString data = getLocalContents(aLocalFile);
		// Now simply call the main data mover to the host
		error = !createFile(aRemoteFile, data);
	}

	return !error;
}


/*
 * This method implements the traditional FTP role of moving a
 * file from the remote host to the local host. In FTP terms,
 * this is a 'GET <filename>'. Of course, since a full path can
 * be used to describe the source and destination, it offers a
 * bit more flexibility than the traditional FTP command.
 *
 * If the file cannot be copied, this method returns false.
 * Otherwise it returns true after copying the file from the
 * remote host.
 */
bool CKFTPConnection::copyHostToLocal( const CKString & aRemoteFile,
                                       const CKString & aLocalFile )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::copyHostToLocal(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	if (!error) {
		// read the remote file into a CKString
		CKString data = getContents(aRemoteFile);
		// now simply write it out to the local filesystem
		setLocalContents(aLocalFile, data);
	}

	return !error;
}


/*
 * This method takes a CKString and creates a file on the remote
 * host at the location specified filled with the contents of
 * the string. This is a simplified way to create what is needed
 * in memory, and transfer it from memory without having to go
 * through the intermediate file steps.
 *
 * If the file cannot be created, this method returns false.
 * Otherwise it returns true after creating the file at the
 * remote host.
 */
bool CKFTPConnection::createFile( const CKString & aFilename,
                                  const CKString & aData )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::createFile(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the data stored remotely
	if (!error) {
		CKString		type = "I";
		CKString		cmd = CKFTPCommandStringSTOR;
		cmd += aFilename;
		try {
			transferData(type, cmd, aData);
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::createFile(const CKString &, "
				"const CKString &) - while trying to transfer the data to "
				"FTP server a CKException was thrown: " << e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method is a copyLocalToHost() followed by a removal of the
 * local file. This is a convenient way to ensure that the copy is
 * done before the deletion in situations where there is no need to
 * keep the local copy.
 *
 * If the file cannot be copied and the local copy deleted, this
 * method returns false. Otherwise it returns true after copying the
 * file to the remote host and deleting the local file. It is possible
 * that the file will be copied successfully, and the removal will
 * fail. If this is a significant concern, check for the existance of
 * the file at the remote host with fileExists().
 */
bool CKFTPConnection::moveLocalToHost( const CKString & aLocalFile,
                                       const CKString & aRemoteFile )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveLocalToHost(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// first, copy the local file to the remote host
	if (!error) {
		if (!copyLocalToHost(aLocalFile, aRemoteFile)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveLocalToHost(const CKString &, "
				"const CKString &) - we were unable to copy the local file '" <<
				aLocalFile << "' to the remote host as '" << aRemoteFile << "'. "
				"Please make sure that the local file exists and that you have "
				"permissions to put the file on the remote host.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, try to remove the file
	if (!error) {
		if (unlink(aLocalFile.c_str()) == -1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveLocalToHost(const CKString &, "
				"const CKString &) - the copy to the remote system worked, "
				"but the removal of '" << aLocalFile << "' on the local filesystem "
				"failed with the errno=" << errno << " (" << strerror(errno) <<
				"). Please check on this file and manually delete it.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method is a copyHostToLocal() followed by a removal of the
 * remote file via removeFile(). This is a convenient way to ensure
 * that the copy is done before the deletion in situations where
 * there is no need to keep the remote copy.
 *
 * If the file cannot be copied and the remote copy deleted, this
 * method returns false. Otherwise it returns true after copying the
 * file from the remote host and deleting the remote file. It is
 * possible that the file will be copied successfully, and the removal
 * will fail. If this is a significant concern, check for the
 * existance of the file on the local host.
 */
bool CKFTPConnection::moveHostToLocal( const CKString & aRemoteFile,
                                       const CKString & aLocalFile )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveHostToLocal(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// first, copy the remote file to the local host
	if (!error) {
		if (!copyHostToLocal(aRemoteFile, aLocalFile)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveHostToLocal(const CKString &, "
				"const CKString &) - we were unable to copy the remote file '" <<
				aRemoteFile << "' to the local host as '" << aLocalFile << "'. "
				"Please make sure that the remote file exists and that you have "
				"permissions to put the file on the local filesystem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, try to remove the file
	if (!error) {
		if (removeFile(aRemoteFile)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::moveLocalToHost(const CKString &, "
				"const CKString &) - the copy to the local system worked, "
				"but the removal of '" << aRemoteFile << "' on the remote host "
				"failed. Please check on this file and manually delete it.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method removes the file off the remote host. Since there's
 * no 'Recycler' or 'Recycle Bin' in general, this is a very
 * dangerous method, and should not be called without user
 * verification.
 *
 * If the file cannot be removed, this method returns false.
 * Otherwise it returns true after removing the file at the
 * remote host.
 */
bool CKFTPConnection::removeFile( const CKString & aRemoteFile )
{
	bool		error = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::removeFile(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to remove the remote file
	if (!error) {
		if (!isPositiveCompletionReply(doDELE(aRemoteFile))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::removeFile(const CKString &) - while "
				"trying to remove the remote file '" << aRemoteFile << "' from "
				"the host: " << getHostname() << " the FTP command failed and "
				"the file could not be deleted. This could be for any number of "
				"reasons, and you probably need to do a little digging on the "
				"remote host.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/********************************************************
 *
 *           Getting and Comparing File Contents
 *
 ********************************************************/
/*
 * This method loads the file from the remote host at the path
 * specified into an CKString and then returns that
 * object. This is the simplified method of reading in a file
 * on a remote system rather than copying it over to the local
 * host, and then reading it in.
 */
CKString CKFTPConnection::getContents( const CKString & aFilename )
{
	bool			error = false;
	CKString		retval;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getContents(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the data loaded into a string
	if (!error) {
		CKString		type = "I";
		CKString		cmd = CKFTPCommandStringRETR;
		cmd += aFilename;
		try {
			retval = transferData(type, cmd, CKString(""));
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getContents(const CKString &) - while "
				"trying to transfer the data from the FTP server a CKException "
				"was thrown: " << e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


/*
 * This method simply compares two files - one on the local
 * host, and another on the remote host. If they are the same,
 * then it returns true. If there is an error such as one or
 * both of the files doesn't exist, then a standard
 * CKException is raised, and false will be returned.
 */
bool CKFTPConnection::areContentsEqual( const CKString & aLocalFile,
										const CKString & aRemoteFile )
{
	bool		error = false;
	bool		equal = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::areContentsEqual(const CKString &, "
				"const CKString &) - this instance is not logged into a valid "
				"FTP server and so nothing can be done. Please login to the "
				"server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// first, get the contents of the remote file
	CKString		remote;
	if (!error) {
		try {
			remote = getContents(aRemoteFile);
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::areContentsEqual(const CKString &, "
				"const CKString &) - while trying to get the file '" <<
				aRemoteFile << "' from the remote host " << getHostname() <<
				"a CKException was thrown: " << e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, try to get the local contents too
	CKString		local;
	if (!error) {
		try {
			local = getLocalContents(aLocalFile);
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::areContentsEqual(const CKString &, "
				"const CKString &) - while trying to get the file '" <<
				aLocalFile << "' from the local host a CKException was "
				"thrown: " << e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now see if they are equal
	if (!error) {
		if (remote == local) {
			equal = true;
		}
	}

	return (error ? false : equal);
}


/********************************************************
 *
 *              Determine Access to Files
 *
 ********************************************************/
/*
 * This method returns a simple bool indicating if the file exists
 * on the remote host. There is of course a similar routines for
 * local files, and these should be used if advanced
 * error-detection is desired. In other words, don't count on
 * return codes from a  copy telling you the file didn't exist in
 * the first place - check before calling the copy.
 */
bool CKFTPConnection::fileExists( const CKString & aRemoteFile )
{
	bool		error = false;
	bool		exists = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::fileExists(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the details on the file
	if (!error) {
		try {
			CKFilePermissions	p = getFileAttributes(aRemoteFile);
			if (p.type != '\0') {
				exists = true;
			}
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::fileExists(const CKString &) - while "
				"trying to get the details on the remote file '" << aRemoteFile <<
				"' from the FTP server a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return exists;
}


/*
 * On a Unix remote host this method checks to see if the file
 * on the remote host has the 'read' permission set so that
 * subsequent calls like getContents() are successful.
 */
bool CKFTPConnection::isFileReadable( const CKString & aRemoteFile )
{
	bool		error = false;
	bool		readable = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileReadable(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the details on the file
	if (!error) {
		try {
			CKFilePermissions	p = getFileAttributes(aRemoteFile);
			if (p.userReadable || p.groupReadable || p.othersReadable) {
				readable = true;
			}
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileReadable(const CKString &) - while "
				"trying to get the details on the remote file '" << aRemoteFile <<
				"' from the FTP server a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return readable;
}


/*
 * On a Unix remote host this method checks to see if the
 * file or directory on the remote host has the 'write'
 * permission set for this user. It is a good test to check this
 * before a call to createFile() is made.
 */
bool CKFTPConnection::isFileWritable( const CKString & aRemoteFile )
{
	bool		error = false;
	bool		writable = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileWritable(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the details on the file
	if (!error) {
		try {
			CKFilePermissions	p = getFileAttributes(aRemoteFile);
			if (p.userWritable || p.groupWritable || p.othersWritable) {
				writable = true;
			}
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileWritable(const CKString &) - while "
				"trying to get the details on the remote file '" << aRemoteFile <<
				"' from the FTP server a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return writable;
}


/*
 * On a Unix remote host this checks to see if the file on
 * the remote host can be deleted by the user. It's probably
 * a good idea to check this before calling removeFile().
 */
bool CKFTPConnection::isFileDeletable( const CKString & aRemoteFile )
{
	bool		error = false;
	bool		deletable = false;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileDeletable(const CKString &) - this "
				"instance is not logged into a valid FTP server and so nothing "
				"can be done. Please login to the server with a call to "
				"loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's do the FTP work to get the details on the file
	if (!error) {
		try {
			CKFilePermissions	p = getFileAttributes(aRemoteFile);
			if (p.userWritable || p.groupWritable || p.othersWritable) {
				deletable = true;
			}
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isFileDeletable(const CKString &) - while "
				"trying to get the details on the remote file '" << aRemoteFile <<
				"' from the FTP server a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return deletable;
}


/********************************************************
 *
 *              Getting and Setting Attributes
 *
 ********************************************************/
/*
 * On some remote hosts this method sets the file permissions
 * provided in the set 'aSet' for the file in 'aFile'.
 * This is useful for setting the permissions on a file, and
 * might fail if the user doesn't have permission to change
 * the permissions on the file, or if the remote host does
 * not support the FTP SITE command.
 *
 * If everything goes well, this method returns true. If there are
 * any problems in processing it returns false.
 */
bool CKFTPConnection::setFileAttributes( const CKString & aFile,
                                         const CKFilePermissions & aSet )
{
	bool				error = false;

	// See if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::setFileAttributes(const CKString &, "
				"const CKFileAttributes &) - this instance is not logged into "
				"a valid FTP server and so nothing can be done. Please login to "
				"the server with a call to loginToHost() before calling this "
				"method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now let's build up the command to do the job for us, and then send
	 * it off to the remote host
	 */
	if (!error) {
		CKString	cmd = "CHMOD ";
		cmd += permissionsToNumber(aSet);
		cmd += " ";
		cmd += aFile;
		if (!isPositiveCompletionReply(doSITE(cmd))) {
			error = true;
			std::ostringstream	msg;
			if (isPermanentNegativeReply(getState()) &&
				isSyntaxReply(getState())) {
				msg << "CKFTPConnection::getContents(const CKString &) - the "
					"remote host " << getHostname() << " does not seem to support "
					"the SITE command in it's FTP server. That means that we cannot "
					"change the file permissions on this server.";
			} else {
				msg << "CKFTPConnection::getContents(const CKString &) - while "
					"trying to change the file permissions on the file '" <<
					aFile << "' on the server " << getHostname() << " an "
					"error occurred and the change could not be done. Please "
					"check into the file and see what might have caused the "
					"problem.";
			}
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * On a Unix remote host this method returns a CKFilePermissions
 * structure whose contents have been set by the file permissions
 * of the passed-in remote file.
 *
 * If there is any error in the processing, or the file
 * 'aFile' is not located on the host, a CKException is returned.
 */
CKFilePermissions CKFTPConnection::getFileAttributes( const CKString & aFile )
{
	bool				error = false;
	CKFilePermissions	retval;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getFileAttributes(const CKString &) - "
				"this instance is not logged into a valid FTP server and so "
				"nothing can be done. Please login to the server with a call "
				"to loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// get the raw data from the remote host for this file
	CKString		rawDirList;
	if (!error) {
		CKString		type = "A";
		CKString		cmd = CKFTPCommandStringLIST;
		cmd += aFile;
		try {
			rawDirList = transferData(type, cmd, CKString(""));
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getFileAttributes(const CKString &) - "
				"while trying to get the file listing from the FTP server for "
				"the file '" << aFile << "' a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Next, we need to break up the response into lines separated by line
	 * endings. The first part of this is to get the right line endings
	 * for the type of FTP server that sent the data, and then we need to
	 * use this line ending to chop up the response into lines.
	 */
	CKStringList	dirList;
	if (!error) {
		// assume that the FTP server sent CRLF line endings
		CKString		eol = "\r\n";
		if (rawDirList.find(eol) == -1) {
			// nope... then it's going to be just the NEWLINE character
			eol = "\n";
		}

		// now we need to chop up the response into lines
		dirList = parseIntoChunks(rawDirList, eol);

		/*
		 * Now let's go through all the lines of the response and remove all
		 * that are empty lines. Chances are that there will be at least one
		 * of them.
		 */
		for (CKStringNode *i = dirList.getHead(); i != NULL; ) {
			if (((CKString *)i)->length() == 0) {
				// got it! let's erase it
				dirList.erase(i);
			} else {
				// check the next one
				i = i->getNext();
			}
		}

		/*
		 * Now we need to look at the vector and parse the line for the
		 * file at 'path' and read out the pemissions. If the file exists
		 * it should be dirList[0], but if the vector is empty then there
		 * was no file matching the 'path'
		 */
		if (dirList.size() == 0) {
			/*
			 * This is an error, but don't raise a debug exception,
			 * it's a normal "file not found" condition
			 */
			error = true;
		}
	}

	/*
	 * Now get the permissions out of the first array element
	 * Note: this *ONLY* works for Unix-like remote hosts
	 */
	if (!error) {
		const char *fileInfo 	= dirList[0].c_str();
		char dirChar			= fileInfo[0];
		char userReadChar		= fileInfo[1];
		char userWriteChar		= fileInfo[2];
		char userExecuteChar	= fileInfo[3];
		char groupReadChar		= fileInfo[4];
		char groupWriteChar		= fileInfo[5];
		char groupExecuteChar	= fileInfo[6];
		char othersReadChar		= fileInfo[7];
		char othersWriteChar	= fileInfo[8];
		char othersExecuteChar	= fileInfo[9];

		// check to see that it was a valid permission string
		if ((fileInfo == NULL) ||
			(!((dirChar == '-') || (dirChar == 'd') || (dirChar == 'l'))) ||
			(!((userReadChar == '-')    || (userReadChar == 'r')    || (userReadChar == 's'))) ||
			(!((userWriteChar == '-')   || (userWriteChar == 'w')   || (userWriteChar == 's'))) ||
			(!((userExecuteChar == '-') || (userExecuteChar == 'x') || (userExecuteChar == 's'))) ||
			(!((groupReadChar == '-')    || (groupReadChar == 'r')    || (groupReadChar == 's'))) ||
			(!((groupWriteChar == '-')   || (groupWriteChar == 'w')   || (groupWriteChar == 's'))) ||
			(!((groupExecuteChar == '-') || (groupExecuteChar == 'x') || (groupExecuteChar == 's'))) ||
			(!((othersReadChar == '-')    || (othersReadChar == 'r')    || (othersReadChar == 's'))) ||
			(!((othersWriteChar == '-')   || (othersWriteChar == 'w')   || (othersWriteChar == 's'))) ||
			(!((othersExecuteChar == '-') || (othersExecuteChar == 'x') || (othersExecuteChar == 's'))) ) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getFileAttributes(const CKString &) - "
				"while trying to parse the file permissions from the returned "
				"data: '" << dirList[0] << "' we ran into problems because it's "
				"not a format that we understand. Please inform the developers.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// this is 'd' for a directory, 'l' for a link, etc.
			retval.type = dirChar;
			// these are the user's permissions
			retval.userReadable = !(userReadChar == '-');
			retval.userWritable = !(userWriteChar == '-');
			retval.userExecutable = !(userExecuteChar == '-');
			// these are the group permissions
			retval.groupReadable = !(groupReadChar == '-');
			retval.groupWritable = !(groupWriteChar == '-');
			retval.groupExecutable = !(groupExecuteChar == '-');
			// these are the world permissions
			retval.othersReadable = !(othersReadChar == '-');
			retval.othersWritable = !(othersWriteChar == '-');
			retval.othersExecutable = !(othersExecuteChar == '-');
		}
	}

	return retval;
}


/********************************************************
 *
 *              Discovering Directory Contents
 *
 ********************************************************/
/*
 * This method returns a CKStringList of the directories
 * and files contained in the directory specified on the remote
 * host. This does not distinguish between symbolic links, files
 * or directories, and only includes the files in that directory -
 * not any subdirectories of that directory.
 *
 * If there is any problem in creating the directory list, an empty
 * list is returned, and if necessary, a CKException is thrown.
 */
CKStringList CKFTPConnection::getDirectoryContents( const CKString & aDir )
{
	bool			error = false;
	CKStringList	retval;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getDirectoryContents(const CKString &) - "
				"this instance is not logged into a valid FTP server and so "
				"nothing can be done. Please login to the server with a call "
				"to loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// get the raw data from the remote host for this file
	CKString		rawDirList;
	if (!error) {
		CKString		type = "A";
		CKString		cmd = CKFTPCommandStringNLST;
		cmd += aDir;
		try {
			rawDirList = transferData(type, cmd, CKString(""));
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getDirectoryContents(const CKString &) - "
				"while trying to get the directory listing from the FTP server for "
				"the directory '" << aDir << "' a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Next, we need to break up the response into lines separated by line
	 * endings. The first part of this is to get the right line endings
	 * for the type of FTP server that sent the data, and then we need to
	 * use this line ending to chop up the response into lines.
	 */
	CKStringList	dirList;
	if (!error) {
		// assume that the FTP server sent CRLF line endings
		CKString		eol = "\r\n";
		if (rawDirList.find(eol) == -1) {
			// nope... then it's going to be just the NEWLINE character
			eol = "\n";
		}

		// now we need to chop up the response into lines
		dirList = parseIntoChunks(rawDirList, eol);

		/*
		 * Now let's go through all the lines of the response and remove all
		 * that are empty lines. Chances are that there will be at least one
		 * of them.
		 */
		for (CKStringNode *i = dirList.getHead(); i != NULL; ) {
			if (((CKString *)i)->length() == 0) {
				// got it! let's erase it
				dirList.erase(i);
			} else {
				// check the next one
				i = i->getNext();
			}
		}
	}

	/*
	 * On some systems, the returned directory list includes the fully
	 * qualified path name. If so, we need to strip this off each
	 * entry because this method is supposed to return a list of just
	 * the names, not the fully qualified path names.
	 */
	if (!error && (dirList.size() > 0)) {
		int				dirSize = aDir.size();
		CKStringNode	*i = NULL;
		for (i = dirList.getHead(); i != NULL; i = i->getNext()) {
			if (i->left(dirSize) == aDir) {
				i->erase(0, dirSize);
			}
		}
	}

	return retval;
}


/*
 * This method returns a CKStringList of the entire
 * tree structure in the passed-in directory, and below.
 * Again, in a manner similar to the OPENSTEP NSFileManager,
 * this method can be used to search an entire directory tree
 * structure to find a particular file, or directory.
 *
 * If there is any problem in creating the directory list, an empty
 * list is returned, otherwise, a CKStringList is returned.
 */
CKStringList CKFTPConnection::getSubpathsAtPath( const CKString & aDir )
{
	bool			error = false;
	CKStringList	retval;

	// see if we are logged into the host
	if (!error) {
		if (!isLoggedIn()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getSubpathsAtPath(const CKString &) - "
				"this instance is not logged into a valid FTP server and so "
				"nothing can be done. Please login to the server with a call "
				"to loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// get the current directory so that we can return here when done
	CKString		startingDir;
	if (!error) {
		try {
			startingDir = currentDirectoryPath();
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getSubpathsAtPath(const CKString &) - "
				"while trying to get the current directory from the FTP server "
				"a CKException was thrown: " << e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// get the root data from the remote host for this file
	CKStringList		rootDirList;
	if (!error) {
		try {
			rootDirList = getDirectoryContents(aDir);
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getSubpathsAtPath(const CKString &) - "
				"while trying to get the directory listing from the FTP server for "
				"the directory '" << aDir << "' a CKException was thrown: " <<
				e.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * For each element in the root directory list, let's try to change
	 * directory to it. If it's not possible, then it's a file and add it
	 * to the final results as such. However, if we can 'cd' to it, then
	 * we need to look at it's contents recursively. It's nice and complete
	 * if a littls slow at times.
	 */
	if (!error && (rootDirList.size() > 0)) {
		CKStringNode		*i = NULL;
		for (i = rootDirList.getHead(); i != NULL; i = i->getNext()) {
			// create the complete path for this directory entry
			CKString		completePath = aDir;
			completePath += "/";
			completePath += (*i);

			// now see if we can 'cd' to the directory entry
			try {
				if (isPositiveCompletionReply(doCWD(completePath))) {
					// looks like a directory, get the contents
					try {
						CKStringList	sub = getSubpathsAtPath(completePath);
						CKStringNode	*j = NULL;
						for (j = sub.getHead(); j != NULL; j = j->getNext()) {
							// create the new directory entry
							CKString	newbie = completePath;
							newbie += "/";
							newbie += (*j);
							// ...and add it to the return vector
							retval.addToEnd(newbie);
						}
					} catch (CKException & e2) {
						error = true;
						std::ostringstream	msg;
						msg << "CKFTPConnection::getSubpathsAtPath(const CKString &)"
							" - while trying to get the sub-directory listing from the "
							"FTP server for the directory '" << completePath <<
							"' a CKException was thrown: " << e2.getMessage();
						throw CKException(__FILE__, __LINE__, msg.str());
					}
				} else {
					// just a file, so add it to the list as-is
					retval.addToEnd(*i);
				}
			} catch (CKException & e1) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFTPConnection::getSubpathsAtPath(const CKString &)"
					" - while trying to change directory to the sub-directory "
					"'" << completePath << "' a CKException was thrown: "
					<< e1.getMessage();
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	/*
	 * If we're OK, then change back to where we originally started.
	 */
	if (!error) {
		changeCurrentDirectoryPath(startingDir);
	}

	return retval;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKFTPConnections are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKFTPConnection::operator==( const CKFTPConnection & anOther ) const
{
	bool		equal = true;

	if ((getHostname() != anOther.getHostname()) ||
		(getUsername() != anOther.getUsername()) ||
		(getPassword() != anOther.getPassword()) ||
		(getState() != anOther.getState()) ||
		(getControlPort() != anOther.getControlPort()) ||
		(getServerReplyLines() != anOther.getServerReplyLines()) ||
		(isLoggedIn() != anOther.isLoggedIn()) ||
		(getIncomingDataTimeout() != anOther.getIncomingDataTimeout())) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKFTPConnections are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKFTPConnection::operator!=( const CKFTPConnection & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKFTPConnection::toString() const
{
	CKString		retval = "< Host=";
	retval += getHostname();
	retval += ", ";
	retval += " Username=";
	retval += getUsername();
	retval += ", ";
	retval += " Password=";
	retval += "<hidden>";
	retval += ", ";
	retval += " State=";
	retval += getState();
	retval += ", ";
	retval += " ControlPort=";
	retval += getControlPort().toString();
	retval += ", ";
	retval += " IncomingDataTimeout=";
	retval += getIncomingDataTimeout();
	retval += ", ";
	retval += " Logged in:";
	retval += (isLoggedIn() ? "Yes" : "No");
	retval += ">\n";

	return retval;
}


/*
 * This method sets the set of server reply lines for a given
 * control port message to the same contents as the supplied
 * list. A copy is made so you don't have to worry about who
 * owns the argument - the caller does.
 */
void CKFTPConnection::setServerReplyLines( const CKStringList & aList )
{
	mServerReplyLines = aList;
}


/*
 * This method sets the flag when we get logged into the FTP server
 * on the remote host, and when we logoff. This is useful because
 * the user can use this flag to see if they are safe in issuing
 * commands.
 */
void CKFTPConnection::setIsLoggedIn( bool aFlag )
{
	mIsLoggedIn = aFlag;
}


/*
 * This protected method simply returns the value of the currently
 * active password. This is protected because it really shouldn't
 * be used regularly, as it represents a security risk, but must
 * be available for the connection to be initiated.
 *
 * It's also worth noting that as soon as a successful login is
 * completed, the username and password are locked so that they
 * cannot be changed during a connection.
 */
CKString CKFTPConnection::getPassword() const
{
	return mPassword;
}


/*
 * This method returns a copy of the vector of server reply lines
 * that have been returned from the server in response to a control
 * port command. This is useful as there's a lot of useful data in
 * the responses and many times we need to mine it.
 */
CKStringList CKFTPConnection::getServerReplyLines() const
{
	return mServerReplyLines;
}


/*
 * This method clears out all the response lines from the FTP server
 * in preparation for adding in new lines to the vector as the
 * response from another command.
 */
void CKFTPConnection::clearServerReplyLines()
{
	mServerReplyLines.clear();
}


/*
 * This method adds a response line from the FTP server to the end
 * of the vector of reply lines for this FTP command. This is nice
 * because the user doesn't have to deal with the ivar, just the
 * method.
 */
void CKFTPConnection::addToServerReplyLines( const CKString & aLine )
{
	mServerReplyLines.addToEnd(aLine);
}


/********************************************************
 *
 *          FTP Return Code Classification Methods
 *
 ********************************************************/
/*
 * This method returns true if the passed in FTP return code
 * is of the Positive Preliminary group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isPositivePreliminaryReply( int aCode )
{
	return ((aCode/100) == CKFTPPositivePreliminaryGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Positive Completion group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isPositiveCompletionReply( int aCode )
{
	return ((aCode/100) == CKFTPPositiveCompletionGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Positive Intermediate group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isPositiveIntermediateReply( int aCode )
{
	return ((aCode/100) == CKFTPPositiveIntermediateGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Transient Negative group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isTransientNegativeReply( int aCode )
{
	return ((aCode/100) == CKFTPTransientNegativeGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Permanent Negative group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isPermanentNegativeReply( int aCode )
{
	return ((aCode/100) == CKFTPPermanentNegativeGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Syntax group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isSyntaxReply( int aCode )
{
	return ((aCode - (aCode/100)*100)/10 == CKFTPSyntaxGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Information group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isInformationReply( int aCode )
{
	return ((aCode - (aCode/100)*100)/10 == CKFTPInformationGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Connections group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isConnectionsReply( int aCode )
{
	return ((aCode - (aCode/100)*100)/10 == CKFTPConnectionsGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the Authentication and Auditing group. The
 * result code groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isAuthenticationReply( int aCode )
{
	return ((aCode - (aCode/100)*100)/10 == CKFTPAuthenticationGroup);
}


/*
 * This method returns true if the passed in FTP return code
 * is of the File System group. The result code
 * groupings are defined in the RFC-959 document.
 */
bool CKFTPConnection::isFileSystemReply( int aCode )
{
	return ((aCode - (aCode/100)*100)/10 == CKFTPFileSystemGroup);
}


/********************************************************
 *
 *          Generic FTP Command Methods
 *
 ********************************************************/
/*
 * This method does all the checking and building to make each
 * of the executions of an FTP command with the remote host a
 * reliable, and verifyable event. This is called by each of
 * the other FTP commands, and returns the corresponding return
 * code returned from the remote host.
 *
 * While it is possible for a user to directly call this
 * method, it is not advised, and in fact, discouraged. The
 * FTP functions are all available and waiting without
 * circumventing the designed flow.
 */
int CKFTPConnection::executeCommand( const CKString & aCmd )
{
	bool		error = false;
	int			retval = CKFTPUnimplementedFunction;

	// see if the control port is connected to anything
	if (!error) {
		if (!mControlPort.isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::executeCommand(const CKString &) - "
				"this instance's control port is not connected to a valid FTP "
				"server and so nothing can be done. Please connect to the server "
				"and then login with a call to loginToHost() before calling "
				"this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// send the command out the control port
	if (!error) {
		CKString		cmd = aCmd;
		cmd += "\r\n";
		if (!mControlPort.send(cmd)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::executeCommand(const CKString &) - "
				"the command '" << aCmd << "' could not successfully be sent to "
				"the FTP server at " << getHostname() << ". This is a serious "
				"problem and could indicate that the server is down.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// try to get the return code from the reply
	if (!error) {
		retval = getReply();
		if (retval <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::executeCommand(const CKString &) - "
				"the reply code for the command '" << aCmd << "' was " <<
				retval << " and that's an illegal value for the return code. "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


/*
 * This executes the standard FTP 'ABOR' command on the command
 * channel to the remote hosts. This will abort the transfer or
 * command in process.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doABOR()
{
	return executeCommand(CKFTPCommandStringABOR);
}


/*
 * This executes the standard FTP 'APPE' command on the command
 * channel to the remote host. This command takes the upcoming
 * data stream transfered from the local host to the remote host
 * and saves it to the remote host in the current directory on
 * that host in a file named 'arg'. If the file already exists,
 * then this transfered file will be appended to the existing
 * file on the host.
 *
 * Traditional implementations of FTP use this same name (arg)
 * as the source of the data stream to send from the local
 * machine to the remote host.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doAPPE( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringAPPE;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'CDUP' command on the command
 * channel to the remote host. This changes the current directory
 * on that host to it's parent directory. This is equivalent to
 * a 'cd ..' on the Unix command-line.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doCDUP()
{
	return executeCommand(CKFTPCommandStringCDUP);
}


/*
 * This executes the standard FTP 'CWD' command on the command
 * channel to the remote host. This attempts to set the current
 * working directory on the remote host to the argument provided.
 * This is very important because standard FTP transfers must
 * go through the current directory on the remote host.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doCWD( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringCWD;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'DELE' command on the command
 * channel to the remote host. This deletes the file on the
 * remote host whose name is passed into this method. Since
 * there is no 'undo' feature in FTP, this is a very powerful
 * command and should only be used after proper confirmation
 * is obtained.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doDELE( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringDELE;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 *	This executes the standard FTP 'LIST' command on the command
 *	channel to the remote host. This is a general directory listing
 *	command that unfortunately generates a very human-readable
 *	format, that is not very machine-readable. Differences between
 *	operating systems and FTP server implementations make for a
 *	'natural' directory listing for the given platform.
 *
 *	Of interest, though, are the three ways this method can be
 *	called: with an empty aregument, with the name of a file (or
 *	wildcard expression of files), and with the name of a directory.
 *	If the empty is passed in, the currect directory on the remote
 *	host is used. In all cases, the 'target' of the command is
 *	presented with information about file size, etc. that a user
 *	would expect to see on that system were they logged in.
 *
 *	As has been said before, this is not a common way to determine
 *	the contents of a directory, or gain information about a file.
 *	It is; however, a very good way of presenting this kind of
 *	information to a user.
 *
 *	It returns the FTP return code for the execution.
 */
int CKFTPConnection::doLIST( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringLIST;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'MKD' command on the command
 * channel to the remote host. This command attempts to create
 * the directory passed in as an argument on the remote host.
 * It is important to note that the current directory on the
 * remote host is assumed for the execution of this command. This
 * makes creating directories simplified because the complete
 * path does not have to be included each time.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doMKD( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringMKD;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'MODE' command on the command
 * channel to the remote host. This command is directed at the
 * two running FTP sessions on each machine. It specifies the
 * data transfer mode for subsequent transfers and can be one
 * of three possible values: S - stream mode, and the default;
 * B - block mode; or C - compressed mode.
 *
 * This method does not have to be called on each transfer
 * session because the default (stream) is the most common in
 * today's systems, and this command is only here for access to
 * legacy mainframes that required block mode data transfer.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doMODE( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringMODE;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'NLST' command on the command
 * channel to the remote host. This is the machine-readable
 * way to determine the contents of a directory or a series of
 * file(s) specified by a wildcard. Every system returns the
 * exact same format: one filename per line, each terminated
 * by a CRLF.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doNLST( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringNLST;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'NOOP' command on the command
 * channel to the remote host. This is a simple "no operation"
 * that does nothing.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doNOOP()
{
	return executeCommand(CKFTPCommandStringNOOP);
}


/*
 * This executes the standard FTP 'PASS' command on the command
 * channel to the remote host. This command is the second part
 * of the login procedure - the first being doUSER(). The
 * response from the doUSER() method should be an intermediate
 * success, waiting on the successful password for authentication.
 * This is a two-step process by design for the FTP protocol.
 * This class has convenience functions that make this a one-step
 * process for convenience.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doPASS( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringPASS;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'PORT' command on the command
 * channel to the remote host. This command sets the socket-level
 * port for the upcoming data transfer. This command takes six
 * (6) comma spearated numbers each of which represents an 8-bit
 * number in base-10 ASCII representation. The first four numbers
 * are the 32-bit internet host address, and the last two represent
 * a 16-bit TCP port number.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doPORT( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringPORT;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'PWD' command on the command
 * channel to the remote host. This returns the current working
 * directory on the remote host. It returns it in the reply to
 * the command, in a similar manner to all the other informational
 * based commands.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doPWD()
{
	return executeCommand(CKFTPCommandStringPWD);
}


/*
 *	This executes the standard FTP 'QUIT' command on the command
 *	channel to the remote host. This command ends the FTP session
 *	with the remote host.\n
 *
 *	It returns the FTP return code for the execution.
 */
int CKFTPConnection::doQUIT()
{
	return executeCommand(CKFTPCommandStringQUIT);
}


/*
 * This executes the standard FTP 'RETR' command on the command
 * channel to the remote host. This command retrieves the passed
 * file name from the remote in the current working directory on
 * the remote host, and places it in the data stream for sending
 * to the local machine.
 *
 * In a traditional FTP implementation, the file name 'arg' is
 * used as the destination of the data stream from the remote
 * host.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doRETR( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringRETR;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'RMD' command on the command
 * channel to the remote host. This is a powerful command that
 * removes the directory on the remote host and all files
 * and directories under that directory specified by the passed
 * value. Since no warning will be given by this command it
 * should not be called without first obtaining confirmation of
 * the deletion.
 *
 * On a Unix system, this is similar to an 'rm -rf arg' and
 * can be quite destructive indeed.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doRMD( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringRMD;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'RNFR' command on the command
 * channel to the remote host. This command is the first part of
 * a two-part command to rename a file on the remote host. First,
 * this command must be executed to specify the from filename.
 * If the file exists and can be renamed by the user creating
 * the FTP session, it will return an intermediate success code.
 * After this, doRNTO() must be called to specify the to
 * filename for the move. After this command the move will occur
 * or not, and a terminal return code will be returned.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doRNFR( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringRNFR;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'RNTO' command on the command
 * channel to the remote host. This command is the second part of
 * a two-part command to rename a file on the remote host. First,
 * doRNFR() must be executed to specify the from filename.
 * If the file exists and can be renamed by the user creating
 * the FTP session, it will return an intermediate success code.
 * Next this command must be called to specify the to
 * filename for the move. After this command the move will occur
 * or not, and a terminal return code will be returned.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doRNTO( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringRNTO;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'SITE' command on the command
 * channel to the remote host. This command allows for platform-
 * specific commands to be executed on the remote host. These
 * "extensions" to the standard FTP command set are not quaranteed
 * but several are very common on certain operating systems.
 *
 * For example, one of the common Unix extensions is the
 * 'CHMOD <mode> <file>' to change file permissions on the remote
 * host.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doSITE( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringSITE;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'STAT' command on the command
 * channel to the remote host. During a transfer and with an empty
 * argument, this returns the status of the transfer in progress.
 * This can be very useful in monitoring the progress of the
 * transfer.
 *
 * If not done during a transfer, or where 'arg' is either a file
 * or directory, the output is similar to doLIST() and a
 * human-readable format of a directory listing is done.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doSTAT( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringSTAT;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'STOR' command on the command
 * channel to the remote host. This command takes the upcoming
 * data stream transfered from the local host to the remote host
 * and saves it to the remote host in the current directory on
 * that host in a file named 'arg'. If a file already
 * exists on the remote host, in that directory, with that name,
 * it is deleted and the new file is put in its place. Since no
 * warnings of any kind are given regarding the overwriting, it
 * is good procedure to check to see if the file already exists
 * and rename it, if it is still needed.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doSTOR( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringSTOR;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 *	This executes the standard FTP 'STOU' command on the command
 *	channel to the remote host. This command is similar to
 *	doSTOR() but the name of the file that will contain the
 *	data stream is generated by the remote host and is guaranteed
 *	unique in the current directory on that host. The name is
 *	returned to the local machine in the response to this
 *	command.
 *
 *	It returns the FTP return code for the execution.
 */
int CKFTPConnection::doSTOU()
{
	return executeCommand(CKFTPCommandStringSTOU);
}


/*
 * This executes the standard FTP 'STRU' command on the command
 * channel to the remote host. This %structure command is used
 * to specify the type of transfer that will be used on subsequent
 * data transfers. The possible arguments to this routine are:
 * F - file (the default); R - record; and P - page. For virtually
 * all transfers, the default (F) will suffice. The only reason to
 * use another type is if the remote host is not a file-based
 * system, and instead, works with pages or database records.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doSTRU( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringSTRU;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'TYPE' command on the command
 * channel to the remote host. This command sets up the type of
 * the upcoming data trasnfers. There are many different
 * arguments to this command for specifying transfer types, but
 * the most commonly used ones are:
 *
 * "A" - signifying that the transfer will be straight ASCII
 * "A N" - signifying that the transfer will be ASCII (A) non-
 *         printing (N) transfers.
 * "I" - signifying that image (I) a.k.a. binary transfers are
 *       coming.
 * "L 8" - signifying that the local (L) byte size is 8 (8)
 *         bits long.
 *
 * All this is important because FTP was meant to work across
 * vastly diffeent operating systems - some not even based on
 * ASCII. This meant that the individual FTP implementations
 * had to convert their internal format for text to a standard
 * format for transfer.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doTYPE( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringTYPE;
	cmd += arg;
	return executeCommand(cmd);
}


/*
 * This executes the standard FTP 'USER' command on the command
 * channel to the remote host. This is the first part of the
 * two-part user authentication scheme for the FTP protocol.
 * The response from this command should be an intermediate
 * success code that must be followed immediately by a
 * doPASS() command to send the accompanying password for
 * this user account. The combination of these two commands
 * should yield either a successful login, or an unsuccessful
 * one with a somewhat descriptive return code.
 *
 * It returns the FTP return code for the execution.
 */
int CKFTPConnection::doUSER( const CKString & arg )
{
	CKString		cmd = CKFTPCommandStringUSER;
	cmd += arg;
	return executeCommand(cmd);
}


/********************************************************
 *
 *       Convenience Methods for Compound Commands
 *
 ********************************************************/
/*
 * This method executes the two FTP commands doUSER() and
 * doPASS() to attempt to successfully log in the user that
 * is passed in. Any response from the doUSER() other than
 * "331 - User name OK; need password." will be considered an
 * error and returned immediately.
 *
 * If the return code from doUSER() is 331, then the method
 * doPASS() is called, and it's return code it returned.
 */
int CKFTPConnection::ftpLoginUsername( const CKString & aUser,
									   const CKString & aPassword )
{
	bool	error = false;
	int		returnCode;

    // Try to send the user name
	if (!error) {
		returnCode = doUSER(aUser);
		if (returnCode != CKFTPUserOKNeedPassword) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::ftpLoginUsername(const CKString &, "
				"const CKString &) - the username '" << aUser << "' was rejected "
				"by the FTP server on " << getHostname() << ". This could be a wrong "
				"user or the server could be in trouble. Please check into this as "
				"soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Send the password, if needed
	if (!error) {
		returnCode = doPASS(aPassword);
	}

	// return whatever we have at this point
	return returnCode;
}


/*
 * This method executes the two FTP commands doRNFR() and
 * doRNTO() to attempt to rename the file on the remote
 * host. Any response from doRNTO() other than "350 -
 * Requested file action pending further information" will be
 * considered an error and returned immediately.
 *
 * If the return code from doRNFR() is 350, then the method
 * doRNTO() is called, and it's return code it returned.
 */
int CKFTPConnection::ftpRename( const CKString & aFromFile,
								const CKString & aToFile )
{
	bool	error = false;
	int		returnCode;

    // Try to send the 'rename from' command
	if (!error) {
		returnCode = doRNFR(aFromFile);
		if (returnCode != CKFTPFileActionPendingFurtherInfo) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::ftpRename(const CKString &, "
				"const CKString &) - the existing file '" << aFromFile <<
				"' seems to be invalid with the FTP server on " << getHostname() <<
				". This could be a missing file or the server could be in trouble. "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// send the 'to name' command
	if (!error) {
		returnCode = doRNTO(aToFile);
	}

	// return whatever we have at this point
	return returnCode;
}


/********************************************************
 *
 *             Generic FTP Transfer Method
 *
 ********************************************************/
/*
 * This is a generic transfer method for FTP transfers. It can be
 * used to transfer data to the host if 'aData' is non-empty and
 * the 'aCmd' is one that transfers data to the remote host, or it
 * can be used to transfer data from the remote host if 'aData' is
 * empty and the 'aCmd' is one that transfers data from the remote
 * host.
 *
 * This is a very generic routine that is probably best left alone
 * and the other interfaces to data trasnfer in this class used.
 *
 * On a transfer to the remote host, if the transfer is successful,
 * the method returns an CKString with nothing in it. If it isn't
 * successful, it throws a CKException.
 */
CKString CKFTPConnection::transferData( const CKString & aType,
										const CKString & aCmd,
										const CKString & aData )
{
	bool			error = false;
	CKString		retval;

	// First, let's make sure the transfer mode is right for this
	if (!error) {
		if (!isPositiveCompletionReply(doTYPE(aType))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - the transfer mode "
				"could not be set to '" << aType << "' for the upcoming transfer. "
				"This is probably a problem with the connection to the server. "
				"Error code:" << getState() << " " << stringForLastFTPReturnCode();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Get a new listener socket for the reply
	CKSocket	listenerSocket;
	if (!error) {
		listenerSocket = CKSocket(SOCK_STREAM, IPPROTO_TCP);
		if (!listenerSocket.isActivelyListening()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - a listener socket "
				"- necessary for the transfer from the host, could not be created. "
				"This is a serious resource issue.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * We need to send the remote host the PORT command that tells
	 * it on what port to connect back to us to transfer the data.
	 * This is accomplished in several stages: first, we need to
	 * determine parameters from the CKSocket, and then format
	 * them, and finally ship them to the remote host.
	 */
	/*
	 * Since the listener is on this machine, we need to get the
	 * name of the current machine...
	 */
	char		hostname[MAXHOSTNAMELEN + 1];
	if (!error) {
		if (gethostname(hostname, MAXHOSTNAMELEN) < 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - the hostname of the "
				"current machine could not be obtained. This likely indicates "
				"trouble at the operating system level.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now get the network address for this host
	 */
	struct in_addr		hostAddr;
	if (!error) {
#ifdef __linux__
		struct hostent hostInfo;
		struct hostent *info = NULL;
		char buff[4096];
		int h_error;
		if (gethostbyname_r(hostname, &hostInfo, buff, 4096, &info, &h_error) != 0) {
			/*
			 * We weren't successful, most likely because the name was a
			 * numerical IP address of the form "a.b.c.d". Take this IP
			 * address and try to convert it directly.
			 */
			unsigned long tmpIP = inet_addr(hostname);
			if (tmpIP == (unsigned long)(-1)) {
				error = true;
				hostAddr.s_addr = 0x0;
				std::ostringstream	msg;
				msg << "CKFTPConnection::transferData(const CKString &, "
					"const CKString &, const CKString &) - the IP address "
					"for the host: '" << hostname << "' could not be located. "
					"Please check the DNS entries for proper host name.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// got something, so keep it
				hostAddr.s_addr = tmpIP;
			}
		} else {
			/*
			 * We were successful at getting the host information
			 * structure and now just need to extract the numerical
			 * IP address from the structure. This is a very poor
			 * extraction technique, but at least it is cross-
			 * platform.
			 */
			memcpy(&hostAddr, hostInfo.h_addr, hostInfo.h_length);
		}
#endif
#ifdef __sun__
		struct hostent hostInfo;
		char buff[4096];
		int h_error;
		struct hostent *info = gethostbyname_r(hostname, &hostInfo, buff, 4096, &h_error);
		if (info == NULL) {
			/*
			 * We weren't successful, most likely because the name was a
			 * numerical IP address of the form "a.b.c.d". Take this IP
			 * address and try to convert it directly.
			 */
			unsigned long tmpIP = inet_addr(hostname);
			if (tmpIP == (unsigned long)(-1)) {
				error = true;
				hostAddr.s_addr = 0x0;
				std::ostringstream	msg;
				msg << "CKFTPConnection::transferData(const CKString &, "
					"const CKString &, const CKString &) - the IP address "
					"for the host: '" << hostname << "' could not be located. "
					"Please check the DNS entries for proper host name.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// got something, so keep it
				hostAddr.s_addr = tmpIP;
			}
		} else {
			/*
			 * We were successful at getting the host information
			 * structure and now just need to extract the numerical
			 * IP address from the structure. This is a very poor
			 * extraction technique, but at least it is cross-
			 * platform.
			 */
			memcpy(&hostAddr, hostInfo.h_addr, hostInfo.h_length);
		}
#endif
#ifdef __MACH__
		struct hostent *hostInfo = gethostbyname(hostname);
		if (hostInfo == NULL) {
			/*
			 * We weren't successful, most likely because the name was a
			 * numerical IP address of the form "a.b.c.d". Take this IP
			 * address and try to convert it directly.
			 */
			unsigned long tmpIP = inet_addr(hostname);
			if (tmpIP == (unsigned long)(-1)) {
				error = true;
				hostAddr.s_addr = 0x0;
				std::ostringstream	msg;
				msg << "CKFTPConnection::transferData(const CKString &, "
					"const CKString &, const CKString &) - the IP address "
					"for the host: '" << hostname << "' could not be located. "
					"Please check the DNS entries for proper host name.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// got something, so keep it
				hostAddr.s_addr = tmpIP;
			}
		} else {
			/*
			 * We were successful at getting the host information
			 * structure and now just need to extract the numerical
			 * IP address from the structure. This is a very poor
			 * extraction technique, but at least it is cross-
			 * platform.
			 */
			memcpy(&hostAddr, hostInfo->h_addr, hostInfo->h_length);
		}
#endif
	}

	// Finally, format the data for the PORT command and send it
	if (!error) {
		unsigned int	listenerSocketAddr[4];
		unsigned int	listenerSocketPort[2];
		char			buff[256];
		char			*aPtr = (char *)&hostAddr.s_addr;

		// copy over the first four 8-bit characters for the addr
		listenerSocketAddr[0] = ((unsigned int)aPtr[0]) & 0x00ff;
		listenerSocketAddr[1] = ((unsigned int)aPtr[1]) & 0x00ff;
		listenerSocketAddr[2] = ((unsigned int)aPtr[2]) & 0x00ff;
		listenerSocketAddr[3] = ((unsigned int)aPtr[3]) & 0x00ff;

		// copy over the first two 8-bit characters for the port
		listenerSocketPort[0] = (listenerSocket.getPort() & 0xff00) >> 8;
		listenerSocketPort[1] = listenerSocket.getPort() & 0x00ff;

		// Next, we need to build up the PORT arguments
		if (snprintf(buff, 255, "%d,%d,%d,%d,%d,%d",
					listenerSocketAddr[0], listenerSocketAddr[1],
					listenerSocketAddr[2], listenerSocketAddr[3],
					listenerSocketPort[0], listenerSocketPort[1]) < 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - the formatted "
				"argument to the PORT FTP command could not successfully be "
				"generated. Please alert the developers of this problem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// ...now send it out to the remote host
			if (!isPositiveCompletionReply(doPORT(CKString(buff)))) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFTPConnection::transferData(const CKString &, "
					"const CKString &, const CKString &) - the reply port:" <<
					listenerSocket.getPort() << " could not be set with the remote "
					"host through the PORT command. This is a serious problem.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	/*
	 * Send the passed-in command to the remote host for processing.
	 *
	 * This can be a little complex if we are expecting this command to
	 * initiate a sending of data from the remote host to the local host.
	 * In that case, we need to parse the server reply line and get the
	 * expected number of bytes in the upcoming transfer.
	 */
	bool		sentRemoteHostCommand = false;
	bool		expectRemoteHostReply = false;
	long		expectedReceivedBytes = -1;
	if (!error) {
		if (!isPositivePreliminaryReply(executeCommand(aCmd))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - the command:'" <<
				aCmd << "' was not successfully processed on the remote host. "
				"This is a serious problem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK... we sent the command
			sentRemoteHostCommand = true;
			// ...and *tentatively* expect a reply. But on a send we won't.
			expectRemoteHostReply = true;

			/*
			 * Now, if we're receiving data, we need some idea of how
			 * much to get. That is, if this info is part of the reply
			 */
			if (aData.size() == 0) {
				// try to find the bracketing '()' and get what's between them
				int		end = mServerReplyLines[0].findLast(')');
				if (end != -1) {
					int		beg = mServerReplyLines[0].findLast('(', end);
					if (beg != -1) {
						expectedReceivedBytes = atol(mServerReplyLines[0].
								substr((beg+1), (end - beg - 1)).c_str());
					}
				}
			} else {
				// as stated above, a send has no server-side reply
				expectRemoteHostReply = false;
			}
		}
	}

	// do an accept() on the listener to get the new data socket
	CKSocket		*dataSocket = NULL;
	if (!error) {
		dataSocket = listenerSocket.socketByAcceptingConnectionFromListener();
		if (dataSocket == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - no connection was "
				"established between a remote host requesting connection and the "
				"local machine. This could mean that the remote machine is not "
				"yet ready to send data.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Transfer everything from the intended source to the intended
	 * destination. If 'aData' is empty, then it's incoming, otherwise
	 * it's outgoing...
	 */
	if (!error) {
		if (aData.size() == 0) {
			/*
			 * We're reading something from the dataSocket
			 */
			CKString		newBlock;
			bool			gotSocketClose = false;
			bool			gotTimeout = false;
			bool			gotEverythingComing = false;

			while (!error && !gotSocketClose && !gotTimeout && !gotEverythingComing) {
				// Get what we can from the socket
				try {
					newBlock = dataSocket->readAvailableData();
				} catch (CKException & e1) {
					// an error here probably means that we're done
					gotSocketClose = true;
					// ...no need to continue this mess
					break;
				}

				// see if anything at all was read at the socket
				if (newBlock.size() > 0) {
					// add the new data to what we've gotten so far
					retval.append(newBlock);
					// ...and jump back to the top to try it again
					continue;
				}

				// nothing? Then see if we have all we expect
				if ((((long)retval.size() >= expectedReceivedBytes) &&
					 (expectedReceivedBytes > 0)) ||
					(expectedReceivedBytes <= 0)) {
					gotEverythingComing = true;
					// all done, so bail out
					break;
				}

				/*
				 * Wait for something at the socket, and if nothing
				 * arrives in the timeout interval, then it's time to
				 * bail out and fail.
				 */
				if (!dataSocket->waitForData(getIncomingDataTimeout())) {
					gotTimeout = true;
					if (expectedReceivedBytes >= 0) {
						std::ostringstream	msg;
						msg << "CKFTPConnection::transferData(const CKString &, "
							"const CKString &, const CKString &) - after "
							"receiving " << retval.size() << " bytes (out of an "
							"expected " << expectedReceivedBytes << " bytes) a read "
							"timeout was encountered. This might be because the "
							"expected size is incorrect from the remote FTP server "
							"or the server stopped sending. Up the timeout and try "
							"again.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						/*
						 * Since the remote FTP server didn't send us how many
						 * bytes we should get, we can only assume that this
						 * timeout means that the server is done sending data
						 * and call it done. This is too bad, but it's not in
						 * the FTP spec to send the number of expected bytes
						 * to the receiver.
						 */
						gotEverythingComing = true;
						break;
					}
				}
			}
		} else {
			/*
			 * We're writing something to the dataSocket
			 */
			if (!dataSocket->send(aData)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFTPConnection::transferData(const CKString &, "
					"const CKString &, const CKString &) - the data could "
					"not be sent to the remote host. This could mean that the "
					"remote machine is not yet ready to receive data, or is unable "
					"to do so.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	/*
	 * We have all the data we need, so close down this socket
	 */
	if (dataSocket != NULL) {
		dataSocket->shutdownSocket();
		delete dataSocket;
	}

	/*
	 * Finally try to get the return code - based on if we sent a
	 * command, and not any intermediate errors
	 */
	if (sentRemoteHostCommand && expectRemoteHostReply) {
		if (getReply() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::transferData(const CKString &, "
				"const CKString &, const CKString &) - the reply from the "
				"remote FTP server process was not properly formatted - no "
				"valid return code was present. This may be a timeout, but most "
				"likely signifies a serious problem with the remote host.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// close the listener socket as well...
	listenerSocket.shutdownSocket();

	return retval;
}


/********************************************************
 *
 *                  STL Helper Methods
 *
 ********************************************************/
/*
 * This class method takes a file name and attempts to load
 * the file into a single CKString and then return that
 * to the caller. This will certainly make large CKString
 * values, but that's OK as it's the purpose of this thing
 * in the first place.
 */
CKString CKFTPConnection::getLocalContents( const CKString & aFilename )
{
	bool			error = false;
	CKString		retval;

	// first, make sure we have something to do
	if (!error) {
		if (aFilename.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getLocalContents(const CKString &) - "
				"the supplied file name was empty and that means that there's "
				"nothing for me to do. Please pass in a valid file name.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, create the input stream
	std::ifstream	src(aFilename.c_str());
	if (!src) {
		error = true;
		std::ostringstream	msg;
		msg << "CKFTPConnection::getLocalContents(const CKString &) - "
			"an input stream could not be created for the file '" << aFilename <<
			"' and that means that there's no way for me to read it's contents. "
			"Please make sure the local file exists.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// read it in a character at a time
	if (!error) {
		char	c;
		while (!src.eof()) {
			if (src.get(c)) {
				retval += c;
			}
		}
		src.close();
	}

	return retval;
}


/*
 * This method takes a filename of a local file as well as a
 * (quite possibly large) CKString and creates a file
 * on the local filesystem with that name and that string as
 * it's contents. This is the opposite of the getLocalContents()
 * method and is used in the methods for this class to write
 * out files.
 */
void CKFTPConnection::setLocalContents( const CKString & aFilename,
										const CKString & aData )
{
	bool			error = false;

	// first, make sure we have something to do
	if (!error) {
		if (aFilename.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::setLocalContents(const CKString &, "
				"const CKString &) - the supplied destination file name was "
				"empty and that means that there's nothing for me to do. Please "
				"pass in a valid file name.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if (aData.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::setLocalContents(const CKString &, "
				"const CKString &) - the supplied data set for the file was "
				"empty and that means that there's nothing for me to do. Please "
				"pass in some real data to write to the file.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, create the output stream
	std::ofstream	dest(aFilename.c_str());
	if (!dest) {
		error = true;
		std::ostringstream	msg;
		msg << "CKFTPConnection::setLocalContents(const CKString &, "
			"const CKString &) - an output stream could not be created for "
			"the file '" << aFilename << "' and that means that there's no way "
			"for me to write out it's contents. Please make sure you have "
			"permissions write the local file.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// write it out a character at a time
	if (!error) {
		for (int i = 0; i < aData.length(); i++) {
			dest.put(aData[i]);
			if (dest.bad()) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFTPConnection::setLocalContents(const CKString &, "
					"const CKString &) - while trying to write out the data "
					"to the file '" << aFilename << "' something bad happened. "
					"I don't have a lot of details, but it's possible that you're "
					"out of disk space.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
		dest.close();
	}
}


/*
 * When dealing with permissions on the remote host, it's nice
 * to be able to convert the three permissions - read, write and
 * execute into a single number that can be used in setting the
 * permissions on the file. This method takes those three bits
 * and returns a char that corresponds to the right number for
 * use in the chmod command.
 */
char CKFTPConnection::bitsToDigit( bool aMSB, bool aBit, bool aLSB )
{
	return (char)((aMSB ? 4 : 0) + (aBit ? 2 : 0) + (aLSB ? 1 : 0) + '0');
}


/*
 * When dealing with permissions on the remote host, it's nice
 * to be able to convert the three sets of permissions - user,
 * group and others into a single three digit number that can be
 * used in setting the permissions on the file. This method takes
 * a CKFilePermissions struct and returns a string that is the
 * three digit number for inclusion in the command to change
 * permissions.
 */
CKString CKFTPConnection::permissionsToNumber( const CKFilePermissions & aSet )
{
	CKString		retval;

	// build up the three numbers
	retval += bitsToDigit(aSet.userReadable, aSet.userWritable, aSet.userExecutable);
	retval += bitsToDigit(aSet.groupReadable, aSet.groupWritable, aSet.groupExecutable);
	retval += bitsToDigit(aSet.othersReadable, aSet.othersWritable, aSet.othersExecutable);

	return retval;
}


/*
 * This is the tokenizer/parser that wasn't in the STL string
 * class for some unknown reason. It takes a source and a
 * delimiter and breaks up the source into chunks that are
 * all separated by the delimiter string. Each chunk is put
 * into the returned vector for accessing by the caller. Since
 * the return value is created on the stack, the user needs to
 * save it if they want it to stay around.
 */
CKStringList CKFTPConnection::parseIntoChunks( const CKString & aString,
											   const CKString & aDelim )
{
	bool			error = false;
	CKStringList	retval;

	// first, see if we have anything to do
	if (!error) {
		if (aString.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::parseIntoChunks(const CKString &, "
				"const CKString &) - the length of the source string is 0 and "
				"that means that there's nothing for me to do. Please make sure "
				"that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	int		delimLength = 0;
	if (!error) {
		delimLength = aDelim.length();
		if (delimLength <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::parseIntoChunks(const CKString &, "
				"const CKString &) - the length of the delimiter string is 0 "
				"and that means that there's nothing for me to do. Please make "
				"sure that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, copy the source to a buffer so I can consume it in the process
	CKString		buff;
	if (!error) {
		buff = aString;
	}

	/*
	 * Now loop picking off the parts bettween the delimiters. Do this by
	 * finding the first delimiter, see if it's located at buff[0], and if
	 * so, then add an empty string to the vector, otherwise, get the
	 * substring up to that delimiter and place it at the end of the vector,
	 * removing it from the buffer as you do this. Then eat up the delimiter
	 * and do it all again. In the end, there will be one more bit and that
	 * will simply be added to the end of the vector.
	 */
	while (!error) {
		// find out wherre, if anyplace, the delimiter sits
		int		pos = buff.find(aDelim);
		if (pos == -1) {
			// nothing left to parse out, bail out
			break;
		} else if (pos == 0) {
			// add an empty string to the vector
			retval.addToEnd(CKString());
		} else {
			// pick off the substring up to the delimiter
			retval.addToEnd(buff.substr(0, pos));
			// ...and then delete them from the buffer
			buff.erase(0, pos);
		}

		// now strip off the delimiter from the buffer
		buff.erase(0, delimLength);
	}
	// if we didn't error out, then add the remaining buff to the end
	if (!error) {
		retval.addToEnd(buff);
	}

	return retval;
}


/*
 * This method sets the current state of the FTP connection to
 * the provided value. This is most often picked off the return
 * data from the FTP server, but it can come from anywhere. A
 * value of -1 means it's in an indeterminate state.
 */
void CKFTPConnection::setState( int aState )
{
	mState = aState;
}


/*
 * This method sets the FTP control port in use to be equal to
 * the provided control port. Since this is a copy, we need to
 * be careful with this method.
 */
void CKFTPConnection::setControlPort( const CKTelnetConnection & aConn )
{
	mControlPort = aConn;
}


/*
 * This method returns the current state of the FTP connection as
 * it's been set using the setState() method. This method does not
 * query the system for it's status - it just reports what's been
 * already set.
 */
int CKFTPConnection::getState() const
{
	return mState;
}


/*
 * This method returns a copy of the control port.
 */
CKTelnetConnection CKFTPConnection::getControlPort() const
{
	return mControlPort;
}


/********************************************************
 *
 *             Generic FTP Transfer Method
 *
 ********************************************************/
/*
 * This method gets the reply from the FTP server through the
 * socket and then updates the state of the connection with the
 * return value code. This may involve several return lines from
 * the server because the FTP protocol allows for multi-line
 * return strings.
 *
 * No matter what the format, his routine will return a single
 * return code, ready for processing - or timeout waiting.
 */
int CKFTPConnection::getReply()
{
	bool		error = false;
	bool		isMultilineReply = false;
	int			retcode = -1;

	// see if the control port is connected to anything
	if (!error) {
		if (!mControlPort.isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getReply() - this instance's control port "
				"is not connected to a valid FTP server and so nothing can be "
				"done. Please connect to the server and then login with a call "
				"to loginToHost() before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * We need to read a returned line from the FTP server.
	 * Thankfully, all responses end in a NEWLINE
	 */
	CKString	response;
	if (!error) {
		try {
			response = mControlPort.readUpToCRLF();
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getReply() - while trying to read a response "
				"line from the FTP server on " << getHostname() << " no complete "
				"line was available within the timeout interval. Please check on "
				"the FTP server or increase the read timeout.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now grab the return code, and set the object's state
	 * this includes saving this response for later use
	 */
	if (!error) {
		retcode = grabFTPReturnCodeOnData(response);
		// ...is it a valid return code?
		if (retcode > 0) {
			// set the return code
			setState(retcode);

			// reset the array of received lines
			clearServerReplyLines();
			// ...and add the first reply line in
			addToServerReplyLines(response);

			// check to see if this is a multi-line reply
			isMultilineReply = isStartOfMultilineReply(response);
		} else {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::getReply() - the FTP server on the remote "
				"host returned a reply string that did not contain a valid return "
				"code. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now, let's see if we have a multiline reply that we
	 * need to read in...
	 */
	if (!error && isMultilineReply) {
		/*
		 * Now read each line, check it for a return code, and save what
		 * is in between until we do get the same return code on the line
		 */
		CKString	line;
		while (!error) {
			try {
				// get a line from the server's reply
				line = mControlPort.readUpToCRLF();
				// good line, so add it to the array of server replies
				addToServerReplyLines(line);
			} catch (CKException & e) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFTPConnection::getReply() - the FTP server on the "
					"remote host never returned the correct terminating line "
					"for a multi-line reply.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// does it have the same return code as the first?
			if (!error) {
				if ((grabFTPReturnCodeOnData(line) == retcode) &&
					!isStartOfMultilineReply(line)) {
					break;
				}
			}
		}
	}

	return retcode;
}


/*
 * When communicating with an FTP host the return code is embedded
 * in the byte string returning from the server. This three-character
 * number needs to become an integer and stored in the state variable
 * in this class ivar. The purpose of this routine is to extract the
 * integer off the returned byte stream and ignore the rest.
 *
 * This routine throws CKExceptions in the event that a
 * processing error occurs, and returns a -1.
 */
int CKFTPConnection::grabFTPReturnCodeOnData( const CKString & aData )
{
	bool	error = false;
	int		retval = -1;

	// check to see if the string isn't long enough
	if (!error) {
		// A three digit number, possible dash and a CR LF are the minimum length
		if ((aData.length() < 6) ||
			(!isdigit(aData[0]) || !isdigit(aData[1]) || !isdigit(aData[2]))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::grabFTPReturnCodeOnData(const CKString &) - "
				"the passed in data: '" << aData << "' does not contain a complete "
				"return code (3 digits).";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now let's process what we have...
	if (!error) {
		retval = (aData[0] - '0')*100 + (aData[1] - '0')*10 + (aData[2] - '0');

		// ...and check to see that it is in the right range
		if ((retval < 100) || (retval > 559)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::grabFTPReturnCodeOnData(const CKString &) - "
				"the passed in data: '" << aData << "' does not contain a valid FTP "
				"return code (between 100 and 559).";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

    return error ? -1 : retval;
}


/*
 * This method looks at the returned data from the FTP server
 * and determins if this is the first line of a multi-line
 * reply from that server. This can be determined by looking
 * at the format of the reply. If there are three digits followed
 * by a '-', then it is the start of a multi-line reply.
 */
bool CKFTPConnection::isStartOfMultilineReply( const CKString & aData )
{
	bool	error = false;
	bool	isContinued = false;

	// check to see if the string isn't long enough
	if (!error) {
		// A three digit number, possible dash and a CR LF are the minimum length
		if (aData.length() < 6) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFTPConnection::isStartOfMultilineReply(const CKString &) - "
				"the passed in data: '" << aData << "' does not contain a complete "
				"return code (3 digits).";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now let's process what we have...
	if (!error) {
		// the first three have to be digits
		if (isdigit(aData[0]) && isdigit(aData[1]) && isdigit(aData[2])) {
			// ...and the fourth needs to be a '-'
			if (aData[3] == '-') {
				isContinued = true;
			}
		}
	}

    return isContinued;
}


/*
 * There will be times that we need to "decode" the FTP return
 * code that is an integer into a human-readable string. This
 * method does just that. It returns a CKString
 * that corresponds to the FTP error code passed in. If the
 * argument does not correcepond to any FTP error code, a
 * descriptive error string is returned and an CKException
 * is thrown.
 */
CKString CKFTPConnection::stringForFTPReturnCode( int aCode )
{
	CKString	retval;

	switch (aCode) {
		case 110 :
			retval = "Restart marker reply.";
			break;
		case 120 :
			retval = "Service ready in nnn minutes";
			break;
		case 125 :
			retval = "Data connection already open, transfer starting.";
			break;
		case 200 :
			retval = "Command OK.";
			break;
		case 202 :
			retval = "Command not implemented, superfluous at this site.";
			break;
		case 211 :
			retval = "System status, or system help reply";
			break;
		case 212 :
			retval = "Directory status.";
			break;
		case 213 :
			retval = "File status.";
			break;
		case 214 :
			retval = "Help message.";
			break;
		case 215 :
			retval = "NAME system type.";
			break;
		case 220 :
			retval = "Service ready for new user.";
			break;
		case 221 :
			retval = "Service closing control connection.";
			break;
		case 225 :
			retval = "Data connection open; no transfer in progress.";
			break;
		case 226 :
			retval = "Closing data connection. Requested file action "
				"successful.";
			break;
		case 227 :
			retval = "Entering Passive Mode (h1,h2,h3,h4,p1,p2).";
			break;
		case 230 :
			retval = "User logged in, proceed.";
			break;
		case 250 :
			retval = "Requested file action OK, completed";
			break;
		case 257 :
			retval = "'PATHNAME' created.";
			break;
		case 331 :
			retval = "User name OK, need password.";
			break;
		case 332 :
			retval = "Need account for login.";
			break;
		case 350 :
			retval = "Requested file action pending further information.";
			break;
		case 421 :
			retval = "Service not available, closing control connection.";
			break;
		case 425 :
			retval = "Can't open data connection.";
			break;
		case 426 :
			retval = "Connection closed; transfer aborted.";
			break;
		case 450 :
			retval = "Requested file action not taken. File unavailable.";
			break;
		case 451 :
			retval = "Requested action aborted: local error in processing.";
			break;
		case 452 :
			retval = "Requested action not taken. Insuffiient storage space "
				"in system.";
			break;
		case 500 :
			retval = "Syntax error, command unrecognized. This may "
				"include errors such as command line too long.";
			break;
		case 501 :
			retval = "Syntax error in parameters or arguments.";
			break;
		case 502 :
			retval = "Command not implemented.";
			break;
		case 503 :
			retval = "Bad sequence of commands.";
			break;
		case 504 :
			retval = "Command not implemented for that parameter.";
			break;
		case 530 :
			retval = "Not logged in.";
			break;
		case 532 :
			retval = "Need account for storing files.";
			break;
		case 550 :
			retval = "Requested action not taken. File unavailable.";
			break;
		case 551 :
			retval = "Requested action aborted: page type unknown.";
			break;
		case 552 :
			retval = "Requested file action aborted. Exceeded storage "
				"allocation (for current directory or dataset).";
			break;
		case 553 :
			retval = "Requested action not taken. File name not allowed.";
			break;

		default :
			char	buff[256];
			snprintf(buff, 255, "The code:%d is NOT a valid FTP server return code, "
				"as defined in the document RFC-959.", aCode);
			retval = buff;
			break;
	}

	return retval;
}


/*
 * This method simply calls setStatus() to get the last FTP
 * return value and then passes it to stringForFTPReturnCode()
 * to convert that to a CKString.
 */
CKString CKFTPConnection::stringForLastFTPReturnCode()
{
	CKString		retval;

	if (getState() > 0) {
		retval = stringForFTPReturnCode(getState());
	} else {
		retval = "The FTP connection is in an indeterminate state, "
			"and does not have a valid return code.";
	}

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKFTPConnection & aConnection )
{
	aStream << aConnection.toString();

	return aStream;
}
