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
	void on_dataSubjectTagsAction_activated();
	void on_dataYearsAction_activated();
	void on_dataGroupsAction_activated();
	void on_dataSubgroupsAction_activated();
	void on_dataStudentsStatisticsAction_activated();
	void on_dataActivitiesAction_activated();
	void on_dataRoomsAction_activated();
	void on_dataAllTimeConstraintsAction_activated();
	void on_dataAllSpaceConstraintsAction_activated();


	void on_dataSpaceConstraintsRoomNotAvailableAction_activated();

	void on_dataSpaceConstraintsBasicCompulsorySpaceAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsSubjectPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomsAction_activated();
	

	void on_dataTimeConstraintsBasicCompulsoryTimeAction_activated();
	void on_dataTimeConstraintsBreakAction_activated();

	void on_dataTimeConstraints2ActivitiesConsecutiveAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimeAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimesAction_activated();
	void on_dataTimeConstraintsActivitiesPreferredTimesAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingHourAction_activated();
	void on_dataTimeConstraintsActivitiesNotOverlappingAction_activated();
	void on_dataTimeConstraintsMinNDaysBetweenActivitiesAction_activated();

	void on_dataTimeConstraintsTeacherNotAvailableAction_activated();
	void on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeacherMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMaxGapsPerWeekAction_activated();
	void on_dataTimeConstraintsTeacherMaxGapsPerWeekAction_activated();

	void on_dataTimeConstraintsStudentsSetNotAvailableAction_activated();
	void on_dataTimeConstraintsStudentsSetNoGapsAction_activated();
	void on_dataTimeConstraintsStudentsNoGapsAction_activated();
	void on_dataTimeConstraintsStudentsEarlyAction_activated();
	void on_dataTimeConstraintsStudentsSetEarlyAction_activated();
	void on_dataTimeConstraintsStudentsMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsMinHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsSetMinHoursDailyAction_activated();


	void on_helpAboutAction_activated();
	void on_helpFAQAction_activated();
	void on_helpInstructionsAction_activated();

	void on_timetableGenerateAction_activated();
	void on_timetableViewStudentsAction_activated();
	void on_timetableViewTeachersAction_activated();
	void on_timetableShowConflictsAction_activated();
	void on_timetableViewRoomsAction_activated();
	void on_timetableGenerateMultipleAction_activated();
	
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
	
	void on_checkForUpdatesAction_toggled();
	
	void httpDone(bool error);
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
