cmake_minimum_required(VERSION 3.28)

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Gui Network Widgets)
find_package(Qt6 6.10 OPTIONAL_COMPONENTS PrintSupport)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/icons/fet.rc")

set(MACOSX_BUNDLE_ICON_FILE fet.icns)
set(app_icon_macos "${CMAKE_SOURCE_DIR}/icons/fet.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

qt_add_executable(fet WIN32 MACOSX_BUNDLE
    ${app_icon_resource_windows}
    ${app_icon_macos}
    activitiesform.cpp activitiesform.h
    activitiesform_template.ui
    activitiesroomsstatisticsform.cpp activitiesroomsstatisticsform.h
    activitiesroomsstatisticsform_template.ui
    activitiestagsform.cpp activitiestagsform.h
    activitiestagsform_template.ui
    activityplanningconfirmationform.cpp activityplanningconfirmationform.h
    activityplanningconfirmationform_template.ui
    activityplanningform.cpp activityplanningform.h
    activitytagsform.cpp activitytagsform.h
    activitytagsform_template.ui
    addactivityform.cpp addactivityform.h
    addactivityform_template.ui
    addbuildingform.cpp addbuildingform.h
    addbuildingform_template.ui
    addexistingstudentsgroupsform.cpp addexistingstudentsgroupsform.h
    addexistingstudentsgroupsform_template.ui
    addexistingstudentssubgroupsform.cpp addexistingstudentssubgroupsform.h
    addexistingstudentssubgroupsform_template.ui
    addgroupactivitiesininitialorderitemform.cpp addgroupactivitiesininitialorderitemform.h
    addgroupactivitiesininitialorderitemform_template.ui
    addormodifyspaceconstraint.cpp addormodifyspaceconstraint.h
    addormodifytimeconstraint.cpp addormodifytimeconstraint.h
    addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.cpp addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h
    addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform_template.ui
    addremovemultipleconstraintsactivitiessamestartinghourform.cpp addremovemultipleconstraintsactivitiessamestartinghourform.h
    addremovemultipleconstraintsactivitiessamestartinghourform_template.ui
    addroomform.cpp addroomform.h
    addroomform_template.ui
    addstudentsgroupform.cpp addstudentsgroupform.h
    addstudentsgroupform_template.ui
    addstudentssubgroupform.cpp addstudentssubgroupform.h
    addstudentssubgroupform_template.ui
    addstudentsyearform.cpp addstudentsyearform.h
    addstudentsyearform_template.ui
    addteacherform.cpp addteacherform.h
    addteacherform_template.ui
    advancedfilterform.cpp advancedfilterform.h
    advancedlockunlockform.cpp advancedlockunlockform.h
    allspaceconstraintsform.cpp allspaceconstraintsform.h
    allspaceconstraintsform_template.ui
    alltimeconstraintsform.cpp alltimeconstraintsform.h
    alltimeconstraintsform_template.ui
    buildingsform.cpp buildingsform.h
    buildingsform_template.ui
    changemindaysselectivelyform.cpp changemindaysselectivelyform.h
    changemindaysselectivelyform_template.ui
    changeminhalfdaysselectivelyform.cpp changeminhalfdaysselectivelyform.h
    changeminhalfdaysselectivelyform_template.ui
    commentsform.cpp commentsform.h
    commentsform_template.ui
    daysform.cpp daysform.h
    daysform_template.ui
    exportinterface.cpp exportinterface.h
    fet.cpp fet.h
    fetmainform.cpp fetmainform.h
    fetmainform_template.ui
    getmodefornewfileform.cpp getmodefornewfileform.h
    getmodefornewfileform_template.ui
    groupactivitiesininitialorderitemsform.cpp groupactivitiesininitialorderitemsform.h
    groupactivitiesininitialorderitemsform_template.ui
    groupsform.cpp groupsform.h
    groupsform_template.ui
    helpaboutform.cpp helpaboutform.h
    helpaboutform_template.ui
    helpaboutlibrariesform.cpp helpaboutlibrariesform.h
    helpaboutlibrariesform_template.ui
    helpalgeriaform.cpp helpalgeriaform.h
    helpalgeriaform_template.ui
    helpblockplanningform.cpp helpblockplanningform.h
    helpblockplanningform_template.ui
    helpfaqform.cpp helpfaqform.h
    helpfaqform_template.ui
    helpinstructionsform.cpp helpinstructionsform.h
    helpinstructionsform_template.ui
    helpmoroccoform.cpp helpmoroccoform.h
    helpmoroccoform_template.ui
    helponstudentsminhoursdaily.cpp helponstudentsminhoursdaily.h
    helptermsform.cpp helptermsform.h
    helptermsform_template.ui
    helptipsform.cpp helptipsform.h
    helptipsform_template.ui
    hoursform.cpp hoursform.h
    hoursform_template.ui
    humanreadabledataform.cpp humanreadabledataform.h
    humanreadabledataform_template.ui
    importinterface.cpp importinterface.h
    institutionnameform.cpp institutionnameform.h
    institutionnameform_template.ui
    listofrelatedspaceconstraintsform.cpp listofrelatedspaceconstraintsform.h
    listofrelatedspaceconstraintsform_template.ui
    listofrelatedtimeconstraintsform.cpp listofrelatedtimeconstraintsform.h
    listofrelatedtimeconstraintsform_template.ui
    listspaceconstraints.cpp listspaceconstraints.h
    listtimeconstraints.cpp listtimeconstraints.h
    longtextmessagebox.cpp longtextmessagebox.h
    longtextmessagebox_auxiliary.h
    messageboxesinterface.cpp
    modifyactivityform.cpp modifyactivityform.h
    modifyactivityform_template.ui
    modifybuildingform.cpp modifybuildingform.h
    modifybuildingform_template.ui
    modifydayform.cpp modifydayform.h
    modifydayform_template.ui
    modifygroupactivitiesininitialorderitemform.cpp modifygroupactivitiesininitialorderitemform.h
    modifygroupactivitiesininitialorderitemform_template.ui
    modifyhourform.cpp modifyhourform.h
    modifyhourform_template.ui
    modifyrealdayform.cpp modifyrealdayform.h
    modifyrealdayform_template.ui
    modifyrealhourform.cpp modifyrealhourform.h
    modifyrealhourform_template.ui
    modifyroomform.cpp modifyroomform.h
    modifyroomform_template.ui
    modifystudentsgroupform.cpp modifystudentsgroupform.h
    modifystudentsgroupform_template.ui
    modifystudentssubgroupform.cpp modifystudentssubgroupform.h
    modifystudentssubgroupform_template.ui
    modifystudentsyearform.cpp modifystudentsyearform.h
    modifystudentsyearform_template.ui
    modifysubactivityform.cpp modifysubactivityform.h
    modifysubactivityform_template.ui
    modifytimeconstraints.cpp modifytimeconstraints.h
    modifyteacherform.cpp modifyteacherform.h
    modifyteacherform_template.ui
    notificationcommandform.cpp notificationcommandform.h
    notificationcommandform_template.ui
    randomseedform.cpp randomseedform.h
    randomseedform_template.ui
    removeredundantconfirmationform.cpp removeredundantconfirmationform.h
    removeredundantconfirmationform_template.ui
    removeredundantform.cpp removeredundantform.h
    removeredundantform_template.ui
    restoredatastateform.cpp restoredatastateform.h
    restoredatastateform_template.ui
    roommakeeditvirtualform.cpp roommakeeditvirtualform.h
    roommakeeditvirtualform_template.ui
    roomsform.cpp roomsform.h
    roomsform_template.ui
    savetimetableconfirmationform.cpp savetimetableconfirmationform.h
    savetimetableconfirmationform_template.ui
    settingsautosaveform.cpp settingsautosaveform.h
    settingsautosaveform_template.ui
    settingsdatatoprintintimetablesform.cpp settingsdatatoprintintimetablesform.h
    settingsdatatoprintintimetablesform_template.ui
    settingsrestoredatafromdiskform.cpp settingsrestoredatafromdiskform.h
    settingsrestoredatafromdiskform_template.ui
    settingsrestoredatafrommemoryform.cpp settingsrestoredatafrommemoryform.h
    settingsrestoredatafrommemoryform_template.ui
    settingsstyleandcolorschemeform.cpp settingsstyleandcolorschemeform.h
    settingsstyleandcolorschemeform_template.ui
    settingstimetablehtmllevelform.cpp settingstimetablehtmllevelform.h
    settingstimetablehtmllevelform_template.ui
    sparseitemmodel.cpp sparseitemmodel.h
    sparsetableview.cpp sparsetableview.h
    splityearform.cpp splityearform.h
    splityearform_template.ui
    spreadconfirmationform.cpp spreadconfirmationform.h
    spreadconfirmationform_template.ui
    spreadmindaysconstraintsfivedaysform.cpp spreadmindaysconstraintsfivedaysform.h
    spreadmindaysconstraintsfivedaysform_template.ui
    statisticsexport.cpp statisticsexport.h
    statisticsprintform.cpp statisticsprintform.h
    studentscomboboxesstyleform.cpp studentscomboboxesstyleform.h
    studentscomboboxesstyleform_template.ui
    studentsnotavailabletimestimehorizontalform.cpp studentsnotavailabletimestimehorizontalform.h
    studentsnotavailabletimestimehorizontalform_template.ui
    studentsstatisticsform.cpp studentsstatisticsform.h
    studentsstatisticsform_template.ui
    subactivitiesform.cpp subactivitiesform.h
    subactivitiesform_template.ui
    subgroupsform.cpp subgroupsform.h
    subgroupsform_template.ui
    subjectsform.cpp subjectsform.h
    subjectsform_template.ui
    subjectsstatisticsform.cpp subjectsstatisticsform.h
    subjectsstatisticsform_template.ui
    teachersform.cpp teachersform.h
    teachersform_template.ui
    teachersnotavailabletimestimehorizontalform.cpp teachersnotavailabletimestimehorizontalform.h
    teachersnotavailabletimestimehorizontalform_template.ui
    teachersstatisticsform.cpp teachersstatisticsform.h
    teachersstatisticsform_template.ui
    teachersubjectsqualificationsform.cpp teachersubjectsqualificationsform.h
    teachersubjectsqualificationsform_template.ui
    termsform.cpp termsform.h
    termsform_template.ui
    timetablegenerateform.cpp timetablegenerateform.h
    timetablegenerateform_template.ui
    timetablegeneratemultipleform.cpp timetablegeneratemultipleform.h
    timetablegeneratemultipleform_template.ui
    timetableprintform.cpp timetableprintform.h
    timetableshowconflictsform.cpp timetableshowconflictsform.h
    timetableshowconflictsform_template.ui
    timetablestowriteondiskform.cpp timetablestowriteondiskform.h
    timetablestowriteondiskform_template.ui
    timetableviewroomsdayshorizontalform.cpp timetableviewroomsdayshorizontalform.h
    timetableviewroomsdayshorizontalform_template.ui
    timetableviewroomsdaysverticalform.cpp timetableviewroomsdaysverticalform.h
    timetableviewroomsdaysverticalform_template.ui
    timetableviewroomstimehorizontalform.cpp timetableviewroomstimehorizontalform.h
    timetableviewroomstimehorizontalform_template.ui
    timetableviewstudentsdayshorizontalform.cpp timetableviewstudentsdayshorizontalform.h
    timetableviewstudentsdayshorizontalform_template.ui
    timetableviewstudentsdaysverticalform.cpp timetableviewstudentsdaysverticalform.h
    timetableviewstudentsdaysverticalform_template.ui
    timetableviewstudentstimehorizontalform.cpp timetableviewstudentstimehorizontalform.h
    timetableviewstudentstimehorizontalform_template.ui
    timetableviewteachersdayshorizontalform.cpp timetableviewteachersdayshorizontalform.h
    timetableviewteachersdayshorizontalform_template.ui
    timetableviewteachersdaysverticalform.cpp timetableviewteachersdaysverticalform.h
    timetableviewteachersdaysverticalform_template.ui
    timetableviewteacherstimehorizontalform.cpp timetableviewteacherstimehorizontalform.h
    timetableviewteacherstimehorizontalform_template.ui
    utilities.cpp utilities.h
    yearsform.cpp yearsform.h
    yearsform_template.ui
)

target_link_libraries(fet PRIVATE
    fetengine
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Widgets
)

if(TARGET Qt::PrintSupport)
    target_link_libraries(fet PRIVATE
        Qt::PrintSupport
    )
endif()

set_source_files_properties("../../icons/fet.png"
    PROPERTIES QT_RESOURCE_ALIAS "images/appicon.png"
)

set(interface_images_resource_files
    "../../icons/fet.png"
    "images/group.png"
    "images/locked.png"
    "images/subgroup.png"
    "images/unlocked.png"
)

qt_add_resources(fet "interface_images"
    PREFIX
        "/"
    BASE
        "."
    FILES
        ${interface_images_resource_files}
)

install(TARGETS fet
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET fet
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
    INCLUDE_PLUGINS ${extra_plugins}
    EXCLUDE_PLUGIN_TYPES ${exclude_plugins}
    DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
)

install(SCRIPT ${deploy_script})
