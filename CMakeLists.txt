cmake_minimum_required(VERSION 3.15)
project(boost_asio_flatbuffers_rpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Boost include directory
set(BOOST_ROOT "D:/BNP_Projects/boost")
set(BOOST_LIBRARYDIR "D:/BNP_Projects/boost/stage/lib")
# Find Boost components
find_package(Boost 1.90 COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(server server/main.cpp)
    target_link_libraries(server PRIVATE ${Boost_LIBRARIES} Threads::Threads)   
else()
    message(WARNING "Boost not found. Building mock client/server binaries without network support.")
endif()

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7 or later
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

#flat Buffer configuration
set(FLATBUFFERS_ROOT "D:\BNP_Projects\flatbuffers") # Path to FlatBuffers root
add_subdirectory(${FLATBUFFERS_ROOT} flatbuffers_build) # Add FlatBuffers as CMake

#####
# Schema file
set(SCHEMA_FILE "${CMAKE_SOURCE_DIR}/monster.fbs")
set(GENERATED_CPP "${CMAKE_BINARY_DIR}/monster_generated.h")

# Command to generate C++ headers
add_custom_command(
    OUTPUT ${GENERATED_CPP}
    COMMAND flatc --cpp -o ${CMAKE_BINARY_DIR} ${SCHEMA_FILE}
    DEPENDS ${SCHEMA_FILE}
    COMMENT "Generating FlatBuffers code from ${SCHEMA_FILE}"
)

# Make sure your executable depends on the generated file
#add_executable(my_app main.cpp ${GENERATED_CPP})

####

include_directories(
    generated
    rpc
)

add_executable(client client/main.cpp)
target_link_libraries(client
        ${Boost_LIBRARIES} # Link boost asio
        flatbuffers # Link FlatBuffers
    )
    

