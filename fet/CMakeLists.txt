cmake_minimum_required(VERSION 3.28)

project(fet VERSION 7.7.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

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

if(USE_SYSTEM_LOCALE)
    add_compile_definitions(USE_SYSTEM_LOCALE)
endif()

find_package(Qt6 6.10 REQUIRED COMPONENTS Core)
find_package(Qt6 6.10 OPTIONAL_COMPONENTS LinguistTools)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES
                          ar bg bs ca cs da de el en_GB es eu fa fr gl he hu id it ja lt mk ms nl pl pt_BR ro
                          ru si sk sq sr tr uk untranslated uz vi zh_CN zh_TW)

add_subdirectory(src)

if(TARGET Qt::lupdate AND TARGET Qt::lrelease)
    qt_add_translations(fet
        LUPDATE_TARGET lupdate
        LUPDATE_OPTIONS -no-obsolete -locations absolute
        LRELEASE_TARGET lrelease
        TS_FILE_BASE fet
        TS_OUTPUT_DIRECTORY translations
        QM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/translations
        NO_GENERATE_PLURALS_TS_FILE
        QM_FILES_OUTPUT_VARIABLE qm_files
    )
endif()

install(FILES ${qm_files} DESTINATION "${CMAKE_INSTALL_DATADIR}/fet/translations")

install(DIRECTORY examples DESTINATION "${CMAKE_INSTALL_DATADIR}/fet")
install(FILES fet.xml DESTINATION "${CMAKE_INSTALL_DATADIR}/mime/packages")
install(FILES fet.desktop DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES man/fet.1.gz man/fet-cl.1.gz DESTINATION "${CMAKE_INSTALL_DATADIR}/man/man1")
install(DIRECTORY licenses DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/fet")
install(DIRECTORY doc/adding-new-translations DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/fet")
install(DIRECTORY doc/algorithm DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/fet")
install(DIRECTORY doc/installing-compiling-running DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/fet")
install(FILES AUTHORS COPYING ChangeLog README REFERENCES THANKS TODO TRANSLATORS  DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/fet")
install(FILES icons/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps")
install(FILES icons/256x256/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps")
install(FILES icons/512x512/fet.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps")
install(FILES icons/fet.svg DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps")
