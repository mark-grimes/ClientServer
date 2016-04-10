#include "tools/CommandLineParser.h"

#include <stdexcept>
#include <algorithm>

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
			case tools::CommandLineParser::error::unexepected_option_value :
				return "An argument was supplied to an option that doesn't accept arguments";
			case tools::CommandLineParser::error::missing_option_value :
				return "A required option argument was missing";
			case tools::CommandLineParser::error::option_as_value :
				return "An option was provided as an argument to another option (use the \"option=argument\" syntax if you want to pass an argument starting with \"--\")";
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

void tools::CommandLineParser::parse( const int argc, char const* const* argv )
{
	std::error_code error;
	parse( argc, argv, error );
	if( error ) throw std::system_error( error, "Error with argument '"+parseErrors_.front().second+"'" );
}

void tools::CommandLineParser::parse( const int argc, char const* const* argv, std::error_code& error )
{
	// Don't know why this method would be called more than once, but I might as well
	// make sure I'm starting with a clean slate.
	executableName_.clear();
	parsedOptions_.clear();
	nonOptionArguments_.clear();
	parseErrors_.clear();

	// First figure out what the executable name is. If there is any path prepended
	// to it strip that off.
	executableName_=argv[0];
	size_t lastSlashPosition=executableName_.find_last_of("/\\");
	if( lastSlashPosition!=std::string::npos ) executableName_=executableName_.substr( lastSlashPosition+1, std::string::npos );

	// Start looping over the rest of argv, checking to see if any of them are options
	bool ignoreFurtherOptions=false; // so the user can use '--' to specify everything following are not options
	for( int optionIndex=1; optionIndex<argc; ++optionIndex )
	{
		std::string argument( argv[optionIndex] );

		if( argument=="--" && !ignoreFurtherOptions ) ignoreFurtherOptions=true;
		else if( argument.substr(0,2)=="--" && !ignoreFurtherOptions ) // See if the first two characters are '--'
		{
			std::string option=argument.substr(2); // remove leading '--'
			std::string value;
			size_t equalsPosition=option.find('='); // see if the value is part of the option (i.e. "option=value")
			if( equalsPosition!=std::string::npos )
			{
				value=option.substr(equalsPosition+1);
				option=option.substr(0,equalsPosition);
			}
			// See if the option specified was registered
			const auto matchingOption=std::find_if( allowedOptions_.begin(), allowedOptions_.end(), [&option](std::pair<std::string,ArgumentType>& element){return element.first==option;});
			if( matchingOption==allowedOptions_.end() ) parseErrors_.emplace_back( CommandLineParser::error::unknown_option, argument );
			else
			{
				// Creating this reference will put a new entry in the map, so
				// there will be a record that an option has been specified even
				// if I don't add an argument into the vector.
				std::vector<std::string>& optionArguments=parsedOptions_[matchingOption->first];
				if( matchingOption->second==NoArgument && !value.empty() ) parseErrors_.emplace_back( CommandLineParser::error::unexepected_option_value, argument );
				else if( matchingOption->second==OptionalArgument && !value.empty() ) optionArguments.push_back( value );
				else if( matchingOption->second==RequiredArgument )
				{
					if( !value.empty() ) optionArguments.push_back( value ); // Value was specified in the same argument with "option=value"
					else if( optionIndex+1<argc ) // Next argument must be the value, make sure there are anough arguments
					{
						std::string nextArgument( argv[optionIndex+1] );
						// Values cannot start with "--". To specify a value with "--" you must use the "option=--value" syntax
						if( nextArgument.substr(0,2)=="--" ) parseErrors_.emplace_back( CommandLineParser::error::option_as_value, argument+" "+nextArgument );
						else
						{
							optionArguments.push_back( nextArgument );
							++optionIndex; // Skip parsing the value on the next loop
						}
					}
					else parseErrors_.emplace_back( CommandLineParser::error::missing_option_value, argument );
				}
			}
		}
		else nonOptionArguments_.push_back( argument );
	}

	if( !parseErrors_.empty() ) error=parseErrors_.front().first;
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
