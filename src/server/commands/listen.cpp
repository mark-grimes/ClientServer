#include "tools/ISubExecutable.h"

class ListenSubExe : public tools::ISubExecutable
{
public:
	virtual int run( int argc, char* argv[] );
};

#include "tools/SubExecutableRegister.h"
#include "tools/CommandLineParser.h"
#include <communique/Server.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

REGISTER_MODULE( ListenSubExe, "listen" );

int ListenSubExe::run( int argc, char* argv[] )
{
	size_t portNumber=9002;

	//
	// Try and parse the command line arguments
	//
	try
	{
		tools::CommandLineParser commandLineParser;
		commandLineParser.addOption( "help", tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "port", tools::CommandLineParser::RequiredArgument );

		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet("help") )
		{
			std::cout << "Usage:" << "\n"
					  << "  " << commandLineParser.executableName() << " [command options]" << "\n"
					  << "\n"
					  << "Available options:" << "\n"
					  << "  --help      Display this help message and exit" << "\n"
					  << "  --port      The port number for the server to listen on. Default is " << portNumber << "." << "\n"
					  << std::endl;
			return 0;
		}

		if( commandLineParser.optionHasBeenSet("port") )
		{
			std::string numberAsString;
			try
			{
				numberAsString=commandLineParser.optionArguments("port").back();
				size_t pos;
				int numberAsInt=std::stoi(numberAsString,&pos);
				if( pos!=numberAsString.size() ) throw std::runtime_error( "the string was not fully converted" );
				if( numberAsInt<0 ) throw std::runtime_error( "the port number is negative" );

				portNumber=numberAsInt;
			}
			catch( std::invalid_argument& error )
			{
				std::cerr << "Couldn't parse the argument for --port (\"" << numberAsString << "\") because it could not be converted to an integer. Only specify positive integers." << std::endl;
				return -1;
			}
			catch( std::out_of_range& error )
			{
				std::cerr << "Couldn't parse the argument for --port (\"" << numberAsString << "\") because the number was out of range. Only specify positive integers in the valid range." << std::endl;
				return -1;
			}
			catch( std::exception& error )
			{
				std::cerr << "Couldn't parse the argument for --port (\"" << numberAsString << "\") because " << error.what() << ". Only specify positive integers." << std::endl;
				return -1;
			}
		} // end of "port" option check
	} // end of parsing arguments try block
	catch( std::exception& error )
	{
		std::cerr << "The following error was encountered while parsing the command line:" << "\n"
		          << "     " << error.what() << "\n"
				  << "Try \"--help\" for usage instructions." << std::endl;
		return -1;
	}

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
	std::cout << "Starting to listen on port " << portNumber << std::endl;
	commandServer.listen(portNumber);
	// ...and wait until told to stop
	std::unique_lock<std::mutex> lock(continueListeningMutex);
	continueListeningCondition.wait( lock, [&]{ return !continueListening; } );

	// Shutdown gracefully
	commandServer.stop();

	return 0;
}
