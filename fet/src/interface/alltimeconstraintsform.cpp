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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "alltimeconstraintsform.h"

#include "modifyconstraintbreaktimesform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"

#include "modifyconstraintactivitypreferredtimeslotsform.h"
#include "modifyconstraintactivitypreferredstartingtimesform.h"
#include "modifyconstrainttwoactivitiesconsecutiveform.h"
#include "modifyconstrainttwoactivitiesgroupedform.h"
#include "modifyconstraintthreeactivitiesgroupedform.h"
#include "modifyconstrainttwoactivitiesorderedform.h"

#include "modifyconstraintactivitiespreferredtimeslotsform.h"
#include "modifyconstraintactivitiespreferredstartingtimesform.h"

#include "modifyconstraintsubactivitiespreferredtimeslotsform.h"
#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"

#include "modifyconstraintactivitiessamestartingtimeform.h"
#include "modifyconstraintactivitiessamestartinghourform.h"
#include "modifyconstraintactivitiessamestartingdayform.h"
#include "modifyconstraintactivitypreferredstartingtimeform.h"
#include "modifyconstraintactivitiesnotoverlappingform.h"
#include "modifyconstraintmindaysbetweenactivitiesform.h"
#include "modifyconstraintmaxdaysbetweenactivitiesform.h"
#include "modifyconstraintmingapsbetweenactivitiesform.h"
#include "modifyconstraintactivityendsstudentsdayform.h"

#include "modifyconstraintactivitiesendstudentsdayform.h"

#include "modifyconstraintteachernotavailabletimesform.h"
#include "modifyconstraintteachersmaxgapsperweekform.h"
#include "modifyconstraintteachermaxgapsperweekform.h"
#include "modifyconstraintteachersmaxgapsperdayform.h"
#include "modifyconstraintteachermaxgapsperdayform.h"
#include "modifyconstraintteachermaxdaysperweekform.h"
#include "modifyconstraintteachersmaxdaysperweekform.h"
#include "modifyconstraintteachersmaxhoursdailyform.h"
#include "modifyconstraintteachermaxhoursdailyform.h"
#include "modifyconstraintteachersmaxhourscontinuouslyform.h"
#include "modifyconstraintteachermaxhourscontinuouslyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteacherminhoursdailyform.h"

#include "modifyconstraintteachermindaysperweekform.h"
#include "modifyconstraintteachersmindaysperweekform.h"

#include "modifyconstraintteachersactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintteacheractivitytagmaxhourscontinuouslyform.h"

#include "modifyconstraintteachersactivitytagmaxhoursdailyform.h"
#include "modifyconstraintteacheractivitytagmaxhoursdailyform.h"

#include "modifyconstraintstudentssetnotavailabletimesform.h"
#include "modifyconstraintstudentssetmaxgapsperweekform.h"
#include "modifyconstraintstudentsmaxgapsperweekform.h"

#include "modifyconstraintstudentssetmaxgapsperdayform.h"
#include "modifyconstraintstudentsmaxgapsperdayform.h"

#include "modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "modifyconstraintstudentssetmaxhoursdailyform.h"
#include "modifyconstraintstudentsmaxhoursdailyform.h"
#include "modifyconstraintstudentssetmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentssetminhoursdailyform.h"
#include "modifyconstraintstudentsminhoursdailyform.h"

#include "modifyconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include "modifyconstraintstudentssetactivitytagmaxhoursdailyform.h"
#include "modifyconstraintstudentsactivitytagmaxhoursdailyform.h"

#include "modifyconstraintteacherintervalmaxdaysperweekform.h"
#include "modifyconstraintteachersintervalmaxdaysperweekform.h"

#include "modifyconstraintstudentssetintervalmaxdaysperweekform.h"
#include "modifyconstraintstudentsintervalmaxdaysperweekform.h"

#include "lockunlock.h"

#include "advancedfilterform.h"

#include <QRegExp>

#include <QTextEdit>
#include <QListWidget>
#include <QScrollBar>

#include <QAbstractItemView>

const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;


bool AllTimeConstraintsForm::filterInitialized=false;
bool AllTimeConstraintsForm::all=true;
QList<int> AllTimeConstraintsForm::descrDetDescr;
QList<int> AllTimeConstraintsForm::contains;
QStringList AllTimeConstraintsForm::text;
bool AllTimeConstraintsForm::caseSensitive=false;

AllTimeConstraintsForm::AllTimeConstraintsForm()
{
    setupUi(this);
    
    modifyConstraintPushButton->setDefault(true);
    
    constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this /*AllTimeConstraintsForm_template*/, SLOT(constraintChanged()));
    connect(closePushButton, SIGNAL(clicked()), this /*AllTimeConstraintsForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*AllTimeConstraintsForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*AllTimeConstraintsForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this /*AllTimeConstraintsForm_template*/, SLOT(modifyConstraint()));

   connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
 
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	//setWindowFlags(Qt::Window);
	
	if(!this->filterInitialized){
		all=true;
		descrDetDescr.clear();
		descrDetDescr.append(DESCRIPTION);
		contains.clear();
		contains.append(CONTAINS);
		text.clear();
		text.append(QString(""));
		caseSensitive=false;
		
		this->filterInitialized=true;
	}

	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);
	
	filterChanged();
}

AllTimeConstraintsForm::~AllTimeConstraintsForm()
{
}

bool AllTimeConstraintsForm::filterOk(TimeConstraint* ctr)
{
	if(!useFilter)
		return true;

	assert(descrDetDescr.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<bool> okPartial;
	
	for(int i=0; i<descrDetDescr.count(); i++){
		QString s;
		if(descrDetDescr.at(i)==DESCRIPTION)
			s=ctr->getDescription(gt.rules);
		else
			s=ctr->getDetailedDescription(gt.rules);
			
		QString t=text.at(i);
		if(contains.at(i)==CONTAINS){
			okPartial.append(s.contains(t, csens));
		}
		else if(contains.at(i)==DOESNOTCONTAIN){
			okPartial.append(!(s.contains(t, csens)));
		}
		else if(contains.at(i)==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)>=0);
		}
		else if(contains.at(i)==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)<0);
		}
		else
			assert(0);
	}
	
	if(all){
		bool ok=true;
		foreach(bool b, okPartial)
			ok = ok && b;
			
		return ok;
	}
	else{ //any
		bool ok=false;
		foreach(bool b, okPartial)
			ok = ok || b;
			
		return ok;
	}
}

void AllTimeConstraintsForm::filterChanged()
{
	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	foreach(TimeConstraint* ctr, gt.rules.timeConstraintsList)
		if(filterOk(ctr)){
			visibleTimeConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
		
	if(constraintsListWidget->count()<=0)
		currentConstraintTextEdit->setText("");
	else
		constraintsListWidget->setCurrentRow(0);
	
	constraintsTextLabel->setText(tr("%1 Time Constraints", "%1 represents the number of constraints").arg(visibleTimeConstraintsList.count()));
}

void AllTimeConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;
		
	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=NULL);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void AllTimeConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
	
	//1
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
		ModifyConstraintBasicCompulsoryTimeForm form((ConstraintBasicCompulsoryTime*)ctr);
		form.exec();
	}
	//2
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE){
		ModifyConstraintTwoActivitiesConsecutiveForm form((ConstraintTwoActivitiesConsecutive*)ctr);
		form.exec();
	}
	//3
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED){
		ModifyConstraintTwoActivitiesOrderedForm form((ConstraintTwoActivitiesOrdered*)ctr);
		form.exec();
	}
	//4
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
		ModifyConstraintActivityPreferredTimeSlotsForm form((ConstraintActivityPreferredTimeSlots*)ctr);
		form.exec();
	}
	//5
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
		ModifyConstraintActivityPreferredStartingTimesForm form((ConstraintActivityPreferredStartingTimes*)ctr);
		form.exec();
	}
	//6
	else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
		ModifyConstraintActivitiesPreferredTimeSlotsForm form((ConstraintActivitiesPreferredTimeSlots*)ctr);
		form.exec();
	}
	//7
	else if(ctr->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
		ModifyConstraintActivitiesPreferredStartingTimesForm form((ConstraintActivitiesPreferredStartingTimes*)ctr);
		form.exec();
	}
	//8
	else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
		ModifyConstraintSubactivitiesPreferredTimeSlotsForm form((ConstraintSubactivitiesPreferredTimeSlots*)ctr);
		form.exec();
	}
	//9
	else if(ctr->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
		ModifyConstraintSubactivitiesPreferredStartingTimesForm form((ConstraintSubactivitiesPreferredStartingTimes*)ctr);
		form.exec();
	}
	//10
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
		ModifyConstraintActivitiesSameStartingTimeForm form((ConstraintActivitiesSameStartingTime*)ctr);
		form.exec();
	}
	//11
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
		ModifyConstraintActivitiesSameStartingHourForm form((ConstraintActivitiesSameStartingHour*)ctr);
		form.exec();
	}
	//12
	else if(ctr->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
		ModifyConstraintActivitiesSameStartingDayForm form((ConstraintActivitiesSameStartingDay*)ctr);
		form.exec();
	}
	//13
	else if(ctr->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
		ModifyConstraintTeacherNotAvailableTimesForm form((ConstraintTeacherNotAvailableTimes*)ctr);
		form.exec();
	}
	//14
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
		ModifyConstraintStudentsSetNotAvailableTimesForm form((ConstraintStudentsSetNotAvailableTimes*)ctr);
		form.exec();
	}
	//15
	else if(ctr->type==CONSTRAINT_BREAK_TIMES){
		ModifyConstraintBreakTimesForm form((ConstraintBreakTimes*)ctr);
		form.exec();
	}
	//16
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherMaxDaysPerWeekForm form((ConstraintTeacherMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//17
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
		ModifyConstraintTeachersMaxHoursDailyForm form((ConstraintTeachersMaxHoursDaily*)ctr);
		form.exec();
	}
	//18
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
		ModifyConstraintTeacherMaxHoursDailyForm form((ConstraintTeacherMaxHoursDaily*)ctr);
		form.exec();
	}
	//19
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersMaxHoursContinuouslyForm form((ConstraintTeachersMaxHoursContinuously*)ctr);
		form.exec();
	}
	//20
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeacherMaxHoursContinuouslyForm form((ConstraintTeacherMaxHoursContinuously*)ctr);
		form.exec();
	}
	//21
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
		ModifyConstraintTeachersMinHoursDailyForm form((ConstraintTeachersMinHoursDaily*)ctr);
		form.exec();
	}
	//22
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
		ModifyConstraintTeacherMinHoursDailyForm form((ConstraintTeacherMinHoursDaily*)ctr);
		form.exec();
	}
	//23
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeachersMaxGapsPerWeekForm form((ConstraintTeachersMaxGapsPerWeek*)ctr);
		form.exec();
	}
	//24
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
		ModifyConstraintTeacherMaxGapsPerWeekForm form((ConstraintTeacherMaxGapsPerWeek*)ctr);
		form.exec();
	}
	//25
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
		ModifyConstraintTeachersMaxGapsPerDayForm form((ConstraintTeachersMaxGapsPerDay*)ctr);
		form.exec();
	}
	//26
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
		ModifyConstraintTeacherMaxGapsPerDayForm form((ConstraintTeacherMaxGapsPerDay*)ctr);
		form.exec();
	}
	//27
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
		ModifyConstraintActivityPreferredStartingTimeForm form((ConstraintActivityPreferredStartingTime*)ctr);
		form.exec();
	}
	//28
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsSetMaxGapsPerWeekForm form((ConstraintStudentsSetMaxGapsPerWeek*)ctr);
		form.exec();
	}
	//29
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK){
		ModifyConstraintStudentsMaxGapsPerWeekForm form((ConstraintStudentsMaxGapsPerWeek*)ctr);
		form.exec();
	}
	//30
	else if(ctr->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm form((ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)ctr);
		form.exec();
	}
	//31
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
		ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm form((ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr);
		form.exec();
	}
	//32
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
		ModifyConstraintStudentsSetMaxHoursDailyForm form((ConstraintStudentsSetMaxHoursDaily*)ctr);
		form.exec();
	}
	//33
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
		ModifyConstraintStudentsMaxHoursDailyForm form((ConstraintStudentsMaxHoursDaily*)ctr);
		form.exec();
	}
	//34
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsSetMaxHoursContinuouslyForm form((ConstraintStudentsSetMaxHoursContinuously*)ctr);
		form.exec();
	}
	//35
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsMaxHoursContinuouslyForm form((ConstraintStudentsMaxHoursContinuously*)ctr);
		form.exec();
	}
	//36
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
		ModifyConstraintStudentsSetMinHoursDailyForm form((ConstraintStudentsSetMinHoursDaily*)ctr);
		form.exec();
	}
	//37
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
		ModifyConstraintStudentsMinHoursDailyForm form((ConstraintStudentsMinHoursDaily*)ctr);
		form.exec();
	}
	//38
	else if(ctr->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
		ModifyConstraintActivitiesNotOverlappingForm form((ConstraintActivitiesNotOverlapping*)ctr);
		form.exec();
	}
	//39
	else if(ctr->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
		ModifyConstraintMinDaysBetweenActivitiesForm form((ConstraintMinDaysBetweenActivities*)ctr);
		form.exec();
	}
	//40
	else if(ctr->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES){
		ModifyConstraintMinGapsBetweenActivitiesForm form((ConstraintMinGapsBetweenActivities*)ctr);
		form.exec();
	}
	//41
	else if(ctr->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
		ModifyConstraintActivityEndsStudentsDayForm form((ConstraintActivityEndsStudentsDay*)ctr);
		form.exec();
	}
	//42
	else if(ctr->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeacherIntervalMaxDaysPerWeekForm form((ConstraintTeacherIntervalMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//43
	else if(ctr->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeachersIntervalMaxDaysPerWeekForm form((ConstraintTeachersIntervalMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//44
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm form((ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//45
	else if(ctr->type==CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK){
		ModifyConstraintStudentsIntervalMaxDaysPerWeekForm form((ConstraintStudentsIntervalMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//46
	else if(ctr->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
		ModifyConstraintActivitiesEndStudentsDayForm form((ConstraintActivitiesEndStudentsDay*)ctr);
		form.exec();
	}
	//47
	else if(ctr->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED){
		ModifyConstraintTwoActivitiesGroupedForm form((ConstraintTwoActivitiesGrouped*)ctr);
		form.exec();
	}
	//48
	else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm form((ConstraintTeachersActivityTagMaxHoursContinuously*)ctr);
		form.exec();
	}
	//49
	else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintTeacherActivityTagMaxHoursContinuouslyForm form((ConstraintTeacherActivityTagMaxHoursContinuously*)ctr);
		form.exec();
	}
	//50
	else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm form((ConstraintStudentsActivityTagMaxHoursContinuously*)ctr);
		form.exec();
	}
	//51
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm form((ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr);
		form.exec();
	}
	//52
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK){
		ModifyConstraintTeachersMaxDaysPerWeekForm form((ConstraintTeachersMaxDaysPerWeek*)ctr);
		form.exec();
	}
	//53
	else if(ctr->type==CONSTRAINT_THREE_ACTIVITIES_GROUPED){
		ModifyConstraintThreeActivitiesGroupedForm form((ConstraintThreeActivitiesGrouped*)ctr);
		form.exec();
	}
	//54
	else if(ctr->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES){
		ModifyConstraintMaxDaysBetweenActivitiesForm form((ConstraintMaxDaysBetweenActivities*)ctr);
		form.exec();
	}
	//55
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK){
		ModifyConstraintTeachersMinDaysPerWeekForm form((ConstraintTeachersMinDaysPerWeek*)ctr);
		form.exec();
	}
	//56
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
		ModifyConstraintTeacherMinDaysPerWeekForm form((ConstraintTeacherMinDaysPerWeek*)ctr);
		form.exec();
	}
	//57
	else if(ctr->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintTeachersActivityTagMaxHoursDailyForm form((ConstraintTeachersActivityTagMaxHoursDaily*)ctr);
		form.exec();
	}
	//58
	else if(ctr->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintTeacherActivityTagMaxHoursDailyForm form((ConstraintTeacherActivityTagMaxHoursDaily*)ctr);
		form.exec();
	}
	//59
	else if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintStudentsActivityTagMaxHoursDailyForm form((ConstraintStudentsActivityTagMaxHoursDaily*)ctr);
		form.exec();
	}
	//60
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
		ModifyConstraintStudentsSetActivityTagMaxHoursDailyForm form((ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr);
		form.exec();
	}

	//61
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
		ModifyConstraintStudentsSetMaxGapsPerDayForm form((ConstraintStudentsSetMaxGapsPerDay*)ctr);
		form.exec();
	}
	//62
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY){
		ModifyConstraintStudentsMaxGapsPerDayForm form((ConstraintStudentsMaxGapsPerDay*)ctr);
		form.exec();
	}

	else{
		QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
		 " is not correctly recognized in all time constraints dialog. FET will skip this error, so that you can continue work."
		 ". Probably the constraint can be modified from the specific constraint dialog."));
		//assert(0);
		//exit(1);
	}
	
	filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
}

void AllTimeConstraintsForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	bool t;
	
	bool recompute;
	
	QListWidgetItem* item;
	
	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		// The user clicked the OK again button or pressed Enter
		
		QMessageBox::StandardButton wr=QMessageBox::Yes;
		
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){ //additional confirmation for this one
			QString s=tr("Do you really want to remove the basic compulsory time constraint?");
			s+=" ";
			s+=tr("You cannot generate a timetable without this constraint.");
			s+="\n\n";
			s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
				"Miscellaneous -> Basic compulsory time constraints.");
				
			wr=QMessageBox::warning(this, tr("FET warning"), s,
				QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		}
		
		if(wr==QMessageBox::Yes){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				recompute=true;
			}
			else{
				recompute=false;
			}
		
			t=gt.rules.removeTimeConstraint(ctr);
			assert(t);
			visibleTimeConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			item=constraintsListWidget->takeItem(i);
			delete item;
		
			constraintsTextLabel->setText(tr("%1 Time Constraints", "%1 represents the number of constraints").arg(visibleTimeConstraintsList.count()));
		
			if(recompute){
				LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
				LockUnlock::increaseCommunicationSpinBox();
			}
		}
	}
	//else if(lres==1){
		// The user clicked the Cancel or pressed Escape
	//}

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		currentConstraintTextEdit->setText(QString(""));
}

void AllTimeConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(all, descrDetDescr, contains, text, caseSensitive);

	//centerWidgetOnScreen(filterForm);

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
			
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescr.append(cb1->currentItem());
			contains.append(cb2->currentItem());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
	}
	
	delete filterForm;
}
