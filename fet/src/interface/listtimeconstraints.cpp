/***************************************************************************
                          listtimeconstraints.cpp  -  description
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

#include "listtimeconstraints.h"

#include "addormodifytimeconstraint.h"
#include "modifytimeconstraints.h"

#include "timetable.h"

#include "longtextmessagebox.h"

#include "lockunlock.h"

#include "helponstudentsminhoursdaily.h"

#include "changemindaysselectivelyform.h"
#include "changeminhalfdaysselectivelyform.h"

#include "addremovemultipleconstraintsactivitiessamestartinghourform.h"

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

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

int timeConstraintsAscendingByDescription(TimeConstraint* t1, TimeConstraint* t2); //defined in alltimeconstraints.cpp

ListTimeConstraintsDialog::ListTimeConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop, QSplitter* _splitter,
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

ListTimeConstraintsDialog::~ListTimeConstraintsDialog()
{
	saveFETDialogGeometry(this, dialogName);

	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(dialogName+QString("/splitter-state"), splitter->saveState());

	if(showRelatedCheckBox!=nullptr)
		settings.setValue(dialogName+QString("/show-related"), showRelatedCheckBox->isChecked());

	eventLoop->quit();
}

ListTimeConstraints::ListTimeConstraints(QWidget* parent, int _type)
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

	first_activityTagsComboBox=nullptr;
	second_activityTagsComboBox=nullptr;

	helpPushButton=nullptr;

	modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton=nullptr;

	addRemoveMultipleConstraintsPushButton=nullptr;

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				dialogTitle=tr("Constraints basic compulsory time", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsBasicCompulsoryTime");
				
				break;
			}
		//2
		case CONSTRAINT_BREAK_TIMES:
			{
				dialogTitle=tr("Constraints break times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsBreakTimes");
				
				firstInstructionsLabel=new QLabel(tr("Note: An activity with duration greater than 1 cannot be split over a break. "
					"Also, the activities constrained by min days between activities constraints with 'consecutive if on the same "
					"day' = true cannot spread over a break."));
				
				break;
			}
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				dialogTitle=tr("Constraints teacher not available times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherNotAvailableTimes");

				teachersComboBox=new QComboBox;

				break;
			}
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teachers max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursDaily");

				firstInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise the algorithm is not perfect)."));

				break;
			}
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxDaysPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max gaps per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxGapsPerWeek");

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max gaps per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxGapsPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teacher max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursDaily");

				firstInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise the algorithm is not perfect)."));

				teachersComboBox=new QComboBox;

				break;
			}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints teachers max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursContinuously");

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints teacher max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursContinuously");

				teachersComboBox=new QComboBox;

				break;
			}
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teachers min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinHoursDaily");

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teacher min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinHoursDaily");

				teachersComboBox=new QComboBox;

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				dialogTitle=tr("Constraints teachers max gaps per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxGapsPerDay");

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				dialogTitle=tr("Constraints teacher max gaps per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxGapsPerDay");

				teachersComboBox=new QComboBox;

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsEarlyMaxBeginningsAtSecondHour");

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students set early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetEarlyMaxBeginningsAtSecondHour");

				studentsComboBox=new QComboBox;

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				dialogTitle=tr("Constraints students set not available times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintStudentsSetNotAvailableTimes");

				studentsComboBox=new QComboBox;

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max gaps per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxGapsPerWeek");

				break;
			}
		//19
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max gaps per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxGapsPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//20
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxHoursDaily");

				firstInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise the algorithm is not perfect)."));

				break;
			}
		//21
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students set max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxHoursDaily");

				firstInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise the algorithm is not perfect)."));

				studentsComboBox=new QComboBox;

				break;
			}
		//22
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints students max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxHoursContinuously");

				break;
			}
		//23
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints students set max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxHoursContinuously");

				studentsComboBox=new QComboBox;

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinHoursDaily");

				if(gt.rules.mode==MORNINGS_AFTERNOONS)
					firstInstructionsLabel=new QLabel(tr("If the user selects not allowed empty days, it is for real days "
														 "(for the mornings-afternoons mode), also respecting minimum hours "
														 "daily for FET days (half days) with allow empty FET days"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students set min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinHoursDaily");

				if(gt.rules.mode==MORNINGS_AFTERNOONS)
					firstInstructionsLabel=new QLabel(tr("If the user selects not allowed empty days, it is for real days "
														 "(for the mornings-afternoons mode), also respecting minimum hours "
														 "daily for FET days (half days) with allow empty FET days"));

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activity ends students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityEndsStudentsDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				dialogTitle=tr("Constraints activity preferred starting time", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityPreferredStartingTime");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				dialogTitle=tr("Constraints activities same starting time", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesSameStartingTime");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Please read Help/Important tips, advice 2). It is IMPORTANT to "
													 "remove redundant min days between activities constraints with weight < 100% after "
													 "adding constraints same starting time. Click the Help button!"));
				secondInstructionsLabel=new QLabel(tr("Note: You might not need this constraint. Click Help!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//29
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
			{
				dialogTitle=tr("Constraints activities not overlapping", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesNotOverlapping");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints min days between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMinDaysBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Min days is for real days. If the activities are on the same real day and consecutive "
													 "if on the same day is checked, they must be consecutive and on the same half day."));
				firstInstructionsLabel->setVisible(gt.rules.mode==MORNINGS_AFTERNOONS);

				modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton=new QPushButton(tr("Modify multiple constraints at once"));

				break;
			}
		//31
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activity preferred time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityPreferredTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Each hour slot of each considered activity must be allowed by this constraint (more "
													 "restrictive than preferred starting times for activities with duration greater than 1)"));

				break;
			}
		//32
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities preferred time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesPreferredTimeSlots");

				firstInstructionsLabel=new QLabel(tr("Each hour slot of each considered activity must be allowed by this constraint (more "
													 "restrictive than preferred starting times for activities with duration greater than 1)"));

				break;
			}
		//33
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				dialogTitle=tr("Constraints activity preferred starting times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityPreferredStartingTimes");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//34
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				dialogTitle=tr("Constraints activities preferred starting times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesPreferredStartingTimes");

				break;
			}
		//35
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
			{
				dialogTitle=tr("Constraints activities same starting hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesSameStartingHour");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				addRemoveMultipleConstraintsPushButton=new QPushButton(tr("Add/remove multiple constraints"));

				break;
			}
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				dialogTitle=tr("Constraints activities same starting day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesSameStartingDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Please read Help/Important tips, advice 2). It is IMPORTANT to remove redundant "
													 "min days between activities constraints with weight < 100% after adding constraints same starting day. "
													 "Please also click the Help button here for information!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//37
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
			{
				dialogTitle=tr("Constraints two activities consecutive", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoActivitiesConsecutive");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces two activities A1 and A2: A2 to be immediately after A1, "
													 "same day; no gaps are allowed between them, except for break constraints."));
				secondInstructionsLabel=new QLabel(tr("It is a good practice NOT to add this type of constraint to activities which are "
													  "constrained to be in different days by min days between activities constraints. "
													  "Please read Help/Important tips, tip number 3)."));

				break;
			}
		//38
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
			{
				dialogTitle=tr("Constraints two activities ordered", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoActivitiesOrdered");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces two activities A1 and A2: A2 to begin later than A1 has "
													 "finished, separated by any time interval in the week"));

				break;
			}
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints min gaps between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMinGapsBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures, for a set of activities: if any pair of activities are on the "
													 "same day, they must be separated by at least min gaps (hours)."));
				secondInstructionsLabel=new QLabel(tr("Note: please click Help for important advice"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints subactivities preferred time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubactivitiesPreferredTimeSlots");

				firstInstructionsLabel=new QLabel(tr("This constraint is useful to constrain only the n-th component of the activities. Please "
													 "click the Help button for details!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				dialogTitle=tr("Constraints subactivities preferred starting times", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsSubactivitiesPreferredStartingTimes");

				firstInstructionsLabel=new QLabel(tr("This constraint is useful to constrain only the n-th component of the activities. Please "
													 "click the Help button for details!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherIntervalMaxDaysPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersIntervalMaxDaysPerWeek");

				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetIntervalMaxDaysPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsIntervalMaxDaysPerWeek");

				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activities end students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesEndStudentsDay");

				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
			{
				dialogTitle=tr("Constraints two activities grouped", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoActivitiesGrouped");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces two activities to be on the same day, one immediately "
													 "following the other, in any order; no gaps are allowed between them, except for break constraints."));
				secondInstructionsLabel=new QLabel(tr("It is a good practice NOT to add this type of constraint to activities which "
													  "are constrained to be in different days by min days between activities "
													  "constraints. Please read Help/Important tips, tip number 3)."));

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints teachers activity tag max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersActivityTagMaxHoursContinuously");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all teachers do not have more than max hours continuously of activities "
													 "with the specified activity tag"));

				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints teacher activity tag max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherActivityTagMaxHoursContinuously");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have more than max hours continuously "
													 "of activities with the specified activity tag"));

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints students activity tag max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsActivityTagMaxHoursContinuously");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all students do not have more than max hours continuously of activities "
													 "with the specified activity tag"));

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Constraints students set activity tag max hours continuously", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetActivityTagMaxHoursContinuously");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students do not have more than max hours continuously of "
													 "activities with the specified activity tag"));

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxDaysPerWeek");

				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
			{
				dialogTitle=tr("Constraints three activities grouped", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsThreeActivitiesGrouped");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces 3 activities to be on the same day, one immediately "
													 "following the other, as a block of 3 activities, in any order; no gaps are allowed between them, except for break constraints."));
				secondInstructionsLabel=new QLabel(tr("It is a good practice NOT to add this type of constraint to activities which "
													  "are constrained to be in different days by min days between activities "
													  "constraints. Please read Help/Important tips, tip number 3)."));

				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints max days between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMaxDaysBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Max days is for real days"));
				firstInstructionsLabel->setVisible(gt.rules.mode==MORNINGS_AFTERNOONS);

				secondInstructionsLabel=new QLabel(tr("Please read Help/Important tips, advice 2). It is IMPORTANT to "
													 "remove redundant min days between activities constraints with weight < 100% after "
													 "adding constraints max 0 days between activities. Click the Help button!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers min days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinDaysPerWeek");

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher min days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinDaysPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teachers activity tag max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersActivityTagMaxHoursDaily");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all teachers do not have more than max hours daily of activities "
													 "with the specified activity tag."));

				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teacher activity tag max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherActivityTagMaxHoursDaily");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have more than max hours daily of "
													 "activities with the specified activity tag."));

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students activity tag max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsActivityTagMaxHoursDaily");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all students do not have more than max hours daily of activities "
													 "with the specified activity tag."));

				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students set activity tag max hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetActivityTagMaxHoursDaily");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students do not have more than max hours daily of "
													 "activities with the specified activity tag."));

				break;
			}
		//61
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				dialogTitle=tr("Constraints students max gaps per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxGapsPerDay");

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				dialogTitle=tr("Constraints students set max gaps per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxGapsPerDay");

				studentsComboBox=new QComboBox;

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints activities occupy max time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOccupyMaxTimeSlotsFromSelection");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities max simultaneous in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMaxSimultaneousInSelectedTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxDaysPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxDaysPerWeek");

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				dialogTitle=tr("Constraints teacher max span per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxSpanPerDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the "
													 "count of slots (free or occupied) starting with the first occupied slot of the day "
													 "and ending with the last occupied slot of the day."));

				teachersComboBox=new QComboBox;

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				dialogTitle=tr("Constraints teachers max span per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxSpanPerDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the "
													 "count of slots (free or occupied) starting with the first occupied slot of the day "
													 "and ending with the last occupied slot of the day."));

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				dialogTitle=tr("Constraints teacher min resting hours", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinRestingHours");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of a day "
													 "and the beginning of the next day. Circular means that the time between the end of "
													 "the last day of the week and the beginning of the first day of the week is also "
													 "considered."));

				teachersComboBox=new QComboBox;

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				dialogTitle=tr("Constraints teachers min resting hours", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinRestingHours");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of a day "
													 "and the beginning of the next day. Circular means that the time between the end of "
													 "the last day of the week and the beginning of the first day of the week is also "
													 "considered."));

				break;
			}
		//71
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				dialogTitle=tr("Constraints students set max span per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxSpanPerDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the count "
													 "of slots (free or occupied) starting with the first occupied slot of the day and ending "
													 "with the last occupied slot of the day."));

				studentsComboBox=new QComboBox;

				break;
			}
		//72
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				dialogTitle=tr("Constraints students max span per day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxSpanPerDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the count "
													 "of slots (free or occupied) starting with the first occupied slot of the day and ending "
													 "with the last occupied slot of the day."));

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				dialogTitle=tr("Constraints students set min resting hours", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinRestingHours");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of a day and "
													 "the beginning of the next day. Circular means that the time between the end of the last "
													 "day of the week and the beginning of the first day of the week is also considered."));

				studentsComboBox=new QComboBox;

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				dialogTitle=tr("Constraints students min resting hours", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinRestingHours");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of a day and "
													 "the beginning of the next day. Circular means that the time between the end of the last "
													 "day of the week and the beginning of the first day of the week is also considered."));

				break;
			}
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
			{
				dialogTitle=tr("Constraints two activities ordered if same day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoActivitiesOrderedIfSameDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces two activities A1 and A2: if they are on the same day, A2 needs "
													 "to begin later than A1 has finished."));

				break;
			}
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				dialogTitle=tr("Constraints students set min gaps between ordered pair of activity tags", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTags");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each day, there are at least min gaps (hours) between "
													 "the first activity tag and the second activity tag, if they appear in this order."));

				studentsComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				dialogTitle=tr("Constraints students min gaps between ordered pair of activity tags", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTags");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each day, there are at least min gaps (hours) between the "
													 "first activity tag and the second activity tag, if they appear in this order."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				dialogTitle=tr("Constraints teacher min gaps between ordered pair of activity tags", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTags");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each day, there are at least min gaps (hours) between "
													 "the first activity tag and the second activity tag, if they appear in this order."));

				teachersComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				dialogTitle=tr("Constraints teachers min gaps between ordered pair of activity tags", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTags");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each day, there are at least min gaps (hours) between the "
													 "first activity tag and the second activity tag, if they appear in this order."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				dialogTitle=tr("Constraints activity tags not overlapping", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityTagsNotOverlapping");

				activityTagsComboBox=new QComboBox;

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints activities occupy min time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOccupyMinTimeSlotsFromSelection");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities min simultaneous in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMinSimultaneousInSelectedTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teachers activity tag min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersActivityTagMinHoursDaily");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all teachers do not have less than min hours daily of activities "
													 "with the specified activity tag."));

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints teacher activity tag min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherActivityTagMinHoursDaily");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have less than min hours daily "
													 "of activities with the specified activity tag."));

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students activity tag min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsActivityTagMinHoursDaily");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all students do not have less than min hours daily of activities "
													 "with the specified activity tag."));

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Constraints students set activity tag min hours daily", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetActivityTagMinHoursDaily");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students do not have less than min hours daily of "
													 "activities with the specified activity tag."));

				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activity ends teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityEndsTeachersDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activities end teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesEndTeachersDay");

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teachers max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));
				secondInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise "
													  "the algorithm is not perfect)."));

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				teachersComboBox=new QComboBox;

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teacher max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));
				secondInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise "
													  "the algorithm is not perfect)."));

				teachersComboBox=new QComboBox;

				break;
			}
		//92
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));
				secondInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise "
													  "the algorithm is not perfect)."));

				break;
			}
		//93
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students set max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));
				secondInstructionsLabel=new QLabel(tr("It is recommended to use only a 100% weight for these constraints (otherwise "
													  "the algorithm is not perfect)."));

				studentsComboBox=new QComboBox;

				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers min real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher min real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				teachersComboBox=new QComboBox;

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teachers activity tag max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersActivityTagMaxHoursDailyRealDays");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all teachers do not have more than max hours daily of activities with the "
													 "specified activity tag, for a real day (two half-days)."));

				break;
			}
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teacher activity tag max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherActivityTagMaxHoursDailyRealDays");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have more than max hours daily of activities "
													 "with the specified activity tag, for a real day (two half-days)."));

				break;
			}
		//99
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students activity tag max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsActivityTagMaxHoursDailyRealDays");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all students do not have more than max hours daily of activities with the "
													 "specified activity tag, for a real day (two half-days)."));

				break;
			}
		//100
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students set activity tag max hours daily real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetActivityTagMaxHoursDailyRealDays");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students do not have more than max hours daily of activities "
													 "with the specified activity tag, for a real day (two half-days)."));

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxAfternoonsPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxAfternoonsPerWeek");

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher max mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxMorningsPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers max mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxMorningsPerWeek");

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints teacher max activity tags per day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxActivityTagsPerDayFromSet");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have more than the specified number of "
													 "activity tags from those selected, on any day."));

				break;
			}
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints teachers max activity tags per day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxActivityTagsPerDayFromSet");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all the teachers do not have more than the specified number of "
													 "activity tags from those selected, on any day."));

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers min mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinMorningsPerWeek");

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher min mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinMorningsPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers min afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinAfternoonsPerWeek");

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher min afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinAfternoonsPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				dialogTitle=tr("Constraints teacher max two consecutive mornings", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxTwoConsecutiveMornings");

				teachersComboBox=new QComboBox;

				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				dialogTitle=tr("Constraints teachers max two consecutive mornings", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxTwoConsecutiveMornings");

				break;
			}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				dialogTitle=tr("Constraints teacher max two consecutive afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxTwoConsecutiveAfternoons");

				teachersComboBox=new QComboBox;

				break;
			}
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				dialogTitle=tr("Constraints teachers max two consecutive afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxTwoConsecutiveAfternoons");

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers max gaps per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxGapsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day (2 FET days, morning and afternoon)."));

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher max gaps per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxGapsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day (2 FET days, morning and afternoon)."));

				teachersComboBox=new QComboBox;

				break;
			}
		//117
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students max gaps per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxGapsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day (2 FET days, morning and afternoon)."));

				break;
			}
		//118
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set max gaps per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxGapsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day (2 FET days, morning and afternoon)."));

				studentsComboBox=new QComboBox;

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teachers min hours daily for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teacher min hours daily for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinHoursDailyRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				teachersComboBox=new QComboBox;

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints teachers afternoons early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHour");

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints teacher afternoons early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHour");

				teachersComboBox=new QComboBox;

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				dialogTitle=tr("Constraints teachers min hours per morning", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinHoursPerMorning");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the teachers must have a min hours daily constraint"));

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				dialogTitle=tr("Constraints teacher min hours per morning", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinHoursPerMorning");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the teacher must have a min hours daily constraint"));

				teachersComboBox=new QComboBox;

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher max span per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxSpanPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the "
													 "count of slots (free or occupied) starting with the first occupied slot of the day "
													 "and ending with the last occupied slot of the day."));
				secondInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				teachersComboBox=new QComboBox;

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers max span per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxSpanPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means the "
													 "count of slots (free or occupied) starting with the first occupied slot of the day "
													 "and ending with the last occupied slot of the day."));
				secondInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//127
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set max span per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxSpanPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means "
													 "the count of slots (free or occupied) starting with the first occupied slot of "
													 "the day and ending with the last occupied slot of the day."));
				secondInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				studentsComboBox=new QComboBox;

				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students max span per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxSpanPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a maximum span of activities on each day. Span means "
													 "the count of slots (free or occupied) starting with the first occupied slot of "
													 "the day and ending with the last occupied slot of the day."));
				secondInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher morning interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMorningIntervalMaxDaysPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers morning interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMorningIntervalMaxDaysPerWeek");

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teacher afternoon interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherAfternoonIntervalMaxDaysPerWeek");

				teachersComboBox=new QComboBox;

				break;
			}
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints teachers afternoon interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersAfternoonIntervalMaxDaysPerWeek");

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				dialogTitle=tr("Constraints students min hours per morning", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinHoursPerMorning");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the students must have a min hours daily constraint"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				dialogTitle=tr("Constraints students set min hours per morning", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinHoursPerMorning");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the students set must have a min hours daily constraint"));

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher max zero gaps per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxZeroGapsPerAfternoon");

				teachersComboBox=new QComboBox;

				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers max zero gaps per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxZeroGapsPerAfternoon");

				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxAfternoonsPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxAfternoonsPerWeek");

				break;
			}
		//139
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxMorningsPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//140
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxMorningsPerWeek");

				break;
			}
		//141
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students min mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinMorningsPerWeek");

				break;
			}
		//142
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set min mornings per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinMorningsPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//143
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students min afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinAfternoonsPerWeek");

				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set min afternoons per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinAfternoonsPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//145
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set morning interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMorningIntervalMaxDaysPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//146
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students morning interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMorningIntervalMaxDaysPerWeek");

				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set afternoon interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeek");

				studentsComboBox=new QComboBox;

				break;
			}
		//148
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students afternoon interval max days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsAfternoonIntervalMaxDaysPerWeek");

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				dialogTitle=tr("Constraints teacher max hours per all afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursPerAllAfternoons");

				teachersComboBox=new QComboBox;

				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				dialogTitle=tr("Constraints teachers max hours per all afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursPerAllAfternoons");

				break;
			}
		//151
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				dialogTitle=tr("Constraints students set max hours per all afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxHoursPerAllAfternoons");

				studentsComboBox=new QComboBox;

				break;
			}
		//152
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				dialogTitle=tr("Constraints students max hours per all afternoons", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxHoursPerAllAfternoons");

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher min resting hours between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of each morning and the beginning "
													 "of the next afternoon (of the same real day)."));

				teachersComboBox=new QComboBox;

				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers min resting hours between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of each morning and the beginning "
													 "of the next afternoon (of the same real day)."));

				break;
			}
		//155
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students set min resting hours between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of each morning and the beginning "
													 "of the next afternoon (of the same real day)."));

				studentsComboBox=new QComboBox;

				break;
			}
		//156
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students min resting hours between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures a minimum number of resting hours between the end of each morning and the beginning "
													 "of the next afternoon (of the same real day)."));

				break;
			}
		//157
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students set afternoons early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour");

				studentsComboBox=new QComboBox;

				break;
			}
		//158
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students afternoons early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHour");

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teachers max gaps per week for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxGapsPerWeekForRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for real days (a real day = 2 FET days, morning and afternoon)."));

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				dialogTitle=tr("Constraints teacher max gaps per week for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxGapsPerWeekForRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for real days (a real day = 2 FET days, morning and afternoon)."));

				teachersComboBox=new QComboBox;

				break;
			}
		//161
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students max gaps per week for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxGapsPerWeekForRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for real days (a real day = 2 FET days, morning and afternoon)."));

				break;
			}
		//162
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				dialogTitle=tr("Constraints students set max gaps per week for real days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxGapsPerWeekForRealDays");

				firstInstructionsLabel=new QLabel(tr("This constraint is for real days (a real day = 2 FET days, morning and afternoon)."));

				studentsComboBox=new QComboBox;

				break;
			}
		//163
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students set max real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				studentsComboBox=new QComboBox;

				break;
			}
		//164
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Constraints students max real days per week", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxRealDaysPerWeek");

				firstInstructionsLabel=new QLabel(tr("This constraint is for a real day\n(a real day = a morning + an afternoon)"));

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints max total activities from set in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMaxActivitiesFromSetInSelectedTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints max gaps between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMaxGapsBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures, for a set of activities: if any pair of activities are on "
													 "the same day, they must be separated by at most max gaps (hours)."));

				break;
			}
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
			{
				dialogTitle=tr("Constraints activities max in a term", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMaxInATerm");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
			{
				dialogTitle=tr("Constraints activities occupy max terms", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOccupyMaxTerms");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers max gaps per morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxGapsPerMorningAndAfternoon");

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher max gaps per morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxGapsPerMorningAndAfternoon");

				teachersComboBox=new QComboBox;

				break;
			}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints teachers mornings early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHour");

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints teacher mornings early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHour");

				teachersComboBox=new QComboBox;

				break;
			}
		//173
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students set mornings early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHour");

				studentsComboBox=new QComboBox;

				break;
			}
		//174
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Constraints students mornings early max beginnings at second hour", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHour");

				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
			{
				dialogTitle=tr("Constraints two sets of activities ordered", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoSetsOfActivitiesOrdered");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint forces two sets of activities SA1 and SA2: each activity A2 from SA2 "
													 "to begin later than the moment each activity A1 from SA1 has finished, separated by "
													 "any time interval in the week"));

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints teachers max three consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxThreeConsecutiveDays");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints teacher max three consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxThreeConsecutiveDays");

				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				dialogTitle=tr("Constraints students set min gaps between activity tag", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenActivityTag");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				dialogTitle=tr("Constraints students min gaps between activity tag", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenActivityTag");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each day, there are at least min gaps (hours) "
													 "between the activities which have this activity tag."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				dialogTitle=tr("Constraints teacher min gaps between activity tag", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenActivityTag");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				dialogTitle=tr("Constraints teachers min gaps between activity tag", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenActivityTag");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints students max three consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxThreeConsecutiveDays");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints students set max three consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxThreeConsecutiveDays");

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints min half days between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMinHalfDaysBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Min days is for half days"));

				modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton=new QPushButton(tr("Modify multiple constraints at once"));

				break;
			}
		//185 is activity preferred day, which is not in the interface
		case CONSTRAINT_ACTIVITY_PREFERRED_DAY:
			{
				QMessageBox::warning(dialog, tr("FET information"),
									 tr("You have encountered a FET bug. The constraint activity preferred day should not "
										"be editable in a separate dialog. The request will be ignored."));

				return;

				break;
			}
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				dialogTitle=tr("Constraints activities min in a term", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMinInATerm");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints max terms between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMaxTermsBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints students set max activity tags per day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxActivityTagsPerDayFromSet");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students set does not have more "
													 "than the specified number of activity tags from those selected, "
													 "on any day."));

				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints students max activity tags per day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxActivityTagsPerDayFromSet");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all the students do not have more than the specified number "
													 "of activity tags from those selected, on any day."));

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints teacher max activity tags per real day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxActivityTagsPerRealDayFromSet");

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified teacher does not have more than the specified number of activity "
													 "tags from those selected, on any real day."));

				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints teachers max activity tags per real day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxActivityTagsPerRealDayFromSet");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all the teachers do not have more than the specified number of activity tags "
													 "from those selected, on any real day."));

				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints students set max activity tags per real day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxActivityTagsPerRealDayFromSet");

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that the specified students set does not have more than the specified number of "
													 "activity tags from those selected, on any real day."));

				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				dialogTitle=tr("Constraints students max activity tags per real day from set", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxActivityTagsPerRealDayFromSet");

				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that all the students do not have more than the specified number of activity tags "
													 "from those selected, on any real day."));

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				dialogTitle=tr("Constraints max half days between activities", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsMaxHalfDaysBetweenActivities");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				firstInstructionsLabel=new QLabel(tr("Max days is for half days"));

				secondInstructionsLabel=new QLabel(tr("Please read Help/Important tips, advice 2). It is IMPORTANT to "
													 "remove redundant min (half) days between activities constraints with weight < 100% after "
													 "adding constraints max 0 half days between activities. Click the Help button!"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activity begins students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityBeginsStudentsDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activities begin students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesBeginStudentsDay");

				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activity begins teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityBeginsTeachersDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activities begin teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesBeginTeachersDay");

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers min hours per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinHoursPerAfternoon");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the teachers must have a min hours daily constraint"));

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher min hours per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinHoursPerAfternoon");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the teacher must have a min hours daily constraint"));

				teachersComboBox=new QComboBox;

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints students min hours per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinHoursPerAfternoon");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the students must have a min hours daily constraint"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				dialogTitle=tr("Constraints students set min hours per afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinHoursPerAfternoon");

				firstInstructionsLabel=new QLabel(tr("To use this constraint, the students set must have a min hours daily constraint"));

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				dialogTitle=tr("Constraints activities max hourly span", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMaxHourlySpan");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teachers max hours daily in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursDailyInInterval");

				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints teacher max hours daily in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursDailyInInterval");

				teachersComboBox=new QComboBox;

				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students max hours daily in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxHoursDailyInInterval");

				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				dialogTitle=tr("Constraints students set max hours daily in interval", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxHoursDailyInInterval");

				studentsComboBox=new QComboBox;

				break;
			}
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set min gaps between ordered pair of activity tags per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each real day, there are at "
													 "least min gaps (hours) between the first activity tag and the second activity tag, if they "
													 "appear in this order."));

				studentsComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students min gaps between ordered pair of activity tags per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each real day, there are at least "
													 "min gaps (hours) between the first activity tag and the second activity tag, if they appear "
													 "in this order."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher min gaps between ordered pair of activity tags per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each real day, there "
													 "are at least min gaps (hours) between the first activity tag and the second activity "
													 "tag, if they appear in this order."));

				teachersComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers min gaps between ordered pair of activity tags per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each real day, there are at least min "
													 "gaps (hours) between the first activity tag and the second activity tag, if they appear in "
													 "this order."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students set min gaps between activity tag per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each real day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints students min gaps between activity tag per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenActivityTagPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each real day, there are at least min gaps (hours) "
													 "between the activities which have this activity tag."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teacher min gaps between activity tag per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenActivityTagPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each real day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				dialogTitle=tr("Constraints teachers min gaps between activity tag per real day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenActivityTagPerRealDay");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each real day, there are at least min gaps "
													 "(hours) between the activities which have this activity tag."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students set min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each real day, there are at least min gaps (hours) between the first activity tag and "
													 "the second activity tag, if the first activity tag appears in the morning and the second activity tag appears in the afternoon."));

				studentsComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each real day, there are at least min gaps (hours) between the first activity tag and the "
													 "second activity tag, if the first activity tag appears in the morning and the second activity tag appears in the afternoon."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each real day, there are at least min gaps (hours) between the first activity tag "
													 "and the second activity tag, if the first activity tag appears in the morning and the second activity tag appears in the afternoon."));

				teachersComboBox=new QComboBox;
				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each real day, there are at least min gaps (hours) between the first activity tag and "
													 "the second activity tag, if the first activity tag appears in the morning and the second activity tag appears in the afternoon."));

				first_activityTagsComboBox=new QComboBox;
				second_activityTagsComboBox=new QComboBox;

				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students set min gaps between activity tag between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified students, on each real day, there are at least min gaps (hours) between the last activity from the morning which has this activity tag and the first activity from the afternoon which has this activity tag (the constraint is considered respected if the activity tag is not met both in the morning and in the afternoon)."));

				studentsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints students min gaps between activity tag between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the students, on each real day, there are at least min gaps (hours) between the last activity from the morning which has this activity tag and the first activity from the afternoon which has this activity tag (the constraint is considered respected if the activity tag is not met both in the morning and in the afternoon)."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teacher min gaps between activity tag between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for the specified teacher, on each real day, there are at least min gaps (hours) between the last activity from the morning which has this activity tag and the first activity from the afternoon which has this activity tag (the constraint is considered respected if the activity tag is not met both in the morning and in the afternoon)."));

				teachersComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Constraints teachers min gaps between activity tag between morning and afternoon", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon");

				firstInstructionsLabel=new QLabel(tr("This constraint ensures that for all the teachers, on each real day, there are at least min gaps (hours) between the last activity from the morning which has this activity tag and the first activity from the afternoon which has this activity tag (the constraint is considered respected if the activity tag is not met both in the morning and in the afternoon)."));

				activityTagsComboBox=new QComboBox;

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints teachers no two consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersNoTwoConsecutiveDays");

				break;
			}
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				dialogTitle=tr("Constraints teacher no two consecutive days", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherNoTwoConsecutiveDays");

				teachersComboBox=new QComboBox;

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teacher pair of mutually exclusive time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherPairOfMutuallyExclusiveTimeSlots");

				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teachers pair of mutually exclusive time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersPairOfMutuallyExclusiveTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students set pair of mutually exclusive time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetPairOfMutuallyExclusiveTimeSlots");

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students pair of mutually exclusive time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsPairOfMutuallyExclusiveTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
			{
				dialogTitle=tr("Constraints two sets of activities have the same sections", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTwoSetsOfActivitiesSameSections");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students max single gaps in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsMaxSingleGapsInSelectedTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students set max single gaps in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetMaxSingleGapsInSelectedTimeSlots");

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teachers max single gaps in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxSingleGapsInSelectedTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teacher max single gaps in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxSingleGapsInSelectedTimeSlots");

				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			{
				dialogTitle=tr("Constraints teacher max hours per term", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherMaxHoursPerTerm");

				teachersComboBox=new QComboBox;

				break;
			}
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				dialogTitle=tr("Constraints teachers max hours per term", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersMaxHoursPerTerm");

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teacher pair of mutually exclusive sets of time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherPairOfMutuallyExclusiveSetsOfTimeSlots");

				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints teachers pair of mutually exclusive sets of time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersPairOfMutuallyExclusiveSetsOfTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students set pair of mutually exclusive sets of time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots");

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints students pair of mutually exclusive sets of time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsPairOfMutuallyExclusiveSetsOfTimeSlots");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities pair of mutually exclusive sets of time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities pair of mutually exclusive time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesPairOfMutuallyExclusiveTimeSlots");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints teacher occupies max sets of time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeacherOccupiesMaxSetsOfTimeSlotsFromSelection");

				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints teachers occupy max sets of time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsTeachersOccupyMaxSetsOfTimeSlotsFromSelection");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints students set occupies max sets of time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection");

				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints students occupy max sets of time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsStudentsOccupyMaxSetsOfTimeSlotsFromSelection");

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
			{
				dialogTitle=tr("Constraints activities overlap completely or do not overlap", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOverlapCompletelyOrDoNotOverlap");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				dialogTitle=tr("Constraints activities occupy max sets of time slots from selection", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesOccupyMaxSetsOfTimeSlotsFromSelection");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//249
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activity begins or ends students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityBeginsOrEndsStudentsDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//250
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
			{
				dialogTitle=tr("Constraints activities begin or end students day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesBeginOrEndStudentsDay");

				break;
			}
		//251
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activity begins or ends teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivityBeginsOrEndsTeachersDay");

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				break;
			}
		//252
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
			{
				dialogTitle=tr("Constraints activities begin or end teachers day", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesBeginOrEndTeachersDay");

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
			{
				dialogTitle=tr("Constraints activities max total number of students in selected time slots", "The title of the dialog to list the constraints of this type");
				dialogName=QString("ConstraintsActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots");

				firstInstructionsLabel=new QLabel(tr("WARNING: potentially very SLOW constraint! Use ONLY if strictly necessary! Read the Help!"));
				secondInstructionsLabel=new QLabel(tr("Note: it is recommended to use this constraint only if strictly necessary and not redundantly, because it might be very slow."));

				teachersComboBox=new QComboBox;
				studentsComboBox=new QComboBox;
				subjectsComboBox=new QComboBox;
				activityTagsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

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
		
		connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::filter);
	}

	if(studentsComboBox!=nullptr){
		QSize tmp=studentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		populateStudentsComboBox(studentsComboBox, QString(""), true);
		studentsComboBox->setCurrentIndex(0);

		connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::showRelatedCheckBoxToggled);
	}

	if(subjectsComboBox!=nullptr){
		QSize tmp=subjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		subjectsComboBox->addItem("");
		for(Subject* sbj : std::as_const(gt.rules.subjectsList))
			subjectsComboBox->addItem(sbj->name);
		subjectsComboBox->setCurrentIndex(0);

		connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::filter);
	}

	if(activityTagsComboBox!=nullptr){
		QSize tmp=activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		activityTagsComboBox->addItem("");
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			activityTagsComboBox->addItem(at->name);
		activityTagsComboBox->setCurrentIndex(0);

		connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::filter);
	}

	if(first_activityTagsComboBox!=nullptr){
		QSize tmp=first_activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		first_activityTagsComboBox->addItem("");
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			first_activityTagsComboBox->addItem(at->name);
		first_activityTagsComboBox->setCurrentIndex(0);

		connect(first_activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::filter);
	}

	if(second_activityTagsComboBox!=nullptr){
		QSize tmp=second_activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);

		second_activityTagsComboBox->addItem("");
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			second_activityTagsComboBox->addItem(at->name);
		second_activityTagsComboBox->setCurrentIndex(0);

		connect(second_activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ListTimeConstraints::filter);
	}

	if(teachersComboBox!=nullptr
			|| studentsComboBox!=nullptr
			|| subjectsComboBox!=nullptr
			|| activityTagsComboBox!=nullptr
			|| first_activityTagsComboBox!=nullptr
			|| second_activityTagsComboBox!=nullptr){
		filterGroupBox=new QGroupBox(tr("Filter"));

		QVBoxLayout* layout=new QVBoxLayout;

		if(teachersComboBox!=nullptr)
			layout->addWidget(teachersComboBox);
		if(studentsComboBox!=nullptr){
			showRelatedCheckBox=new QCheckBox(tr("Show related"));

			QSettings settings(COMPANY, PROGRAM);
			showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/show-related"), "false").toBool());

			connect(showRelatedCheckBox, &QCheckBox::toggled, this, &ListTimeConstraints::showRelatedCheckBoxToggled);

			QHBoxLayout* ssrl=new QHBoxLayout;

			ssrl->addWidget(studentsComboBox);
			ssrl->addWidget(showRelatedCheckBox);

			layout->addLayout(ssrl);
		}
		if(subjectsComboBox!=nullptr)
			layout->addWidget(subjectsComboBox);
		if(activityTagsComboBox!=nullptr)
			layout->addWidget(activityTagsComboBox);
		if(first_activityTagsComboBox!=nullptr)
			layout->addWidget(first_activityTagsComboBox);
		if(second_activityTagsComboBox!=nullptr)
			layout->addWidget(second_activityTagsComboBox);

		filterGroupBox->setLayout(layout);
	}
	
	eventLoop=new QEventLoop;

	splitter=new QSplitter;
	splitter->setSizePolicy(splitter->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);

	dialog=new ListTimeConstraintsDialog(parent, dialogName, dialogTitle, eventLoop, splitter,
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
	sortedCheckBox=new QCheckBox(tr("Sorted", "It refers to time constraints"));
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
	
	switch(type){
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			[[fallthrough]];
		//
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			modifyMultiplePushButton=new QPushButton(tr("Modify selected", "It refers to time constraints"));
			break;
		
		default:
			modifyMultiplePushButton=nullptr;
			break;
	}
	
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
	if(modifyMultiplePushButton!=nullptr)
		buttons1Layout->addWidget(modifyMultiplePushButton);
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
	if(modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton!=nullptr){
		//wholeDialog->addWidget(modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton);
		QHBoxLayout* layoutM=new QHBoxLayout;
		layoutM->addStretch();
		layoutM->addWidget(modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton);
		wholeDialog->addLayout(layoutM);
	}
	if(addRemoveMultipleConstraintsPushButton!=nullptr){
		//wholeDialog->addWidget(addRemoveMultipleConstraintsPushButton);
		QHBoxLayout* layoutM=new QHBoxLayout;
		layoutM->addStretch();
		layoutM->addWidget(addRemoveMultipleConstraintsPushButton);
		wholeDialog->addLayout(layoutM);
	}
	wholeDialog->addLayout(buttons2Layout);

	connect(addPushButton, &QPushButton::clicked, this, &ListTimeConstraints::addClicked);
	connect(modifyPushButton, &QPushButton::clicked, this, &ListTimeConstraints::modifyClicked);
	if(modifyMultiplePushButton!=nullptr)
		connect(modifyMultiplePushButton, &QPushButton::clicked, this, &ListTimeConstraints::modifyMultipleClicked);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ListTimeConstraints::modifyClicked);
	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ListTimeConstraints::constraintChanged);
	connect(removePushButton, &QPushButton::clicked, this, &ListTimeConstraints::removeClicked);
	
	if(helpPushButton!=nullptr)
		connect(helpPushButton, &QPushButton::clicked, this, &ListTimeConstraints::helpClicked);
	connect(closePushButton, &QPushButton::clicked, this, &ListTimeConstraints::closeClicked);
	if(modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton!=nullptr)
		connect(modifyMultipleMinDaysBetweenActivitiesConstraintsPushButton, &QPushButton::clicked, this, &ListTimeConstraints::modifyMultipleMinDaysBetweenActivitiesConstraintsClicked);
	if(addRemoveMultipleConstraintsPushButton!=nullptr)
		connect(addRemoveMultipleConstraintsPushButton, &QPushButton::clicked, this, &ListTimeConstraints::addRemoveMultipleConstraintsActivitiesSameStartingHourClicked);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ListTimeConstraints::constraintChanged);

	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListTimeConstraints::selectionChanged);

	connect(filterCheckBox, &QCheckBox::toggled, this, &ListTimeConstraints::advancedFilter);
	connect(sortedCheckBox, &QCheckBox::toggled, this, &ListTimeConstraints::sortedChanged);

	connect(activatePushButton, &QPushButton::clicked, this, &ListTimeConstraints::activateConstraints);
	connect(deactivatePushButton, &QPushButton::clicked, this, &ListTimeConstraints::deactivateConstraints);
	connect(commentsPushButton, &QPushButton::clicked, this, &ListTimeConstraints::constraintComments);
	connect(weightsPushButton, &QPushButton::clicked, this, &ListTimeConstraints::changeWeights);

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

ListTimeConstraints::~ListTimeConstraints()
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

void ListTimeConstraints::advancedFilter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;

		filter();

		//constraintsListWidget->setFocus();

		return;
	}

	assert(active);

	filterForm=new AdvancedFilterForm(dialog, tr("Advanced filter for time constraints"), false, all, descrDetDescr, contains, text, caseSensitive, dialogName+"AdvancedFilterForm");

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

		disconnect(filterCheckBox, &QCheckBox::toggled, this, &ListTimeConstraints::advancedFilter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &ListTimeConstraints::advancedFilter);
	}

	delete filterForm;
}

bool ListTimeConstraints::filterOk(TimeConstraint* tc)
{
	if(tc->type!=type)
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
					s=tc->getDescription(gt.rules);
			}
			else{
				assert(descrDetDescr.at(perm.at(i))==DETDESCRIPTION);

				assert(firstPosWithDetDescr>=0);

				if(i==firstPosWithDetDescr)
					s=tc->getDetailedDescription(gt.rules);
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
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				return true;

				break;
			}
		//2
		case CONSTRAINT_BREAK_TIMES:
			{
				return true;

				break;
			}
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherNotAvailableTimes* ctr=(ConstraintTeacherNotAvailableTimes*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher;

				break;
			}
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				return true;

				break;
			}
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxDaysPerWeek* ctr=(ConstraintTeacherMaxDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				return true;

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxGapsPerWeek* ctr=(ConstraintTeacherMaxGapsPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursDaily* ctr=(ConstraintTeacherMaxHoursDaily*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				return true;

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursContinuously* ctr=(ConstraintTeacherMaxHoursContinuously*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				return true;

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinHoursDaily* ctr=(ConstraintTeacherMinHoursDaily*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				return true;

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxGapsPerDay* ctr=(ConstraintTeacherMaxGapsPerDay*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				return true;

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetNotAvailableTimes* ctr=(ConstraintStudentsSetNotAvailableTimes*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
			{
				return true;

				break;
			}
		//19
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxGapsPerWeek* ctr=(ConstraintStudentsSetMaxGapsPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//20
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				return true;

				break;
			}
		//21
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxHoursDaily* ctr=(ConstraintStudentsSetMaxHoursDaily*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//22
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				return true;

				break;
			}
		//23
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxHoursContinuously* ctr=(ConstraintStudentsSetMaxHoursContinuously*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				return true;

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinHoursDaily* ctr=(ConstraintStudentsSetMinHoursDaily*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityEndsStudentsDay* ctr=(ConstraintActivityEndsStudentsDay*)tc;

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

				return true;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityPreferredStartingTime* ctr=(ConstraintActivityPreferredStartingTime*)tc;

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

				return true;

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
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

				ConstraintActivitiesSameStartingTime* ctr=(ConstraintActivitiesSameStartingTime*)tc;

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
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
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

				ConstraintActivitiesNotOverlapping* ctr=(ConstraintActivitiesNotOverlapping*)tc;

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
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
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

				ConstraintMinDaysBetweenActivities* ctr=(ConstraintMinDaysBetweenActivities*)tc;

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
		//31
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityPreferredTimeSlots* ctr=(ConstraintActivityPreferredTimeSlots*)tc;

				int id=ctr->p_activityId;
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

				return true;

				break;
			}
		//32
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				return true;

				break;
			}
		//33
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityPreferredStartingTimes* ctr=(ConstraintActivityPreferredStartingTimes*)tc;

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

				return true;

				break;
			}
		//34
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				return true;

				break;
			}
		//35
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
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

				ConstraintActivitiesSameStartingHour* ctr=(ConstraintActivitiesSameStartingHour*)tc;

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
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
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

				ConstraintActivitiesSameStartingDay* ctr=(ConstraintActivitiesSameStartingDay*)tc;

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
		//37
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
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

				ConstraintTwoActivitiesConsecutive* ctr=(ConstraintTwoActivitiesConsecutive*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int i=0; i<2; i++){
					//bool found=true;

					int id=-1;

					if(i==0)
						id=ctr->firstActivityId;
					else if(i==1)
						id=ctr->secondActivityId;

					assert(id>=0);

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
		//38
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
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

				ConstraintTwoActivitiesOrdered* ctr=(ConstraintTwoActivitiesOrdered*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int i=0; i<2; i++){
					//bool found=true;

					int id=-1;

					if(i==0)
						id=ctr->firstActivityId;
					else if(i==1)
						id=ctr->secondActivityId;

					assert(id>=0);

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
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
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

				ConstraintMinGapsBetweenActivities* ctr=(ConstraintMinGapsBetweenActivities*)tc;

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
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				return true;

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				return true;

				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherIntervalMaxDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				return true;

				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
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

				ConstraintTwoActivitiesGrouped* ctr=(ConstraintTwoActivitiesGrouped*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int i=0; i<2; i++){
					//bool found=true;

					int id=-1;

					if(i==0)
						id=ctr->firstActivityId;
					else if(i==1)
						id=ctr->secondActivityId;

					assert(id>=0);

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
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersActivityTagMaxHoursContinuously* ctr=(ConstraintTeachersActivityTagMaxHoursContinuously*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherActivityTagMaxHoursContinuously* ctr=(ConstraintTeacherActivityTagMaxHoursContinuously*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsActivityTagMaxHoursContinuously*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)tc;
				return (studentsComboBox->currentText()==QString("") || studentsComboBox->currentText()==ctr->students)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
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

				ConstraintThreeActivitiesGrouped* ctr=(ConstraintThreeActivitiesGrouped*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int i=0; i<3; i++){
					//bool found=true;

					int id=-1;

					if(i==0)
						id=ctr->firstActivityId;
					else if(i==1)
						id=ctr->secondActivityId;
					else if(i==2)
						id=ctr->thirdActivityId;

					assert(id>=0);

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
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
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

				ConstraintMaxDaysBetweenActivities* ctr=(ConstraintMaxDaysBetweenActivities*)tc;

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
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinDaysPerWeek* ctr=(ConstraintTeacherMinDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersActivityTagMaxHoursDaily* ctr=(ConstraintTeachersActivityTagMaxHoursDaily*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherActivityTagMaxHoursDaily* ctr=(ConstraintTeacherActivityTagMaxHoursDaily*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsActivityTagMaxHoursDaily* ctr=(ConstraintStudentsActivityTagMaxHoursDaily*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetActivityTagMaxHoursDaily* ctr=(ConstraintStudentsSetActivityTagMaxHoursDaily*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//61
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				return true;

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxGapsPerDay* ctr=(ConstraintStudentsSetMaxGapsPerDay*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
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

				ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)tc;

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
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
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

				ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)tc;

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
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxDaysPerWeek* ctr=(ConstraintStudentsSetMaxDaysPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxSpanPerDay* ctr=(ConstraintTeacherMaxSpanPerDay*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				return true;

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinRestingHours* ctr=(ConstraintTeacherMinRestingHours*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				return true;

				break;
			}
		//71
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxSpanPerDay* ctr=(ConstraintStudentsSetMaxSpanPerDay*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//72
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				return true;

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinRestingHours* ctr=(ConstraintStudentsSetMinRestingHours*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				return true;

				break;
			}
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
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

				ConstraintTwoActivitiesOrderedIfSameDay* ctr=(ConstraintTwoActivitiesOrderedIfSameDay*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int i=0; i<2; i++){
					//bool found=true;

					int id=-1;

					if(i==0)
						id=ctr->firstActivityId;
					else if(i==1)
						id=ctr->secondActivityId;

					assert(id>=0);

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
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				ConstraintActivityTagsNotOverlapping* ctr=(ConstraintActivityTagsNotOverlapping*)tc;
				return activityTagsComboBox->currentText()==QString("") || ctr->activityTagsNames.contains(activityTagsComboBox->currentText());

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
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

				ConstraintActivitiesOccupyMinTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)tc;

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
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
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

				ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)tc;

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
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersActivityTagMinHoursDaily* ctr=(ConstraintTeachersActivityTagMinHoursDaily*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherActivityTagMinHoursDaily* ctr=(ConstraintTeacherActivityTagMinHoursDaily*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsActivityTagMinHoursDaily* ctr=(ConstraintStudentsActivityTagMinHoursDaily*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetActivityTagMinHoursDaily* ctr=(ConstraintStudentsSetActivityTagMinHoursDaily*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityEndsTeachersDay* ctr=(ConstraintActivityEndsTeachersDay*)tc;

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

				return true;

				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				return true;

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				return true;

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxRealDaysPerWeek* ctr=(ConstraintTeacherMaxRealDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursDailyRealDays* ctr=(ConstraintTeacherMaxHoursDailyRealDays*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//92
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				return true;

				break;
			}
		//93
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetMaxHoursDailyRealDays*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinRealDaysPerWeek* ctr=(ConstraintTeacherMinRealDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeachersActivityTagMaxHoursDailyRealDays*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeacherActivityTagMaxHoursDailyRealDays*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//99
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsActivityTagMaxHoursDailyRealDays*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName;

				break;
			}
		//100
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetActivityTagMaxHoursDailyRealDays*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTagName);

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxAfternoonsPerWeek* ctr=(ConstraintTeacherMaxAfternoonsPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				return true;

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxMorningsPerWeek* ctr=(ConstraintTeacherMaxMorningsPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				return true;

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerDayFromSet*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerDayFromSet*)tc;
				return activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText());

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				return true;

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinMorningsPerWeek* ctr=(ConstraintTeacherMinMorningsPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				return true;

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinAfternoonsPerWeek* ctr=(ConstraintTeacherMinAfternoonsPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxTwoConsecutiveMornings* ctr=(ConstraintTeacherMaxTwoConsecutiveMornings*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				return true;

				break;
			}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxTwoConsecutiveAfternoons* ctr=(ConstraintTeacherMaxTwoConsecutiveAfternoons*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				return true;

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxGapsPerRealDay* ctr=(ConstraintTeacherMaxGapsPerRealDay*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//117
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//118
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxGapsPerRealDay* ctr=(ConstraintStudentsSetMaxGapsPerRealDay*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				return true;

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinHoursDailyRealDays* ctr=(ConstraintTeacherMinHoursDailyRealDays*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				return true;

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				return true;

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinHoursPerMorning* ctr=(ConstraintTeacherMinHoursPerMorning*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxSpanPerRealDay* ctr=(ConstraintTeacherMaxSpanPerRealDay*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//127
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxSpanPerRealDay* ctr=(ConstraintStudentsSetMaxSpanPerRealDay*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				return true;

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMorningIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherMorningIntervalMaxDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherAfternoonIntervalMaxDaysPerWeek*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				return true;

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinHoursPerMorning* ctr=(ConstraintStudentsSetMinHoursPerMorning*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxZeroGapsPerAfternoon* ctr=(ConstraintTeacherMaxZeroGapsPerAfternoon*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				return true;

				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxAfternoonsPerWeek* ctr=(ConstraintStudentsSetMaxAfternoonsPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				return true;

				break;
			}
		//139
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxMorningsPerWeek* ctr=(ConstraintStudentsSetMaxMorningsPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//140
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				return true;

				break;
			}
		//141
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				return true;

				break;
			}
		//142
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinMorningsPerWeek* ctr=(ConstraintStudentsSetMinMorningsPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//143
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				return true;

				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinAfternoonsPerWeek* ctr=(ConstraintStudentsSetMinAfternoonsPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//145
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMorningIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetMorningIntervalMaxDaysPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//146
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//148
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursPerAllAfternoons* ctr=(ConstraintTeacherMaxHoursPerAllAfternoons*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				return true;

				break;
			}
		//151
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxHoursPerAllAfternoons* ctr=(ConstraintStudentsSetMaxHoursPerAllAfternoons*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//152
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				return true;

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				return true;

				break;
			}
		//155
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//156
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				return true;

				break;
			}
		//157
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//158
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				return true;

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				return true;

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxGapsPerWeekForRealDays* ctr=(ConstraintTeacherMaxGapsPerWeekForRealDays*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//161
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				return true;

				break;
			}
		//162
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxGapsPerWeekForRealDays* ctr=(ConstraintStudentsSetMaxGapsPerWeekForRealDays*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//163
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxRealDaysPerWeek* ctr=(ConstraintStudentsSetMaxRealDaysPerWeek*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//164
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				return true;

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
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

				ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots* ctr=(ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots*)tc;

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
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
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

				ConstraintMaxGapsBetweenActivities* ctr=(ConstraintMaxGapsBetweenActivities*)tc;

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
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
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

				ConstraintActivitiesMaxInATerm* ctr=(ConstraintActivitiesMaxInATerm*)tc;

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
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
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

				ConstraintActivitiesOccupyMaxTerms* ctr=(ConstraintActivitiesOccupyMaxTerms*)tc;

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
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				return true;

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				return true;

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//173
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//174
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				return true;

				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
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

				ConstraintTwoSetsOfActivitiesOrdered* ctr=(ConstraintTwoSetsOfActivitiesOrdered*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				QList<int> tl=ctr->firstActivitiesIdsList+ctr->secondActivitiesIdsList;

				for(int id : std::as_const(tl)){
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
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				return true;

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxThreeConsecutiveDays* ctr=(ConstraintTeacherMaxThreeConsecutiveDays*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenActivityTag* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTag*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintStudentsMinGapsBetweenActivityTag* ctr=(ConstraintStudentsMinGapsBetweenActivityTag*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenActivityTag* ctr=(ConstraintTeacherMinGapsBetweenActivityTag*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintTeachersMinGapsBetweenActivityTag* ctr=(ConstraintTeachersMinGapsBetweenActivityTag*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				return true;

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxThreeConsecutiveDays* ctr=(ConstraintStudentsSetMaxThreeConsecutiveDays*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
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

				ConstraintMinHalfDaysBetweenActivities* ctr=(ConstraintMinHalfDaysBetweenActivities*)tc;

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
		//185 is activity preferred day, which is not in the interface
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
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

				ConstraintActivitiesMinInATerm* ctr=(ConstraintActivitiesMinInATerm*)tc;

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
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
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

				ConstraintMaxTermsBetweenActivities* ctr=(ConstraintMaxTermsBetweenActivities*)tc;

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
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerDayFromSet*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerDayFromSet*)tc;
				return activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText());

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				assert(teachersComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeacherMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerRealDayFromSet*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName)
						&& (activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintTeachersMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerRealDayFromSet*)tc;
				return activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText());

				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				assert(studentsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText()));

				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				assert(activityTagsComboBox!=nullptr);
				ConstraintStudentsMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerRealDayFromSet*)tc;
				return activityTagsComboBox->currentText()==QString("") || ctr->tagsList.contains(activityTagsComboBox->currentText());

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
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

				ConstraintMaxHalfDaysBetweenActivities* ctr=(ConstraintMaxHalfDaysBetweenActivities*)tc;

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
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityBeginsStudentsDay* ctr=(ConstraintActivityBeginsStudentsDay*)tc;

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

				return true;

				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				return true;

				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityBeginsTeachersDay* ctr=(ConstraintActivityBeginsTeachersDay*)tc;

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

				return true;

				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				return true;

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				return true;

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinHoursPerAfternoon* ctr=(ConstraintTeacherMinHoursPerAfternoon*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				return true;

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinHoursPerAfternoon* ctr=(ConstraintStudentsSetMinHoursPerAfternoon*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
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

				ConstraintActivitiesMaxHourlySpan* ctr=(ConstraintActivitiesMaxHourlySpan*)tc;

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
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursDailyInInterval* ctr=(ConstraintTeacherMaxHoursDailyInInterval*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				return true;

				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxHoursDailyInInterval* ctr=(ConstraintStudentsSetMaxHoursDailyInInterval*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintStudentsMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenActivityTagPerRealDay*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenActivityTagPerRealDay*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintTeachersMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenActivityTagPerRealDay*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)tc;
				return (first_activityTagsComboBox->currentText()==QString("") || first_activityTagsComboBox->currentText()==ctr->firstActivityTag)
						&& (second_activityTagsComboBox->currentText()==QString("") || second_activityTagsComboBox->currentText()==ctr->secondActivityTag);

				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)tc;
				return (studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students))
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)tc;
				return (teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher)
						&& (activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag);

				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)tc;
				return activityTagsComboBox->currentText()==QString("") || activityTagsComboBox->currentText()==ctr->activityTag;

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				return true;

				break;
			}
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherNoTwoConsecutiveDays* ctr=(ConstraintTeacherNoTwoConsecutiveDays*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveTimeSlots*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				return true;

				break;
			}
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				return true;

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
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

				ConstraintTwoSetsOfActivitiesSameSections* ctr=(ConstraintTwoSetsOfActivitiesSameSections*)tc;

				bool foundTeacher=false;
				bool foundSubject=false;
				bool foundActivityTag=false;
				bool foundStudents=false;

				for(int id : ctr->activitiesAIds){
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
				
				for(int id : ctr->activitiesBIds){
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
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				return true;

				break;
			}
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				return true;

				break;
			}
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintTeacherMaxSingleGapsInSelectedTimeSlots*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacher;

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherMaxHoursPerTerm* ctr=(ConstraintTeacherMaxHoursPerTerm*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				return true;

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				return true;

				break;
			}
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				return true;

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
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

				ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots*)tc;

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
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
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

				ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots*)tc;

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
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				assert(teachersComboBox!=nullptr);
				ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection*)tc;
				return teachersComboBox->currentText()==QString("") || teachersComboBox->currentText()==ctr->teacherName;

				break;
			}
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				return true;

				break;
			}
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				assert(studentsComboBox!=nullptr);
				ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection*)tc;
				return studentsComboBox->currentText()==QString("") || showedStudents.contains(ctr->students);

				break;
			}
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				return true;

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
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

				ConstraintActivitiesOverlapCompletelyOrDoNotOverlap* ctr=(ConstraintActivitiesOverlapCompletelyOrDoNotOverlap*)tc;

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
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
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

				ConstraintActivitiesOverlapCompletelyOrDoNotOverlap* ctr=(ConstraintActivitiesOverlapCompletelyOrDoNotOverlap*)tc;

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
		//249
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityBeginsOrEndsStudentsDay* ctr=(ConstraintActivityBeginsOrEndsStudentsDay*)tc;

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

				return true;

				break;
			}
		//250
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
			{
				return true;

				break;
			}
		//251
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
			{
				assert(teachersComboBox!=nullptr);
				assert(studentsComboBox!=nullptr);
				assert(subjectsComboBox!=nullptr);
				assert(activityTagsComboBox!=nullptr);

				ConstraintActivityBeginsOrEndsTeachersDay* ctr=(ConstraintActivityBeginsOrEndsTeachersDay*)tc;

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

				return true;

				break;
			}
		//252
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
			{
				return true;

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
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

				ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots*)tc;

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

		default:
			assert(0);
			break;
	}
}

/*void ListTimeConstraints::filter()
{
	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	
	int k=0;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(filterOk(tc)){
			visibleTimeConstraintsList.append(tc);
			constraintsListWidget->addItem(tc->getDescription(gt.rules));
			k++;
			
			if((true || USE_GUI_COLORS) && !tc->active)
				constraintsListWidget->item(k-1)->setBackground(constraintsListWidget->palette().alternateBase());
		}
	}
}

void ListTimeConstraints::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0){
		constraintDescriptionTextEdit->setPlainText(QString(""));
		return;
	}
	
	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	constraintDescriptionTextEdit->setPlainText(s);
}*/

void ListTimeConstraints::addClicked()
{
	int oldRow=constraintsListWidget->currentRow();

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int initialNumberOfTimeConstraints=gt.rules.timeConstraintsList.count();

	QString preselectedTeacherName;
	QString preselectedStudentsSetName;
	QString preselectedActivityTagName;
	QString preselectedFirstActivityTagName;
	QString preselectedSecondActivityTagName;

	if(teachersComboBox!=nullptr)
		preselectedTeacherName=teachersComboBox->currentText();
	else
		preselectedTeacherName=QString();

	if(studentsComboBox!=nullptr)
		preselectedStudentsSetName=studentsComboBox->currentText();
	else
		preselectedStudentsSetName=QString();

	if(activityTagsComboBox!=nullptr)
		preselectedActivityTagName=activityTagsComboBox->currentText();
	else
		preselectedActivityTagName=QString();

	if(first_activityTagsComboBox!=nullptr)
		preselectedFirstActivityTagName=first_activityTagsComboBox->currentText();
	else
		preselectedFirstActivityTagName=QString();

	if(second_activityTagsComboBox!=nullptr)
		preselectedSecondActivityTagName=second_activityTagsComboBox->currentText();
	else
		preselectedSecondActivityTagName=QString();

	AddOrModifyTimeConstraint aomtc(dialog, type, nullptr,
	 preselectedTeacherName, preselectedStudentsSetName, preselectedActivityTagName,
	 preselectedFirstActivityTagName, preselectedSecondActivityTagName);

	int finalNumberOfTimeConstraints=gt.rules.timeConstraintsList.count();

	filter();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);
	
	int newRow=oldRow;
	if(initialNumberOfTimeConstraints!=finalNumberOfTimeConstraints){
		newRow=constraintsListWidget->count()-1;
		
		if(sortedCheckBox->isChecked()){
			assert(finalNumberOfTimeConstraints>=1);
			for(int i=0; i<visibleTimeConstraintsList.count(); i++)
				if(visibleTimeConstraintsList.at(i)==gt.rules.timeConstraintsList.constLast()){
					newRow=i;
					break;
				}
		}
	}
	
	constraintsListWidget->setCurrentRow(newRow);
	constraintChanged();

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::modifyClicked()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected constraint"));
	
		//constraintsListWidget->setFocus();

		return;
	}
	
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	AddOrModifyTimeConstraint aomtc(dialog, type, ctr);
	
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

void ListTimeConstraints::modifyMultipleClicked()
{
	int crtRow=constraintsListWidget->currentRow();

	QList<TimeConstraint*> constraintsToModify;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected())
			constraintsToModify.append(visibleTimeConstraintsList.at(i));

	if(constraintsToModify.isEmpty()){
		QMessageBox::information(dialog, tr("FET information"), tr("No constraints selected"));

		//constraintsListWidget->setFocus();

		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	ModifyTimeConstraints mtc(dialog, type, constraintsToModify);

	filter();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(crtRow>=constraintsListWidget->count())
		crtRow=constraintsListWidget->count()-1;

	if(crtRow>=0)
		constraintsListWidget->setCurrentRow(crtRow);

	constraintChanged();

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::removeClicked()
{
	bool recompute=false;

	QList<TimeConstraint*> tl;

	bool firstBasic=true;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			tl.append(ctr);

			if(firstBasic && ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
				firstBasic=false;

				QMessageBox::StandardButton wr;

				QString s=tr("Your selection contains the basic compulsory time constraint(s). Do you really want to remove all the selected "
				 "constraints, including this (these) one(s)? You cannot generate a timetable without this (these) constraint(s).");
				s+=QString("\n\n");
				s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
					"Miscellaneous -> Basic compulsory time constraints.");

				wr=QMessageBox::warning(dialog, tr("FET warning"), s,
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

				if(wr==QMessageBox::No){
					//constraintsListWidget->setFocus();
					return;
				}
			}
			else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				recompute=true;
			}
		}

	QString s;
	s=tr("Remove these selected time constraints?");
	s+=QString("\n\n");
	for(TimeConstraint* ctr : std::as_const(tl))
		s+=ctr->getDetailedDescription(gt.rules)+"\n";
	int lres=LongTextMessageBox::confirmation(dialog, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );

	if(lres!=0){
		//constraintsListWidget->setFocus();
		return;
	}

	QString su;
	if(!tl.isEmpty()){
		su=tr("Removed %1 time constraints:").arg(tl.count());
		su+=QString("\n\n");
		for(TimeConstraint* ctr : std::as_const(tl))
			su+=ctr->getDetailedDescription(gt.rules)+"\n";
	}

	//The user clicked the OK button or pressed Enter

	gt.rules.removeTimeConstraints(tl);

	if(!tl.isEmpty())
		gt.rules.addUndoPoint(su);

	if(recompute){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
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

void ListTimeConstraints::closeClicked()
{
	dialog->close();
}

void ListTimeConstraints::helpClicked()
{
	switch(type){
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				HelpOnStudentsMinHoursDaily::help(dialog);

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				QString s;

				s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise the generation might be impossible) to remove 'bad' (redundant and with weight < 100%) constraints"
				 " min days between activities. If you are sure that you don't have 'bad' constraints, you can skip this step, but it doesn't hurt to do it, as a precaution."
				 " Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
				 " Please read Help/Important tips - tip number 2 for details.");
				s+="\n\n";
				s+=tr("If you divide the years by categories (the students have optional courses), you may not need this kind of constraint. If the year is for instance"
				 " divided by language, English/French/German, sometimes people think that it is needed that these optional activities to have the same starting"
				 " time. But solutions exist without adding same starting time constraints. Some people try to avoid students gaps by using this"
				 " constraint, because they need to do it this way in other timetabling software. But it is not the best practice to do it with such a"
				 " constraint. The correct way is to add a max 0 gaps per week constraint for students and maybe also an early max 0 beginnings at second"
				 " hour constraint for students.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				QString s=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise the generation might be impossible) to remove 'bad' (redundant and with weight < 100%) constraints"
				" min days between activities. If you are sure that you don't have 'bad' constraints, you can skip this step, but it doesn't hurt to do it, as a precaution."
				" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
				" Please read Help/Important tips - tip number 2 for details.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				QString s=tr("Please make sure that the selected activities are not forced to be"
					" consecutive by some other constraint 'min days between activities' (with"
					" 'consecutive if on the same day' true), by a constraint 'two activities consecutive',"
					" or by a constraint 'two activities grouped' (also, if you have a constraint"
					" 'three activities grouped' related to the selected activities, make sure that the"
					" constraints do not contradict).");

				LongTextMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				LongTextMessageBox::largeInformation(dialog, tr("FET help"), tr(
				 "This constraint is used to specify that for some components of a type of"
				" activities, for instance for math activities, you need that from say 4-5"
				" per week, at least the first 2 to be early (say in the first 4 hours).")
				+"\n\n"+
				tr("This is simple: just add 2 constraints of this type, with the component"
				" number 1, (then 2) and the corresponding subject (you can also"
				" define teacher, students set and activity tag, of course).")
				 +"\n\n"+
				 tr("Note: if you have math with 4 and 5 splits and want to constrain 2 subactivities for those with 4 per week and 3 subactivities"
				 " for those with 5 per week, you can add constraints only for math with components 3, 4 and 5 (nice trick)")
				 +"\n\n"+
				 tr("Note: if an activity does not belong to a larger group (is a single activity),"
				 " it is considered to have component number 1. As an advice, if you have activities of say biology split into 1"
				 " or 2 per week and want to constrain only one component if there are 2 per week, add a constraint for biology with component number=2")
				 +"\n\n"+
				 tr("Note: if a teacher or a students set cannot have activities in some slots because of this constraint, gaps will be counted (if you have max gaps constraints)"
				  ". So be careful if you add this constraint for all subactivities, for only a teacher or for only a students set"));

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				LongTextMessageBox::largeInformation(dialog, tr("FET help"), tr(
				 "This constraint is used to specify that for some components of a type of"
				" activities, for instance for math activities, you need that from say 4-5"
				" per week, at least the first 2 to start early (say in the first 4 hours).")+
				+"\n\n"+
				tr("This is simple: just add 2 constraints of this type, with the component"
				" number 1, (then 2) and the corresponding subject (you can also"
				" define teacher, students set and activity tag, of course)."
				 )
				 +"\n\n"+
				 tr("Note: if you have math with 4 and 5 splits and want to constrain 2 subactivities for those with 4 per week and 3 subactivities"
				 " for those with 5 per week, you can add constraints only for math with components 3, 4 and 5 (nice trick)")
				 +"\n\n"+
				 tr("Note: if an activity does not belong to a larger group (is a single activity),"
				 " it is considered to have component number 1. As an advice, if you have activities of say biology split into 1"
				 " or 2 per week and want to constrain only one component if there are 2 per week, add a constraint for biology with component number=2")
				 +"\n\n"+
				 tr("Note: if a teacher or a students set cannot have activities in some slots because of this constraint, gaps will be counted (if you have max gaps constraints)"
				  ". So be careful if you add this constraint for all subactivities, for only a teacher or for only a students set"));

				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				QString s;

				s+=tr("Help about the constraint max days between activities:");
				s+="\n\n";
				s+=tr("This constraint was suggested for the following situation: a user needed that activities A1, A2 and A3 to be in consecutive days"
				 " (like: A1 on Tuesday, A2 on Wednesday and A3 on Thursday. So, they must be in 3 consecutive days). This is simple: add a constraint"
				 " max days between activities for A1, A2 and A3, with max 2 days between them. It is supposed that these activities are constrained"
				 " not to be on the same day by a constraint min days between activities.");
				s+="\n\n";
				s+=tr("So, the general situation: this constraint ensures that between each pair from the selected activities, the distance in days is at most the selected value."
				 " Distance = 1 day between a pair A1 and A2 means that A1 and A2 are in consecutive days (like Thursday and Friday)."
				 " Distance = 3 days means that A1 and A2 are 3 days apart, for instance Monday and Thursday.");
				s+="\n\n";
				s+=tr("Another example: teacher T wants to ensure that his activities take place in at most 4 consecutive days (so, from Monday to Thursday or from"
				 " Tuesday to Friday). Then, add all his activities and max days between them = 3.");

				s+="\n\n";
				s+=tr("IMPORTANT: after adding constraints of this type with max days between the activities = 0, it is necessary (otherwise the generation might be impossible) to "
				 "remove 'bad' (redundant and with weight < 100%) constraints min days between activities. If you are sure that you don't have 'bad' constraints, you can skip "
				 "this step, but it doesn't hurt to do it, as a precaution. Also, you don't have to do that after each added constraint, but only once after adding more constraints "
				 "of this type. Please read Help/Important tips - tip number 2 for details.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				QString s=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
				 "of time slots from those selected which can be occupied by the specified activities.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				QString s=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
				 "of activities from those selected which can be simultaneous in each of the selected time slots.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				QString s=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a minimum number "
				 "of time slots from those selected which must be occupied by the specified activities.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				QString s=QString("");

				s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a minimum number "
				 "of activities from those selected which must be simultaneous in each of the selected time slots.");
				s+=QString("\n\n");
				s+=tr("You can select the option 'Allow empty slots'. This means that each selected slot may either have the minimum number of simultaneous activities "
				 "out of the selected ones, or no activities out of the selected ones.");
				s+=QString("\n\n");
				s+=tr("IMPORTANT NOTE: If you need a solution without empty slots or if a solution with empty slots does not exist, please leave the option 'Allow "
				 "empty slots' unselected, because the generation is more efficient this way.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				HelpOnStudentsMinHoursDaily::help(dialog);

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				HelpOnStudentsMinHoursDaily::help(dialog);

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				QString s=QString("");

				s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum total number "
				 "of activities from those selected which can be placed (even partially) in any of the selected time slots.");
				s+=QString("\n\n");
				s+=tr("This constraint is useful to limit the number of activities in the overflow (or fake) blocks.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				QString s=tr("This constraint was suggested by %1, %2, and other users on this FET forum topic: %3.", "%1 and %2 are two persons, %3 is an internet link")
				 .arg("Benahmed Abdelkrim").arg("Hiba Hadi").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4990.0");
				s+="\n\n";
				s+=tr("It was designed to disallow the teachers to have activities in two consecutive real days, morning and afternoon on both,"
				 " which can be tiring. If you can allow the teachers to have activities only on the afternoon of a real day (half day 1), then the whole next real day"
				 " (both morning and afternoon, half days 2 and 3), then only on the next morning (half day 4), you can select the exception option."
				 " In this case only a succession of these four half-days: morning-afternoon-morning-afternoon is NOT allowed by the algorithm.");
				LongTextMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				QString s=tr("This constraint was suggested by %1, %2, and other users on this FET forum topic: %3.", "%1 and %2 are two persons, %3 is an internet link")
				 .arg("Benahmed Abdelkrim").arg("Hiba Hadi")
				 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4990.0");
				s+="\n\n";
				s+=tr("It was designed to disallow a teacher to have activities in two consecutive real days, morning and afternoon on both,"
				 " which can be tiring. If you can allow the teacher to have activities only on the afternoon of a real day (half day 1), then the whole next real day"
				 " (both morning and afternoon, half days 2 and 3), then only on the next morning (half day 4), you can select the exception option."
				 " In this case only a succession of these four half-days: morning-afternoon-morning-afternoon is NOT allowed by the algorithm.");
				LongTextMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				QString s=tr("This constraint was suggested by the user %1 on this FET forum topic: %2.").arg("Benahmed Abdelkrim")
				 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4990.0");
				s+="\n\n";
				s+=tr("It was designed to disallow the students to have activities in two consecutive real days, morning and afternoon on both,"
				 " which can be tiring. If you can allow the students to have activities only on the afternoon of a real day (half day 1), then the whole next real day"
				 " (both morning and afternoon, half days 2 and 3), then only on the next morning (half day 4), you can select the exception option."
				 " In this case only a succession of these four half-days: morning-afternoon-morning-afternoon is NOT allowed by the algorithm.");
				LongTextMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				QString s=tr("This constraint was suggested by the user %1 on this FET forum topic: %2.").arg("Benahmed Abdelkrim")
				 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4990.0");
				s+="\n\n";
				s+=tr("It was designed to disallow a students set to have activities in two consecutive real days, morning and afternoon on both,"
				 " which can be tiring. If you can allow the students set to have activities only on the afternoon of a real day (half day 1), then the whole next real day"
				 " (both morning and afternoon, half days 2 and 3), then only on the next morning (half day 4), you can select the exception option."
				 " In this case only a succession of these four half-days: morning-afternoon-morning-afternoon is NOT allowed by the algorithm.");
				LongTextMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				QString s=QString("");

				s+=tr("Help about the constraint max terms between activities:");
				s+="\n\n";
				s+=tr("This constraint was suggested for the following situation: a user needed that a set of activities to be in three consecutive terms."
				 " In this case add a constraint of type max terms between activities for these activities with max terms between activities = 2 (it is supposed that"
				 " there are also other constraints that ensure that these activities occupy at least three terms).");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				QString s=QString("");

				s+=tr("Help about the constraint max half days between activities:");
				s+="\n\n";
				s+=tr("This constraint was suggested by two users, for two different situations in the Mornings-Afternoons mode:");
				s+="\n\n";
				s+=tr("1)")+QString(" ");
				s+=tr("This constraint was suggested by %1, so that a teacher does not work both on the afternoon of the last day of the week and the morning"
				 " of the first day of the week. The user needs to add all the activities of the teacher, max half days between them = n_half_days_per_week-2."
				 " See the forum topic %2 for more details.", "%1 is a person").arg("Mohamed Ait Ichou").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=5207.0");
				s+="\n\n";
				s+=tr("2)")+QString(" ");
				s+=tr("Also, this constraint was suggested by %1, to do the timetable for two weeks using tricks. Here are this user's words:"
				 " 'For the Tunisian high schools I opted for the morning evening mode with two weeks A and B for the weekly activities with the constraints"
				 " min days between activities 6, max days between activities 6 and the same starting hour with max different rooms 1. I added a trick so that"
				 " the two activities are in the same half of the day: for each real room I created two rooms, one valid in the morning and the other valid in"
				 " the evening; to avoid this bypass, is it possible to add a max half days between activities constraint?' (Note: The constraint type min half days"
				 " between activities was existing at the time of this post.) See the forum topic %2 for more details.", "%1 is a person")
				 .arg("fourat").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=5208.0");

				s+="\n\n";
				s+=tr("IMPORTANT: after adding constraints of this type with max half days between the activities = 0, it is necessary (otherwise the generation might be impossible) to "
				 "remove 'bad' (redundant and with weight < 100%) constraints min (half) days between activities. If you are sure that you don't have 'bad' constraints, you can skip "
				 "this step, but it doesn't hurt to do it, as a precaution. Also, you don't have to do that after each added constraint, but only once after adding more constraints "
				 "of this type. Please read Help/Important tips - tip number 2 for details.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				HelpOnStudentsMinHoursDaily::help(dialog);

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				HelpOnStudentsMinHoursDaily::help(dialog);

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				QString s=QString("");

				s+=tr("Help about the constraint activities max hourly span:");
				s+="\n\n";
				s+=tr("This constraint was suggested by %1.", "%1 is a user").arg("pnlshd");
				s+=" ";
				s+=tr("The activities can be on any days, but their hourly span must be limited by the specified maximum value.");
				s+="\n\n";
				s+=tr("More details and an example: the hourly span of the a set of activities is the number of hours between the start of the earliest activity"
				 " and the end of the latest activity, neglecting their days. So, if for instance the constraint addresses A1, A2, A3, and A4, each of them having duration 1,"
				 " and say: A1 is on day 1 hour 2, A2 is on day 1 hour 3, A3 is on day 3 hour 5, and A4 is on day 5 hour 2, then the earliest activities"
				 " (neglecting their days) are A1 and A4 (starting at hour 2), and the latest activity (neglecting its day) is A3 (ending after hour 5)."
				 " The hourly span of A1, A2, A3, and A4 is thus (5 + 1) - 2 = 4.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
			{
				QString s=tr("This constraint was suggested by %1 and %2.", "%1 and %2 are the two persons who suggested this constraint.")
				 .arg("Edward Downes").arg("Richard B. Gross");
				s+="\n\n";
				s+=tr("The two sets must have the same number of activities, and the activities should have the same students sets, respectively.");
				s+=" ";
				s+=tr("Also, all the activities in such a constraint must have equal durations.");
				s+="\n\n";
				s+=tr("You can add exceptions for the time slots. If an activity is placed, even partially, in these exception time slots,"
				 " it is not forced to respect this constraint.");
				s+="\n\n";

				s+=tr("More details, written by %1 and %2:", "%1 and %2 are two persons.").arg("Edward Downes").arg("Richard B. Gross");
				s+="\n\n";
				s+=tr("*Two sets of activities have the same sections*"
				 "\n"
				 "This constraint can be used to create *identical groupings* of students for classes that meet multiple times with multiple sections."
				 " Note: both sets of activities MUST have the same number of activities and be all of the same duration."
				 "\n\n"
				 "Example 1:"
				 "\n\n"
				 "   - Your block schedule has two days (A day and B day) with all students in 7th grade taking a Math class."
				 "\n"
				 "   - There are max >=2 sections of 7th grade Math on either day."
				 "\n"
				 "   - You have A day *and* B day activities for each student's Math class (Student1 - Math - A day, Student1 - Math - B day, Student2 - Math - A day, etc.)"
				 "\n"
				 "   - Use this constraint and the tabs to select,"
				 "\n"
				 "      -  all the A day activities in the First activities set, followed by all the B day activities in the Second activities set."
				 "\n"
				 "   - The constraint will now form the same groupings or sections of students on the B days as it does on the A days."
				 "\n\n"
				 "Example 2:"
				 "\n\n"
				 "   - You have two classes, say Math and Chemistry (both with same duration), with the exact same students lists."
				 "\n"
				 "   - You want max >=2 sections of Math and max >=2 sections of Chemistry in your block schedule."
				 "\n"
				 "   - You want identical groupings of students in the Math sections as the Chemistry sections."
				 "\n"
				 "   - Use this constraint, then the tabs and select,"
				 "\n"
				 "      -  all the Math activities in the First activities set, followed by all the Chemistry activities in the Second activities set.");
				s+="\n\n";
				s+=tr("Note (by %1): if you use overflow slots for the activities which cannot be placed in the real time slots, these overflow slots should"
				 " be added as exception slots, in which an activity is not forced to respect this 'same sections' constraint."
				 , "%1 is the person who wrote this note.").arg("Liviu Lalescu");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			[[fallthrough]];
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			[[fallthrough]];
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			[[fallthrough]];
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QString s=tr("This constraint was suggested by %1.", "%1 is the person who suggested this constraint.")
				 .arg("Yush Yuen");
				s+="\n\n";
				s+=tr("This constraint is useful if you want to avoid small chunks of free time"
				 " between classes. A gap is defined as a single FET hour between two"
				 " activities. Select the students/teachers for whom you want this"
				 " constraint applied to, then select the time slots where you DO NOT want gaps"
				 " to appear. The maximum allowed number of single gaps is defaulted to zero. Increasing"
				 " this number will result in more gaps appearing within the select time"
				 " slots. This might be helpful in troubleshooting an impossible timetable."
				 " Note that two consecutive single gaps are not considered a 'gap'.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			[[fallthrough]];
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			[[fallthrough]];
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			[[fallthrough]];
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				/*QString tt=tr("This constraint ensures that the specified teacher(s)/students (set) can have activities either in"
				 " the first selected time slot or in the second one, but not in both together."
				 " The unselected slots are not constrained.");*/
				QString s=tr("This constraint ensures that the teachers (or the specified teacher), or the students (or the specified"
				 " students set) can have activities in either the first selected time slot or in the second one, but not in both"
				 " at the same time. The unselected time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			[[fallthrough]];
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			[[fallthrough]];
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			[[fallthrough]];
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QString s=tr("This constraint was suggested by %1.", "%1 is the person who suggested this constraint.")
				 .arg("YOUSSEF HOUIET");
				s+="\n\n";
				/*QString tt=tr("This constraint ensures that the specified teacher(s)/students (set) can have activities either in"
				 " the first set of selected time slots or in the second one, but not in both together."
				 " The unselected slots are not constrained.");*/
				s+=tr("This constraint ensures that the teachers (or the specified teacher), or the students (or the specified"
				 " students set) can have activities in either the first set of selected time slots or in the second one, but"
				 " not in both at the same time. The unselected time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QString s=tr("This constraint was suggested by %1.", "%1 is the person who suggested this constraint.")
				 .arg("Vũ Ngọc Thành");
				s+="\n\n";
				/*QString tt=tr("This constraint ensures that the selected set of activities are placed either in"
				 " the first set of selected time slots or in the second one, but not in both together."
				 " The unselected slots are not constrained.");*/
				s+=tr("This constraint ensures that the selected activities will be placed in either the"
				 " first set of selected time slots or in the second one, but not in both at the same time."
				 " The unselected time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				/*QString tt=tr("This constraint ensures that the selected set of activities are placed either in"
				 " the first selected time slot or in the second one, but not in both together."
				 " The unselected slots are not constrained.");*/
				QString s=tr("This constraint ensures that the selected activities will be placed in either the"
				 " first selected time slot or in the second one, but not in both at the same time. The unselected"
				 " time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			[[fallthrough]];
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			[[fallthrough]];
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			[[fallthrough]];
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QString s=tr("This constraint was suggested by %1.", "%1 is the person who suggested this constraint.")
				 .arg("Sérgio Augusto Dias Castro");
				s+="\n\n";
				s+=tr("This constraint ensures that the teachers (or the specified teacher), or the students (or the specified"
				 " students set) can have activities in a specified maximum number of sets of time"
				 " slots from a selection of sets of time slots. The unselected time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
			{
				QString s=tr("This constraint was suggested by %1.", "%1 is the person who suggested this constraint.")
				 .arg("alege");
				s+="\n\n";
				s+=tr("This constraint has specified a set of activities, which must all have the same duration, greater than 1,"
				 " and ensures that each pair of activities from this set are either overlapping completely (they have the same"
				 " starting day and hour), or are not overlapping at all (have no common time slot(s)).");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QString s=tr("This constraint was suggested by %1, and maybe by other users in the past.",
				 "%1 is a person who suggested this constraint.").arg("k3nt0n");
				s+="\n\n";
				s+=tr("This constraint ensures that the selected activities can take place in a specified maximum number"
				 " of sets of time slots from a selection of sets of time slots."
				 " The unselected time slots are not subject to constraints.");
				s+="\n";

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
			{
				QString s=tr("Warning: this constraint might slow down very much the generation! Please use only if strictly necessary, and with care!");
				s+="\n\n";
				s+=tr("This constraint was suggested by %1.").arg("Furkan Kızılaslan");
				s+="\n\n";
				s+=tr("This constraint type was added on %1.", "%1 is the date at which this constraint type was implemented in FET").arg(tr("18 October 2025"));
				s+="\n\n";
				s+=tr("To use this constraint, you need to specify a set of activities, a set of time slots, and a maximum number "
				 "of students which can be present, considering the number of students of these activities, in these time slots.");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}

		default: assert(0);
	}
}

void ListTimeConstraints::modifyMultipleMinDaysBetweenActivitiesConstraintsClicked()
{
	if(type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
		ChangeMinDaysSelectivelyForm csDialog(dialog);

		setParentAndOtherThings(&csDialog, dialog);
		bool result=csDialog.exec();

		if(result==QDialog::Accepted){
			double oldWeight=csDialog.oldWeight;
			double newWeight=csDialog.newWeight;
			int oldConsecutive=csDialog.oldConsecutive;
			QString oldConsecutiveString=csDialog.oldConsecutiveString;
			int newConsecutive=csDialog.newConsecutive;
			QString newConsecutiveString=csDialog.newConsecutiveString;
			int oldDays=csDialog.oldDays;
			int newDays=csDialog.newDays;
			int oldNActs=csDialog.oldNActs;
			if(oldWeight==-1){
			}
			else if(oldWeight>=0 && oldWeight<=100.0){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old weight is not -1 and not (>=0.0 and <=100.0))"));
				return;
			}

			if(newWeight==-1){
			}
			else if(newWeight>=0 && newWeight<=100.0){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (new weight is not -1 and not (>=0.0 and <=100.0))"));
				return;
			}

			enum {ANY=0, YES=1, NO=2};
			enum {NOCHANGE=0};

			if(oldConsecutive<0 || oldConsecutive>2){
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old consecutive is not any, yes or no)"));
				return;
			}

			if(newConsecutive<0 || newConsecutive>2){
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (new consecutive is not no_change, yes or no)"));
				return;
			}

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(oldDays==-1){
				}
				else if(oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (old min days is not -1 or 1..ndaysperweek)"));
					return;
				}

				if(newDays==-1){
				}
				else if(newDays>=1 && newDays<=gt.rules.nDaysPerWeek){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (new min days is not -1 or 1..ndaysperweek)"));
					return;
				}
			}
			else{
				if(oldDays==-1){
				}
				else if(oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek/2){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (old min days is not -1 or 1..nrealdaysperweek)"));
					return;
				}

				if(newDays==-1){
				}
				else if(newDays>=1 && newDays<=gt.rules.nDaysPerWeek/2){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (new min days is not -1 or 1..nrealdaysperweek)"));
					return;
				}
			}

			if(oldNActs==-1){
			}
			else if(oldNActs>=1){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old nActivities is not -1 or >=1)"));
				return;
			}

			int count=0;

			for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
				if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
					ConstraintMinDaysBetweenActivities* mc=(ConstraintMinDaysBetweenActivities*)tc;
					bool okw, okd, okc, okn;
					if(oldWeight==-1)
						okw=true;
					else if(oldWeight==mc->weightPercentage)
						okw=true;
					else
						okw=false;

					if(oldConsecutive==ANY)
						okc=true;
					else if(oldConsecutive==YES && mc->consecutiveIfSameDay==true)
						okc=true;
					else if(oldConsecutive==NO && mc->consecutiveIfSameDay==false)
						okc=true;
					else
						okc=false;

					if(oldDays==-1)
						okd=true;
					else if(oldDays==mc->minDays)
						okd=true;
					else
						okd=false;

					if(oldNActs==-1)
						okn=true;
					else if(mc->n_activities==oldNActs)
						okn=true;
					else
						okn=false;

					if(okw && okc && okd && okn){
						if(newWeight>=0)
							mc->weightPercentage=newWeight;

						if(newConsecutive==YES)
							mc->consecutiveIfSameDay=true;
						else if(newConsecutive==NO)
							mc->consecutiveIfSameDay=false;

						if(newDays>=0)
							mc->minDays=newDays;

						count++;
					}
				}

			QMessageBox::information(dialog, tr("FET information"), tr("There were inspected (and possibly modified) %1 constraints min days between activities"
			 " matching your criteria").arg(count)+"\n\n"+
			 tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day or max 0 (half) days between activities, it is important"
			  " (after current operation) to apply the operation of removing redundant constraints.")
			 +" "+tr("Read Help/Important tips - tip 2) for details.")
			 );

			QString s=tr("Modified multiple constraints min days between activities with filter:");
			s+=QString("\n\n");

			s+=tr("Old weight percentage=%1").arg(oldWeight);
			s+=QString("\n");
			s+=tr("New weight percentage=%1").arg(newWeight);
			s+=QString("\n\n");

			s+=tr("Old consecutive if same day=%1").arg(oldConsecutiveString);
			s+=QString("\n");
			s+=tr("New consecutive if same day=%1").arg(newConsecutiveString);
			s+=QString("\n\n");

			s+=tr("Old min days=%1").arg(oldDays);
			s+=QString("\n");
			s+=tr("New min days=%1").arg(newDays);
			s+=QString("\n\n");

			s+=tr("Old number of activities=%1").arg(oldNActs);
			s+=QString("\n");

			gt.rules.addUndoPoint(s);

			gt.rules.internalStructureComputed=false;
			setRulesModifiedAndOtherThings(&gt.rules);

			filter();
		}
	}
	else if(type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
		ChangeMinHalfDaysSelectivelyForm csDialog(dialog);

		setParentAndOtherThings(&csDialog, dialog);
		bool result=csDialog.exec();

		if(result==QDialog::Accepted){
			double oldWeight=csDialog.oldWeight;
			double newWeight=csDialog.newWeight;
			int oldConsecutive=csDialog.oldConsecutive;
			QString oldConsecutiveString=csDialog.oldConsecutiveString;
			int newConsecutive=csDialog.newConsecutive;
			QString newConsecutiveString=csDialog.newConsecutiveString;
			int oldDays=csDialog.oldDays;
			int newDays=csDialog.newDays;
			int oldNActs=csDialog.oldNActs;
			if(oldWeight==-1){
			}
			else if(oldWeight>=0 && oldWeight<=100.0){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old weight is not -1 and not (>=0.0 and <=100.0))"));
				return;
			}

			if(newWeight==-1){
			}
			else if(newWeight>=0 && newWeight<=100.0){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (new weight is not -1 and not (>=0.0 and <=100.0))"));
				return;
			}

			enum {ANY=0, YES=1, NO=2};
			enum {NOCHANGE=0};

			if(oldConsecutive<0 || oldConsecutive>2){
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old consecutive is not any, yes or no)"));
				return;
			}

			if(newConsecutive<0 || newConsecutive>2){
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (new consecutive is not no_change, yes or no)"));
				return;
			}

			if(true){
				if(oldDays==-1){
				}
				else if(oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (old min days is not -1 or 1..ndaysperweek)"));
					return;
				}

				if(newDays==-1){
				}
				else if(newDays>=1 && newDays<=gt.rules.nDaysPerWeek){
				}
				else{
					QMessageBox::critical(dialog, tr("FET information"),
					 tr("FET has met a critical error - aborting current operation, please report bug (new min days is not -1 or 1..ndaysperweek)"));
					return;
				}
			}

			if(oldNActs==-1){
			}
			else if(oldNActs>=1){
			}
			else{
				QMessageBox::critical(dialog, tr("FET information"),
				 tr("FET has met a critical error - aborting current operation, please report bug (old nActivities is not -1 or >=1)"));
				return;
			}

			int count=0;

			for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
				if(tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
					ConstraintMinHalfDaysBetweenActivities* mc=(ConstraintMinHalfDaysBetweenActivities*)tc;
					bool okw, okd, okc, okn;
					if(oldWeight==-1)
						okw=true;
					else if(oldWeight==mc->weightPercentage)
						okw=true;
					else
						okw=false;

					if(oldConsecutive==ANY)
						okc=true;
					else if(oldConsecutive==YES && mc->consecutiveIfSameDay==true)
						okc=true;
					else if(oldConsecutive==NO && mc->consecutiveIfSameDay==false)
						okc=true;
					else
						okc=false;

					if(oldDays==-1)
						okd=true;
					else if(oldDays==mc->minDays)
						okd=true;
					else
						okd=false;

					if(oldNActs==-1)
						okn=true;
					else if(mc->n_activities==oldNActs)
						okn=true;
					else
						okn=false;

					if(okw && okc && okd && okn){
						if(newWeight>=0)
							mc->weightPercentage=newWeight;

						if(newConsecutive==YES)
							mc->consecutiveIfSameDay=true;
						else if(newConsecutive==NO)
							mc->consecutiveIfSameDay=false;

						if(newDays>=0)
							mc->minDays=newDays;

						count++;
					}
				}

			QMessageBox::information(dialog, tr("FET information"), tr("There were inspected (and possibly modified) %1 constraints min half days between activities"
			 " matching your criteria").arg(count)+"\n\n"+
			 tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day or max 0 (half) days between activities, it is important"
			  " (after current operation) to apply the operation of removing redundant constraints.")
			 +" "+tr("Read Help/Important tips - tip 2) for details.")
			 );

			QString s=tr("Modified multiple constraints min half days between activities with filter:");
			s+=QString("\n\n");

			s+=tr("Old weight percentage=%1").arg(oldWeight);
			s+=QString("\n");
			s+=tr("New weight percentage=%1").arg(newWeight);
			s+=QString("\n\n");

			s+=tr("Old consecutive if same day=%1").arg(oldConsecutiveString);
			s+=QString("\n");
			s+=tr("New consecutive if same day=%1").arg(newConsecutiveString);
			s+=QString("\n\n");

			s+=tr("Old min half days=%1").arg(oldDays);
			s+=QString("\n");
			s+=tr("New min half days=%1").arg(newDays);
			s+=QString("\n\n");

			s+=tr("Old number of activities=%1").arg(oldNActs);
			s+=QString("\n");

			gt.rules.addUndoPoint(s);

			gt.rules.internalStructureComputed=false;
			setRulesModifiedAndOtherThings(&gt.rules);

			filter();
		}
	}
	else{
		assert(0);
	}
}

void ListTimeConstraints::addRemoveMultipleConstraintsActivitiesSameStartingHourClicked()
{
	assert(type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR);

	AddRemoveMultipleConstraintsActivitiesSameStartingHourForm form(dialog);
	setParentAndOtherThings(&form, dialog);
	form.exec();

	filter();

	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
	constraintChanged();
}

void ListTimeConstraints::filter()
{
	disconnect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListTimeConstraints::selectionChanged);

	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	for(TimeConstraint* ctr : std::as_const(gt.rules.timeConstraintsList))
		if(filterOk(ctr))
			visibleTimeConstraintsList.append(ctr);

	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleTimeConstraintsList.begin(), visibleTimeConstraintsList.end(), timeConstraintsAscendingByDescription);

	for(TimeConstraint* ctr : std::as_const(visibleTimeConstraintsList)){
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
	 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
	 .arg(n_active).arg(visibleTimeConstraintsList.count()));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));

	selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListTimeConstraints::selectionChanged);
}

void ListTimeConstraints::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;

	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	constraintDescriptionTextEdit->setPlainText(s);
}

void ListTimeConstraints::sortedChanged(bool checked)
{
	Q_UNUSED(checked);

	filter();

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::activateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected time constraints?");
		ret=QMessageBox::question(dialog, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			//constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(!ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=true;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated %1 time constraints:", "%1 is the number of activated time constraints").arg(cnt)+QString("\n\n")+su);

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
			QMessageBox::information(dialog, tr("FET information"), tr("Activated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::deactivateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected time constraints? "
		 "(Note that the basic compulsory time constraints will not be deactivated, even if they are selected.)");
		ret=QMessageBox::question(dialog, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			//constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
				continue;
			if(ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=false;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated %1 time constraints:", "%1 is the number of deactivated time constraints").arg(cnt)+QString("\n\n")+su);

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
			QMessageBox::information(dialog, tr("FET information"), tr("Deactivated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected constraint"));

		//constraintsListWidget->setFocus();

		return;
	}

	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

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

	QTextEdit* commentsPT=new QTextEdit;
	commentsPT->setPlainText(ctr->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();

	vl->addWidget(commentsPT);
	vl->addLayout(hl);

	getCommentsDialog.setLayout(vl);

	const QString settingsName=QString("TimeConstraintCommentsDialog");

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
			visibleTimeConstraintsList.removeAt(i);
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
			for(TimeConstraint* ctr2 : std::as_const(visibleTimeConstraintsList))
				if(ctr2->active)
					n_active++;

			countOfConstraintsLabel->setText(tr("%1 / %2 time constraints",
			 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
			 .arg(n_active).arg(visibleTimeConstraintsList.count()));
		}
		else{
			constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
			constraintChanged();
		}
	}

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(constraintsListWidget->count()==visibleTimeConstraintsList.count());
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleTimeConstraintsList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected time constraints, %1 is the number of active"
	 " selected time constraints, %2 is the total number of selected time constraints").arg(nActive).arg(nTotal));
}

void ListTimeConstraints::changeWeights()
{
	int cnt_pre=0;
	int cnt_unchanged=0;
	double nw=100.0;
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
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
			nw=QInputDialog::getDouble(dialog, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 selected time constraints.\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (3 or even 4), if needed. %1 is the number of constraints which will change.")
			 .arg(cnt_pre),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		else
			nw=QInputDialog::getDouble(dialog, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 time constraints from your\n"
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
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->canHaveAnyWeight()){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->weightPercentage=nw;
				if(ctr->active && ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}

	assert(cnt>0);
	assert(cnt==cnt_pre);

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Changed the weights of the following %1 selected time constraints to %2%:",
		 "%1 is the number of time constraints for which the user has changed the weight, %2 is the new weight for all the selected constraints")
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
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	//constraintsListWidget->setFocus();
}

void ListTimeConstraints::showRelatedCheckBoxToggled()
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
