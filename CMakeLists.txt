# Require CMAKE 3.1 or higher
cmake_minimum_required(VERSION 3.1)

# Project name
project(touchdecodeTest)

# Build for C++20 if we can
set(CMAKE_CXX_STANDARD 14)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

set(BOOST_ROOT "/home/ansible/dev/cpp/boost/boost_1_75_0")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")

# Turn on any compiler-specific flags
if (WIN32)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
else()
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	# if ("$ENV{SANITIZE}" STREQUAL "ON")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	# endif()
	
	if(APPLE)
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Debug] "YES")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=MinSizeRel] "NO")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=RelWithDebInfo] "YES")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Release] "NO")
	endif()
endif()


#if(Boost_FOUND)

# Any source files in this directory
   set(SOURCE_FILES src/main/main.cpp)

	set(LIBRARY_NAME touchdecoder)
	include(FetchContent)

#	set(BOOST_ROOT "/home/ansible/dev/cpp/boost/boost_1_75_0")
#	include_directories(${BOOST_ROOT} include)
	
	# Where any include files are
	include_directories(src/lib ${BOOST_ROOT})

	# Subdirectories to build
	add_subdirectory(src/lib)
	add_subdirectory(src/tests)

	# Name of executable
	add_executable(${PROJECT_NAME} ${SOURCE_FILES})

	# Link
	target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})
#endif()



