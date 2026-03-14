cmake_minimum_required(VERSION 3.16)
project(fbs_stub_generator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build the generator first
add_subdirectory(generator)


#-------------------------------------------------------------------
# Custom command to run the generator before main build - generates .fbs files
#-------------------------------------------------------------------
add_custom_command(
    OUTPUT fbs_schemas/dummy.txt
    COMMAND ${CMAKE_COMMAND} -E make_directory fbs_schemas
    COMMAND fbs_generator
    COMMAND ${CMAKE_COMMAND} -E touch fbs_schemas/dummy.txt
    DEPENDS fbs_generator
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating FlatBuffers .fbs files..."
)

add_custom_target(generate_fbs ALL
    DEPENDS fbs_schemas/dummy.txt
)
#-------------------------------------------------------------------
# END
#-------------------------------------------------------------------

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)
add_dependencies(${PROJECT_NAME} generate_fbs)

# Include FlatBuffers headers (assume you have it in flatbuffers/)
set(FLAT_BUFFER_INSTALL_DIR "D:/BNP_Projects/flatbuffers")
include_directories(${FLAT_BUFFER_INSTALL_DIR}/include)
include_directories(${FLAT_BUFFER_INSTALL_DIR}/src/generated)

#--------------------------------------------------------------------
# Auto-generate stubs
#--------------------------------------------------------------------

# 1. Collect all .fbs files
file(GLOB FBS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/fbs_schemas/*.fbs")

# 2. Set output folder for generated C++ headers
set(FBS_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated_flatbuffers")
file(MAKE_DIRECTORY ${FBS_GEN_DIR})

set(FLATC_EXE "D:/BNP_Projects/flatbuffers/build/Release/flatc.exe")

# 3. Generate C++ headers from each .fbs file
foreach(fbs_file ${FBS_FILES})
    get_filename_component(fname ${fbs_file} NAME_WE)  # basename without extension

    add_custom_command(
        OUTPUT ${FBS_GEN_DIR}/${fname}.h ${FBS_GEN_DIR}/${fname}.cpp
        COMMAND ${FLATC_EXE} --cpp -o ${FBS_GEN_DIR} ${fbs_file}
        DEPENDS ${fbs_file}
        COMMENT "Generating C++ code from ${fbs_file}"
        VERBATIM
    )

    list(APPEND GENERATED_FLATBUFFERS ${FBS_GEN_DIR}/${fname}.h ${FBS_GEN_DIR}/${fname}.cpp)
endforeach()

# 4. Add a custom target so it runs when building
add_custom_target(generate_flatbuffers1 ALL
    DEPENDS ${GENERATED_FLATBUFFERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FBS_GEN_DIR})
add_dependencies(${PROJECT_NAME} generate_flatbuffers1)