# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
	catch.hpp
)

set(SOURCE_FILES
	Catch.cpp
	StudentTests.cpp
	GradedTests.cpp   
)

# Don't change this
add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests src tbb)

if (WIN32)
   add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TBB_DLL_PATH}/tbb.dll"
        $<TARGET_FILE_DIR:tests>)
   add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TBB_DLL_PATH}/tbb_debug.dll"
        $<TARGET_FILE_DIR:tests>)
endif()
