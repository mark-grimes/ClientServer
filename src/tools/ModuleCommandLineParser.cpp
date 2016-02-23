#include "tools/ModuleCommandLineParser.h"

#include <stdexcept>
#include <getopt.h>

void tools::ModuleCommandLineParser::addOption( const std::string& name, tools::CommandLineParser::ArgumentType argumentType )
{
	parser_.addOption( name, argumentType );
}

void tools::ModuleCommandLineParser::parse( const int argc, char* argv[] )
{
	// First use the CommandLineParser to parse everything. The problem with this is that
	// I can't tell the order options were specified in, so global options will be mixed
	// with command options. The nonOptionArguments will be in order though, and the first
	// one of these is the command name. Once I have that, I can manually search argv for
	// its position.
	try { parser_.parse( argc, argv ); }
	catch( std::runtime_error& error )
	{
		// I know this is bad form to catch exceptions and hide them, but I don't
		// care if any options after the command name are unregistered (which would
		// normally throw an exception). As long as a command name was specified I'll
		// take that and let someone else worry about parsing everything after it. If
		// a command name wasn't specified, rethrow.
		if( parser_.nonOptionArguments().empty() ) throw;
		// If there is actually a problem with the arguments the exception will throw
		// on the next call of parse(...) anyway.
	}

	if( !parser_.nonOptionArguments().empty() ) // If empty then no command was specified and all options are global
	{
		commandName_=parser_.nonOptionArguments().front();

		size_t commandNameIndex=1;
		for( ; commandNameIndex<argc; ++commandNameIndex )
		{
			if( argv[commandNameIndex]==commandName_ ) break;
		}

		commandArgV_=&argv[commandNameIndex];
		commandArgC_=argc-commandNameIndex;
		// Parse everything again, but only up to the command name. That way the options
		// found will be only the global options.
		parser_.parse( commandNameIndex, argv );
	}
}

bool tools::ModuleCommandLineParser::optionHasBeenSet( const std::string& optionName ) const
{
	return parser_.optionHasBeenSet(optionName);
}

const std::vector<std::string>& tools::ModuleCommandLineParser::optionArguments( const std::string& optionName ) const
{
	return parser_.optionArguments(optionName);
}

const std::string& tools::ModuleCommandLineParser::executableName() const
{
	return parser_.executableName();
}

const std::string& tools::ModuleCommandLineParser::commandName() const
{
	return commandName_;
}

int tools::ModuleCommandLineParser::commandArgC() const
{
	return commandArgC_;
}

char** tools::ModuleCommandLineParser::commandArgV() const
{
	return commandArgV_;
}
