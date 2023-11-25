cmake_minimum_required(VERSION 3.16)
project(Sim LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

file(GLOB SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/gui/*.cpp")
add_executable(Sim ${SOURCES})

target_link_libraries(Sim PRIVATE sfml-graphics)
target_compile_features(Sim PRIVATE cxx_std_20)

if(WIN32)
    add_custom_command(
        TARGET Sim
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Sim>
        VERBATIM)
endif()

install(TARGETS Sim)