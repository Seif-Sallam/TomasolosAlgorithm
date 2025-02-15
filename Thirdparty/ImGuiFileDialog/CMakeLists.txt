cmake_minimum_required(VERSION 3.16)

add_library(
    ImGuiFileDialog
    STATIC
    dirent/dirent.h
    stb/stb_image.h
    stb/stb_image_resize.h
    ImGuiFileDialog.cpp
    ImGuiFileDialog.h
    ImGuiFileDialogConfig.h
)

target_link_libraries(
    ImGuiFileDialog
    PRIVATE
    sfmlimgui
)