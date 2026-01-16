/***************************************************************************
                          listspaceconstraints.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "listspaceconstraints.h"

#include "addormodifyspaceconstraint.h"

#include "addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h"

#include "timetable.h"

#include "longtextmessagebox.h"

#include "lockunlock.h"

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QVBoxLayout>
#include <QScrollBar>

#include <QTextEdit>

#include <QInputDialog>

#include <QSplitter>

#include <QBrush>
#include <QPalette>

#include <QSizePolicy>

#include <QSettings>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

int spaceConstraintsAscendingByDescription(SpaceConstraint* t1, SpaceConstraint* t2); //defined in allspaceconstraints.cpp

ListSpaceConstraintsDialog::ListSpaceConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop, QSplitter* _splitter,
													   QCheckBox* _showRelatedCheckBox): QDialog(parent)
{
	dialogName=_dialogName;
	dialogTitle=_dialogTitle;
	eventLoop=_eventLoop;

	showRelatedCheckBox=_showRelatedCheckBox;

	setWindowTitle(dialogTitle);

	setAttribute(Qt::WA_DeleteOnClose);

	//resize(600, 400);

	splitter=_splitter;

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, dialogName);
}

ListSpaceConstraintsDialog::~ListSpaceConstraintsDialog()
{
	saveFETDialogGeometry(this, dialogName);

	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(dialogName+QString("/splitter-state"), splitter->saveState());

	if(showRelatedCheckBox!=nullptr)
		settings.setValue(dialogName+QString("/show-related"), showRelatedCheckBox->isChecked());

	eventLoop->quit();
}

ListSpaceConstraints::ListSpaceConstraints(QWidget* parent, int _type)
{
	type=_type;

	showRelatedCheckBox=nullptr;

	filterCheckBox=nullptr;
	sortedCheckBox=nullptr;

	countOfConstraintsLabel=nullptr;
	mSLabel=nullptr;
	activatePushButton=nullptr;
	deactivatePushButton=nullptr;
	weightsPushButton=nullptr;
	commentsPushButton=nullptr;

	firstInstructionsLabel=nullptr;
	secondInstructionsLabel=nullptr;

	filterGroupBox=nullptr;
	teachersComboBox=nullptr;
	studentsComboBox=nullptr;
	subjectsComboBox=nullptr;
	activityTagsComboBox=nullptr;
	roomsComboBox=nullptr;

	helpPushButton=nullptr;

	addRemoveMultipleConstraintsPushButton=nullptr;

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				dialogTitle=tr("Constraints basic compulsory space", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsBasicCompulsorySpace");

				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				dialogTitle=tr("Constraints room not available times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsRoomNotAvailableTimes");

				roomsComboBox=new QComboBox;

				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				dialogTitle=tr("Constraints activity preferred room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityPreferredRoom");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				dialogTitle=tr("Constraints activity preferred rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityPreferredRooms");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				dialogTitle=tr("Constraints students set home room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetHomeRoom");

				firstInstructionsLabel=new QLabel(tr("Note: a home room for a students set means that every activity which has "
													 "this and ONLY this students set will have this as home room (not other "
													 "superior or inferior set). You can add home rooms for a group, for a year "
													 "or for a subgroup, careful not to make impossible timetables. Preferred "
													 "rooms override the home rooms."));

				studentsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				dialogTitle=tr("Constraints students set home rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetHomeRooms");

				firstInstructionsLabel=new QLabel(tr("Note: home rooms for a students set means that every activity which has this "
													 "and ONLY this students set will have these home rooms (not other superior or "
													 "inferior set). You can add home rooms for a group, for a year or for a subgroup, "
													 "careful not to make impossible timetables. Preferred rooms override the home rooms."));

				studentsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				dialogTitle=tr("Constraints teacher home room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherHomeRoom");

				firstInstructionsLabel=new QLabel(tr("Note: a home room for a teacher means that every activity which has this "
													 "and ONLY this teacher will have this as home room. Preferred rooms override "
													 "the home rooms."));

				teachersComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				dialogTitle=tr("Constraints teacher home rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherHomeRooms");

				firstInstructionsLabel=new QLabel(tr("Note: home rooms for a teacher means that every activity which has this and "
													 "ONLY this teacher will have these home rooms. Preferred rooms override the "
													 "home rooms."));

				teachersComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				dialogTitle=tr("Constraints subject preferred room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubjectPreferredRoom");

				subjectsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				dialogTitle=tr("Constraints subject preferred rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubjectPreferredRooms");

				subjectsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				dialogTitle=tr("Constraints subject activity tag preferred room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubjectActivityTagPreferredRoom");

				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				dialogTitle=tr("Constraints subject activity tag preferred rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubjectActivityTagPreferredRooms");

				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//13
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints students max building changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxBuildingChangesPerDay");

				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints students set max building changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxBuildingChangesPerDay");

				studentsComboBox=new QComboBox;

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max building changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxBuildingChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of building changes per week is considered to be the sum of the building "
													 "changes on each day."));

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max building changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxBuildingChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of building changes per week is considered to be the sum of the building "
													 "changes on each day."));

				studentsComboBox=new QComboBox;

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				dialogTitle=tr("Constraints students min gaps between building changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenBuildingChanges");

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				dialogTitle=tr("Constraints students set min gaps between building changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenBuildingChanges");

				studentsComboBox=new QComboBox;

				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints teachers max building changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxBuildingChangesPerDay");

				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints teacher max building changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxBuildingChangesPerDay");

				teachersComboBox=new QComboBox;

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max building changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxBuildingChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of building changes per week is considered to be the sum of the building "
													 "changes on each day."));

				break;
			}
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max building changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxBuildingChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of building changes per week is considered to be the sum of the building "
													 "changes on each day."));

				teachersComboBox=new QComboBox;

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				dialogTitle=tr("Constraints teachers min gaps between building changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenBuildingChanges");

				break;
			}
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				dialogTitle=tr("Constraints teacher min gaps between building changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenBuildingChanges");

				teachersComboBox=new QComboBox;

				break;
			}
		//25
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				dialogTitle=tr("Constraints activity tag preferred room", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityTagPreferredRoom");

				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				dialogTitle=tr("Constraints activity tag preferred rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityTagPreferredRooms");

				activityTagsComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				dialogTitle=tr("Constraints activities occupy max different rooms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOccupyMaxDifferentRooms");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				addRemoveMultipleConstraintsPushButton=new QPushButton(tr("Add/remove multiple constraints"));

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			{
				dialogTitle=tr("Constraints activities same room if consecutive", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesSameRoomIfConsecutive");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//29
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints students max room changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRoomChangesPerDay");

				break;
			}
		//30
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints students set max room changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRoomChangesPerDay");

				studentsComboBox=new QComboBox;

				break;
			}
		//31
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max room changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRoomChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of room changes per week is considered to be the sum of the room changes on "
													 "each day. If you want the same room throughout the week, please use one or more constraints "
													 "activities occupy max 1 different rooms."));

				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max room changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRoomChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of room changes per week is considered to be the sum of the room changes on each "
													 "day. If you want the same room throughout the week, please use one or more constraints activities "
													 "occupy max 1 different rooms."));

				studentsComboBox=new QComboBox;

				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				dialogTitle=tr("Constraints students min gaps between room changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenRoomChanges");

				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				dialogTitle=tr("Constraints students set min gaps between room changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenRoomChanges");

				studentsComboBox=new QComboBox;

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints teachers max room changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRoomChangesPerDay");

				break;
			}
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				dialogTitle=tr("Constraints teacher max room changes per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRoomChangesPerDay");

				teachersComboBox=new QComboBox;

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max room changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRoomChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of room changes per week is considered to be the sum of the room changes on "
													 "each day. If you want the same room throughout the week, please use one or more constraints "
													 "activities occupy max 1 different rooms."));

				break;
			}
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max room changes per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRoomChangesPerWeek");

				firstInstructionsLabel=new QLabel(tr("Note: The number of room changes per week is considered to be the sum of the room changes on "
													 "each day. If you want the same room throughout the week, please use one or more constraints "
													 "activities occupy max 1 different rooms."));

				teachersComboBox=new QComboBox;

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				dialogTitle=tr("Constraints teachers min gaps between room changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenRoomChanges");

				break;
			}
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				dialogTitle=tr("Constraints teacher min gaps between room changes", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenRoomChanges");

				teachersComboBox=new QComboBox;

				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				dialogTitle=tr("Constraints teacher room not available times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherRoomNotAvailableTimes");

				teachersComboBox=new QComboBox;

				roomsComboBox=new QComboBox;

				break;
			}
		//42
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students max room changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRoomChangesPerRealDay");

				break;
			}
		//43
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set max room changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRoomChangesPerRealDay");

				studentsComboBox=new QComboBox;

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers max room changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRoomChangesPerRealDay");

				break;
			}
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher max room changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRoomChangesPerRealDay");

				teachersComboBox=new QComboBox;

				break;
			}
		//46
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students max building changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxBuildingChangesPerRealDay");

				break;
			}
		//47
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set max building changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxBuildingChangesPerRealDay");

				studentsComboBox=new QComboBox;

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers max building changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxBuildingChangesPerRealDay");

				break;
			}
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher max building changes per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxBuildingChangesPerRealDay");

				teachersComboBox=new QComboBox;

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students max building changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxBuildingChangesPerDayInInterval");

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students set max building changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxBuildingChangesPerDayInInterval");

				studentsComboBox=new QComboBox;

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teachers max building changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxBuildingChangesPerDayInInterval");

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teacher max building changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxBuildingChangesPerDayInInterval");

				teachersComboBox=new QComboBox;

				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students max building changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxBuildingChangesPerRealDayInInterval");

				break;
			}
		//55
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students set max building changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxBuildingChangesPerRealDayInInterval");

				studentsComboBox=new QComboBox;

				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teachers max building changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxBuildingChangesPerRealDayInInterval");

				break;
			}
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teacher max building changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxBuildingChangesPerRealDayInInterval");

				teachersComboBox=new QComboBox;

				break;
			}
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students max room changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRoomChangesPerDayInInterval");

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students set max room changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRoomChangesPerDayInInterval");

				studentsComboBox=new QComboBox;

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teachers max room changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRoomChangesPerDayInInterval");

				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teacher max room changes per day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRoomChangesPerDayInInterval");

				teachersComboBox=new QComboBox;

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students max room changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRoomChangesPerRealDayInInterval");

				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students set max room changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRoomChangesPerRealDayInInterval");

				studentsComboBox=new QComboBox;

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teachers max room changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRoomChangesPerRealDayInInterval");

				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teacher max room changes per real day in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRoomChangesPerRealDayInInterval");

				teachersComboBox=new QComboBox;

				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints room max activity tags per day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsRoomMaxActivityTagsPerDayFromSet");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified room does not have more than the specified number "
													 "of activity tags from those selected, on any day."));
				secondInstructionsLabel=new QLabel(tr("Note: For FET to be able to start the generation, the specified room must be real (not virtual), "
													  "and should not be found in any set of real rooms of any virtual room."));

				roomsComboBox=new QComboBox;

				activityTagsComboBox=new QComboBox;

				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints room max activity tags per real day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsRoomMaxActivityTagsPerRealDayFromSet");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified room does not have more than the specified number of "
													 "activity tags from those selected, on any real day."));
				secondInstructionsLabel=new QLabel(tr("Note: For FET to be able to start the generation, the specified room must be real (not virtual), "
													  "and should not be found in any set of real rooms of any virtual room."));

				roomsComboBox=new QComboBox;

				activityTagsComboBox=new QComboBox;

				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				dialogTitle=tr("Constraints room max activity tags per week from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsRoomMaxActivityTagsPerWeekFromSet");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified room does not have more than the specified number of "
													 "activity tags from those selected, throughout the week."));
				secondInstructionsLabel=new QLabel(tr("Note: For FET to be able to start the generation, the specified room must be real (not virtual), "
													  "and should not be found in any set of real rooms of any virtual room."));

				roomsComboBox=new QComboBox;

				activityTagsComboBox=new QComboBox;

				break;
			}

		default:
			assert(0);
			break;
	}

	///////
	QSettings settings(COMPANY, PROGRAM);
	QString settingsName=dialogName+"AdvancedFilterForm";

	all=settings.value(settingsName+"/all-conditions", "true").toBool();

	descrDetDescr.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescr.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());

	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());

	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());

	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();

	useFilter=false;

	//not yet created!!!
	//assert(filterCheckBox->isChecked()==false);
	///////

	if(firstInstructionsLabel!=nullptr){
		firstInstructionsLabel->setWordWrap(true);
		//firstInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
	if(secondInstructionsLabel!=nullptr){
		secondInstructionsLabel->setWordWrap(true);
		//secondInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if(teachersComboBox!=nullptr){
		QSize tmp=teachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		teachersComboBox->addItem("");
		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			teachersComboBox->addItem(tch->name);
		teachersComboBox->setCurrentIndex(0);

		connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListSpaceConstraints::filter);
	}

	if(studentsComboBox!=nullptr){
		QSize tmp=studentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		populateStudentsComboBox(studentsComboBox, QString(""), true);
		studentsComboBox->setCurrentIndex(0);

		connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListSpaceConstraints::showRelatedCheckBoxToggled);
	}

	if(subjectsComboBox!=nullptr){
		QSize tmp=subjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		subjectsComboBox->addItem("");
		for(Subject* sbj : std::as_const(gt.rules.subjectsList))
			subjectsComboBox->addItem(sbj->name);
		subjectsComboBox->setCurrentIndex(0);

		connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListSpaceConstraints::filter);
	}

	if(activityTagsComboBox!=nullptr){
		QSize tmp=activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		activityTagsComboBox->addItem("");
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			activityTagsComboBox->addItem(at->name);
		activityTagsComboBox->setCurrentIndex(0);

		connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListSpaceConstraints::filter);
	}

	if(roomsComboBox!=nullptr){
		QSize tmp=roomsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		roomsComboBox->addItem("");
		for(Room* rm : std::as_const(gt.rules.roomsList))
			roomsComboBox->addItem(rm->name);
		roomsComboBox->setCurrentIndex(0);

		connect(roomsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListSpaceConstraints::filter);
	}

	if(teachersComboBox!=nullptr
			|| studentsComboBox!=nullptr
			|| subjectsComboBox!=nullptr
			|| activityTagsComboBox!=nullptr
			|| roomsComboBox!=nullptr){
		filterGroupBox=new QGroupBox(tr("Filter"));

		QVBoxLayout* layout=new QVBoxLayout;

		if(teachersComboBox!=nullptr)
			layout->addWidget(teachersComboBox);
		if(studentsComboBox!=nullptr){
			showRelatedCheckBox=new QCheckBox(tr("Show related"));

			QSettings settings(COMPANY, PROGRAM);
			showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/show-related"), "false").toBool());

			connect(showRelatedCheckBox, &QCheckBox::toggled, this, &ListSpaceConstraints::showRelatedCheckBoxToggled);

			QHBoxLayout* ssrl=new QHBoxLayout;

			ssrl->addWidget(studentsComboBox);
			ssrl->addWidget(showRelatedCheckBox);

			layout->addLayout(ssrl);
		}
		if(subjectsComboBox!=nullptr)
			layout->addWidget(subjectsComboBox);
		if(activityTagsComboBox!=nullptr)
			layout->addWidget(activityTagsComboBox);
		if(roomsComboBox!=nullptr)
			layout->addWidget(roomsComboBox);

		filterGroupBox->setLayout(layout);
	}

	eventLoop=new QEventLoop;

	splitter=new QSplitter;
	splitter->setSizePolicy(splitter->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);

	dialog=new ListSpaceConstraintsDialog(parent, dialogName, dialogTitle, eventLoop, splitter,
										  showRelatedCheckBox);

	//dialog->setWindowTitle(dialogTitle);

	//centerWidgetOnScreen(dialog);
	//restoreFETDialogGeometry(dialog, dialogName);

	constraintsListWidget=new QListWidget;
	constraintsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	constraintDescriptionTextEdit=new QTextEdit;
	constraintDescriptionTextEdit->setReadOnly(true);

	filterCheckBox=new QCheckBox(tr("Filter"));
	filterCheckBox->setChecked(false);
	sortedCheckBox=new QCheckBox(tr("Sorted", "It refers to space constraints"));
	sortedCheckBox->setChecked(false);

	countOfConstraintsLabel=new QLabel;
	mSLabel=new QLabel;
	activatePushButton=new QPushButton(tr("Activate"));
	deactivatePushButton=new QPushButton(tr("Deactivate"));
	weightsPushButton=new QPushButton(tr("Weights"));
	commentsPushButton=new QPushButton(tr("Comments"));

	addPushButton=new QPushButton(tr("Add"));
	modifyPushButton=new QPushButton(tr("Modify"));
	modifyPushButton->setDefault(true);
	removePushButton=new QPushButton(tr("Remove"));
	closePushButton=new QPushButton(tr("Close"));

	QVBoxLayout* filterAndDescriptionLayout=new QVBoxLayout;
	filterAndDescriptionLayout->addWidget(constraintDescriptionTextEdit);
	if(filterGroupBox!=nullptr)
		filterAndDescriptionLayout->addWidget(filterGroupBox);

	QHBoxLayout* upLayout=new QHBoxLayout;
	upLayout->addWidget(countOfConstraintsLabel);
	upLayout->addWidget(mSLabel);
	QVBoxLayout* leftLayout=new QVBoxLayout;
	leftLayout->addLayout(upLayout);
	leftLayout->addWidget(constraintsListWidget);

	/*QHBoxLayout* constraintsLayout=new QHBoxLayout;
	constraintsLayout->addLayout(leftLayout);
	constraintsLayout->addLayout(filterAndDescriptionLayout);*/
	QWidget* leftWidget=new QWidget;
	leftWidget->setLayout(leftLayout);
	QWidget* filterAndDescriptionWidget=new QWidget;
	filterAndDescriptionWidget->setLayout(filterAndDescriptionLayout);
	splitter->addWidget(leftWidget);
	splitter->addWidget(filterAndDescriptionWidget);
	//restore splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(dialogName+QString("/splitter-state")))
		splitter->restoreState(settings.value(dialogName+QString("/splitter-state")).toByteArray());

	QHBoxLayout* buttons1Layout=new QHBoxLayout;
	buttons1Layout->addStretch();
	buttons1Layout->addWidget(addPushButton);
	buttons1Layout->addWidget(modifyPushButton);
	buttons1Layout->addWidget(removePushButton);
	if(helpPushButton!=nullptr)
		buttons1Layout->addWidget(helpPushButton);
	buttons1Layout->addWidget(commentsPushButton);

	QHBoxLayout* buttons2Layout=new QHBoxLayout;
	buttons2Layout->addStretch();
	buttons2Layout->addWidget(filterCheckBox);
	buttons2Layout->addWidget(sortedCheckBox);
	buttons2Layout->addWidget(activatePushButton);
	buttons2Layout->addWidget(deactivatePushButton);
	buttons2Layout->addWidget(weightsPushButton);
	buttons2Layout->addWidget(closePushButton);

	QVBoxLayout* wholeDialog=new QVBoxLayout(dialog);
	if(firstInstructionsLabel!=nullptr)
		wholeDialog->addWidget(firstInstructionsLabel);
	if(secondInstructionsLabel!=nullptr)
		wholeDialog->addWidget(secondInstructionsLabel);
	//wholeDialog->addLayout(constraintsLayout);
	wholeDialog->addWidget(splitter);
	wholeDialog->addLayout(buttons1Layout);
	if(addRemoveMultipleConstraintsPushButton!=nullptr){
		//wholeDialog->addWidget(addRemoveMultipleConstraintsPushButton);
		QHBoxLayout* layoutM=new QHBoxLayout;
		layoutM->addStretch();
		layoutM->addWidget(addRemoveMultipleConstraintsPushButton);
		wholeDialog->addLayout(layoutM);
	}
	wholeDialog->addLayout(buttons2Layout);

	connect(addPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::addClicked);
	connect(modifyPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::modifyClicked);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ListSpaceConstraints::modifyClicked);
	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ListSpaceConstraints::constraintChanged);
	connect(removePushButton, &QPushButton::clicked, this, &ListSpaceConstraints::removeClicked);
	
	if(helpPushButton!=nullptr)
		connect(helpPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::helpClicked);
	connect(closePushButton, &QPushButton::clicked, this, &ListSpaceConstraints::closeClicked);

	if(addRemoveMultipleConstraintsPushButton!=nullptr)
		connect(addRemoveMultipleConstraintsPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::addRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsClicked);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ListSpaceConstraints::constraintChanged);

	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListSpaceConstraints::selectionChanged);

	connect(filterCheckBox, &QCheckBox::toggled, this, &ListSpaceConstraints::advancedFilter);
	connect(sortedCheckBox, &QCheckBox::toggled, this, &ListSpaceConstraints::sortedChanged);

	connect(activatePushButton, &QPushButton::clicked, this, &ListSpaceConstraints::activateConstraints);
	connect(deactivatePushButton, &QPushButton::clicked, this, &ListSpaceConstraints::deactivateConstraints);
	connect(commentsPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::constraintComments);
	connect(weightsPushButton, &QPushButton::clicked, this, &ListSpaceConstraints::changeWeights);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), dialog);
		connect(addShortcut, &QShortcut::activated, [=]{addPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), dialog);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, dialog);
		connect(removeShortcut, &QShortcut::activated, [=]{removePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removePushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), dialog);
		connect(activateShortcut, &QShortcut::activated, [=]{activatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activatePushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), dialog);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivatePushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), dialog);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_W){
		QShortcut* weightsShortcut=new QShortcut(QKeySequence(Qt::Key_W), dialog);
		connect(weightsShortcut, &QShortcut::activated, [=]{weightsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	weightsPushButton->setToolTip(QString("W"));
	}

	filter();

	constraintsListWidget->setFocus();

	dialog->setModal(true);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->show();

	eventLoop->exec();
}

ListSpaceConstraints::~ListSpaceConstraints()
{
	//saveFETDialogGeometry(dialog, dialogName);

	//dialog->hide();

	//delete dialog;

	///////
	QSettings settings(COMPANY, PROGRAM);
	QString settingsName=dialogName+"AdvancedFilterForm";

	settings.setValue(settingsName+"/all-conditions", all);

	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescr.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescr.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescr.at(i));

	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));

	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));

	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
	///////

	assert(!eventLoop->isRunning());
	delete eventLoop;
}

void ListSpaceConstraints::advancedFilter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;

		filter();

		//constraintsListWidget->setFocus();

		return;
	}

	assert(active);

	filterForm=new AdvancedFilterForm(dialog, tr("Advanced filter for space constraints"), false, all, descrDetDescr, contains, text, caseSensitive, dialogName+"AdvancedFilterForm");

	int t=filterForm->exec();

	if(t==QDialog::Accepted){
		assert(useFilter==false);
		useFilter=true;

		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);

		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();

		descrDetDescr.clear();
		contains.clear();
		text.clear();

		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);

			descrDetDescr.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}

		filter();

		//constraintsListWidget->setFocus();
	}
	else{
		assert(useFilter==false);
		useFilter=false;

		disconnect(filterCheckBox, &QCheckBox::toggled, this, &ListSpaceConstraints::advancedFilter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &ListSpaceConstraints::advancedFilter);
	}

	delete filterForm;
}

bool ListSpaceConstraints::filterOk(SpaceConstraint* sc)
{
	if(sc->type!=type)
		return false;

	if(useFilter){
		assert(descrDetDescr.count()==contains.count());
		assert(contains.count()==text.count());

		Qt::CaseSensitivity csens=Qt::CaseSensitive;
		if(!caseSensitive)
			csens=Qt::CaseInsensitive;

		QList<int> perm;
		for(int i=0; i<descrDetDescr.count(); i++)
			perm.append(i);
		//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries.
		std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescr.at(a)<descrDetDescr.at(b);});
		for(int i=0; i<perm.count()-1; i++)
			assert(descrDetDescr.at(perm.at(i))<=descrDetDescr.at(perm.at(i+1)));

		int firstPosWithDescr=-1;
		int firstPosWithDetDescr=-1;
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescr.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
				firstPosWithDescr=i;
			}
			else if(descrDetDescr.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
				firstPosWithDetDescr=i;
			}
		}

		QString s=QString("");
		for(int i=0; i<perm.count(); i++){
			if(descrDetDescr.at(perm.at(i))==DESCRIPTION){
				assert(firstPosWithDescr>=0);

				if(i==firstPosWithDescr)
					s=sc->getDescription(gt.rules);
			}
			else{
				assert(descrDetDescr.at(perm.at(i))==DETDESCRIPTION);

				assert(firstPosWithDetDescr>=0);

				if(i==firstPosWithDetDescr)
					s=sc->getDetailedDescription(gt.rules);
			}

			bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.

			QString t=text.at(perm.at(i));
			if(contains.at(perm.at(i))==CONTAINS){
				okPartial=s.contains(t, csens);
			}
			else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
				okPartial=!(s.contains(t, csens));
			}
			else if(contains.at(perm.at(i))==REGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=(regExp.match(s)).hasMatch();
			}
			else if(contains.at(perm.at(i))==NOTREGEXP){
				QRegularExpression regExp(t);
				if(!caseSensitive)
					regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				okPartial=!(regExp.match(s)).hasMatch();
			}
			else
				assert(0);

			if(all && !okPartial)
				return false;
			else if(!all && okPartial)
				goto filtered_ok;
		}

		if(!all)
			return false;
	}

filtered_ok:
	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				return true;

				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				assert(roomsComboBox!=nullptr);

				ConstraintRoomNotAvailableTimes* ctr=(ConstraintRoomNotAvailableTimes*)sc;
				return roomsComboBox->currentText()=="" || ctr->room==roomsComboBox->currentText();

				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintActivityPreferredRoom* ctr=(ConstraintActivityPreferredRoom*)sc;

				int id=ctr->activityId;
				Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);

				if(act!=nullptr){
					if(teachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(teachersComboBox->currentText()))
						return false;
					if(subjectsComboBox->currentText()!=QString("") && subjectsComboBox->currentText()!=act->subjectName)
						return false;
					if(activityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(activityTagsComboBox->currentText()))
						return false;
					if(studentsComboBox->currentText()!=QString("") && !showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
						return false;
				}

				return ctr->roomName==roomsComboBox->currentText()
				 || ctr->preferredRealRoomsNames.contains(roomsComboBox->currentText())
				 || roomsComboBox->currentText()=="";

				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintActivityPreferredRooms* ctr=(ConstraintActivityPreferredRooms*)sc;

				int id=ctr->activityId;
				Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);

				if(act!=nullptr){
					if(teachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(teachersComboBox->currentText()))
						return false;
					if(subjectsComboBox->currentText()!=QString("") && subjectsComboBox->currentText()!=act->subjectName)
						return false;
					if(activityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(activityTagsComboBox->currentText()))
						return false;
					if(studentsComboBox->currentText()!=QString("") && !showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
						return false;
				}

				return roomsComboBox->currentText()==QString("") || ctr->roomsNames.contains(roomsComboBox->currentText());

				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				assert(studentsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintStudentsSetHomeRoom* ctr=(ConstraintStudentsSetHomeRoom*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomName==roomsComboBox->currentText())
				 && (studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName));

				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				assert(studentsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintStudentsSetHomeRooms* ctr=(ConstraintStudentsSetHomeRooms*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomsNames.contains(roomsComboBox->currentText()))
				 && (studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName));

				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				assert(teachersComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintTeacherHomeRoom* ctr=(ConstraintTeacherHomeRoom*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomName==roomsComboBox->currentText())
				 && (teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText());

				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				assert(teachersComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintTeacherHomeRooms* ctr=(ConstraintTeacherHomeRooms*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomsNames.contains(roomsComboBox->currentText()))
				 && (teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText());

				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				assert(subjectsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintSubjectPreferredRoom* ctr=(ConstraintSubjectPreferredRoom*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomName==roomsComboBox->currentText())
				 && (subjectsComboBox->currentText()=="" || ctr->subjectName==subjectsComboBox->currentText());

				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				assert(subjectsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintSubjectPreferredRooms* ctr=(ConstraintSubjectPreferredRooms*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomsNames.contains(roomsComboBox->currentText()))
				 && (subjectsComboBox->currentText()=="" || ctr->subjectName==subjectsComboBox->currentText());

				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintSubjectActivityTagPreferredRoom* ctr=(ConstraintSubjectActivityTagPreferredRoom*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomName==roomsComboBox->currentText())
				 && (subjectsComboBox->currentText()=="" || ctr->subjectName==subjectsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->activityTagName==activityTagsComboBox->currentText());

				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintSubjectActivityTagPreferredRooms* ctr=(ConstraintSubjectActivityTagPreferredRooms*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomsNames.contains(roomsComboBox->currentText()))
				 && (subjectsComboBox->currentText()=="" || ctr->subjectName==subjectsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->activityTagName==activityTagsComboBox->currentText());

				break;
			}
		//13
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				return true;

				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ConstraintStudentsSetMaxBuildingChangesPerDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDay*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				return true;

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ConstraintStudentsSetMaxBuildingChangesPerWeek* ctr=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				return true;

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ConstraintStudentsSetMinGapsBetweenBuildingChanges* ctr=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				return true;

				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ConstraintTeacherMaxBuildingChangesPerDay* ctr=(ConstraintTeacherMaxBuildingChangesPerDay*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				return true;

				break;
			}
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ConstraintTeacherMaxBuildingChangesPerWeek* ctr=(ConstraintTeacherMaxBuildingChangesPerWeek*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				return true;

				break;
			}
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ConstraintTeacherMinGapsBetweenBuildingChanges* ctr=(ConstraintTeacherMinGapsBetweenBuildingChanges*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//25
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintActivityTagPreferredRoom* ctr=(ConstraintActivityTagPreferredRoom*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomName==roomsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->activityTagName==activityTagsComboBox->currentText());

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				assert(activityTagsComboBox!=nullptr);

				assert(roomsComboBox!=nullptr);

				ConstraintActivityTagPreferredRooms* ctr=(ConstraintActivityTagPreferredRooms*)sc;

				return (roomsComboBox->currentText()=="" || ctr->roomsNames.contains(roomsComboBox->currentText()))
				 && (activityTagsComboBox->currentText()=="" || ctr->activityTagName==activityTagsComboBox->currentText());

				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				if(teachersComboBox->currentText()==QString("")
						&& subjectsComboBox->currentText()==QString("")
						&& activityTagsComboBox->currentText()==QString("")
						&& studentsComboBox->currentText()==QString(""))
					return true;

				ConstraintActivitiesOccupyMaxDifferentRooms* ctr=(ConstraintActivitiesOccupyMaxDifferentRooms*)sc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int id : ctr->activitiesIds){
					Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);

					if(act!=nullptr){
						if(teachersComboBox->currentText()==QString("") || act->teachersNames.contains(teachersComboBox->currentText()))
							foundTeacher=true;
						if(subjectsComboBox->currentText()==QString("") || subjectsComboBox->currentText()==act->subjectName)
							foundSubject=true;
						if(activityTagsComboBox->currentText()==QString("") || act->activityTagsNames.contains(activityTagsComboBox->currentText()))
							foundActivityTag=true;
						if(studentsComboBox->currentText()==QString("") || showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
							foundStudents=true;
					}

					if(foundTeacher && foundSubject && foundActivityTag && foundStudents)
						return true;
				}

				return false;

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				if(teachersComboBox->currentText()==QString("")
						&& subjectsComboBox->currentText()==QString("")
						&& activityTagsComboBox->currentText()==QString("")
						&& studentsComboBox->currentText()==QString(""))
					return true;

				ConstraintActivitiesSameRoomIfConsecutive* ctr=(ConstraintActivitiesSameRoomIfConsecutive*)sc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int id : ctr->activitiesIds){
					Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);

					if(act!=nullptr){
						if(teachersComboBox->currentText()==QString("") || act->teachersNames.contains(teachersComboBox->currentText()))
							foundTeacher=true;
						if(subjectsComboBox->currentText()==QString("") || subjectsComboBox->currentText()==act->subjectName)
							foundSubject=true;
						if(activityTagsComboBox->currentText()==QString("") || act->activityTagsNames.contains(activityTagsComboBox->currentText()))
							foundActivityTag=true;
						if(studentsComboBox->currentText()==QString("") || showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
							foundStudents=true;
					}

					if(foundTeacher && foundSubject && foundActivityTag && foundStudents)
						return true;
				}

				return false;

				break;
			}
		//29
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				return true;

				break;
			}
		//30
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				ConstraintStudentsSetMaxRoomChangesPerDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerDay*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//31
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				return true;

				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ConstraintStudentsSetMaxRoomChangesPerWeek* ctr=(ConstraintStudentsSetMaxRoomChangesPerWeek*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				return true;

				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ConstraintStudentsSetMinGapsBetweenRoomChanges* ctr=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				return true;

				break;
			}
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				ConstraintTeacherMaxRoomChangesPerDay* ctr=(ConstraintTeacherMaxRoomChangesPerDay*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				return true;

				break;
			}
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ConstraintTeacherMaxRoomChangesPerWeek* ctr=(ConstraintTeacherMaxRoomChangesPerWeek*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				return true;

				break;
			}
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ConstraintTeacherMinGapsBetweenRoomChanges* ctr=(ConstraintTeacherMinGapsBetweenRoomChanges*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				assert(roomsComboBox!=nullptr);

				ConstraintTeacherRoomNotAvailableTimes* ctr=(ConstraintTeacherRoomNotAvailableTimes*)sc;
				return (roomsComboBox->currentText()=="" || ctr->room==roomsComboBox->currentText())
						&& (teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText());

				break;
			}
		//42
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//43
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ConstraintStudentsSetMaxRoomChangesPerRealDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDay*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ConstraintTeacherMaxRoomChangesPerRealDay* ctr=(ConstraintTeacherMaxRoomChangesPerRealDay*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//46
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//47
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ConstraintStudentsSetMaxBuildingChangesPerRealDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDay*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ConstraintTeacherMaxBuildingChangesPerRealDay* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDay*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ConstraintStudentsSetMaxBuildingChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDayInInterval*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ConstraintTeacherMaxBuildingChangesPerDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerDayInInterval*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//55
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ConstraintTeacherMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDayInInterval*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ConstraintStudentsSetMaxRoomChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerDayInInterval*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ConstraintTeacherMaxRoomChangesPerDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerDayInInterval*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval*)sc;

				return studentsComboBox->currentText()=="" || showedStudents.contains(ctr->studentsName);

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ConstraintTeacherMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerRealDayInInterval*)sc;

				return teachersComboBox->currentText()=="" || ctr->teacherName==teachersComboBox->currentText();

				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				assert(roomsComboBox!=nullptr);

				assert(activityTagsComboBox!=nullptr);

				ConstraintRoomMaxActivityTagsPerDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerDayFromSet*)sc;

				return (roomsComboBox->currentText()=="" || ctr->room==roomsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				assert(roomsComboBox!=nullptr);

				assert(activityTagsComboBox!=nullptr);

				ConstraintRoomMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerRealDayFromSet*)sc;

				return (roomsComboBox->currentText()=="" || ctr->room==roomsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				assert(roomsComboBox!=nullptr);

				assert(activityTagsComboBox!=nullptr);

				ConstraintRoomMaxActivityTagsPerWeekFromSet* ctr=(ConstraintRoomMaxActivityTagsPerWeekFromSet*)sc;

				return (roomsComboBox->currentText()=="" || ctr->room==roomsComboBox->currentText())
				 && (activityTagsComboBox->currentText()=="" || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}

		default:
			assert(0);
			break;
	}
}

/*void ListSpaceConstraints::filter()
{
	visibleSpaceConstraintsList.clear();
	constraintsListWidget->clear();

	int k=0;

	for(SpaceConstraint* tc : std::as_const(gt.rules.spaceConstraintsList)){
		if(filterOk(tc)){
			visibleSpaceConstraintsList.append(tc);
			constraintsListWidget->addItem(tc->getDescription(gt.rules));
			k++;

			if((true || USE_GUI_COLORS) && !tc->active)
				constraintsListWidget->item(k-1)->setBackground(constraintsListWidget->palette().alternateBase());
		}
	}
}

void ListSpaceConstraints::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0){
		constraintDescriptionTextEdit->setPlainText(QString(""));
		return;
	}

	assert(index<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	constraintDescriptionTextEdit->setPlainText(s);
}*/

void ListSpaceConstraints::addClicked()
{
	int oldRow=constraintsListWidget->currentRow();

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int initialNumberOfSpaceConstraints=gt.rules.spaceConstraintsList.count();

	QString preselectedTeacherName;
	QString preselectedStudentsSetName;
	QString preselectedSubjectName;
	QString preselectedActivityTagName;
	QString preselectedRoomName;

	if(teachersComboBox!=nullptr)
		preselectedTeacherName=teachersComboBox->currentText();
	else
		preselectedTeacherName=QString();

	if(studentsComboBox!=nullptr)
		preselectedStudentsSetName=studentsComboBox->currentText();
	else
		preselectedStudentsSetName=QString();

	if(subjectsComboBox!=nullptr)
		preselectedSubjectName=subjectsComboBox->currentText();
	else
		preselectedSubjectName=QString();

	if(activityTagsComboBox!=nullptr)
		preselectedActivityTagName=activityTagsComboBox->currentText();
	else
		preselectedActivityTagName=QString();

	if(roomsComboBox!=nullptr)
		preselectedRoomName=roomsComboBox->currentText();
	else
		preselectedRoomName=QString();

	AddOrModifySpaceConstraint aomsc(dialog, type, nullptr,
	 preselectedTeacherName, preselectedStudentsSetName, preselectedSubjectName, preselectedActivityTagName,
	 preselectedRoomName);

	int finalNumberOfSpaceConstraints=gt.rules.spaceConstraintsList.count();

	filter();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);
	
	int newRow=oldRow;
	if(initialNumberOfSpaceConstraints!=finalNumberOfSpaceConstraints){
		newRow=constraintsListWidget->count()-1;
		
		if(sortedCheckBox->isChecked()){
			assert(finalNumberOfSpaceConstraints>=1);
			for(int i=0; i<visibleSpaceConstraintsList.count(); i++)
				if(visibleSpaceConstraintsList.at(i)==gt.rules.spaceConstraintsList.constLast()){
					newRow=i;
					break;
				}
		}
	}
	
	constraintsListWidget->setCurrentRow(newRow);
	constraintChanged();

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::modifyClicked()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected constraint"));

		//constraintsListWidget->setFocus();

		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	AddOrModifySpaceConstraint aomsc(dialog, type, ctr);

	filter();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);

	constraintChanged();

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::removeClicked()
{
	bool recompute=false;

	QList<SpaceConstraint*> tl;

	bool firstBasic=true;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			tl.append(ctr);

			if(firstBasic && ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
				firstBasic=false;

				QMessageBox::StandardButton wr;

				QString s=tr("Your selection contains the basic compulsory space constraint(s). Do you really want to remove all the selected "
				 "constraints, including this (these) one(s)? You cannot generate a timetable without this (these) constraint(s).");
				s+=QString("\n\n");
				s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
					"Miscellaneous -> Basic compulsory space constraints.");

				wr=QMessageBox::warning(dialog, tr("FET warning"), s,
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

				if(wr==QMessageBox::No){
					//constraintsListWidget->setFocus();
					return;
				}
			}
			else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				recompute=true;
			}
		}

	QString s;
	s=tr("Remove these selected space constraints?");
	s+=QString("\n\n");
	for(SpaceConstraint* ctr : std::as_const(tl))
		s+=ctr->getDetailedDescription(gt.rules)+"\n";
	int lres=LongTextMessageBox::confirmation(dialog, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );

	if(lres!=0){
		//constraintsListWidget->setFocus();
		return;
	}

	QString su;
	if(!tl.isEmpty()){
		su=tr("Removed %1 space constraints:").arg(tl.count());
		su+=QString("\n\n");
		for(SpaceConstraint* ctr : std::as_const(tl))
			su+=ctr->getDetailedDescription(gt.rules)+"\n";
	}

	//The user clicked the OK button or pressed Enter

	gt.rules.removeSpaceConstraints(tl);

	if(!tl.isEmpty())
		gt.rules.addUndoPoint(su);

	if(recompute){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int cr=constraintsListWidget->currentRow();

	filter();

	if(cr>=0){
		if(cr<constraintsListWidget->count())
			constraintsListWidget->setCurrentRow(cr);
		else if(constraintsListWidget->count()>0)
			constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
	}

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	constraintChanged();

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::closeClicked()
{
	dialog->close();
}

void ListSpaceConstraints::helpClicked()
{
	switch(type){
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				QString s=QString("");

				s+=tr("To use this constraint, you need to specify a set of activities and the maximum number of different rooms which can be occupied by these "
				 "activities. Note that each activity can take place in a single room, and a single room can hold a single activity at a certain moment. "
				 "This constraint may be used for instance to constrain all activities of type Physics of a certain students set to be in the same room "
				 "throughout the week, by adding all Physics activities for this students set and making max different rooms = 1.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}

		default: assert(0);
	}
}

void ListSpaceConstraints::addRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsClicked()
{
	assert(type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS);

	AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm form(dialog);
	setParentAndOtherThings(&form, dialog);
	form.exec();

	filter();

	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
	constraintChanged();
}

void ListSpaceConstraints::filter()
{
	disconnect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListSpaceConstraints::selectionChanged);

	visibleSpaceConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList))
		if(filterOk(ctr))
			visibleSpaceConstraintsList.append(ctr);

	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleSpaceConstraintsList.begin(), visibleSpaceConstraintsList.end(), spaceConstraintsAscendingByDescription);

	for(SpaceConstraint* ctr : std::as_const(visibleSpaceConstraintsList)){
		assert(filterOk(ctr));
		constraintsListWidget->addItem(ctr->getDescription(gt.rules));

		if(ctr->active){
			n_active++;
		}
		else{
			constraintsListWidget->item(constraintsListWidget->count()-1)->setBackground(constraintsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			constraintsListWidget->item(constraintsListWidget->count()-1)->setForeground(constraintsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}

	if(constraintsListWidget->count()<=0)
		constraintDescriptionTextEdit->setPlainText("");
	else
		constraintsListWidget->setCurrentRow(0);

	countOfConstraintsLabel->setText(tr("No: %1 / %2",
	 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
	 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));

	selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListSpaceConstraints::selectionChanged);
}

void ListSpaceConstraints::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;

	assert(index<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	constraintDescriptionTextEdit->setPlainText(s);
}

void ListSpaceConstraints::sortedChanged(bool checked)
{
	Q_UNUSED(checked);

	filter();

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::activateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected space constraints?");
		ret=QMessageBox::question(dialog, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			//constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(!ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=true;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated %1 space constraints:", "%1 is the number of activated space constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filter();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);

		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(dialog, tr("FET information"), tr("Activated %1 space constraints").arg(cnt));
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::deactivateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected space constraints? "
		 "(Note that the basic compulsory space constraints will not be deactivated, even if they are selected.)");
		ret=QMessageBox::question(dialog, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			//constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
				continue;
			if(ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=false;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated %1 space constraints:", "%1 is the number of deactivated space constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filter();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);

		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(dialog, tr("FET information"), tr("Deactivated %1 space constraints").arg(cnt));
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected constraint"));

		//constraintsListWidget->setFocus();

		return;
	}

	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	QDialog getCommentsDialog(dialog);

	getCommentsDialog.setWindowTitle(tr("Constraint comments"));

	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));

	connect(okPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);

	QVBoxLayout* vl=new QVBoxLayout();

	QTextEdit* commentsPT=new QTextEdit();
	commentsPT->setPlainText(ctr->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();

	vl->addWidget(commentsPT);
	vl->addLayout(hl);

	getCommentsDialog.setLayout(vl);

	const QString settingsName=QString("SpaceConstraintCommentsDialog");

	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);

	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);

	if(t==QDialog::Accepted){
		QString cb=ctr->getDetailedDescription(gt.rules);

		ctr->comments=commentsPT->toPlainText();

		gt.rules.addUndoPoint(tr("Changed a constraint's comments. Constraint before:\n\n%1\nComments after:\n\n%2").arg(cb).arg(ctr->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		if(!filterOk(ctr)){ //Maybe the constraint is no longer visible in the list widget, because of the filter.
			visibleSpaceConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			QListWidgetItem* item=constraintsListWidget->takeItem(i);
			delete item;

			if(i>=constraintsListWidget->count())
				i=constraintsListWidget->count()-1;
			if(i>=0)
				constraintsListWidget->setCurrentRow(i);
			else
				constraintDescriptionTextEdit->setPlainText(QString(""));

			int n_active=0;
			for(SpaceConstraint* ctr2 : std::as_const(visibleSpaceConstraintsList))
				if(ctr2->active)
					n_active++;

			countOfConstraintsLabel->setText(tr("%1 / %2 space constraints",
			 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
			 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
		}
		else{
			constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
			constraintChanged();
		}
	}

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(constraintsListWidget->count()==visibleSpaceConstraintsList.count());
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleSpaceConstraintsList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected space constraints, %1 is the number of active"
	 " selected space constraints, %2 is the total number of selected space constraints").arg(nActive).arg(nTotal));
}

void ListSpaceConstraints::changeWeights()
{
	int cnt_pre=0;
	int cnt_unchanged=0;
	double nw=100.0;
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(ctr->canHaveAnyWeight())
				cnt_pre++;
			else
				cnt_unchanged++;
		}
	if(cnt_pre==0){
		QMessageBox::information(dialog, tr("FET information"), tr("No constraints from your selection can change their weight"
		 " (remember that some types of constraints are allowed to have only 100% weight)."));

		//constraintsListWidget->setFocus();

		return;
	}
	else{
		bool ok;
		if(cnt_unchanged==0)
			nw=QInputDialog::getDouble(dialog, tr("Modify the weights of the selected space constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 selected space constraints.\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (3 or even 4), if needed. %1 is the number of constraints which will change.")
			 .arg(cnt_pre),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		else
			nw=QInputDialog::getDouble(dialog, tr("Modify the weights of the selected space constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 space constraints from your\n"
			 "selection (remember that some types of constraints\n"
			 "are only allowed to have 100% weight, so %2\n"
			 "constraints out of the %3 selected will not change).\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (6 or even 7), if needed. %1 is the number of constraints which will change, %2 is the number of constraints"
			 " which will not change, and %3 is the number of all selected constraints.")
			 .arg(cnt_pre).arg(cnt_unchanged).arg(cnt_pre+cnt_unchanged),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);

		if(!ok)
			return;
	}

	QString su;

	int cnt=0;
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(ctr->canHaveAnyWeight()){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->weightPercentage=nw;
				if(ctr->active && ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}

	assert(cnt>0);
	assert(cnt==cnt_pre);

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Changed the weights of the following %1 selected space constraints to %2%:",
		 "%1 is the number of space constraints for which the user has changed the weight, %2 is the new weight for all the selected constraints")
		 .arg(cnt).arg(CustomFETString::number(nw))+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filter();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListSpaceConstraints::showRelatedCheckBoxToggled()
{
	assert(studentsComboBox!=nullptr);
	
	bool showRelated=showRelatedCheckBox->isChecked();
	
	showedStudents.clear();
	
	if(!showRelated){
		showedStudents.insert(studentsComboBox->currentText());
	}
	else{
		if(studentsComboBox->currentText()=="")
			showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(studentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=studentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							showedStudents.insert(year->name);
							showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}
	
	filter();
}
