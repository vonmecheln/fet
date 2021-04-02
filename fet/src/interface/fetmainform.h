//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FETMAINFORM_H
#define FETMAINFORM_H

#include "ui_fetmainform_template.h"

#include <qmutex.h>
#include <qthread.h>
#include <qmessagebox.h>

#include <QCloseEvent>

#include <QMainWindow>

#include "httpget.h"

#include <QSpinBox>

#include <QtGui>

#include <QResizeEvent>

extern const QString COMPANY;
extern const QString PROGRAM;

class RandomSeedDialog: public QDialog
{
	Q_OBJECT
	
public:
	QLabel* label;
	
	QLabel* valuesLabel;
	
	QLineEdit* lineEdit;
	QPushButton* helpPB;
	QPushButton* okPB;
	QPushButton* cancelPB;
	QHBoxLayout* seedLayout;
	QHBoxLayout* buttonsLayout;
	QVBoxLayout* mainLayout;

	RandomSeedDialog();
	~RandomSeedDialog();
	
public slots:
	void help();
	void ok();
};


class FetMainForm:public QMainWindow, Ui::FetMainForm_template
{
	Q_OBJECT
	
private:
	QSpinBox communicationSpinBox;
	
	void setEnabledIcon(QAction* action, bool enabled);
	
/*protected:
	void resizeEvent(QResizeEvent* event);*/

public:
	HttpGet getter;
	
	bool useGetter;

	FetMainForm();
	~FetMainForm();
	
//	void updateLogo();
	
public slots:
	void enableNotPerfectMessage();

	void on_fileNewAction_activated();
	void on_fileSaveAction_activated();
	void on_fileSaveAsAction_activated();
	void on_fileExitAction_activated();
	void on_fileOpenAction_activated();

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
	void on_dataHelpOnStatisticsAction_activated();
	
	void on_helpSettingsAction_activated();
	void on_settingsUseColorsAction_toggled();
	
	void enableActivityTagMaxHoursDailyToggled(bool checked);
	void enableStudentsMaxGapsPerDayToggled(bool checked);
	void showWarningForNotPerfectConstraintsToggled(bool checked);
	
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

	void on_spreadActivitiesAction_activated();
	void on_removeRedundantConstraintsAction_activated();

	void on_helpAboutAction_activated();
	void on_helpFAQAction_activated();
	void on_helpTipsAction_activated();
	void on_helpInstructionsAction_activated();
	void on_helpManualAction_activated();
	void on_helpInOtherLanguagesAction_activated();
	void on_helpForumAction_activated();

	void on_timetableGenerateAction_activated();
	void on_timetableViewStudentsAction_activated();
	void on_timetableViewTeachersAction_activated();
	void on_timetableShowConflictsAction_activated();
	void on_timetableViewRoomsAction_activated();
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

	void on_settingsPrintActivitiesWithSameStartingTimeAction_toggled();

	void on_selectOutputDirAction_activated();
	
	void httpDone(bool error);
	
	void on_statisticsExportToDiskAction_activated();
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
