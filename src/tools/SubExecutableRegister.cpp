#include "tools/SubExecutableRegister.h"
#include "tools/ISubExecutable.h"

tools::SubExecutableRegister& tools::SubExecutableRegister::instance()
{
	static tools::SubExecutableRegister onlyInstance;
	return onlyInstance;
}

tools::SubExecutableRegister::SubExecutableRegister()
{
	// no operation
}

tools::SubExecutableRegister::~SubExecutableRegister()
{
	// no operation
}

bool tools::SubExecutableRegister::registerSubExecutable( std::function<std::unique_ptr<tools::ISubExecutable>()> creatorFunction, const std::string& name, bool throwOnFailure )
{
	auto result=executables_.insert( std::make_pair(name,creatorFunction) );
	if( !result.second ) // this will be false if the name already existed and the new entry wasn't inserted
	{
		if( throwOnFailure ) throw std::runtime_error( "The module name \""+name+"\" has already been used ("+std::string(__FILE__)+":L"+std::to_string(__LINE__)+")" );
		return false;
	}
	else return true;
}

std::unique_ptr<tools::ISubExecutable> tools::SubExecutableRegister::getSubExecutable( const std::string& name ) const
{
	auto iFindResult=executables_.find(name);
	if( iFindResult==executables_.end() ) return nullptr;
	else return iFindResult->second();
}

std::vector<std::string> tools::SubExecutableRegister::availableSubExecutableNames() const
{
	std::vector<std::string> returnValue;
	for( const auto& mapEntry : executables_ )
	{
		returnValue.push_back( mapEntry.first );
	}

	return returnValue;
}
