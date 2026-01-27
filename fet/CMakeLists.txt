cmake_minimum_required(VERSION 3.28)

file(STRINGS VERSION fetversion LIMIT_COUNT 1)
message(STATUS "FET VERSION: ${fetversion}")
project(fet VERSION ${fetversion} LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX STREQUAL "")
    file(REAL_PATH "fet-${fetversion}" CMAKE_INSTALL_PREFIX
        BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
message(STATUS "Installation dir: ${CMAKE_INSTALL_PREFIX}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_compile_definitions(QT_NO_FOREACH QT_NO_KEYWORDS)

option(USE_SYSTEM_LOCALE "Use system locale" OFF)
option(COMMAND_LINE_ONLY "Compile only the command line version" OFF)
option(NO_WINDOWS_CONSOLE "Don't add a console on Windows for the command line version" OFF)

if(USE_SYSTEM_LOCALE)
    add_compile_definitions(USE_SYSTEM_LOCALE)
    message(STATUS "Using system locale")
endif()

set(fetdir "fet")
set(docfetdir "doc/fet")
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_INSTALL_DATADIR ".")
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
    set(fetdir ".")
    set(docfetdir ".")
endif()

find_package(Qt6 6.10 REQUIRED COMPONENTS Core)
find_package(Qt6 6.10 QUIET OPTIONAL_COMPONENTS LinguistTools)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES
                          ar bg bs ca cs da de el en_GB es eu fa fr gl he hu id it ja lt mk ms nl pl pt_BR ro
                          ru si sk sq sr tr uk untranslated uz vi zh_CN zh_TW)

unset(w32)
set(fetcl "fet-cl")
if(NO_WINDOWS_CONSOLE AND (CMAKE_SYSTEM_NAME STREQUAL "Windows"))
    message(STATUS "Compiling fet-clw instead of fet-cl")
    set(w32 "WIN32")
    set(fetcl "fet-clw")
endif()

unset(exclude_plugins)
unset(deploy_tool_options_arg)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(exclude_plugins generic iconengines imageformats networkinformation)
    set(deploy_tool_options_arg
        --no-opengl-sw
        --no-system-d3d-compiler
        --no-system-dxc-compiler)
endif()

unset(extra_plugins)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(extra_plugins wayland)
endif()

add_subdirectory(src)

if(TARGET Qt::lupdate AND TARGET Qt::lrelease)
    if(NOT COMMAND_LINE_ONLY)
        set(QT_NO_MISSING_CATALOG_LANGUAGE_WARNING ON)
        qt_add_translations(fet
            LUPDATE_TARGET lupdate
            LUPDATE_OPTIONS -locations absolute -no-obsolete
            LRELEASE_TARGET lrelease
            TS_FILE_BASE fet
            TS_OUTPUT_DIRECTORY translations
            QM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/translations
            NO_GENERATE_PLURALS_TS_FILE
            QM_FILES_OUTPUT_VARIABLE qm_files
            MERGE_QT_TRANSLATIONS
        )
    else()
        message(STATUS "Compiling only the command line version - the 'lupdate' and 'lrelease' targets are not available")
    endif()
else()
    message(STATUS "Linguist tools not found - the 'lupdate' and 'lrelease' targets are not available")
endif()

install(FILES ${qm_files} DESTINATION "${CMAKE_INSTALL_DATADIR}/${fetdir}/translations")
install(DIRECTORY examples DESTINATION "${CMAKE_INSTALL_DATADIR}/${fetdir}")
install(DIRECTORY licenses DESTINATION "${CMAKE_INSTALL_DATADIR}/${docfetdir}")
install(DIRECTORY doc/adding-new-translations DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${fetdir}")
install(DIRECTORY doc/algorithm DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${fetdir}")
install(DIRECTORY doc/installing-compiling-running DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${fetdir}")
install(FILES AUTHORS CONTRIBUTORS COPYING COPYING.FDL ChangeLog README REFERENCES THANKS TODO TRANSLATORS DESTINATION "${CMAKE_INSTALL_DATADIR}/${docfetdir}")
if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Windows"))
    install(FILES man/fet-cl.1.gz DESTINATION "${CMAKE_INSTALL_DATADIR}/man/man1")
    if(NOT COMMAND_LINE_ONLY)
        install(FILES man/fet.1.gz DESTINATION "${CMAKE_INSTALL_DATADIR}/man/man1")
        install(FILES fet.xml DESTINATION "${CMAKE_INSTALL_DATADIR}/mime/packages")
        install(FILES fet.desktop DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
        install(FILES icons/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps")
        install(FILES icons/256x256/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps")
        install(FILES icons/512x512/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps")
        install(FILES icons/fet.svg DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps")
    endif()
endif()
