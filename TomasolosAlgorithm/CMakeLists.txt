cmake_minimum_required(VERSION 3.16)

FILE(GLOB SOURCES ./src/*.cpp)

FILE(GLOB HEADERS ./headers/*.h)

add_executable(
    TomasolosAlgorithm
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(
    TomasolosAlgorithm
    PRIVATE
    ./headers/
)

if(WIN32)
    set(OS_LIBRARIES
        opengl32
    )
else()
    set(OS_LIBRARIES
        GL
    )
endif()

target_link_libraries(TomasolosAlgorithm
    PRIVATE
    ${OS_LIBRARIES}
    sfmlimgui
    ImGuiFileDialog
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)


set_target_properties(TomasolosAlgorithm PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(TomasolosAlgorithm PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
