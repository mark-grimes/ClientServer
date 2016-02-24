#include "catch.hpp"
#include "tools/SubExecutableRegister.h"
#include "tools/ISubExecutable.h"

namespace // Unnamed namespace for things only used in this file
{
	class TestSubExecutable1 : public tools::ISubExecutable
	{
	public:
		TestSubExecutable1() { ++numberOfInstances_; }
		virtual ~TestSubExecutable1() { --numberOfInstances_; }
		virtual int run( int argc, char* argv[] ) override { return 0; }
		static int numberOfInstances() { return numberOfInstances_; }
	protected:
		static int numberOfInstances_;
	};
	int TestSubExecutable1::numberOfInstances_=0;

	class StaticallyRegisteredTestSubExecutable : public tools::ISubExecutable
	{
	public:
		StaticallyRegisteredTestSubExecutable() { ++numberOfInstances_; }
		virtual ~StaticallyRegisteredTestSubExecutable() { --numberOfInstances_; }
		virtual int run( int argc, char* argv[] ) override { return 0; }
		static int numberOfInstances() { return numberOfInstances_; }
		static const std::string staticallyRegisteredName;
	protected:
		static int numberOfInstances_;
	};
	int StaticallyRegisteredTestSubExecutable::numberOfInstances_=0;
	const std::string StaticallyRegisteredTestSubExecutable::staticallyRegisteredName="StaticRegisterTest";
	REGISTER_MODULE( StaticallyRegisteredTestSubExecutable, StaticallyRegisteredTestSubExecutable::staticallyRegisteredName );

	class DuplicateNameTestSubExecutable : public tools::ISubExecutable
	{
	public:
		DuplicateNameTestSubExecutable() { ++numberOfInstances_; }
		virtual ~DuplicateNameTestSubExecutable() { --numberOfInstances_; }
		virtual int run( int argc, char* argv[] ) override { return 0; }
		static int numberOfInstances() { return numberOfInstances_; }
	protected:
		static int numberOfInstances_;
	};
	int DuplicateNameTestSubExecutable::numberOfInstances_=0;

} // end of the unnamed namespace

SCENARIO( "Test that SubExecutableRegister can register sub-executables correctly", "[subexecutables]" )
{
	GIVEN( "The global instance of sub-executable register" )
	{
		tools::SubExecutableRegister& theRegister=tools::SubExecutableRegister::instance();

		WHEN( "Adding a sub-executable by hand and retrieving an instance" )
		{
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
			// Register the sub-executable. This should not create an instance.
			bool success=theRegister.registerSubExecutable( []()->std::unique_ptr<tools::ISubExecutable>{ return std::unique_ptr< ::TestSubExecutable1>(new ::TestSubExecutable1); }, "test" );
			CHECK( success==true );
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
			// Get an instance of the sub-executable
			{ // Block to auto-delete the instance
				auto pTestSubExecutable1=theRegister.getSubExecutable( "test" );
				CHECK( pTestSubExecutable1!=nullptr );
				CHECK( ::TestSubExecutable1::numberOfInstances()==1 );
			}
			// Make sure the instance was deleted
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
		}

		WHEN( "Adding sub-executables with duplicate names" )
		{
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
			CHECK( ::DuplicateNameTestSubExecutable::numberOfInstances()==0 );
			// Not sure if the registration in the previous test has been done yet.
			// Try again to make sure but ignore the result.
			theRegister.registerSubExecutable( []()->std::unique_ptr<tools::ISubExecutable>{ return std::unique_ptr< ::TestSubExecutable1>(new ::TestSubExecutable1); }, "test" );
			// Register the sub-executable with the same name as TestSubExecutable1. This should fail.
			bool success=theRegister.registerSubExecutable( []()->std::unique_ptr<tools::ISubExecutable>{ return std::unique_ptr< ::DuplicateNameTestSubExecutable>(new ::DuplicateNameTestSubExecutable); }, "test" );
			CHECK( success==false );
			// Make sure this didn't create any instances
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
			CHECK( ::DuplicateNameTestSubExecutable::numberOfInstances()==0 );
			// Get an instance of the sub-executable, and make sure it's the correct type
			{ // Block to auto-delete the instance
				auto pSubExecutable=theRegister.getSubExecutable( "test" );
				CHECK( pSubExecutable!=nullptr );
				CHECK( ::TestSubExecutable1::numberOfInstances()==1 );
				CHECK( ::DuplicateNameTestSubExecutable::numberOfInstances()==0 );
			}
			// Make sure the instance was deleted
			CHECK( ::TestSubExecutable1::numberOfInstances()==0 );
			CHECK( ::DuplicateNameTestSubExecutable::numberOfInstances()==0 );
		}

		WHEN( "Adding a sub-executable statically using the macro and retrieving an instance" )
		{
			CHECK( ::StaticallyRegisteredTestSubExecutable::numberOfInstances()==0 );
			// Register the sub-executable. This should fail because the sub-executable should already
			// be registered statically by the macro.
			bool success=theRegister.registerSubExecutable( []()->std::unique_ptr<tools::ISubExecutable>{ return std::unique_ptr< ::StaticallyRegisteredTestSubExecutable>(new ::StaticallyRegisteredTestSubExecutable); }, ::StaticallyRegisteredTestSubExecutable::staticallyRegisteredName );
			CHECK( success==false );
			CHECK( ::StaticallyRegisteredTestSubExecutable::numberOfInstances()==0 );
			// Get an instance of the sub-executable
			{ // Block to auto-delete the instance
				auto pStaticallyRegisteredTestSubExecutable=theRegister.getSubExecutable( ::StaticallyRegisteredTestSubExecutable::staticallyRegisteredName );
				CHECK( pStaticallyRegisteredTestSubExecutable!=nullptr );
				CHECK( ::StaticallyRegisteredTestSubExecutable::numberOfInstances()==1 );
			}
			// Make sure the instance was deleted
			CHECK( ::StaticallyRegisteredTestSubExecutable::numberOfInstances()==0 );
		}
	}
}
