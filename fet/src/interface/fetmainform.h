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
	void on_dataEquipmentsAction_activated();
	void on_dataBuildingsAction_activated();
	void on_dataRoomsAction_activated();
	void on_dataAllTimeConstraintsAction_activated();
	void on_dataAllSpaceConstraintsAction_activated();

	void on_dataSpaceConstraintsBasicCompulsorySpaceAction_activated();
	void on_dataSpaceConstraintsTeacherRequiresRoomAction_activated();
	void on_dataSpaceConstraintsTeacherSubjectRequireRoomAction_activated();
	void on_dataSpaceConstraintsRoomNotAvailableAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomAction_activated();
	void on_dataSpaceConstraintsMinimizeNumberOfRoomsForStudentsAction_activated();
	void on_dataSpaceConstraintsMinimizeNumberOfRoomsForTeachersAction_activated();
	void on_dataSpaceConstraintsRoomTypeNotAllowedSubjectsAction_activated();
	void on_dataSpaceConstraintsSubjectRequiresEquipmentsAction_activated();
	void on_dataSpaceConstraintsSubjectSubjectTagRequireEquipmentsAction_activated();
	void on_dataSpaceConstraintsActivitiesSameRoomAction_activated();
	void on_dataSpaceConstraintsActivityPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsSubjectPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomAction_activated();
	void on_dataSpaceConstraintsSubjectPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomsAction_activated();
	void on_dataSpaceConstraintsMaxBuildingChangesPerDayForTeachersAction_activated();
	void on_dataSpaceConstraintsMaxBuildingChangesPerDayForStudentsAction_activated();
	void on_dataSpaceConstraintsMaxRoomChangesPerDayForTeachersAction_activated();
	void on_dataSpaceConstraintsMaxRoomChangesPerDayForStudentsAction_activated();
	
	void on_dataTimeConstraints2ActivitiesConsecutiveAction_activated();
	void on_dataTimeConstraints2ActivitiesOrderedAction_activated();
	void on_dataTimeConstraints2ActivitiesGroupedAction_activated();
	void on_dataTimeConstraintsActivitiesPreferredTimesAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated();
	void on_dataTimeConstraintsActivitiesSameStartingHourAction_activated();
	void on_dataTimeConstraintsTeacherNotAvailableAction_activated();
	void on_dataTimeConstraintsBasicCompulsoryTimeAction_activated();
	void on_dataTimeConstraintsStudentsSetNotAvailableAction_activated();
	void on_dataTimeConstraintsBreakAction_activated();
	void on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersMinHoursDailyAction_activated();
	void on_dataTimeConstraintsTeachersSubgroupsMaxHoursDailyAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimeAction_activated();
	void on_dataTimeConstraintsStudentsSetNoGapsAction_activated();
	void on_dataTimeConstraintsStudentsNoGapsAction_activated();
	void on_dataTimeConstraintsTeachersNoGapsAction_activated();
	void on_dataTimeConstraintsStudentsEarlyAction_activated();
	void on_dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_activated();
	void on_dataTimeConstraintsStudentsSetNHoursDailyAction_activated();
	void on_dataTimeConstraintsStudentsNHoursDailyAction_activated();
	void on_dataTimeConstraintsActivityEndsDayAction_activated();
	void on_dataTimeConstraintsActivitiesNotOverlappingAction_activated();
	void on_dataTimeConstraintsMinNDaysBetweenActivitiesAction_activated();
	void on_dataTimeConstraintsActivityPreferredTimesAction_activated();
	void on_dataTimeConstraintsTeachersSubjectTagsMaxHoursContinuouslyAction_activated();
	void on_dataTimeConstraintsTeachersSubjectTagMaxHoursContinuouslyAction_activated();

	void on_helpAboutAction_activated();
	void on_helpFAQAction_activated();
	void on_helpTimeConstraintsAction_activated();
	void on_helpTimeConstraintsSubtagsAction_activated();

	void on_timetableAllocateHoursAction_activated();
	void on_timetableViewStudentsAction_activated();
	void on_timetableViewTeachersAction_activated();
	void on_timetableViewStudentsWithRoomsAction_activated();
	void on_timetableViewTeachersWithRoomsAction_activated();
	void on_timetableViewStudentsWithRooms2Action_activated();
	void on_timetableViewTeachersWithRooms2Action_activated();
	void on_timetableShowConflictsTimeAction_activated();
	void on_timetableAllocateRoomsAction_activated();
	void on_timetableViewRoomsAction_activated();
	void on_timetableViewRooms2Action_activated();
	void on_timetableShowConflictsSpaceAction_activated();
	void on_timetableShowConflictsTimeSpaceAction_activated();
	void on_timetableAllocateHoursRoomsAction_activated();
	void on_timetableExportXmlHtmlAction_activated();
	void on_timetableExportiCalTeachersAction_activated();
	void on_timetableExportiCalStudentsAction_activated();
	void on_timetableExportiCalTeachersWithRooms1Action_activated();
	void on_timetableExportiCalStudentsWithRooms1Action_activated();
	void on_timetableExportiCalTeachersWithRooms2Action_activated();
	void on_timetableExportiCalStudentsWithRooms2Action_activated();
	
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
	
	void on_parametersPopulationNumberAction_activated();
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
