#
# CMake file to build the CLientServer skeleton program.
# This program is intended as a starting point for several projects that
# rely on coordinated action between a user written client and a user written
# server.
#
# Mark Grimes
# 17/Feb/2016
#

project( ClientServer )

cmake_minimum_required(VERSION 2.8)

set( ${PROJECT_NAME}_MAJOR_VERSION 0 )
set( ${PROJECT_NAME}_MINOR_VERSION 0 )
set( ${PROJECT_NAME}_PATCH_LEVEL 0 )


if( EMSCRIPTEN_ROOT_PATH )
        message( "-- EMSCRIPTEN_ROOT_PATH set to \"${EMSCRIPTEN_ROOT_PATH}\"" )
else()
        set( EMSCRIPTEN_ROOT_PATH "/usr/bin" )
        message( "-- EMSCRIPTEN_ROOT_PATH was not set, so trying \"${EMSCRIPTEN_ROOT_PATH}\"." )
        message( "--      If this is incorrect, invoke cmake with \"-DEMSCRIPTEN_ROOT_PATH=<path to em++>\"" )
endif()
set( EMCXX "${EMSCRIPTEN_ROOT_PATH}/em++" )
set( EMCC "${EMSCRIPTEN_ROOT_PATH}/emcc" )

# Directory for the emscripten specific version of protobuf. This is probably not the
# same as the C++ library, because it needs to have been compiled to LLVM bitcode not
# to .so libraries.
if( EMSCRIPTEN_PROTOBUF_ROOT_PATH )
        message( "-- EMSCRIPTEN_PROTOBUF_ROOT_PATH set to \"${EMSCRIPTEN_PROTOBUF_ROOT_PATH}\"" )
else()
        set( EMSCRIPTEN_PROTOBUF_ROOT_PATH "/protobuf-2.5.0-emscripten" )
        message( "-- EMSCRIPTEN_PROTOBUF_ROOT_PATH was not set, so trying \"${EMSCRIPTEN_PROTOBUF_ROOT_PATH}\"." )
        message( "--      If this is incorrect, invoke cmake with \"-DEMSCRIPTEN_PROTOBUF_ROOT_PATH=<path to protobuf installation>\"" )
        message( "--      This is the location of a version of protobuf compiled to LLVM bitcode with Emscripten." )
endif()
set( EMSCRIPTEN_PROTOBUF_INCLUDE_DIR "${EMSCRIPTEN_PROTOBUF_ROOT_PATH}/include" )
set( EMSCRIPTEN_PROTOBUF_LIBRARIES "${EMSCRIPTEN_PROTOBUF_ROOT_PATH}/lib" )
# When compiled with emscripten, the protobuf LLVM bitcode is output with the
# original filename (e.g. "libprotobuf.so") even though it's not a library. Hence
# I'll use the find_library cmake command to find the LLVM bitcode file.
find_library( emscripten_libprotobuf NAMES protobuf PATHS ${EMSCRIPTEN_PROTOBUF_LIBRARIES} NO_DEFAULT_PATH )

add_definitions( "-std=c++11" )

include_directories( "${CMAKE_SOURCE_DIR}/include" )
aux_source_directory( "${CMAKE_SOURCE_DIR}/src/tools" source_files )
aux_source_directory( "${CMAKE_SOURCE_DIR}/src/server" source_files )

# Add specific subset of the source files to the client code
list( APPEND client_source_files "${CMAKE_SOURCE_DIR}/src/emscripten/example.cpp" )
set( client_static_files "Controller.html" )

set( client_code_dir "www" )
set( client_destination_file "${client_code_dir}/ClientCode.js" )

foreach( FILE ${client_static_files} )
	set( INPUT_FILE "${CMAKE_SOURCE_DIR}/${FILE}" )
	set( OUTPUT_FILE "${client_code_dir}/${FILE}" )

	add_custom_command( OUTPUT ${OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -E copy "${INPUT_FILE}" "${OUTPUT_FILE}"
		DEPENDS ${INPUT_FILE} )
	list( APPEND CLIENT_STATIC_OUTPUT "${OUTPUT_FILE}" )
endforeach( FILE )

add_custom_command( OUTPUT ${client_destination_file} "${client_destination_file}.mem"
	COMMAND ${EMCXX} -O3 ${client_source_files} -o ${client_destination_file}
		-std=c++11
		--bind
		-D GOOGLE_PROTOBUF_NO_THREAD_SAFETY
		-I ${EMSCRIPTEN_PROTOBUF_INCLUDE_DIR}
		-I ${CMAKE_SOURCE_DIR}
		${emscripten_libprotobuf}
	DEPENDS ${client_source_files} )

#set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )
find_package( OpenSSL REQUIRED )
find_package( Protobuf REQUIRED )
find_package( Communique REQUIRED )

include_directories( "${OPENSSL_INCLUDE_DIR}" )
include_directories( "${PROTOBUF_INCLUDE_DIR}" )
include_directories( "${Communique_INCLUDE_DIRS}" )

# Copy the Communique javascript file into the client code directory
add_custom_command( OUTPUT ${client_code_dir}/Communique.js
	COMMAND ${CMAKE_COMMAND} -E copy "${Communique_INCLUDE_DIRS}/Communique.js" ${client_code_dir}/Communique.js
	DEPENDS "${Communique_INCLUDE_DIRS}/Communique.js" )
list( APPEND CLIENT_STATIC_OUTPUT "${client_code_dir}/Communique.js" )

add_executable( server ${source_files} )
#add_custom_target( "${PROJECT_NAME}ClientCode" ALL DEPENDS ${CLIENT_STATIC_OUTPUT} ${client_destination_file} )

target_link_libraries( server ${OPENSSL_LIBRARIES} )
target_link_libraries( server ${PROTOBUF_LIBRARIES} )
target_link_libraries( server ${Communique_LIBRARIES} )

