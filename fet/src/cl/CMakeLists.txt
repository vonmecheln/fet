cmake_minimum_required(VERSION 3.28)

find_package(Qt6 6.10 REQUIRED COMPONENTS Core)

qt_add_executable(fet-cl
    clsettings.cpp
    exportcl.cpp exportcl.h
    fetcl.cpp fetcl.h
    longtextmessageboxcl.cpp longtextmessageboxcl.h
    messageboxescl.cpp
)

target_link_libraries(fet-cl PRIVATE
    fetengine
    Qt::Core
)

install(TARGETS fet-cl
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET fet-cl
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
