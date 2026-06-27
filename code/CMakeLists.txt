cmake_minimum_required(VERSION 3.16)

project(oop_project_doom_alike VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(doom-alike
    main.cpp
    main.hpp
    character.cpp
    character.hpp
    game_logic.cpp
    game_logic.hpp
    render.cpp
    render.hpp
    window.cpp
    window.hpp
    walls.hpp
    map.cpp
    map.hpp
    scene3d.cpp
    scene3d.hpp
    constants.hpp

)

target_link_libraries(doom-alike PRIVATE Qt6::Core Qt6::Widgets)

# --- Deploy the correct Qt runtime DLLs next to the executable (Windows) ---
# DLLs in the exe's own folder take precedence over PATH, so this stops Windows
# from loading mismatched Qt DLLs that other tools put on PATH (e.g. STM32CubeCLT)
# and fixes "entry point qt_version_tag_6_10 could not be located" at startup.

# I personally got troubles because of that STM32CubeCLT installated in my system, had to
# include those .dlls 
if(WIN32 AND TARGET Qt6::qmake)
    get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
    find_program(WINDEPLOYQT_EXE windeployqt HINTS "${_qt_bin_dir}")
    if(WINDEPLOYQT_EXE)
        add_custom_command(TARGET doom-alike POST_BUILD
            COMMAND "${WINDEPLOYQT_EXE}" --no-translations "$<TARGET_FILE:doom-alike>"
            COMMENT "Deploying Qt runtime DLLs next to doom-alike.exe"
            VERBATIM)
    endif()
endif()