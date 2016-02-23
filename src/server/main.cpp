#include <iostream>
#include "tools/CommandLineParser.h"

int main( int argc, char* argv[] )
{
	try
	{
		tools::CommandLineParser commandLineParser;
		commandLineParser.addOption( "help", tools::CommandLineParser::NoArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet("help") )
		{
			std::cout << "Usage:" << "\n"
			          << "  " << commandLineParser.executableName() << " [--help]" << "\n"
			          << "  --help      Display this help message" << "\n"
			          << "\n";
			return 0;
		}
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
