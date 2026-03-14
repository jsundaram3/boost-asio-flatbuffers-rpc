cmake_minimum_required(VERSION 3.16)
project(fbs_generator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#[[
# -------------------------------------------------------------------------
# Clean previously generated .fbs files - Executes suring cmake .. command
# -------------------------------------------------------------------------
message(STATUS "Cleanup Workspace before build")
set(FBS_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fbs_schemas")

# Glob all .fbs files in the output folder
file(GLOB FBS_FILES "${FBS_OUTPUT_DIR}/*.fbs" "${FBS_OUTPUT_DIR}/dummy.txt")

# Remove them if any exist
if(FBS_FILES)
    file(REMOVE ${FBS_FILES})
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/entities/generated_entity_includes.h)
    message(STATUS "Removed previous .fbs files in ${FBS_OUTPUT_DIR}")
endif()
#-------------------------------------------------------------------
# Clean Completed
#-------------------------------------------------------------------
]]
#-------------------------------------------------------------------
#generate all the header includes
#-------------------------------------------------------------------

file(GLOB ENTITY_HEADERS  # Find all entity headers
    ${CMAKE_SOURCE_DIR}/entities/*.h
)
# Remove reflect.h, generated_entity_includes.h if present
list(REMOVE_ITEM ENTITY_HEADERS
    ${CMAKE_SOURCE_DIR}/entities/reflect.h
    ${CMAKE_SOURCE_DIR}/entities/generated_entity_includes.h
)
# Create generated include file
set(GENERATED_HEADER ${CMAKE_SOURCE_DIR}/entities/generated_entity_includes.h)
file(WRITE ${GENERATED_HEADER} "// Auto-generated file - Do not modify\n")
foreach(header ${ENTITY_HEADERS})
    get_filename_component(name ${header} NAME)
    file(APPEND ${GENERATED_HEADER} "#include \"${name}\"\n")
endforeach()


# ---------------------- Source Files ----------------------
set(SOURCES
    fbs_generator.cpp
    #fbs_enum_map_generator.cpp
)


include_directories(${CMAKE_SOURCE_DIR}/entities)
add_executable(${PROJECT_NAME} ${SOURCES})

# run generator after build
# add_custom_target(generate_flatbuffers ALL
#     COMMAND $<TARGET_FILE:fbs_generator>
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#     DEPENDS fbs_generator
#     COMMENT "Generating FlatBuffers C++ stubs"
# )