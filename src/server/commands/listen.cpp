#include "tools/ISubExecutable.h"

class ListenSubExe : public tools::ISubExecutable
{
public:
	virtual int run( int argc, char* argv[] );
};

#include "tools/SubExecutableRegister.h"
#include <communique/Server.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

REGISTER_MODULE( ListenSubExe, "listen" );

int ListenSubExe::run( int argc, char* argv[] )
{
	// The synchronisation variables required to decide when to quit
	bool continueListening=true;
	std::mutex continueListeningMutex;
	std::condition_variable continueListeningCondition;

	communique::Server commandServer;

	// As the default example just echo every command sent
	commandServer.setDefaultRequestHandler( [](const std::string& message,communique::IConnection* pConnection)->std::string
		{
			std::cout << "Got request " << message << std::endl;
			return message;
		});
	// Just print what the message was and quit if necessary
	commandServer.setDefaultInfoHandler( [&](const std::string& message,communique::IConnection* pConnection)
		{
			std::cout << "Got info " << message << std::endl;
			if( message=="quit" )
			{
				std::unique_lock<std::mutex> lock(continueListeningMutex);
				continueListening=false;
				continueListeningCondition.notify_all();
			}

		});

	// Start listening...
	commandServer.listen(9002);
	// ...and wait until told to stop
	std::unique_lock<std::mutex> lock(continueListeningMutex);
	continueListeningCondition.wait( lock, [&]{ return !continueListening; } );

	// Shutdown gracefully
	commandServer.stop();

	return 0;
}
