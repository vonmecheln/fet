//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FETMAINFORM_H
#define FETMAINFORM_H

#include "ui_fetmainform_template.h"

#include <QMutex>
#include <QThread>

#include <QtGui>
#include <QResizeEvent>
#include <QCloseEvent>

#include <QStringList>

#include <QAction>

class QNetworkAccessManager;
class QNetworkReply;

extern const QString COMPANY;
extern const QString PROGRAM;

const int MAX_RECENT_FILES=10;

class RandomSeedDialog: public QDialog
{
	Q_OBJECT
	
public:
	QLabel* labelX;
	QLabel* labelY;
	
	QLabel* valuesLabelX;
	QLabel* valuesLabelY;
	
	QLineEdit* lineEditX;
	QLineEdit* lineEditY;
	QPushButton* helpPB;
	QPushButton* okPB;
	QPushButton* cancelPB;
	QGridLayout* seedLayoutX;
	QGridLayout* seedLayoutY;
	QHBoxLayout* buttonsLayout;
	QVBoxLayout* mainLayout;

	RandomSeedDialog(QWidget* parent);
	~RandomSeedDialog();
	
public slots:
	void help();
	void ok();
};

class FetMainForm: public QMainWindow, public Ui::FetMainForm_template
{
	Q_OBJECT
	
private:
	QSpinBox communicationSpinBox;
	
	QMenu* shortcutBasicMenu;
	QMenu* shortcutDataSpaceMenu;
	QMenu* shortcutDataAdvancedMenu;
	QMenu* shortcutAdvancedTimeMenu;
	
	QNetworkAccessManager* networkManager;
	
	QStringList recentFiles;
	
	QAction* recentFileActions[MAX_RECENT_FILES];
	QAction* recentSeparatorAction;
	
	void setEnabledIcon(QAction* action, bool enabled);
	
	void setCurrentFile(const QString& fileName);
	QString strippedName(const QString& fullFileName);
	void updateRecentFileActions();

public:
	FetMainForm();
	~FetMainForm();
	
	void closeOtherWindows();
	
	void openFile(const QString& fileName);
	bool fileSave();
	bool fileSaveAs();
	
public slots:
	void enableNotPerfectMessage();

	void on_fileNewAction_activated();
	void on_fileSaveAction_activated();
	void on_fileSaveAsAction_activated();
	void on_fileExitAction_activated();
	void on_fileOpenAction_activated();
	void on_fileClearRecentFilesListAction_activated();
	void openRecentFile();

	void on_fileImportCSVActivityTagsAction_activated();
	void on_fileImportCSVActivitiesAction_activated();
	void on_fileImportCSVRoomsBuildingsAction_activated();
	void on_fileImportCSVSubjectsAction_activated();
	void on_fileImportCSVTeachersAction_activated();
	void on_fileImportCSVYearsGroupsSubgroupsAction_activated();
	void on_fileExportCSVAction_activated();
	
	void on_dataInstitutionNameAction_activated();
	void on_dataCommentsAction_activated();
	void on_dataDaysAction_activated();
	void on_dataHoursAction_activated();
	void on_dataTeachersAction_activated();
	void on_dataTeachersStatisticsAction_activated();
	void on_dataSubjectsAction_activated();
	void on_dataSubjectsStatisticsAction_activated();
	void on_dataActivityTagsAction_activated();
	void on_dataYearsAction_activated();
	void on_dataGroupsAction_activated();
	void on_dataSubgroupsAction_activated();
	void on_dataStudentsStatisticsAction_activated();
	void on_dataActivitiesRoomsStatisticsAction_activated();
	void on_dataHelpOnStatisticsAction_activated();
	
	void on_helpSettingsAction_activated();
	void on_settingsUseColorsAction_toggled();
	void on_settingsShowShortcutsOnMainWindowAction_toggled();

	//////confirmations
	void on_settingsConfirmActivityPlanningAction_toggled();
	void on_settingsConfirmSpreadActivitiesAction_toggled();
	void on_settingsConfirmRemoveRedundantAction_toggled();
	void on_settingsConfirmSaveTimetableAction_toggled();
	//////
	
	void enableActivityTagMaxHoursDailyToggled(bool checked);
	void enableStudentsMaxGapsPerDayToggled(bool checked);
	void showWarningForNotPerfectConstraintsToggled(bool checked);

	void enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);
	void showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked);
	
	void on_dataActivitiesAction_activated();
	void on_dataSubactivitiesAction_activated();
	void on_dataRoomsAction_activated();
	void on_dataBuildingsAction_activated();
	void on_dataAllTimeConstraintsAction_activated();
	void on_dataAllSpaceConstraintsAction_activated();

	void on_dataSpaceConstraintsRoomNotAvailableTimesAction_activated();

	void on_dataSpaceConstraintsBasicCompulsorySpaceAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomsAction_activated();
	
	void on_dataSpaceConstraintsStudentsSetHomeRoomAction_activated();
	void on_dataSpaceConstraintsStudentsSetHomeRoomsAction_activated();
	void on_dataSpaceConstraintsTeacherHomeRoomAction_activated();
	void on_dataSpaceConstraintsTeacherHomeRoomsAction_activated();

	void on_dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_activated();
	void on_dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_activated();
	void on_dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_activated();
	void on_dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_activated();
	void on_dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_activated();
	void on_dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_activated();

	void on_dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_activated();
	void on_dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_activated();
	void on_dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_activated();
	void on_dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_activated();
	void on_dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_activated();
	void on_dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_activated();
	
	void on_dataSpaceConstraintsSubjectPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_activated();

	void on_dataSpaceConstraintsActivityTagPreferredRoomAction_activated();
	void on_dataSpaceConstraintsActivityTagPreferredRoomsAction_activated();

	void on_dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_activated();

	void on_dataTimeConstraintsBasicCompulsoryTimeAction_activated();
	void on_dataTimeConstraintsBreakTimesAction_activated();

	void on_dataTimeConstraintsTwoActivitiesConsecutiveAction_activated();
	void on_dataTimeConstraintsTwoActivitiesGroupedAction_activated();
	void on_dataTimeConstraintsThreeActivitiesGroupedAction_activated();
	void on_dataTimeConstraintsTwoActivitiesOrderedAction_activated();
	void on_dataTimeConstraintsActivityPreferredStartingTimeAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimeSlotsAction_activated();
	void on_dataTimeConstraintsActivitiesPreferredTimeSlotsAction_activated();
	void on_dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_activated();
	void on_dataTimeConstraintsActivityPreferredStartingTimesAction_activated();
	void on_dataTimeConstraintsActivitiesPreferredStartingTimesAction_activated();
	void on_dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingHourAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingDayAction_activated();
	void on_dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_activated();
	void on_dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_activated();
	void on_dataTimeConstraintsActivitiesNotOverlappingAction_activated();
	void on_dataTimeConstraintsMinDaysBetweenActivitiesAction_activated();
	void on_dataTimeConstraintsMaxDaysBetweenActivitiesAction_activated();
	void on_dataTimeConstraintsMinGapsBetweenActivitiesAction_activated();
	void on_dataTimeConstraintsActivityEndsStudentsDayAction_activated();
	void on_dataTimeConstraintsActivitiesEndStudentsDayAction_activated();

	void on_dataTimeConstraintsTeacherNotAvailableTimesAction_activated();
	void on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxDaysPerWeekAction_activated();

	void on_dataTimeConstraintsTeacherMinDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMinDaysPerWeekAction_activated();

	void on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeacherMaxHoursContinuouslyAction_activated();

	void on_dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_activated();

	void on_dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_activated();

	void on_dataTimeConstraintsTeachersMinHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherMinHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsTeacherMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxGapsPerDayAction_activated();
	void on_dataTimeConstraintsTeacherMaxGapsPerDayAction_activated();
	
	void on_dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_activated();

	void on_dataTimeConstraintsStudentsSetNotAvailableTimesAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsMaxGapsPerWeekAction_activated();

	void on_dataTimeConstraintsStudentsSetMaxGapsPerDayAction_activated();
	void on_dataTimeConstraintsStudentsMaxGapsPerDayAction_activated();

	void on_dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_activated();
	void on_dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_activated();
	void on_dataTimeConstraintsStudentsMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_activated();

	void on_dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_activated();

	void on_dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_activated();

	void on_dataTimeConstraintsStudentsMinHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMinHoursDailyAction_activated();

	void on_activityPlanningAction_activated();
	void on_spreadActivitiesAction_activated();
	void on_removeRedundantConstraintsAction_activated();

	//about
	void on_helpAboutAction_activated();
	//offline
	void on_helpFAQAction_activated();
	void on_helpTipsAction_activated();
	void on_helpInstructionsAction_activated();
	//online
	void on_helpHomepageAction_activated();
	void on_helpContentsAction_activated();
	void on_helpForumAction_activated();
	void on_helpAddressesAction_activated();

	void on_timetableGenerateAction_activated();
	void on_timetableViewStudentsAction_activated();
	void on_timetableViewTeachersAction_activated();
	void on_timetableViewRoomsAction_activated();
	void on_timetableShowConflictsAction_activated();
	void on_timetablePrintAction_activated();
	void on_timetableGenerateMultipleAction_activated();

	void on_timetableLockAllActivitiesAction_activated();
	void on_timetableUnlockAllActivitiesAction_activated();
	void on_timetableLockActivitiesDayAction_activated();
	void on_timetableUnlockActivitiesDayAction_activated();
	void on_timetableLockActivitiesEndStudentsDayAction_activated();
	void on_timetableUnlockActivitiesEndStudentsDayAction_activated();

	void on_timetableSaveTimetableAsAction_activated();

	void on_randomSeedAction_activated();
	
	void on_languageAction_activated();
	
	void on_checkForUpdatesAction_toggled();
	
	void on_timetablesDivideByDaysAction_toggled();

	void on_settingsRestoreDefaultsAction_activated();

	void on_settingsTimetableHtmlLevelAction_activated();
	void on_settingsPrintNotAvailableSlotsAction_toggled();
	void on_settingsPrintBreakSlotsAction_toggled();

	void on_settingsPrintActivitiesWithSameStartingTimeAction_toggled();

	void on_selectOutputDirAction_activated();
	
	void on_statisticsExportToDiskAction_activated();
	
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
