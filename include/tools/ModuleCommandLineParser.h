#ifndef tools_ModuleCommandLineParser_h
#define tools_ModuleCommandLineParser_h

#include "tools/CommandLineParser.h"

namespace tools
{

	/** @brief Similar to tools::CommandLineParser, but just does an initial pass to find the name of a command to run.
	 *
	 * The intention is for this class to be used to decide on which command to run, e.g. the command line
	 *
	 *     exeName --globalOption commandName --commandOption commandArgument
	 *
	 * would be parsed just until commandName, everything else would be left and should be passed to the command
	 * referenced by commandName for that to parse. This is just part of the structure to modialarise the different
	 * running options. Think of it in the same way git is invoked, e.g. "git push origin branch" is completely
	 * different to "git log". This class would decide whether "push" or "log" should be invoked, and this information
	 * used to invoke a module to handle the command (including parsing the remainder of the command line).
	 *
	 * In practice this means that parsing is done to the first entry that does *not* begin with "-". Anything that
	 * does start with "-" is considered a global option and parsed in this class. Everything after that entry can
	 * be retrieved with commandArgV and commandArgC to feed into the command module to do as it wishes.
	 *
	 * @author Mark Grimes
	 * @date 11/Jan/2016
	 */
	class ModuleCommandLineParser
	{
	public:
		typedef tools::CommandLineParser::ArgumentType ArgumentType;
		void addOption( const std::string& name, ArgumentType argumentType );
		/** @brief Parses the command line, for later querying by the query methods.
		 *
		 * @throw std::runtime_error     If there are any problems, e.g. invalid option specifications.
		 */
		void parse( const int argc, char* argv[] );

		/** @brief Checks whether the global option was specified. Anything after the command name (i.e. non-global options) is not included. */
		bool optionHasBeenSet( const std::string& optionName ) const;

		/** @brief Arguments to the registered global arguments. Anything after the command name is not included. */
		const std::vector<std::string>& optionArguments( const std::string& optionName ) const;
		const std::string& executableName() const;

		/** @brief The first argument encountered that does not start with "-". */
		const std::string& commandName() const;

		/** @brief The argc after stripping all global options */
		int commandArgC() const;

		 /** @brief argv after stripping all global options.
		  *
		  * Note that this is just a pointer to a portion of the argv passed to parse(). Make sure
		  * that variable is still in scope when calling this method.
		  *
		  * The command name is left as the first entry. */
		char** commandArgV() const;
	protected:
		tools::CommandLineParser parser_; ///< CommandLineParser actually does the heavy lifting.
		std::string commandName_;
		int commandArgC_;
		char** commandArgV_;
	};

} // end of namespace tools

#endif // end of ifndef tools_ModuleCommandLineParser_h
