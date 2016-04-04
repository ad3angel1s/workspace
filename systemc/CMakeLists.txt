# Set minimum required version equal to what I have
cmake_minimum_required (VERSION 3.2.3)

# Define project name and enable C language
project (SystemCBuild CXX)

# Sources set individually in case
set (HELLO_SOURCES         ./src/hello_sc.cpp )

# Use wildcards with file() and GLOB (best to avoid)
# file ( GLOB SOURCES "./src/*.cpp" )

# Set output directory for binaries
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib" )
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib" )
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin" )

# set_target_properties( <target_name>
#                        PROPERTIES
#                        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib"
#                        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib"
#                        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
# )

# NOTE: We can append _[CONFIG] to the variable/property name to make the output directory apply to a specific configuration
# (the standard values for config are DEBUG, RELEASE, MINSIZEREL and RELWITHDEBINFO).

# check the languages which are currently enabled (read the ENABLED_LANGUAGES property into my own CMAKE_ENABLED_LANGUAGES)
get_property(CMAKE_ENABLED_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
message(STATUS "Enabled languages are: ${CMAKE_ENABLED_LANGUAGES}")

# Set standard versions for CXX or CC based on the enabled languages

# in_list operator is available only on >= 3.3
#
# if("CXX" in_list CMAKE_ENABLED_LANGUAGES)
#    ...
# elseif("C" in_list CMAKE_ENABLED_LANGUAGES)
#    ...
# endif()
#
# ...so use the list/find constructs:
list( FIND CMAKE_ENABLED_LANGUAGES "CXX" _INDEX_CXX )
list( FIND CMAKE_ENABLED_LANGUAGES "C" _INDEX_C )

if(${_INDEX_CXX} GREATER -1)
   include (CheckCXXCompilerFlag)
   check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
   check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

   if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
      message(STATUS "${CMAKE_CXX_COMPILER_ID} set to support C++11")
   elseif(COMPILER_SUPPORTS_CXX0X)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
      message(STATUS "${CMAKE_CXX_COMPILER_ID} set to support C++0x")
   else()
      message(WARNING "The compiler ${CMAKE_CXX_COMPILER_ID} has no C++11/C++0x support. Please use a different C++ compiler")
   endif()

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
   message(STATUS "C++ compiler flags enabled: ${CMAKE_CXX_FLAGS}")
   
elseif(${_INDEX_C} GREATER -1)
 
   include(CheckCCompilerFlag)
   check_c_compiler_flag("-std=c11" COMPILER_SUPPORTS_C11)
   check_c_compiler_flag("-std=c99" COMPILER_SUPPORTS_C99)

   if(COMPILER_SUPPORTS_C11)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
      message(STATUS "${CMAKE_C_COMPILER_ID} set to support C11")
   elseif(COMPILER_SUPPORTS_C99)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
      message(STATUS "${CMAKE_C_COMPILER_ID} set to support C99")
   else()
      message(WARNING "The compiler ${CMAKE_C_COMPILER_ID} has no C11/C99 support. Please use a different C compiler")
   endif()

   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
   message(STATUS "C compiler flags enabled: ${CMAKE_C_FLAGS}")

else()

   message(WARNING "C/C++ standards version not specified, indexes returned are CXX: ${_INDEX_CXX}, C: ${_INDEX_C}")

endif()

# Provided FindSystemC.cmake module 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(SystemC REQUIRED)

include_directories("${SystemC_INCLUDE_DIRS}")

# Define executable
add_executable ( hello_sc ${HELLO_SOURCES} )

# Specify to link against the needed library
target_link_libraries( hello_sc SystemC::SystemC)

