#include "catch.hpp"
#include "tools/ModuleCommandLineParser.h"

SCENARIO( "Test that ModuleCommandLineParser parses arguments correctly", "[modules]" )
{
	GIVEN( "A ModuleCommandLineParser instance registered with some dummy options" )
	{
		tools::ModuleCommandLineParser globalOptionParser;
		globalOptionParser.addOption( "help", tools::CommandLineParser::NoArgument );
		globalOptionParser.addOption( "port", tools::CommandLineParser::RequiredArgument );
		globalOptionParser.addOption( "verbose", tools::CommandLineParser::NoArgument );

		WHEN( "Parsing a dummy command lines" )
		{
			const char* testLine1[]={ "myExe", "--port", "22", "connect", "file1.txt", "file2.txt", "file3.txt" };
			// All parsers take a non-const (because getopt does), but c++11 insists on
			// const char* from string literals. Therefore have to cast to another variable.
			char** testArgv1=const_cast<char**>(testLine1);

			globalOptionParser.parse( 7, testArgv1 );
			CHECK( globalOptionParser.executableName()=="myExe" );
			CHECK( globalOptionParser.optionHasBeenSet("help")==false );
			CHECK( globalOptionParser.optionHasBeenSet("verbose")==false );
			CHECK( globalOptionParser.optionHasBeenSet("port")==true );
			CHECK( globalOptionParser.optionArguments("port").size()==1 );
			if( !globalOptionParser.optionArguments("port").empty() ) // Protect against segfault
			{
				CHECK( globalOptionParser.optionArguments("port").front()=="22" );
			}
			CHECK( globalOptionParser.commandName()=="connect" );
			CHECK( globalOptionParser.commandArgC()==4 );
			CHECK( globalOptionParser.commandArgV()==&testArgv1[3] );
			CHECK( globalOptionParser.commandArgV()[0]=="connect" );
			CHECK( globalOptionParser.commandArgV()[1]=="file1.txt" );
			CHECK( globalOptionParser.commandArgV()[2]=="file2.txt" );
			CHECK( globalOptionParser.commandArgV()[3]=="file3.txt" );
		}
		WHEN( "Providing global options that haven't been registered" )
		{
			const char* testLine1[]={ "myExe", "--blah", "--foo", "connect", "file1.txt", "file2.txt", "file3.txt" };
			// All parsers take a non-const (because getopt does), but c++11 insists on
			// const char* from string literals. Therefore have to cast to another variable.
			char** testArgv1=const_cast<char**>(testLine1);

			CHECK_THROWS( globalOptionParser.parse( 7, testArgv1 ) );
		}
		WHEN( "Providing command options (that aren't registered as globals)" )
		{
			const char* testLine1[]={ "myExe", "--verbose", "analyse", "--input", "file1.txt", "--output", "output.txt" };
			// All parsers take a non-const (because getopt does), but c++11 insists on
			// const char* from string literals. Therefore have to cast to another variable.
			char** testArgv1=const_cast<char**>(testLine1);

			CHECK_NOTHROW( globalOptionParser.parse( sizeof(testLine1)/sizeof(testLine1[0]), testArgv1 ) );
			CHECK( globalOptionParser.executableName()=="myExe" );
			CHECK( globalOptionParser.optionHasBeenSet("help")==false );
			CHECK( globalOptionParser.optionHasBeenSet("port")==false );
			CHECK( globalOptionParser.optionHasBeenSet("verbose")==true );
			CHECK( globalOptionParser.optionArguments("verbose").size()==0 );
			CHECK( globalOptionParser.commandName()=="analyse" );
			CHECK( globalOptionParser.commandArgC()==5 );
			CHECK( globalOptionParser.commandArgV()==&testArgv1[2] );
			CHECK( globalOptionParser.commandArgV()[0]=="analyse" );
			CHECK( globalOptionParser.commandArgV()[1]=="--input" );
			CHECK( globalOptionParser.commandArgV()[2]=="file1.txt" );
			CHECK( globalOptionParser.commandArgV()[3]=="--output" );
			CHECK( globalOptionParser.commandArgV()[4]=="output.txt" );
		}
	}
}
