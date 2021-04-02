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

class FetMainForm:public QMainWindow, Ui::FetMainForm_template
{
	Q_OBJECT

public:
	FetMainForm();
	~FetMainForm();

public slots:
	void on_fileNewAction_activated();
	void on_fileSaveAction_activated();
	void on_fileSaveAsAction_activated();
	void on_fileExitAction_activated();
	void on_fileOpenAction_activated();

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
	void on_dataActivitiesAction_activated();
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


	void on_dataTimeConstraintsBasicCompulsoryTimeAction_activated();
	void on_dataTimeConstraintsBreakTimesAction_activated();

	void on_dataTimeConstraints2ActivitiesConsecutiveAction_activated();
	void on_dataTimeConstraints2ActivitiesOrderedAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimeAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimesAction_activated();
	void on_dataTimeConstraintsActivitiesPreferredTimesAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingHourAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingDayAction_activated();
	void on_dataTimeConstraintsActivitiesNotOverlappingAction_activated();
	void on_dataTimeConstraintsMinNDaysBetweenActivitiesAction_activated();
	void on_dataTimeConstraintsActivityEndsStudentsDayAction_activated();

	void on_dataTimeConstraintsTeacherNotAvailableTimesAction_activated();
	void on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeacherMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeachersMinHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherMinHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsTeacherMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxGapsPerDayAction_activated();
	void on_dataTimeConstraintsTeacherMaxGapsPerDayAction_activated();

	void on_dataTimeConstraintsStudentsSetNotAvailableTimesAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_activated();
	void on_dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_activated();
	void on_dataTimeConstraintsStudentsMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsStudentsMinHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMinHoursDailyAction_activated();


	void on_helpAboutAction_activated();
	void on_helpFAQAction_activated();
	void on_helpInstructionsAction_activated();
	void on_helpInOtherLanguagesAction_activated();

	void on_timetableGenerateAction_activated();
	void on_timetableViewStudentsAction_activated();
	void on_timetableViewTeachersAction_activated();
	void on_timetableShowConflictsAction_activated();
	void on_timetableViewRoomsAction_activated();
	void on_timetableGenerateMultipleAction_activated();

	void on_timetableSaveTimetableAsAction_activated();
	
	void on_languageEnglishAction_activated();
	void on_languageFrenchAction_activated();
	void on_languageCatalanAction_activated();
	void on_languageRomanianAction_activated();
	void on_languageMalayAction_activated();
	void on_languagePolishAction_activated();
	void on_languageTurkishAction_activated();
	void on_languageDutchAction_activated();
	void on_languageGermanAction_activated();
	void on_languageHungarianAction_activated();
	void on_languageMacedonianAction_activated();
	void on_languageSpanishAction_activated();
	void on_languageGreekAction_activated();
	void on_languageArabicAction_activated();
	void on_languageIndonesianAction_activated();
	void on_languageItalianAction_activated();
	
	void on_checkForUpdatesAction_toggled();

	void on_settingsRestoreDefaultsAction_activated();

	void on_settingsTimetableHtmlLevelAction_activated();
	
	void httpDone(bool error);
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
