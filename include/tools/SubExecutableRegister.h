#ifndef INCLUDEGUARD_tools_SubExecutableRegister_h
#define INCLUDEGUARD_tools_SubExecutableRegister_h

#include <memory>
#include <map>
#include <vector>
#include <string>

//
// Forward declarations
//
namespace tools
{
	class ISubExecutable;
}


namespace tools
{
	/** @brief Class that registers a "sub-executable" under a particular name, then runs it as required.
	 *
	 * This is intended to help manage different sub-classes of tools::ISubExecutable.
	 *
	 * @author Mark Grimes
	 * @date 17/Nov/2015
	 */
	class SubExecutableRegister
	{
	public:
		static tools::SubExecutableRegister& instance();
		bool registerSubExecutable( std::function<std::unique_ptr<tools::ISubExecutable>()> creatorFunction, const std::string& name, bool throwOnFailure=false );
		std::unique_ptr<tools::ISubExecutable> getSubExecutable( const std::string& name ) const;
		std::vector<std::string> availableSubExecutableNames() const;
	protected:
		SubExecutableRegister();
		~SubExecutableRegister();
		SubExecutableRegister( const SubExecutableRegister& other ) = delete;
		SubExecutableRegister& operator=( const SubExecutableRegister& other ) = delete;
		std::map<std::string,std::function<std::unique_ptr<tools::ISubExecutable>()> > executables_;
	};

} // end of namespace tools

//
// Create a macro that can be used to register the sub-executables. This creates a new class
// with the name of the new sub-executable class but with "Factory" appended. A single instance
// of this class is declared statically. In the constructor of this single instance
// SubExecutableRegister::registerSubExecutable(...) is called so that the given sub-executable\
// is registered when the program starts up.
//
#define REGISTER_MODULE( MODULECLASSNAME, MODULENAME ) \
	class MODULECLASSNAME##Factory \
	{ \
	public : \
		MODULECLASSNAME##Factory() \
		{ \
			tools::SubExecutableRegister::instance().registerSubExecutable( []()->std::unique_ptr<tools::ISubExecutable>{return std::unique_ptr<MODULECLASSNAME>(new MODULECLASSNAME);}, MODULENAME, true ); \
		} \
	}; \
	MODULECLASSNAME##Factory MODULECLASSNAME##FactoryOnlyInstance;

#endif // end of "#ifndef INCLUDEGUARD_tools_SubExecutableRegister_h"
