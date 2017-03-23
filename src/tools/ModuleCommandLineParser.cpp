#include "tools/ModuleCommandLineParser.h"

#include <stdexcept>

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
	std::error_code error;
	parser_.parse( argc, argv, error );
	if( error )
	{
		// There is the potential for ignorable errors if the parser doesn't
		// recognise options that are meant for the command.
		if( error!=tools::CommandLineParser::error::unknown_option ) throw std::system_error(error);
	}

	if( !parser_.nonOptionArguments().empty() ) // If empty then no command was specified and all options are global
	{
		commandName_=parser_.nonOptionArguments().front();

		int commandNameIndex=1;
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
