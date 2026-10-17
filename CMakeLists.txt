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
    AreaEstimator.cpp
    Array.cpp
    Curve.cpp
    DataSet.cpp
    Dataflow.cpp
    DataflowTypes.cpp
    GLContext.cpp
    Global.cpp
    Labeling.cpp
    RectangleLayout.cpp
    Spreadsheet.cpp
    Environment.cpp
    SimpleEnvironment.cpp
    Matrix.cpp
    Vector.cpp
    ColormapModules.cpp
    BasicModules.cpp
    modules.cpp
    Rotation.cpp
    Regions.cpp
    PlotModules.cpp
    EventSystem.cpp
    Locations.cpp
    MeasureSet.cpp
    RenderSystem.cpp
    Species.cpp
    TagCloudModules.cpp
    Tokenizer.cpp
    View.cpp
    Color.cpp
    ViewRect.cpp
    Colormap1d.cpp
    gl_util.cpp
    bvdata.cpp
    csv2grid.cpp
    datatypes.cpp
    Texture.cpp
    font.c
    image.c
    readrgb.c
    Triangulator.cpp
    Util.cpp
)
target_include_directories(birdvis_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${FREEIMAGE_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(birdvis_core PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets
    OpenGL::GL OpenGL::GLU GLEW::GLEW GDAL::GDAL ${FREEIMAGE_LIBRARY} EmbeddedR)
target_compile_definitions(birdvis_core PUBLIC GL_SILENCE_DEPRECATION
    BIRDVIS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    BIRDVIS_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
    BIRDVIS_R_HOME="${R_HOME}"
    BIRDVIS_R_LIBS="${R_PACKAGE_LIBRARIES}"
    BIRDVIS_IMAGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/images")
add_executable(birdvis app.cpp)
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
