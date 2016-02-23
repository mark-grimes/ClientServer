#include "tools/CommandLineParser.h"

#include <stdexcept>
#include <getopt.h>

//
// Use the unnamed namespace for things only used in this file
//
namespace
{
	/** @brief error_category class for all errors the CommandLineParser class can generate
	 * @author Mark Grimes
	 * @date 23/Feb/2016
	 */
	class CommandLineParser_error_category : public std::error_category
	{
	public:
		virtual const char* name() const noexcept { return "CommandLineParser::error"; }
		virtual std::string message( int code ) const
		{
			switch( static_cast<tools::CommandLineParser::error>(code) )
			{
			case tools::CommandLineParser::error::ok :
				return "success";
			case tools::CommandLineParser::error::parse_error :
				return "getopt_long returned an unknown code";
			case tools::CommandLineParser::error::unknown_option :
				return "Unknown option(s)";
			default :
				return "Unknown error code";
			}
		}
	};

	/** @brief Returns the only instance of CommandLineParser_error_category
	 *
	 * C++11 guarantees that static local variables are thread safe.
	 *
	 * @author Mark Grimes
	 * @date 23/Feb/2016
	 */
	const std::error_category& error_category()
	{
		static CommandLineParser_error_category category;
		return category;
	}
} // end of the unnamed namespace

std::error_code tools::make_error_code( tools::CommandLineParser::error e )
{
	return std::error_code( static_cast<int>(e), ::error_category() );
}

std::error_condition tools::make_error_condition( tools::CommandLineParser::error e )
{
	return std::error_condition( static_cast<int>(e), ::error_category() );
}


void tools::CommandLineParser::addOption( const std::string& name, ArgumentType argumentType )
{
	allowedOptions_.push_back( std::make_pair( name, argumentType ) );
}

void tools::CommandLineParser::parse( const int argc, char* argv[] )
{
	std::error_code error;
	parse( argc, argv, error );
	if( error )
	{
		if( error==CommandLineParser::error::unknown_option )
		{
			// Extend the description to include the options that weren't recognised
			std::string message;//="Unknown option(s): ";
			for( const auto& option : unknownOptions_ ) message+=option+", ";
			throw std::system_error( error, message );
		}
		else throw std::system_error( error );
	}
}

void tools::CommandLineParser::parse( const int argc, char* argv[], std::error_code& error )
{
	// Don't know why this method would be called more than once, but I might as well
	// make sure I'm starting with a clean slate.
	executableName_.clear();
	parsedOptions_.clear();
	nonOptionArguments_.clear();
	unknownOptions_.clear();

	// Make a copy of argv, because getopt reorders it. I want to make sure calling
	// this function multiple times always has the same result. Pretty sure it just
	// get reordered so I want copy the strings themselves.
	std::vector<char*> argvCopy(argc);
	for( size_t index=0; index<argc; ++index ) argvCopy[index]=argv[index];
	argv=&argvCopy[0];

	// Suppress the getopt default error because I'll throw an exception if something
	// goes wrong.
	opterr=0;
	optind=1; // reset to make sure scanning begins at the start

	// First figure out what the executable name is. If there is any path prepended
	// to it strip that off.
	executableName_=argv[0];
	size_t lastSlashPosition=executableName_.find_last_of('/');
	if( lastSlashPosition!=std::string::npos ) executableName_=executableName_.substr( lastSlashPosition+1, std::string::npos );

	// First need to turn the vector allowedOptions_ into something that getopt_long
	// can understand.
	struct option longOptions[allowedOptions_.size()+1];
	for( size_t index=0; index<allowedOptions_.size(); ++index )
	{
		longOptions[index].name=allowedOptions_[index].first.c_str();
		longOptions[index].has_arg=allowedOptions_[index].second;
		longOptions[index].flag=0;
		longOptions[index].val=0;
	}
	// getopt_long instructions say the last entry has to be all zeros
	longOptions[allowedOptions_.size()].name=0;
	longOptions[allowedOptions_.size()].has_arg=0;
	longOptions[allowedOptions_.size()].flag=0;
	longOptions[allowedOptions_.size()].val=0;

	int getoptReturn;

	// Record all unregistered options encountered, then create the error *at the end*.
	// This is so that nonOptionArguments_ has the chance to be set, because this could
	// be called from ModuleCommandLineParser which doesn't care about unregistered options
	// after the command name.
	do
	{
		int optionIndex;
		int currentInd=optind;
		getoptReturn=getopt_long( argc, argv, "", longOptions, &optionIndex );

		if( getoptReturn==0 )
		{
			// Creating this reference will put a new entry in the map, so
			// there will be a record that an option has been specified even
			// if I don't add an argument into the vector.
			std::vector<std::string>& optionArguments=parsedOptions_[longOptions[optionIndex].name];
			if( optarg ) optionArguments.push_back( optarg );
		}
		else if( getoptReturn=='?' ) unknownOptions_.push_back(argv[currentInd]);
		else if( getoptReturn!=-1 ) { error=CommandLineParser::error::parse_error; return; }

	} while( getoptReturn!=-1 );

	// Now that all of the options have been parsed, make a note of any other command line
	// arguments.
	while( optind<argc ) nonOptionArguments_.push_back( argv[optind++] );

	// If options were specified that weren't registered, throw an exception.
	if( !unknownOptions_.empty() ) error=CommandLineParser::error::unknown_option;
	else error=CommandLineParser::error::ok;
}

bool tools::CommandLineParser::optionHasBeenSet( const std::string& optionName ) const
{
	// Search the map to see if anything with the supplied name exists
	const auto& findResult=parsedOptions_.find(optionName);

	// Return true if the name is in the map
	return findResult!=parsedOptions_.end();
}

const std::vector<std::string>& tools::CommandLineParser::optionArguments( const std::string& optionName ) const
{
	// Search the map to see if anything with the supplied name exists
	const auto& iFindResult=parsedOptions_.find(optionName);

	if( iFindResult==parsedOptions_.end() ) return emptyVector_;
	else return iFindResult->second;
}

const std::vector<std::string>& tools::CommandLineParser::nonOptionArguments() const
{
	return nonOptionArguments_;
}

const std::string& tools::CommandLineParser::executableName() const
{
	return executableName_;
}
