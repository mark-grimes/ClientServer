#ifndef INCLUDEGUARD_tools_ISubExecutable_h
#define INCLUDEGUARD_tools_ISubExecutable_h

namespace tools
{
	/** @brief Interface for sub-executables.
	 *
	 * Intended to allow splitting different use-cases of an executable into different
	 * files. As an example, consider a re-implementation of git. "git clone" and "git
	 * log" are completely separate use-cases, and each would be coded as a sub class of
	 * ISubExecutable. The actual main function would just check the first argument
	 * ("clone" or "log") to see which ISubExecutable to run, and then invoke with all
	 * the remaining arguments. This way the functionality of each use-case can be kept
	 * separated in different files.
	 *
	 * To help manage different ISubExecutables, have a look at the
	 * tools::SubExecutableRegister class.
	 *
	 * @author Mark Grimes
	 * @date 17/Nov/2015
	 */
	class ISubExecutable
	{
	public:
		virtual ~ISubExecutable() {}
		virtual int run( int argc, char* argv[] ) = 0;
	};

} // end of namespace tools

#endif // end of "#ifndef INCLUDEGUARD_tools_ISubExecutable_h"
