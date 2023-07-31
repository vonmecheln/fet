//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef FETMAINFORM_H
#define FETMAINFORM_H

#include <QtGlobal>

#include "ui_fetmainform_template.h"

#include <QMutex>
#include <QThread>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QResizeEvent>
#include <QCloseEvent>

#include <QString>
#include <QStringList>

#include <QAction>
#include <QMenu>

#include <QMap>

#include <QList>
#include <QHash>

class QNetworkAccessManager;
class QNetworkReply;

extern const QString COMPANY;
extern const QString PROGRAM;

const int MAX_RECENT_FILES=10;

class FetMainForm: public QMainWindow, public Ui::FetMainForm_template
{
	Q_OBJECT
	
private:
	//
	QMenu* menuA_teacher_time_constraints;
	QMenu* menuAll_teachers_time_constraints;
	//
	QMenu* menuA_students_set_time_constraints;
	QMenu* menuAll_students_time_constraints;
	//
	QMenu* menuA_teacher_1_time_constraints; //for Mornings-Afternoons mode
	QMenu* menuA_teacher_2_time_constraints;
	QMenu* menuAll_teachers_1_time_constraints;
	QMenu* menuAll_teachers_2_time_constraints;
	//
	QMenu* menuA_students_set_1_time_constraints;
	QMenu* menuA_students_set_2_time_constraints;
	QMenu* menuAll_students_1_time_constraints;
	QMenu* menuAll_students_2_time_constraints;

	QAction* dataTimeConstraintsActivitiesPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsActivitiesSameStartingTimeAction;
	QAction* dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsTeacherNotAvailableTimesAction;
	QAction* dataTimeConstraintsBasicCompulsoryTimeAction;
	QAction* dataTimeConstraintsStudentsSetNotAvailableTimesAction;
	QAction* dataTimeConstraintsBreakTimesAction;
	QAction* dataTimeConstraintsTeacherMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxHoursDailyAction;
	QAction* dataTimeConstraintsActivityPreferredStartingTimeAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsActivitiesNotOverlappingAction;
	QAction* dataTimeConstraintsActivityTagsNotOverlappingAction;
	QAction* dataTimeConstraintsMinDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsMinHalfDaysBetweenActivitiesAction;
	QAction* dataSpaceConstraintsBasicCompulsorySpaceAction;
	QAction* dataSpaceConstraintsRoomNotAvailableTimesAction;
	QAction* dataSpaceConstraintsTeacherRoomNotAvailableTimesAction;
	QAction* dataSpaceConstraintsActivityPreferredRoomAction;
	QAction* dataTimeConstraintsActivitiesSameStartingHourAction;
	QAction* dataSpaceConstraintsActivityPreferredRoomsAction;
	QAction* dataSpaceConstraintsStudentsSetHomeRoomAction;
	QAction* dataSpaceConstraintsStudentsSetHomeRoomsAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction;
	//
	//2021-12-15
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction;
	//
	QAction* dataTimeConstraintsTwoActivitiesConsecutiveAction;
	QAction* dataTimeConstraintsActivityEndsStudentsDayAction;
	QAction* dataTimeConstraintsActivityEndsTeachersDayAction;
	QAction* dataTimeConstraintsActivityBeginsStudentsDayAction;
	QAction* dataTimeConstraintsActivityBeginsTeachersDayAction;
	QAction* dataTimeConstraintsTeachersMinHoursDailyAction;
	QAction* dataTimeConstraintsTeacherMinHoursDailyAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerDayAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerDayAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxSpanPerDayAction;
	QAction* dataTimeConstraintsTeachersMaxSpanPerDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxSpanPerDayAction;
	QAction* dataTimeConstraintsStudentsMaxSpanPerDayAction;
	QAction* dataTimeConstraintsTeacherMinRestingHoursAction;
	QAction* dataTimeConstraintsTeachersMinRestingHoursAction;
	QAction* dataTimeConstraintsStudentsSetMinRestingHoursAction;
	QAction* dataTimeConstraintsStudentsMinRestingHoursAction;
	QAction* dataSpaceConstraintsSubjectPreferredRoomAction;
	QAction* dataSpaceConstraintsSubjectPreferredRoomsAction;
	QAction* dataSpaceConstraintsSubjectActivityTagPreferredRoomAction;
	QAction* dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction;
	QAction* dataSpaceConstraintsTeacherHomeRoomAction;
	QAction* dataSpaceConstraintsTeacherHomeRoomsAction;
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction;
	QAction* dataTimeConstraintsActivitiesSameStartingDayAction;
	QAction* dataTimeConstraintsTwoActivitiesOrderedAction;
	QAction* dataTimeConstraintsTwoSetsOfActivitiesOrderedAction;
	QAction* dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction;
	QAction* dataTimeConstraintsTeachersMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeacherMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsActivitiesPreferredStartingTimesAction;
	QAction* dataTimeConstraintsActivityPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsActivityPreferredStartingTimesAction;
	QAction* dataTimeConstraintsMinGapsBetweenActivitiesAction;
	QAction* dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsSubactivitiesPreferredStartingTimesAction;
	QAction* dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsActivitiesEndStudentsDayAction;
	QAction* dataTimeConstraintsActivitiesEndTeachersDayAction;
	QAction* dataTimeConstraintsActivitiesBeginStudentsDayAction;
	QAction* dataTimeConstraintsActivitiesBeginTeachersDayAction;
	QAction* dataTimeConstraintsTwoActivitiesGroupedAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction;
	QAction* dataSpaceConstraintsActivityTagPreferredRoomAction;
	QAction* dataSpaceConstraintsActivityTagPreferredRoomsAction;
	QAction* dataTimeConstraintsTeachersMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsThreeActivitiesGroupedAction;
	QAction* dataTimeConstraintsMaxDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsActivitiesMaxHourlySpanAction;
	QAction* dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsMaxTermsBetweenActivitiesAction;
	QAction* dataTimeConstraintsTeacherMinDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsTeacherActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerDayAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerDayAction;
	QAction* dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction;
	QAction* dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction;
	QAction* dataTimeConstraintsStudentsSetMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxDaysPerWeekAction;

	//mornings-afternoons
	QAction* dataTimeConstraintsTeacherMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction;
	QAction* dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction;
	QAction* dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction;
	QAction* dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction;
	QAction* dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsMinHoursPerMorningAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursPerMorningAction;
	QAction* dataTimeConstraintsTeachersMinHoursPerMorningAction;

	//2022-09-10
	QAction* dataTimeConstraintsStudentsMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMinHoursPerAfternoonAction;

	QAction* dataTimeConstraintsTeachersMinHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsTeacherMinHoursPerMorningAction;
	QAction* dataTimeConstraintsTeacherMinHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action;
	QAction* dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action;
	QAction* dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action;
	QAction* dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action;
	QAction* dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action;
	QAction* dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action;
	QAction* dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action;
	QAction* dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action;
	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction;

	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction;

	QAction* dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction;
	QAction* dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction;

	QAction* dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction;
	QAction* dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction;

	//block-planning
	QAction* dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsMaxGapsBetweenActivitiesAction;

	//terms
	QAction* dataTimeConstraintsActivitiesMaxInATermAction;
	QAction* dataTimeConstraintsActivitiesMinInATermAction;
	QAction* dataTimeConstraintsActivitiesOccupyMaxTermsAction;

	QLabel modeLabel;
	
//	QSpinBox communicationSpinBox;
	
	QMenu* shortcutBasicMenu;
	QMenu* shortcutDataSpaceMenu;
	QMenu* shortcutDataAdvancedMenu;
	QMenu* shortcutAdvancedTimeMenu;
	//2014-07-01
	QMenu* shortcutTimetableLockingMenu;
	QMenu* shortcutTimetableAdvancedMenu;
	
	QNetworkAccessManager* networkManager;
	
	QStringList recentFiles;
	
	QAction* recentFileActions[MAX_RECENT_FILES];
	QAction* recentSeparatorAction;
	
	void setEnabledIcon(QAction* action, bool enabled);
	
	void setCurrentFile(const QString& fileName);
	QString strippedName(const QString& fullFileName);
	void updateRecentFileActions();

	void populateLanguagesMap(QMap<QString, QString>& languagesMap);
	
	int currentMode;
	bool dataAvailable;
	bool oldDataAvailable;
	void updateMode(bool forceUpdate=false);
	
	bool getLastConfirmation(int newMode);
	
	void createActionsForConstraints();
	void createMenusOfActionsForConstraints();
	
public:
	FetMainForm();
	~FetMainForm();
	
	void retranslateMode();
	void retranslateConstraints();

	void closeOtherWindows();
	
	void openFile(const QString& fileName);
	bool fileSave();
	bool fileSaveAs();
	
public slots:
	void enableNotPerfectMessage();
	
	void on_modeOfficialAction_triggered();
	void on_modeMorningsAfternoonsAction_triggered();
	void on_modeBlockPlanningAction_triggered();
	void on_modeTermsAction_triggered();
	
	void on_dataTermsAction_triggered();

	void on_fileNewAction_triggered();
	void on_fileSaveAction_triggered();
	void on_fileSaveAsAction_triggered();
	void on_fileQuitAction_triggered();
	void on_fileOpenAction_triggered();
	void on_fileClearRecentFilesListAction_triggered();
	void openRecentFile();

	void on_fileImportCSVActivityTagsAction_triggered();
	void on_fileImportCSVActivitiesAction_triggered();
	void on_fileImportCSVRoomsBuildingsAction_triggered();
	void on_fileImportCSVSubjectsAction_triggered();
	void on_fileImportCSVTeachersAction_triggered();
	void on_fileImportCSVYearsGroupsSubgroupsAction_triggered();
	void on_fileExportCSVAction_triggered();
	
	void on_dataInstitutionNameAction_triggered();
	void on_dataCommentsAction_triggered();
	void on_dataDaysAction_triggered();
	void on_dataHoursAction_triggered();
	void on_dataTeachersAction_triggered();
	void on_dataTeachersStatisticsAction_triggered();
	void on_dataSubjectsAction_triggered();
	void on_dataSubjectsStatisticsAction_triggered();
	void on_dataActivityTagsAction_triggered();
	void on_dataYearsAction_triggered();
	void on_dataGroupsAction_triggered();
	void on_dataSubgroupsAction_triggered();
	void on_dataStudentsStatisticsAction_triggered();
	void on_dataActivitiesRoomsStatisticsAction_triggered();
	void on_dataTeachersSubjectsQualificationsStatisticsAction_triggered();
	void on_dataHelpOnStatisticsAction_triggered();
	
	void checkForUpdatesToggled(bool checked);
	void on_helpSettingsAction_triggered();
	void useColorsToggled(bool checked);
	void showSubgroupsInComboBoxesToggled(bool checked);
	void showSubgroupsInActivityPlanningToggled(bool checked);
	void on_settingsShowShortcutsOnMainWindowAction_toggled();
	void on_settingsFontIsUserSelectableAction_toggled();
	void on_settingsFontAction_triggered();
	void on_settingsShowToolTipsForConstraintsWithTablesAction_toggled();
	
	void on_timetablesToWriteOnDiskAction_triggered();
	
	void on_studentsComboBoxesStyleAction_triggered();
	
	void on_settingsShowVirtualRoomsInTimetablesAction_toggled();

	void on_settingsCommandAfterFinishingAction_triggered();
	///

	//////confirmations
	void on_settingsConfirmActivityPlanningAction_toggled();
	void on_settingsConfirmSpreadActivitiesAction_toggled();
	void on_settingsConfirmRemoveRedundantAction_toggled();
	void on_settingsConfirmSaveTimetableAction_toggled();
	//////

	void showWarningForSubgroupsWithTheSameActivitiesToggled(bool checked);

	void showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool checked);
	
	void showWarningForMaxHoursDailyWithUnder100WeightToggled(bool checked);
	
	void enableActivityTagMaxHoursDailyToggled(bool checked);
	void enableActivityTagMinHoursDailyToggled(bool checked);
	void enableStudentsMaxGapsPerDayToggled(bool checked);
	void showWarningForNotPerfectConstraintsToggled(bool checked);

	void enableMaxGapsPerRealDayToggled(bool checked);

	void enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);
	void showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);

	void enableStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool checked);
	void showWarningForStudentsMinHoursPerMorningWithAllowEmptyMorningsToggled(bool checked);

	void enableStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled(bool checked);
	void showWarningForStudentsMinHoursPerAfternoonWithAllowEmptyAfternoonsToggled(bool checked);

	void enableGroupActivitiesInInitialOrderToggled(bool checked);
	void showWarningForGroupActivitiesInInitialOrderToggled(bool checked);
	
	void on_groupActivitiesInInitialOrderAction_triggered();
	
	void on_dataActivitiesAction_triggered();
	void on_dataSubactivitiesAction_triggered();
	void on_dataRoomsAction_triggered();
	void on_dataBuildingsAction_triggered();
	void on_dataAllTimeConstraintsAction_triggered();
	void on_dataAllSpaceConstraintsAction_triggered();

	void dataSpaceConstraintsRoomNotAvailableTimesAction_triggered();
	void dataSpaceConstraintsTeacherRoomNotAvailableTimesAction_triggered();

	void dataSpaceConstraintsBasicCompulsorySpaceAction_triggered();
	void dataSpaceConstraintsActivityPreferredRoomAction_triggered();
	void dataSpaceConstraintsActivityPreferredRoomsAction_triggered();
	
	void dataSpaceConstraintsStudentsSetHomeRoomAction_triggered();
	void dataSpaceConstraintsStudentsSetHomeRoomsAction_triggered();
	void dataSpaceConstraintsTeacherHomeRoomAction_triggered();
	void dataSpaceConstraintsTeacherHomeRoomsAction_triggered();

	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_triggered();
	void dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_triggered();

	void dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_triggered();
	void dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_triggered();
	
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction_triggered();
	void dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction_triggered();

	void dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction_triggered();
	void dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction_triggered();
	
	void dataSpaceConstraintsSubjectPreferredRoomAction_triggered();
	void dataSpaceConstraintsSubjectPreferredRoomsAction_triggered();
	void dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_triggered();
	void dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_triggered();

	void dataSpaceConstraintsActivityTagPreferredRoomAction_triggered();
	void dataSpaceConstraintsActivityTagPreferredRoomsAction_triggered();

	void dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_triggered();
	void dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction_triggered();

	void dataTimeConstraintsBasicCompulsoryTimeAction_triggered();
	void dataTimeConstraintsBreakTimesAction_triggered();

	void dataTimeConstraintsTwoActivitiesConsecutiveAction_triggered();
	void dataTimeConstraintsTwoActivitiesGroupedAction_triggered();
	void dataTimeConstraintsThreeActivitiesGroupedAction_triggered();
	void dataTimeConstraintsTwoActivitiesOrderedAction_triggered();
	void dataTimeConstraintsTwoSetsOfActivitiesOrderedAction_triggered();
	void dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction_triggered();
	void dataTimeConstraintsActivityPreferredStartingTimeAction_triggered();
	void dataTimeConstraintsActivityPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsActivityPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsActivitiesPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingTimeAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingHourAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingDayAction_triggered();
	void dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesNotOverlappingAction_triggered();
	void dataTimeConstraintsActivityTagsNotOverlappingAction_triggered();
	void dataTimeConstraintsMinDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMinHalfDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMaxDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsActivitiesMaxHourlySpanAction_triggered();
	void dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMaxTermsBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMinGapsBetweenActivitiesAction_triggered();

	void dataTimeConstraintsActivityEndsStudentsDayAction_triggered();
	void dataTimeConstraintsActivitiesEndStudentsDayAction_triggered();

	void dataTimeConstraintsActivityEndsTeachersDayAction_triggered();
	void dataTimeConstraintsActivitiesEndTeachersDayAction_triggered();

	void dataTimeConstraintsActivityBeginsStudentsDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginStudentsDayAction_triggered();

	void dataTimeConstraintsActivityBeginsTeachersDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginTeachersDayAction_triggered();

	void dataTimeConstraintsTeacherNotAvailableTimesAction_triggered();
	void dataTimeConstraintsTeacherMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMinDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeachersMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeachersMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMinHoursDailyAction_triggered();

	void dataTimeConstraintsTeachersMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction_triggered();
	
	void dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetNotAvailableTimesAction_triggered();
	void dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered();

	void dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMinHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsMinHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();

	//2021-12-15
	void dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction_triggered();

	void dataTimeConstraintsTeacherMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsTeachersMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsStudentsSetMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsStudentsMaxSpanPerDayAction_triggered();

	void dataTimeConstraintsTeacherMinRestingHoursAction_triggered();
	void dataTimeConstraintsTeachersMinRestingHoursAction_triggered();
	void dataTimeConstraintsStudentsSetMinRestingHoursAction_triggered();
	void dataTimeConstraintsStudentsMinRestingHoursAction_triggered();

	//mornings-afternoons
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction_triggered();

	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMaxRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxMorningsPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered();
	void dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction_triggered();
	void dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction_triggered();
	void dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction_triggered();

	void dataTimeConstraintsTeacherMinRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMinAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeacherMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered();
	void dataTimeConstraintsTeachersMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered();

	void dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered();
	void dataTimeConstraintsStudentsMaxTwoActivityTagsPerDayFromN1N2N3Action_triggered();

	void dataTimeConstraintsTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered();
	void dataTimeConstraintsTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered();

	void dataTimeConstraintsStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered();
	void dataTimeConstraintsStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Action_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction_triggered();
	void dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction_triggered();

	void dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction_triggered();

	void dataTimeConstraintsTeachersMinHoursPerMorningAction_triggered();
	void dataTimeConstraintsTeacherMinHoursPerMorningAction_triggered();

	void dataTimeConstraintsTeachersMinHoursPerAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMinHoursPerAfternoonAction_triggered();

	void dataTimeConstraintsTeachersMinHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMinHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction_triggered();

	void dataTimeConstraintsTeachersMaxGapsPerRealDayAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMinAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerRealDayAction_triggered();

	void dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction_triggered();

	void dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsStudentsMinHoursPerMorningAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursPerMorningAction_triggered();

	void dataTimeConstraintsStudentsMinHoursPerAfternoonAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction_triggered();

	void dataTimeConstraintsTeacherMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsTeachersMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMaxSpanPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction_triggered();

	void dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction_triggered();
	void dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction_triggered();

	void dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction_triggered();
	void dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction_triggered();

	//block-planning
	void dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsMaxGapsBetweenActivitiesAction_triggered();

	//terms
	void dataTimeConstraintsActivitiesMaxInATermAction_triggered();
	void dataTimeConstraintsActivitiesMinInATermAction_triggered();
	void dataTimeConstraintsActivitiesOccupyMaxTermsAction_triggered();

	void on_helpMoroccoAction_triggered();
	void on_helpAlgeriaAction_triggered();
	void on_helpBlockPlanningAction_triggered();
	void on_helpTermsAction_triggered();

	void on_activityPlanningAction_triggered();
	void on_spreadActivitiesAction_triggered();
	void on_removeRedundantConstraintsAction_triggered();

	//about
	void on_helpAboutFETAction_triggered();
	void on_helpAboutQtAction_triggered();
	void on_helpAboutLibrariesAction_triggered();
	//offline
	void on_helpFAQAction_triggered();
	void on_helpTipsAction_triggered();
	void on_helpInstructionsAction_triggered();
	//online
	void on_helpHomepageAction_triggered();
	void on_helpContentsAction_triggered();
	void on_helpForumAction_triggered();
	void on_helpAddressesAction_triggered();

	void on_timetableGenerateAction_triggered();
	void on_timetableViewStudentsDaysHorizontalAction_triggered();
	void on_timetableViewStudentsTimeHorizontalAction_triggered();
	void on_timetableViewTeachersDaysHorizontalAction_triggered();
	void on_timetableViewTeachersTimeHorizontalAction_triggered();
	void on_timetableViewRoomsDaysHorizontalAction_triggered();
	void on_timetableViewRoomsTimeHorizontalAction_triggered();
	void on_timetableShowConflictsAction_triggered();
	void on_timetablePrintAction_triggered();
	void on_timetableGenerateMultipleAction_triggered();

	void on_timetableLockAllActivitiesAction_triggered();
	void on_timetableUnlockAllActivitiesAction_triggered();
	void on_timetableLockActivitiesDayAction_triggered();
	void on_timetableUnlockActivitiesDayAction_triggered();
	void on_timetableLockActivitiesEndStudentsDayAction_triggered();
	void on_timetableUnlockActivitiesEndStudentsDayAction_triggered();
	void on_timetableLockActivitiesWithASpecifiedActivityTagAction_triggered();
	void on_timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered();
	///
	void on_timetableLockActivitiesWithAdvancedFilterAction_triggered();
	void on_timetableUnlockActivitiesWithAdvancedFilterAction_triggered();

	void on_timetableSaveTimetableAsAction_triggered();

	void on_randomSeedAction_triggered();
	
	void on_languageAction_triggered();
	
	void on_settingsDivideTimetablesByDaysAction_toggled();
	void on_settingsDuplicateVerticalNamesAction_toggled();

	void on_settingsRestoreDefaultsAction_triggered();

	void on_settingsTimetableHtmlLevelAction_triggered();
	void on_settingsPrintActivityTagsAction_toggled();
	void on_settingsOrderSubgroupsInTimetablesAction_toggled();
	void on_settingsPrintDetailedTimetablesAction_toggled();
	void on_settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled();
	void on_settingsPrintNotAvailableSlotsAction_toggled();
	void on_settingsPrintBreakSlotsAction_toggled();

	void on_settingsPrintActivitiesWithSameStartingTimeAction_toggled();

	void on_selectOutputDirAction_triggered();
	
	void on_statisticsExportToDiskAction_triggered();
	void on_statisticsPrintAction_triggered();
	
	void on_shortcutAllTimeConstraintsPushButton_clicked();
	void on_shortcutBreakTimeConstraintsPushButton_clicked();
	void on_shortcutTeachersTimeConstraintsPushButton_clicked();
	void on_shortcutStudentsTimeConstraintsPushButton_clicked();
	void on_shortcutActivitiesTimeConstraintsPushButton_clicked();
	void on_shortcutAdvancedTimeConstraintsPushButton_clicked();

	void on_shortcutAllSpaceConstraintsPushButton_clicked();
	void on_shortcutRoomsSpaceConstraintsPushButton_clicked();
	void on_shortcutTeachersSpaceConstraintsPushButton_clicked();
	void on_shortcutStudentsSpaceConstraintsPushButton_clicked();
	void on_shortcutSubjectsSpaceConstraintsPushButton_clicked();
	void on_shortcutActivityTagsSpaceConstraintsPushButton_clicked();
	void on_shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked();
	void on_shortcutActivitiesSpaceConstraintsPushButton_clicked();
	
	void on_shortcutGeneratePushButton_clicked();
	void on_shortcutGenerateMultiplePushButton_clicked();
	void on_shortcutViewTeachersPushButton_clicked();
	void on_shortcutViewStudentsPushButton_clicked();
	void on_shortcutViewRoomsPushButton_clicked();
	void on_shortcutShowSoftConflictsPushButton_clicked();
	//2014-07-01
	void on_shortcutsTimetableAdvancedPushButton_clicked();
	void on_shortcutsTimetablePrintPushButton_clicked();
	void on_shortcutsTimetableLockingPushButton_clicked();
	
	void on_shortcutBasicPushButton_clicked();
	void on_shortcutSubjectsPushButton_clicked();
	void on_shortcutActivityTagsPushButton_clicked();
	void on_shortcutTeachersPushButton_clicked();
	void on_shortcutStudentsPushButton_clicked();
	void on_shortcutActivitiesPushButton_clicked();
	void on_shortcutSubactivitiesPushButton_clicked();
	void on_shortcutDataAdvancedPushButton_clicked();
	void on_shortcutDataSpacePushButton_clicked();

	void on_shortcutOpenPushButton_clicked();
	void on_shortcutOpenRecentPushButton_clicked();
	void on_shortcutNewPushButton_clicked();
	void on_shortcutSavePushButton_clicked();
	void on_shortcutSaveAsPushButton_clicked();
	
	void replyFinished(QNetworkReply* networkReply);
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
