/***************************************************************************
                          alltimeconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "alltimeconstraintsform.h"

#include "modifyconstraintbreaktimesform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"

#include "modifyconstraintactivitypreferredtimesform.h"
#include "modifyconstraint2activitiesconsecutiveform.h"
#include "modifyconstraint2activitiesorderedform.h"
#include "modifyconstraintactivitiespreferredtimesform.h"
#include "modifyconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartingdayform.h"
#include "modifyconstraintactivitypreferredtimeform.h"
#include "modifyconstraintactivitiesnotoverlappingform.h"
#include "modifyconstraintminndaysbetweenactivitiesform.h"
#include "modifyconstraintactivityendsstudentsdayform.h"

#include "modifyconstraintteachernotavailabletimesform.h"
#include "modifyconstraintteachersmaxgapsperweekform.h"
#include "modifyconstraintteachermaxgapsperweekform.h"
#include "modifyconstraintteachersmaxgapsperdayform.h"
#include "modifyconstraintteachermaxgapsperdayform.h"
#include "modifyconstraintteachermaxdaysperweekform.h"
#include "modifyconstraintteachersmaxhoursdailyform.h"
#include "modifyconstraintteachermaxhoursdailyform.h"
#include "modifyconstraintteachersmaxhourscontinuouslyform.h"
#include "modifyconstraintteachermaxhourscontinuouslyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteacherminhoursdailyform.h"

#include "modifyconstraintstudentssetnotavailabletimesform.h"
#include "modifyconstraintstudentssetmaxgapsperweekform.h"
#include "modifyconstraintstudentsmaxgapsperweekform.h"
#include "modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetmaxhoursdailyform.h"
#include "modifyconstraintstudentsmaxhoursdailyform.h"
#include "modifyconstraintstudentssetmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentssetminhoursdailyform.h"
#include "modifyconstraintstudentsminhoursdailyform.h"

#include <QDesktopWidget>

AllTimeConstraintsForm::AllTimeConstraintsForm()
{
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	setWindowFlags(Qt::Window);
	
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		constraintsListBox->insertItem(ctr->getDescription(gt.rules));
	}
}

AllTimeConstraintsForm::~AllTimeConstraintsForm()
{
}

void AllTimeConstraintsForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<gt.rules.timeConstraintsList.size());
	TimeConstraint* ctr=gt.rules.timeConstraintsList[index];
	assert(ctr!=NULL);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void AllTimeConstraintsForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
	/*if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("You cannot erase the basic time constraints"));
		return;
	}*/
	QString s;
	s=QObject::tr("Removing constraint:\n");
	s+=ctr->getDetailedDescription(gt.rules);
	s+=QObject::tr("\nAre you sure?");
	
	bool t;

	switch( QMessageBox::warning( this, QObject::tr("FET warning"),
		s, QObject::tr("OK"), QObject::tr("Cancel"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		t=gt.rules.removeTimeConstraint(ctr);
		assert(t);
		constraintsListBox->removeItem(constraintsListBox->currentItem());
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i)>=constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}

void AllTimeConstraintsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
	
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
		ModifyConstraintBasicCompulsoryTimeForm* form=
		 new ModifyConstraintBasicCompulsoryTimeForm((ConstraintBasicCompulsoryTime*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE){
		ModifyConstraint2ActivitiesConsecutiveForm* form=
		 new ModifyConstraint2ActivitiesConsecutiveForm((Constraint2ActivitiesConsecutive*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_2_ACTIVITIES_ORDERED){
		ModifyConstraint2ActivitiesOrderedForm* form=
		 new ModifyConstraint2ActivitiesOrderedForm((Constraint2ActivitiesOrdered*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIMES){
		ModifyConstraintActivityPreferredTimesForm* form=
		 new ModifyConstraintActivityPreferredTimesForm((ConstraintActivityPreferredTimes*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
		ModifyConstraintActivitiesPreferredTimesForm* form=
		 new ModifyConstraintActivitiesPreferredTimesForm((ConstraintActivitiesPreferredTimes*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
		ModifyConstraintActivitiesSameStartingTimeForm* form=
		 new ModifyConstraintActivitiesSameStartingTimeForm((ConstraintActivitiesSameStartingTime*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
		ModifyConstraintActivitiesSameStartingHourForm* form=
		 new ModifyConstraintActivitiesSameStartingHourForm((ConstraintActivitiesSameStartingHour*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
		ModifyConstraintActivitiesSameStartingDayForm* form=
		 new ModifyConstraintActivitiesSameStartingDayForm((ConstraintActivitiesSameStartingDay*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
		ModifyConstraintTeacherNotAvailableTimesForm* form=
		 new ModifyConstraintTeacherNotAvailableTimesForm((ConstraintTeacherNotAvailableTimes*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
		ModifyConstraintStudentsSetNotAvailableTimesForm* form=
		 new ModifyConstraintStudentsSetNotAvailableTimesForm((ConstraintStudentsSetNotAvailableTimes*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_BREAK_TIMES){
		ModifyConstraintBreakTimesForm* form=
		 new ModifyConstraintBreakTimesForm((ConstraintBreakTimes*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherMaxDaysPerWeekForm* form=
		 new ModifyConstraintTeacherMaxDaysPerWeekForm((ConstraintTeacherMaxDaysPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
		ModifyConstraintTeachersMaxHoursDailyForm* form=
		 new ModifyConstraintTeachersMaxHoursDailyForm((ConstraintTeachersMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
		ModifyConstraintTeacherMaxHoursDailyForm* form=
		 new ModifyConstraintTeacherMaxHoursDailyForm((ConstraintTeacherMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersMaxHoursContinuouslyForm* form=
		 new ModifyConstraintTeachersMaxHoursContinuouslyForm((ConstraintTeachersMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeacherMaxHoursContinuouslyForm* form=
		 new ModifyConstraintTeacherMaxHoursContinuouslyForm((ConstraintTeacherMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
		ModifyConstraintTeachersMinHoursDailyForm* form=
		 new ModifyConstraintTeachersMinHoursDailyForm((ConstraintTeachersMinHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
		ModifyConstraintTeacherMinHoursDailyForm* form=
		 new ModifyConstraintTeacherMinHoursDailyForm((ConstraintTeacherMinHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeachersMaxGapsPerWeekForm* form=
		 new ModifyConstraintTeachersMaxGapsPerWeekForm((ConstraintTeachersMaxGapsPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeacherMaxGapsPerWeekForm* form=
		 new ModifyConstraintTeacherMaxGapsPerWeekForm((ConstraintTeacherMaxGapsPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
		ModifyConstraintTeachersMaxGapsPerDayForm* form=
		 new ModifyConstraintTeachersMaxGapsPerDayForm((ConstraintTeachersMaxGapsPerDay*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
		ModifyConstraintTeacherMaxGapsPerDayForm* form=
		 new ModifyConstraintTeacherMaxGapsPerDayForm((ConstraintTeacherMaxGapsPerDay*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
		ModifyConstraintActivityPreferredTimeForm* form=
		 new ModifyConstraintActivityPreferredTimeForm((ConstraintActivityPreferredTime*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsSetMaxGapsPerWeekForm* form=
		 new ModifyConstraintStudentsSetMaxGapsPerWeekForm((ConstraintStudentsSetMaxGapsPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsMaxGapsPerWeekForm* form=
		 new ModifyConstraintStudentsMaxGapsPerWeekForm((ConstraintStudentsMaxGapsPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm* form=
		 new ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm((ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm* form=
		 new ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm((ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
		ModifyConstraintStudentsSetMaxHoursDailyForm* form=
		 new ModifyConstraintStudentsSetMaxHoursDailyForm((ConstraintStudentsSetMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
		ModifyConstraintStudentsMaxHoursDailyForm* form=
		 new ModifyConstraintStudentsMaxHoursDailyForm((ConstraintStudentsMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsSetMaxHoursContinuouslyForm* form=
		 new ModifyConstraintStudentsSetMaxHoursContinuouslyForm((ConstraintStudentsSetMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsMaxHoursContinuouslyForm* form=
		 new ModifyConstraintStudentsMaxHoursContinuouslyForm((ConstraintStudentsMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
		ModifyConstraintStudentsSetMinHoursDailyForm* form=
		 new ModifyConstraintStudentsSetMinHoursDailyForm((ConstraintStudentsSetMinHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
		ModifyConstraintStudentsMinHoursDailyForm* form=
		 new ModifyConstraintStudentsMinHoursDailyForm((ConstraintStudentsMinHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
		ModifyConstraintActivitiesNotOverlappingForm* form=
		 new ModifyConstraintActivitiesNotOverlappingForm((ConstraintActivitiesNotOverlapping*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
		ModifyConstraintMinNDaysBetweenActivitiesForm* form=
		 new ModifyConstraintMinNDaysBetweenActivitiesForm((ConstraintMinNDaysBetweenActivities*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
		ModifyConstraintActivityEndsStudentsDayForm* form=
		 new ModifyConstraintActivityEndsStudentsDayForm((ConstraintActivityEndsStudentsDay*)ctr);
		form->exec();
	}
	else{
		assert(0);
		exit(1);
	}

	constraintsListBox->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		constraintsListBox->insertItem(ctr->getDescription(gt.rules));
	}

	constraintsListBox->setCurrentItem(i);
}
