/*
 * CKFTPConnection.h - this file defines the class that can be used as a
 *                     rather advanced FTP-based file manipulation tool for
 *                     the user. The idea is that all local and remote file
 *                     access can be done through this class - which includes
 *                     moving files, copying them from one host to another,
 *                     creating and reading them - pretty much everything
 *                     you might want to do with files you can do with this
 *                     class.
 *
 *                     It exists because there was a need to have the ease
 *                     of FTP but keep everything in the process space and
 *                     not shell out to have the file copied and then have
 *                     to the read it in.
 *
 * $Id: CKFTPConnection.h,v 1.6 2004/09/16 09:34:14 drbob Exp $
 */
#ifndef __CKFTPCONNECTION_H
#define __CKFTPCONNECTION_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <vector>
#include <map>

//	Third-Party Headers

//	Other Headers
#include "CKTelnetConnection.h"

//	Forward Declarations

//	Public Constants
/*
 * This is the standard FTP control port address for the standard
 * FTP protocol implementation. The transfer sockets will be dynamically
 * allocated as they are needed, but this one is pegged for the standard.
 */
#define	DEFAULT_FTP_PORT					21
/*
 * This is the default timeout parameter for receiving data
 * from the remote host. This is important because a timeout
 * will have to occur because we simply do not know how much
 * data is coming in any transfer. Thankfully, there are public
 * accessor methods that allow the user to set these on the
 * FTP connection before each receipt, if necessary.
 */
#define DEFAULT_INCOMING_DATA_TIMEOUT		10
/*
 * These are selected FTP error return codes that will be
 * used and tested for in the course of the implementation.
 * They are by no means complete, but are the essential
 * ones to the implementation. Under no circumstances
 * should any of these be altered in any way.
 */
#define	CKFTPUnimplementedFunction 			500
#define	CKFTPServiceReadyForNewUser			220
#define	CKFTPUserOKNeedPassword 			331
#define	CKFTPUserSuccessfullyLoggedIn		230
#define	CKFTPFileActionPendingFurtherInfo	350
/*
 * These are the basic groups of replys that come from an
 * FTP server. These numbers are the first of three digits
 * in a standard FTP return code, and can be used to tell
 * the basic nature of the reply.
 */
#define	CKFTPPositivePreliminaryGroup		1
#define	CKFTPPositiveCompletionGroup		2
#define	CKFTPPositiveIntermediateGroup		3
#define	CKFTPTransientNegativeGroup			4
#define	CKFTPPermanentNegativeGroup			5

#define	CKFTPSyntaxGroup			0
#define	CKFTPInformationGroup		1
#define	CKFTPConnectionsGroup		2
#define	CKFTPAuthenticationGroup	3
#define	CKFTPFileSystemGroup		5

/*
 * These are the formats of the different FTP commands that
 * will be sent to the remote host on the FTP connection's
 * control port (a.k.a. PI). They are not the complete set
 * of commands as defined in the FTP Protocol document,
 * but form the critical set that will be used in the
 * implementation of CKFTPConnection. Under no circumstances
 * should any of these be altered in any way.
 */
#define	CKFTPCommandStringABOR		"ABOR"
#define	CKFTPCommandStringAPPE		"APPE "
#define	CKFTPCommandStringCDUP		"CDUP"
#define	CKFTPCommandStringCWD		"CWD "
#define	CKFTPCommandStringDELE		"DELE "
#define	CKFTPCommandStringLIST		"LIST "
#define	CKFTPCommandStringMKD		"MKD "
#define	CKFTPCommandStringMODE		"MODE "
#define	CKFTPCommandStringNLST		"NLST "
#define	CKFTPCommandStringNOOP		"NOOP"
#define	CKFTPCommandStringPASS		"PASS "
#define	CKFTPCommandStringPORT		"PORT "
#define	CKFTPCommandStringPWD		"PWD"
#define	CKFTPCommandStringQUIT		"QUIT"
#define	CKFTPCommandStringRETR		"RETR "
#define	CKFTPCommandStringRMD		"RMD "
#define	CKFTPCommandStringRNFR		"RNFR "
#define	CKFTPCommandStringRNTO		"RNTO "
#define	CKFTPCommandStringSITE		"SITE "
#define	CKFTPCommandStringSTAT		"STAT "
#define	CKFTPCommandStringSTOR		"STOR "
#define	CKFTPCommandStringSTOU		"STOU"
#define	CKFTPCommandStringSTRU		"STRU "
#define	CKFTPCommandStringTYPE		"TYPE "
#define	CKFTPCommandStringUSER		"USER "

#ifdef __linux__
/*
 * On Linux, there is no standard definition for the maximum length of
 * a file name. So, in order to make the code as transportable as possible
 * we'll adopt the Solaris/Darwin definition and use that.
 */
#define MAXHOSTNAMELEN			255
#endif

//	Public Datatypes
/*
 * When this class returns data about the file permissions on the
 * remote file, it makes sense to have an easy-to-access format of
 * those permissions. This structure is as simple as it comes and
 * should be just about everything anyone needs.
 */
typedef struct CKFilePermissionsBlock {
	// this is 'd' for a directory, 'l' for a link, etc.
	char		type;
	// these are the user's permissions
	bool		userReadable;
	bool		userWritable;
	bool		userExecutable;
	// these are the group permissions
	bool		groupReadable;
	bool		groupWritable;
	bool		groupExecutable;
	// these are the world permissions
	bool		othersReadable;
	bool		othersWritable;
	bool		othersExecutable;
} CKFilePermissions;


//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFTPConnection
{
	public:
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
		CKFTPConnection();
		/*
		 * This constructor specifies the host, but wse still need to set
		 * the at least the login and password before we can make a
		 * successful connection.
		 */
		CKFTPConnection( const CKString & aHost );
		/*
		 * This constructor specifies everything that we need to get
		 * the FTP connection up and running. The timeout defaults to
		 * a reasonable value, so you shouldn't have to set it unless
		 * you know that there's a serious problem with the FTP server.
		 */
		CKFTPConnection( const CKString & aHost,
						 const CKString & aUser,
						 const CKString & aPassword,
						 int anIncomingDataTimeout = DEFAULT_INCOMING_DATA_TIMEOUT );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKFTPConnection( const CKFTPConnection & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKFTPConnection();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKFTPConnection & operator=( const CKFTPConnection & anOther );

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
		void setHostname( const CKString & aName );
		/*
		 * This accessor method simply sets the user name for use by
		 * subsequent FTP connection(s). This distinction needs to be
		 * made because once a user is logged into the FTP server on
		 * the remote host, the user name is "locked" and cannot be
		 * changed so long as that user is logged in.
		 */
		void setUsername( const CKString & aName );
		/*
		 * This accessor method simply sets the user's password for
		 * use by subsequent FTP connection(s). This distinction needs
		 * to be made because once a user is logged into the FTP server
		 * on the remote host, the user's password is "locked" and
		 * cannot be changed so long as that user is logged in.
		 */
		void setPassword( const CKString & aPassword );
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
		void setTraceControlCommunications( bool aFlag );
		/*
		 * This method sets the timeout for all incoming data for the
		 * FTP transfer socket connections.
		 */
		void setIncomingDataTimeout( int aTimeoutInSecs );

		/*
		 * This accessor method simply returns the name of the remote host
		 * to use, or in use. This distinction needs to be made because once
		 * a connection is made the hostname is "locked" and cannot be
		 * changed so long as a connection exists.
		 */
		CKString getHostname() const;
		/*
		 * This accessor method simply returns the user name in use, or
		 * ready for use by the connection. This distinction needs to be
		 * made because once a user is logged into a remote host, the
		 * user name is "locked" and cannot be changed so long as that
		 * user is logged in.
		 */
		CKString getUsername() const;
		/*
		 * This accessor method simply returns the state of the FTP control
		 * connection to the remote host. If the user has logged in
		 * successfully, this method will return true, otherwise it will
		 * return false.
		 */
		bool isLoggedIn() const;
		/*
		 * This method simply returns the state of data tracing on the
		 * FTP control port. It actually goes to the control port and
		 * asks it it's status, and returns that. If the
		 * CKTelnetConnection is logging all received and sent data, then
		 * this method will return true, otherwise it returns false.
		 */
		bool traceControlCommunications() const;
		/*
		 * This method returns the value (in seconds) of the incoming data
		 * timeout for all FTP communications. This is important because
		 * we need to be creating new transfer sockets and they need to
		 * have a timeout that might be different from the control port.
		 */
		int getIncomingDataTimeout() const;

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
		bool connectToHost( const CKString & aHost );
		/*
		 * This method simply goes through the FTP server login procedure
		 * using the username and password already set up, and the control
		 * port already connected to a remote host.
		 *
		 * If no remote host is connected to, this method obviously fails.
		 */
		bool loginToHost();
		/*
		 * This method attempts to set the username and password for this
		 * FTP connection, and if successful, tries to log into the
		 * remote host already connected to. Due to the nature of the
		 * login information, these changes will only be accepted if there
		 * isn't already a user logged into the remote host.
		 */
		bool loginToHost( const CKString & aUser, const CKString & aPassword );
		/*
		 * This is the call to log out of the FTP server on the remote
		 * host and close the command connection to that remote host.
		 * If the user has not already logged-out this command will do it
		 * for them.
		 */
		void disconnect();

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
		bool changeCurrentDirectoryPath( const CKString & aPath );
		/*
		 * This method creates a directory on the remote host at the
		 * path specified. It is similar to the OPENSTEP's NSFileManager
		 * method, but executed on the remote host.
		 *
		 * If the directory cannot be created, this method returns false.
		 * Otherwise it returns true after creating the directory on the
		 * remote host.
		 */
		bool createDirectoryAtPath( const CKString & aPath );
		/*
		 * This method simply returns the current path on the remote
		 * host, just like the OPENSTEP NSFileManager does for the local
		 * machine. It is returned in an CKString. If there is no
		 * current path, or there is no established connection, this
		 * method returns the empty string.
		 */
		CKString currentDirectoryPath();

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
		bool copyLocalToHost( const CKString & aLocalFile,
		                      const CKString & aRemoteFile );
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
		bool copyHostToLocal( const CKString & aRemoteFile,
		                      const CKString & aLocalFile );
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
		bool createFile( const CKString & aFilename,
		                 const CKString & aData );
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
		bool moveLocalToHost( const CKString & aLocalFile,
		                      const CKString & aRemoteFile );
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
		bool moveHostToLocal( const CKString & aRemoteFile,
		                      const CKString & aLocalFile );
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
		bool removeFile( const CKString & aRemoteFile );

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
		CKString getContents( const CKString & aFilename );
		/*
		 * This method simply compares two files - one on the local
		 * host, and another on the remote host. If they are the same,
		 * then it returns true. If there is an error such as one or
		 * both of the files doesn't exist, then a standard
		 * CKException is raised, and false will be returned.
		 */
		bool areContentsEqual( const CKString & aLocalFile,
							   const CKString & aRemoteFile );

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
		bool fileExists( const CKString & aRemoteFile );
		/*
		 * On a Unix remote host this method checks to see if the file
		 * on the remote host has the 'read' permission set so that
		 * subsequent calls like getContents() are successful.
		 */
		bool isFileReadable( const CKString & aRemoteFile );
		/*
		 * On a Unix remote host this method checks to see if the
		 * file or directory on the remote host has the 'write'
		 * permission set for this user. It is a good test to check this
		 * before a call to createFile() is made.
		 */
		bool isFileWritable( const CKString & aRemoteFile );
		/*
		 * On a Unix remote host this checks to see if the file on
		 * the remote host can be deleted by the user. It's probably
		 * a good idea to check this before calling removeFile().
		 */
		bool isFileDeletable( const CKString & aRemoteFile );

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
		bool setFileAttributes( const CKString & aFile,
								const CKFilePermissions & aSet );
		/*
		 * On a Unix remote host this method returns a CKFilePermissions
		 * structure whose contents have been set by the file permissions
		 * of the passed-in remote file.
		 *
		 * If there is any error in the processing, or the file
		 * 'aFile' is not located on the host, a CKException is thrown.
		 */
		CKFilePermissions getFileAttributes( const CKString & aFile );

		/********************************************************
		 *
		 *              Discovering Directory Contents
		 *
		 ********************************************************/
		/*
		 * This method returns a std::vector of the directories
		 * and files contained in the directory specified on the remote
		 * host. This does not distinguish between symbolic links, files
		 * or directories, and only includes the files in that directory -
		 * not any subdirectories of that directory.
		 *
		 * If there is any problem in creating the directory list, an empty
		 * list is returned, and if necessary, a CKException is thrown.
		 */
		std::vector<CKString> getDirectoryContents( const CKString & aDir );
		/*
		 * This method returns a std::vector of the entire
		 * tree structure in the passed-in directory, and below.
		 * Again, in a manner similar to the OPENSTEP NSFileManager,
		 * this method can be used to search an entire directory tree
		 * structure to find a particular file, or directory.
		 *
		 * If there is any problem in creating the directory list, an empty
		 * list is returned, otherwise, a std::vector is returned.
		 */
		std::vector<CKString> getSubpathsAtPath( const CKString & aDir );

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
		bool operator==( const CKFTPConnection & anOther ) const;
		/*
		 * This method checks to see if the two CKFTPConnections are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKFTPConnection & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	protected:
		/*
		 * This method sets the set of server reply lines for a given
		 * control port message to the same contents as the supplied
		 * list. A copy is made so you don't have to worry about who
		 * owns the argument - the caller does.
		 */
		void setServerReplyLines( const std::vector<CKString> & aList );
		/*
		 * This method sets the flag when we get logged into the FTP server
		 * on the remote host, and when we logoff. This is useful because
		 * the user can use this flag to see if they are safe in issuing
		 * commands.
		 */
		void setIsLoggedIn( bool aFlag );

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
		CKString getPassword() const;
		/*
		 * This method returns a copy of the vector of server reply lines
		 * that have been returned from the server in response to a control
		 * port command. This is useful as there's a lot of useful data in
		 * the responses and many times we need to mine it.
		 */
		std::vector<CKString> getServerReplyLines() const;

		/*
		 * This method clears out all the response lines from the FTP server
		 * in preparation for adding in new lines to the vector as the
		 * response from another command.
		 */
		void clearServerReplyLines();
		/*
		 * This method adds a response line from the FTP server to the end
		 * of the vector of reply lines for this FTP command. This is nice
		 * because the user doesn't have to deal with the ivar, just the
		 * method.
		 */
		void addToServerReplyLines( const CKString & aLine );

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
		bool isPositivePreliminaryReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Positive Completion group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isPositiveCompletionReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Positive Intermediate group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isPositiveIntermediateReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Transient Negative group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isTransientNegativeReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Permanent Negative group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isPermanentNegativeReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Syntax group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isSyntaxReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Information group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isInformationReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Connections group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isConnectionsReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the Authentication and Auditing group. The
		 * result code groupings are defined in the RFC-959 document.
		 */
		bool isAuthenticationReply( int aCode );
		/*
		 * This method returns true if the passed in FTP return code
		 * is of the File System group. The result code
		 * groupings are defined in the RFC-959 document.
		 */
		bool isFileSystemReply( int aCode );

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
		int executeCommand( const CKString & aCmd );
		/*
		 * This executes the standard FTP 'ABOR' command on the command
		 * channel to the remote hosts. This will abort the transfer or
		 * command in process.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doABOR();
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
		int doAPPE( const CKString & arg );
		/*
		 * This executes the standard FTP 'CDUP' command on the command
		 * channel to the remote host. This changes the current directory
		 * on that host to it's parent directory. This is equivalent to
		 * a 'cd ..' on the Unix command-line.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doCDUP();
		/*
		 * This executes the standard FTP 'CWD' command on the command
		 * channel to the remote host. This attempts to set the current
		 * working directory on the remote host to the argument provided.
		 * This is very important because standard FTP transfers must
		 * go through the current directory on the remote host.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doCWD( const CKString & arg );
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
		int doDELE( const CKString & arg );
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
		int doLIST( const CKString & arg );
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
		int doMKD( const CKString & arg );
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
		int doMODE( const CKString & arg );
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
		int doNLST( const CKString & arg );
		/*
		 * This executes the standard FTP 'NOOP' command on the command
		 * channel to the remote host. This is a simple "no operation"
		 * that does nothing.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doNOOP();
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
		int doPASS( const CKString & arg );
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
		int doPORT( const CKString & arg );
		/*
		 * This executes the standard FTP 'PWD' command on the command
		 * channel to the remote host. This returns the current working
		 * directory on the remote host. It returns it in the reply to
		 * the command, in a similar manner to all the other informational
		 * based commands.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doPWD();
		/*
		 *	This executes the standard FTP 'QUIT' command on the command
		 *	channel to the remote host. This command ends the FTP session
		 *	with the remote host.\n
		 *
		 *	It returns the FTP return code for the execution.
		 */
		int doQUIT();
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
		int doRETR( const CKString & arg );
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
		int doRMD( const CKString & arg );
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
		int doRNFR( const CKString & arg );
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
		int doRNTO( const CKString & arg );
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
		int doSITE( const CKString & arg );
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
		int doSTAT( const CKString & arg );
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
		int doSTOR( const CKString & arg );
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
		int doSTOU();
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
		int doSTRU( const CKString & arg );
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
		 * vastly different operating systems - some not even based on
		 * ASCII. This meant that the individual FTP implementations
		 * had to convert their internal format for text to a standard
		 * format for transfer.
		 *
		 * It returns the FTP return code for the execution.
		 */
		int doTYPE( const CKString & arg );
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
		int doUSER( const CKString & arg );

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
		int ftpLoginUsername( const CKString & aUser,
							  const CKString & aPassword );
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
		int ftpRename( const CKString & aFromFile,
					   const CKString & aToFile );

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
		CKString transferData( const CKString & aType,
							   const CKString & aCmd,
							   const CKString & aData );

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
		static CKString getLocalContents( const CKString & aFilename );
		/*
		 * This method takes a filename of a local file as well as a
		 * (quite possibly large) CKString and creates a file
		 * on the local filesystem with that name and that string as
		 * it's contents. This is the opposite of the getLocalContents()
		 * method and is used in the methods for this class to write
		 * out files.
		 */
		static void setLocalContents( const CKString & aFilename,
									  const CKString & aData );

		/*
		 * When dealing with permissions on the remote host, it's nice
		 * to be able to convert the three permissions - read, write and
		 * execute into a single number that can be used in setting the
		 * permissions on the file. This method takes those three bits
		 * and returns a char that corresponds to the right number for
		 * use in the chmod command.
		 */
		static char bitsToDigit( bool aMSB, bool aBit, bool aLSB );
		/*
		 * When dealing with permissions on the remote host, it's nice
		 * to be able to convert the three sets of permissions - user,
		 * group and others into a single three digit number that can be
		 * used in setting the permissions on the file. This method takes
		 * a CKFilePermissions struct and returns a string that is the
		 * three digit number for inclusion in the command to change
		 * permissions.
		 */
		static CKString permissionsToNumber( const CKFilePermissions & aSet );

		/*
		 * This is the tokenizer/parser that wasn't in the STL string
		 * class for some unknown reason. It takes a source and a
		 * delimiter and breaks up the source into chunks that are
		 * all separated by the delimiter string. Each chunk is put
		 * into the returned vector for accessing by the caller. Since
		 * the return value is created on the stack, the user needs to
		 * save it if they want it to stay around.
		 */
		static std::vector<CKString> parseIntoChunks( const CKString & aString,
													  const CKString & aDelim );

	private:
		/*
		 * This method sets the current state of the FTP connection to
		 * the provided value. This is most often picked off the return
		 * data from the FTP server, but it can come from anywhere. A
		 * value of -1 means it's in an indeterminate state.
		 */
		void setState( int aState );
		/*
		 * This method sets the FTP control port in use to be equal to
		 * the provided control port. Since this is a copy, we need to
		 * be careful with this method.
		 */
		void setControlPort( const CKTelnetConnection & aConn );

		/*
		 * This method returns the current state of the FTP connection as
		 * it's been set using the setState() method. This method does not
		 * query the system for it's status - it just reports what's been
		 * already set.
		 */
		int getState() const;
		/*
		 * This method returns a copy of the control port.
		 */
		CKTelnetConnection getControlPort() const;

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
		int getReply();
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
		int grabFTPReturnCodeOnData( const CKString & aData );
		/*
		 * This method looks at the returned data from the FTP server
		 * and determins if this is the first line of a multi-line
		 * reply from that server. This can be determined by looking
		 * at the format of the reply. If there are three digits followed
		 * by a '-', then it is the start of a multi-line reply.
		 */
		bool isStartOfMultilineReply( const CKString & aData );
		/*
		 * There will be times that we need to "decode" the FTP return
		 * code that is an integer into a human-readable string. This
		 * method does just that. It returns a CKString
		 * that corresponds to the FTP error code passed in. If the
		 * argument does not correcepond to any FTP error code, a
		 * descriptive error string is returned and an CKException
		 * is thrown.
		 */
		static CKString stringForFTPReturnCode( int aCode );
		/*
		 * This method simply calls setStatus() to get the last FTP
		 * return value and then passes it to stringForFTPReturnCode()
		 * to convert that to a CKString.
		 */
		CKString stringForLastFTPReturnCode();

		/*
		 * This is the host machine that we'll try to connect to using
		 * the FTP protocol to act as the 'remote' host in this instance.
		 */
		CKString					mHostname;
		/*
		 * This is the username and password that will be used to log into
		 * the FTP service on the remote box referenced by 'Hostname'. While
		 * it's true that FTP is not secure, and that having these in this
		 * class is a serious security risk, the idea is that the most
		 * common users of this class are within a tightly secure network
		 * where general access accounts are used to pass around files in
		 * just this manner.
		 */
		CKString					mUsername;
		CKString					mPassword;
		/*
		 * This is the current state of the FTP connection and is maintained
		 * by the class for interrogation by the user.
		 */
		int							mState;
		/*
		 * The FTP protocol has a control port and a transfer port. The
		 * control port is the one used to issue commands and the transfer
		 * port is created at transfer time just for the movement of data.
		 */
		CKTelnetConnection			mControlPort;
		/*
		 * This vector is a list of the reply lines that the server has sent
		 * us in response to an issued command. The reason we keep them is
		 * they they contain a lot of information about the transfer and we'd
		 * like to have that to make sure things are done properly.
		 */
		std::vector<CKString>		mServerReplyLines;
		/*
		 * This is updated as we log into and out of the remote host's FTP
		 * service.
		 */
		bool						mIsLoggedIn;
		/*
		 * For each transfer, a new port is created. To make it simple, we're
		 * going to have the read timeout for all of them held here, in this
		 * class so that it can be set when each new transfer port is created.
		 */
		int							mIncomingDataTimeout;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKFTPConnection & aConnection );

#endif	// __CKFTPCONNECTION_H
