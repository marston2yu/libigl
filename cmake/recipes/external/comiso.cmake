if(TARGET CoMISo::CoMISo)
    return()
endif()

message(STATUS "Third-party: creating target 'CoMISo::CoMISo'")

include(FetchContent)
FetchContent_Declare(
    comiso
    GIT_REPOSITORY https://github.com/marston2yu/CoMISo.git
    GIT_TAG b103cfd204882f71926301e5b1f75ea4d8ad6f11
)

include(eigen)

set(COMISO_NO_INSTALL True)
FetchContent_MakeAvailable(comiso)

# add_library(CoMISo::CoMISo ALIAS CoMISo)

# Copy .hh headers into a subfolder `CoMISo/`
file(GLOB_RECURSE INC_FILES "${comiso_SOURCE_DIR}/*.hh" "${comiso_SOURCE_DIR}/*.cc")
set(output_folder "${CMAKE_CURRENT_BINARY_DIR}/CoMISo/include/CoMISo")
message(VERBOSE "Copying CoMISo headers to '${output_folder}'")
foreach(filepath IN ITEMS ${INC_FILES})
    file(RELATIVE_PATH filename "${comiso_SOURCE_DIR}" ${filepath})
    configure_file(${filepath} "${output_folder}/${filename}" COPYONLY)
endforeach()

target_include_directories(CoMISo PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/CoMISo/include)

set_target_properties(CoMISo PROPERTIES FOLDER ThirdParty)
