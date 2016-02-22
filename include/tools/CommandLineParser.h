#ifndef tools_CommandLineParser_h
#define tools_CommandLineParser_h

#include <vector>
#include <map>
#include <string>

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
		enum ArgumentType { NoArgument, RequiredArgument, OptionalArgument };
		void addOption( const std::string& name, ArgumentType argumentType );
		/** @brief Parses the command line, for later querying by the query methods.
		 *
		 * @throw std::runtime_error     If there are any problems, e.g. invalid option specifications.
		 */
		void parse( const int argc, char* argv[] );
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
	};

} // end of the tools namespace

#endif
