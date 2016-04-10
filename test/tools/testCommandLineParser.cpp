#include "tools/CommandLineParser.h"
#include "catch.hpp"

SCENARIO( "Test that CommandLineParser parses arguments correctly", "[tools]" )
{
	GIVEN( "A CommandLineParser instance registered with some dummy options" )
	{
		tools::CommandLineParser optionParser;
		optionParser.addOption( "help", tools::CommandLineParser::NoArgument );
		optionParser.addOption( "port", tools::CommandLineParser::RequiredArgument );
		optionParser.addOption( "verbose", tools::CommandLineParser::NoArgument );

		WHEN( "Parsing a dummy command line that is correctly formed" )
		{
			const char* testLine1[]={ "myExe", "--port", "22", "file1.txt", "file2.txt", "file3.txt" };

			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
			CHECK( optionParser.executableName()=="myExe" );
			CHECK( optionParser.optionHasBeenSet("help")==false );
			CHECK( optionParser.optionHasBeenSet("verbose")==false );
			CHECK( optionParser.optionHasBeenSet("port")==true );
			CHECK( optionParser.optionArguments("port").size()==1 );
			if( !optionParser.optionArguments("port").empty() ) // Protect against segfault
			{
				CHECK( optionParser.optionArguments("port").front()=="22" );
			}
			CHECK( optionParser.nonOptionArguments().size()==3 );
			if( optionParser.nonOptionArguments().size()>=3 ) // Protect against segfault
			{
				CHECK( optionParser.nonOptionArguments()[0]=="file1.txt" );
				CHECK( optionParser.nonOptionArguments()[1]=="file2.txt" );
				CHECK( optionParser.nonOptionArguments()[2]=="file3.txt" );
			}
		}
		WHEN( "Parsing a dummy command line that is correctly formed but uses the \"option=value\" syntax" )
		{
			// Exactly the same test as before, except with "--port=22" instead of "--port 22"
			const char* testLine1[]={ "myExe", "--port=22", "file1.txt", "file2.txt", "file3.txt" };

			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
			CHECK( optionParser.executableName()=="myExe" );
			CHECK( optionParser.optionHasBeenSet("help")==false );
			CHECK( optionParser.optionHasBeenSet("verbose")==false );
			CHECK( optionParser.optionHasBeenSet("port")==true );
			CHECK( optionParser.optionArguments("port").size()==1 );
			if( !optionParser.optionArguments("port").empty() ) // Protect against segfault
			{
				CHECK( optionParser.optionArguments("port").front()=="22" );
			}
			CHECK( optionParser.nonOptionArguments().size()==3 );
			if( optionParser.nonOptionArguments().size()>=3 ) // Protect against segfault
			{
				CHECK( optionParser.nonOptionArguments()[0]=="file1.txt" );
				CHECK( optionParser.nonOptionArguments()[1]=="file2.txt" );
				CHECK( optionParser.nonOptionArguments()[2]=="file3.txt" );
			}
		}
		WHEN( "Providing options that haven't been registered" )
		{
			const char* testLine1[]={ "myExe", "--blah", "--foo", "connect", "file1.txt", "file2.txt", "file3.txt" };
			CHECK_THROWS( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
		}
		WHEN( "Providing options that haven't been registered (error_code version)" )
		{
			// Exactly the same as the last test, but the version with an error_code
			const char* testLine1[]={ "myExe", "--blah", "--foo", "connect", "file1.txt", "file2.txt", "file3.txt" };
			std::error_code error;
			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1, error ) );
			CHECK( error==tools::CommandLineParser::error::unknown_option );
		}
		WHEN( "Providing values for options that don't accept options" )
		{
			const char* testLine1[]={ "myExe", "--help=true", "--port", "22", "file1.txt", "file2.txt", "file3.txt" };
			CHECK_THROWS( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
		}
		WHEN( "Providing values for options that don't accept options (error_code version)" )
		{
			// Exactly the same as the last test, but the version with an error_code
			const char* testLine1[]={ "myExe", "--help=true", "--port", "22", "file1.txt", "file2.txt", "file3.txt" };
			std::error_code error;
			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1, error ) );
			CHECK( error==tools::CommandLineParser::error::unexepected_option_value );
		}
		WHEN( "Providing a value starting with \"--\" to an option" )
		{
			const char* testLine1[]={ "myExe", "--port", "--http-port", "file1.txt", "file2.txt", "file3.txt" };
			CHECK_THROWS( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
		}
		WHEN( "Providing a value starting with \"--\" to an option (error_code version)" )
		{
			// Exactly the same as the last test, but the version with an error_code
			const char* testLine1[]={ "myExe", "--port", "--http-port", "file1.txt", "file2.txt", "file3.txt" };
			std::error_code error;
			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1, error ) );
			CHECK( error==tools::CommandLineParser::error::option_as_value );
		}
		WHEN( "Providing a value starting with \"--\" to an option using the \"option=value\" syntax" )
		{
			const char* testLine1[]={ "myExe", "--port=--http-port", "file1.txt", "file2.txt", "file3.txt" };
			CHECK_NOTHROW( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );
			CHECK( optionParser.executableName()=="myExe" );
			CHECK( optionParser.optionHasBeenSet("help")==false );
			CHECK( optionParser.optionHasBeenSet("verbose")==false );
			CHECK( optionParser.optionHasBeenSet("port")==true );
			CHECK( optionParser.optionArguments("port").size()==1 );
			if( !optionParser.optionArguments("port").empty() ) // Protect against segfault
			{
				CHECK( optionParser.optionArguments("port").front()=="--http-port" );
			}
			CHECK( optionParser.nonOptionArguments().size()==3 );
			if( optionParser.nonOptionArguments().size()>=3 ) // Protect against segfault
			{
				CHECK( optionParser.nonOptionArguments()[0]=="file1.txt" );
				CHECK( optionParser.nonOptionArguments()[1]=="file2.txt" );
				CHECK( optionParser.nonOptionArguments()[2]=="file3.txt" );
			}
		}
		WHEN( "Check that a parse error does not stop the rest of the command line being parsed" )
		{
			const char* testLine1[]={ "myExe", "--blah", "--port", "22", "file1.txt", "--verbose", "file2.txt", "file3.txt" };

			CHECK_THROWS( optionParser.parse( sizeof(testLine1)/sizeof(const char*), testLine1 ) );

			// Even though the parse threw an exception, see if all the other arguments were parsed correctly.
			CHECK( optionParser.executableName()=="myExe" );
			CHECK( optionParser.optionHasBeenSet("help")==false );
			CHECK( optionParser.optionHasBeenSet("verbose")==true );
			CHECK( optionParser.optionHasBeenSet("port")==true );
			CHECK( optionParser.optionArguments("port").size()==1 );
			if( !optionParser.optionArguments("port").empty() ) // Protect against segfault
			{
				CHECK( optionParser.optionArguments("port").front()=="22" );
			}
			CHECK( optionParser.nonOptionArguments().size()==3 );
			if( optionParser.nonOptionArguments().size()>=3 ) // Protect against segfault
			{
				CHECK( optionParser.nonOptionArguments()[0]=="file1.txt" );
				CHECK( optionParser.nonOptionArguments()[1]=="file2.txt" );
				CHECK( optionParser.nonOptionArguments()[2]=="file3.txt" );
			}
		}
	}
}
