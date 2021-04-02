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

#include "modifyconstraint2activitiesconsecutiveform.h"
#include "modifyconstraint2activitiesorderedform.h"
#include "modifyconstraint2activitiesgroupedform.h"
#include "modifyconstraintactivitypreferredtimesform.h"
#include "modifyconstraintactivitiespreferredtimesform.h"
#include "modifyconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintteachernotavailableform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"
#include "modifyconstraintstudentssetnotavailableform.h"
#include "modifyconstraintbreakform.h"
#include "modifyconstraintteachermaxdaysperweekform.h"
#include "modifyconstraintteachersmaxhourscontinuouslyform.h"
#include "modifyconstraintteachersmaxhoursdailyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteacherssubgroupsmaxhoursdailyform.h"
#include "modifyconstraintactivitypreferredtimeform.h"
#include "modifyconstraintstudentssetnogapsform.h"
#include "modifyconstraintstudentsnogapsform.h"
#include "modifyconstraintteachersnogapsform.h"
#include "modifyconstraintstudentsearlyform.h"
#include "modifyconstraintstudentssetintervalmaxdaysperweekform.h"
#include "modifyconstraintteacherintervalmaxdaysperweekform.h"
#include "modifyconstraintstudentssetnhoursdailyform.h"
#include "modifyconstraintstudentsnhoursdailyform.h"
#include "modifyconstraintactivityendsdayform.h"
#include "modifyconstraintactivitiesnotoverlappingform.h"
#include "modifyconstraintminndaysbetweenactivitiesform.h"
#include "modifyconstraintteacherssubjecttagsmaxhourscontinuouslyform.h"
#include "modifyconstraintteacherssubjecttagmaxhourscontinuouslyform.h"

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
	else if(ctr->type==CONSTRAINT_2_ACTIVITIES_GROUPED){
		ModifyConstraint2ActivitiesGroupedForm* form=
		 new ModifyConstraint2ActivitiesGroupedForm((Constraint2ActivitiesGrouped*)ctr);
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
	else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE){
		ModifyConstraintTeacherNotAvailableForm* form=
		 new ModifyConstraintTeacherNotAvailableForm((ConstraintTeacherNotAvailable*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
		ModifyConstraintStudentsSetNotAvailableForm* form=
		 new ModifyConstraintStudentsSetNotAvailableForm((ConstraintStudentsSetNotAvailable*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_BREAK){
		ModifyConstraintBreakForm* form=
		 new ModifyConstraintBreakForm((ConstraintBreak*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherMaxDaysPerWeekForm* form=
		 new ModifyConstraintTeacherMaxDaysPerWeekForm((ConstraintTeacherMaxDaysPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersMaxHoursContinuouslyForm* form=
		 new ModifyConstraintTeachersMaxHoursContinuouslyForm((ConstraintTeachersMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
		ModifyConstraintTeachersMaxHoursDailyForm* form=
		 new ModifyConstraintTeachersMaxHoursDailyForm((ConstraintTeachersMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
		ModifyConstraintTeachersMinHoursDailyForm* form=
		 new ModifyConstraintTeachersMinHoursDailyForm((ConstraintTeachersMinHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_SUBGROUPS_MAX_HOURS_DAILY){
		ModifyConstraintTeachersSubgroupsMaxHoursDailyForm* form=
		 new ModifyConstraintTeachersSubgroupsMaxHoursDailyForm((ConstraintTeachersSubgroupsMaxHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
		ModifyConstraintActivityPreferredTimeForm* form=
		 new ModifyConstraintActivityPreferredTimeForm((ConstraintActivityPreferredTime*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
		ModifyConstraintStudentsSetNoGapsForm* form=
		 new ModifyConstraintStudentsSetNoGapsForm((ConstraintStudentsSetNoGaps*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_NO_GAPS){
		ModifyConstraintStudentsNoGapsForm* form=
		 new ModifyConstraintStudentsNoGapsForm((ConstraintStudentsNoGaps*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_NO_GAPS){
		ModifyConstraintTeachersNoGapsForm* form=
		 new ModifyConstraintTeachersNoGapsForm((ConstraintTeachersNoGaps*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_EARLY){
		ModifyConstraintStudentsEarlyForm* form=
		 new ModifyConstraintStudentsEarlyForm((ConstraintStudentsEarly*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm* form=
		 new ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm((ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherIntervalMaxDaysPerWeekForm* form=
		 new ModifyConstraintTeacherIntervalMaxDaysPerWeekForm((ConstraintTeacherIntervalMaxDaysPerWeek*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
		ModifyConstraintStudentsSetNHoursDailyForm* form=
		 new ModifyConstraintStudentsSetNHoursDailyForm((ConstraintStudentsSetNHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_N_HOURS_DAILY){
		ModifyConstraintStudentsNHoursDailyForm* form=
		 new ModifyConstraintStudentsNHoursDailyForm((ConstraintStudentsNHoursDaily*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_DAY){
		ModifyConstraintActivityEndsDayForm* form=
		 new ModifyConstraintActivityEndsDayForm((ConstraintActivityEndsDay*)ctr);
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
	else if(ctr->type==CONSTRAINT_TEACHERS_SUBJECT_TAGS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersSubjectTagsMaxHoursContinuouslyForm* form=
		 new ModifyConstraintTeachersSubjectTagsMaxHoursContinuouslyForm((ConstraintTeachersSubjectTagsMaxHoursContinuously*)ctr);
		form->exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm* form=
		 new ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm((ConstraintTeachersSubjectTagMaxHoursContinuously*)ctr);
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
