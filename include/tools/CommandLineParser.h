#ifndef tools_CommandLineParser_h
#define tools_CommandLineParser_h

#include <vector>
#include <map>
#include <string>
#include <system_error>

namespace tools
{
	/** @brief Simple class to parse the command line arguments.
	 *
	 * @author Mark Grimes
	 * @date 12/Aug/2013
	 */
	class CommandLineParser
	{
	public:
		enum class error { ok=0, parse_error, unknown_option };
	public:
		enum ArgumentType { NoArgument, RequiredArgument, OptionalArgument };
		void addOption( const std::string& name, ArgumentType argumentType );
		/** @brief Parses the command line, for later querying by the query methods.
		 *
		 * @throw std::runtime_error     If there are any problems, e.g. invalid option specifications.
		 */
		void parse( const int argc, char* argv[] );
		/** @brief Non throwing version of parse, where errors are reported in the std::error_code supplie. */
		void parse( const int argc, char* argv[], std::error_code& error );
		bool optionHasBeenSet( const std::string& optionName ) const;
		const std::vector<std::string>& optionArguments( const std::string& optionName ) const;
		const std::vector<std::string>& nonOptionArguments() const;
		const std::string& executableName() const;
	protected:
		std::string executableName_; ///< @brief The name of the executable called. Useful for printing usage information.
		std::map<std::string,std::vector<std::string> > parsedOptions_;
		std::vector<std::string> nonOptionArguments_;
		std::vector<std::pair<std::string,ArgumentType> > allowedOptions_;
		std::vector<std::string> emptyVector_; ///< @brief Sometimes I need to return empty vectors, but I need to return a reference. This stays in scope.
		std::vector<std::string> unknownOptions_; ///< @brief Options specified that haven't been registered with addOption()
	};
} // end of the tools namespace

//
// Functions/structs required to get automatic conversion of CommandLineParser::error
// to std::error_code. For more information on this see
// http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-4.html
//
namespace tools
{
	std::error_code make_error_code( tools::CommandLineParser::error e );
	std::error_condition make_error_condition( tools::CommandLineParser::error e );
}
namespace std
{
	template<> struct is_error_code_enum<tools::CommandLineParser::error> : public true_type {};
}

#endif
