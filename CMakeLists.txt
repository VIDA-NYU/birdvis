cmake_minimum_required(VERSION 3.21)
project(BirdVis LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
include(CTest)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GDAL REQUIRED)
find_package(Boost CONFIG REQUIRED)
find_path(FREEIMAGE_INCLUDE_DIR FreeImage.h REQUIRED)
find_library(FREEIMAGE_LIBRARY NAMES freeimage REQUIRED)
include(cmake/FindEmbeddedR.cmake)
add_library(birdvis_core STATIC
    src/data/AreaEstimator.cpp
    src/math/Array.cpp
    src/math/Curve.cpp
    src/data/DataSet.cpp
    src/dataflow/Dataflow.cpp
    src/dataflow/DataflowTypes.cpp
    src/render/GLContext.cpp
    src/data/Global.cpp
    src/render/Labeling.cpp
    src/ui/RectangleLayout.cpp
    src/ui/Spreadsheet.cpp
    src/env/Environment.cpp
    src/env/SimpleEnvironment.cpp
    src/math/Matrix.cpp
    src/math/Vector.cpp
    src/modules/ColormapModules.cpp
    src/modules/BasicModules.cpp
    src/modules/modules.cpp
    src/math/Rotation.cpp
    src/data/Regions.cpp
    src/modules/PlotModules.cpp
    src/ui/EventSystem.cpp
    src/data/Locations.cpp
    src/data/MeasureSet.cpp
    src/render/RenderSystem.cpp
    src/data/Species.cpp
    src/modules/TagCloudModules.cpp
    src/math/Tokenizer.cpp
    src/ui/View.cpp
    src/math/Color.cpp
    src/ui/ViewRect.cpp
    src/math/Colormap1d.cpp
    src/render/gl_util.cpp
    src/data/bvdata.cpp
    src/data/csv2grid.cpp
    src/data/datatypes.cpp
    src/render/Texture.cpp
    src/render/font.c
    src/render/image.c
    src/render/readrgb.c
    src/render/Triangulator.cpp
    src/math/Util.cpp
)
target_include_directories(birdvis_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app ${CMAKE_CURRENT_SOURCE_DIR}/src/data ${CMAKE_CURRENT_SOURCE_DIR}/src/dataflow ${CMAKE_CURRENT_SOURCE_DIR}/src/env ${CMAKE_CURRENT_SOURCE_DIR}/src/math ${CMAKE_CURRENT_SOURCE_DIR}/src/modules ${CMAKE_CURRENT_SOURCE_DIR}/src/render ${CMAKE_CURRENT_SOURCE_DIR}/src/ui ${FREEIMAGE_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(birdvis_core PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets
    OpenGL::GL OpenGL::GLU GLEW::GLEW GDAL::GDAL ${FREEIMAGE_LIBRARY} EmbeddedR)
target_compile_definitions(birdvis_core PUBLIC GL_SILENCE_DEPRECATION
    BIRDVIS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    BIRDVIS_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
    BIRDVIS_R_HOME="${R_HOME}"
    BIRDVIS_R_LIBS="${R_PACKAGE_LIBRARIES}"
    BIRDVIS_IMAGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/images")
add_executable(birdvis src/app/app.cpp)
target_link_libraries(birdvis PRIVATE birdvis_core)
if(BUILD_TESTING)
    add_test(NAME missing_resources COMMAND ${CMAKE_COMMAND}
        -DBIRDVIS_EXECUTABLE=$<TARGET_FILE:birdvis>
        -DTEST_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}/missing-resource-test
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/missing_resources.cmake)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(native_check tests/native_check.cpp)
    target_link_libraries(native_check PRIVATE birdvis_core Qt6::Test)
    add_executable(resource_check tests/resource_check.cpp)
    target_link_libraries(resource_check PRIVATE birdvis_core)
    add_test(NAME resource_decoding COMMAND resource_check)
    add_executable(numerical_check tests/numerical_check.cpp)
    target_link_libraries(numerical_check PRIVATE birdvis_core)
    add_test(NAME scientific_calculations COMMAND numerical_check)
    set_tests_properties(scientific_calculations PROPERTIES ENVIRONMENT "R_HOME=${R_HOME};R_LIBS_USER=${R_PACKAGE_LIBRARIES}")
    add_executable(r_probe tests/r_probe.cpp)
    target_link_libraries(r_probe PRIVATE EmbeddedR)
    add_test(NAME bundled_r_data COMMAND r_probe "${CMAKE_CURRENT_SOURCE_DIR}/data")
    set_tests_properties(bundled_r_data PROPERTIES
        ENVIRONMENT "R_HOME=${R_HOME};R_LIBS_USER=${R_PACKAGE_LIBRARIES}" TIMEOUT 180)
    add_executable(gl_probe tests/gl_probe.cpp)
    target_link_libraries(gl_probe PRIVATE Qt6::Widgets Qt6::OpenGLWidgets OpenGL::GL GLEW::GLEW)
    target_compile_definitions(gl_probe PRIVATE GL_SILENCE_DEPRECATION)
    # Needs a native display; intentionally not registered as a headless CTest.
endif()
