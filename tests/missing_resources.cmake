# Exercise startup from a different cwd without requiring a window system.
file(MAKE_DIRECTORY "${TEST_DIRECTORY}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "BIRDVIS_RESOURCES=${TEST_DIRECTORY}" "QT_QPA_PLATFORM=unavailable-test-platform"
        "${BIRDVIS_EXECUTABLE}"
    WORKING_DIRECTORY "${TEST_DIRECTORY}"
    RESULT_VARIABLE status OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(NOT "${status}" STREQUAL "1")
    message(FATAL_ERROR "Expected resource error exit 1, got ${status}: ${output}${error}")
endif()
foreach(expected "Missing BirdVis resources in" "BIRDVIS_RESOURCES" "usa_st.shp" "counties.shp" "fonts/arial_200.sgi")
    string(FIND "${error}" "${expected}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Missing diagnostic '${expected}': ${error}")
    endif()
endforeach()
