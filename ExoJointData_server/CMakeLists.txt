project( ExoJointData C )
cmake_minimum_required( VERSION 2.8 )
cmake_policy( SET CMP0015 NEW )

set( SRCS src/exoJointData_server.c src/exoJointData_threadStopFunc.c src/exoJointData_threadPkgRecv.c 
			src/exoJointData_threadPkgSave.c src/exoJointData_threadPrint2Bash.c src/exoJointData_auxFunction.c)

find_package( Threads )

add_executable(exoJointData_server ${SRCS} )

target_link_libraries (exoJointData_server ${CMAKE_THREAD_LIBS_INIT})

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

set_target_properties( exoJointData_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
