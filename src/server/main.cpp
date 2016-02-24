#include <iostream>
#include "tools/ModuleCommandLineParser.h"
#include "tools/SubExecutableRegister.h"
#include "tools/ISubExecutable.h"

int main( int argc, char* argv[] )
{
	tools::ModuleCommandLineParser commandLineParser;
	std::unique_ptr<tools::ISubExecutable> pSubExecutable;

	try
	{
		const tools::SubExecutableRegister& commandRegister=tools::SubExecutableRegister::instance();
		commandLineParser.addOption( "help", tools::CommandLineParser::NoArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet("help") )
		{
			std::cout << "Usage:" << "\n"
			          << "  " << commandLineParser.executableName() << " [global options] <command name> [command options]" << "\n"
			          << "\n"
			          << "Available global options:" << "\n"
			          << "  --help      Display this help message and exit" << "\n"
			          << "\n"
			          << "Available commands:" << "\n";
			for( const auto& name : commandRegister.availableSubExecutableNames() ) std::cout << "    " << name << "\n";
			std::cout << std::endl;
			return 0;
		}

		if( commandLineParser.commandName().empty() ) throw std::runtime_error( "No command was specified" );

		pSubExecutable=commandRegister.getSubExecutable( commandLineParser.commandName() );
		if( !pSubExecutable ) throw std::runtime_error( "The command \""+commandLineParser.commandName()+"\" is not a valid command" );
	}
	catch( std::exception& error )
	{
		std::cerr << "The following error was encountered while parsing the command line:" << "\n"
		          << "     " << error.what() << "\n"
				  << "Try \"" << commandLineParser.executableName() << " --help\"" << std::endl;
		return -1;
	}

	//
	// Command line has been successfully parsed. Now invoke the command in a separate try/catch block.
	//
	try
	{
		return pSubExecutable->run( commandLineParser.commandArgC(), commandLineParser.commandArgV() );
	}
	catch( std::system_error& error )
	{
		std::cerr << "The following error was encountered:" << "\n"
		          << "     " << error.what() << std::endl;
		return error.code().value();
	}
	catch( std::exception& error )
	{
		std::cerr << "The following error was encountered:" << "\n"
		          << "     " << error.what() << std::endl;
		return -1;
	}
	catch(...)
	{
		std::cerr << "An unknown exception occurred" << std::endl;
		return -1;
	}
	return 0;
}
