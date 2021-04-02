/***************************************************************************
                          allspaceconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include <iostream>

using namespace std;

#include "longtextmessagebox.h"

#include "allspaceconstraintsform.h"

#include "modifyconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintroomnotavailabletimesform.h"
#include "modifyconstraintactivitypreferredroomform.h"
#include "modifyconstraintactivitypreferredroomsform.h"

#include "modifyconstraintstudentssethomeroomform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include "modifyconstraintteacherhomeroomform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentsmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintteachermaxbuildingchangesperdayform.h"
#include "modifyconstraintteachersmaxbuildingchangesperdayform.h"
#include "modifyconstraintteachermaxbuildingchangesperweekform.h"
#include "modifyconstraintteachersmaxbuildingchangesperweekform.h"
#include "modifyconstraintteachermingapsbetweenbuildingchangesform.h"
#include "modifyconstraintteachersmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintsubjectpreferredroomform.h"
#include "modifyconstraintsubjectpreferredroomsform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomsform.h"

#include "modifyconstraintactivitytagpreferredroomform.h"
#include "modifyconstraintactivitytagpreferredroomsform.h"

#include <QDesktopWidget>

#include "lockunlock.h"

AllSpaceConstraintsForm::AllSpaceConstraintsForm()
{
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	this->filterChanged();
}

AllSpaceConstraintsForm::~AllSpaceConstraintsForm()
{
}

bool AllSpaceConstraintsForm::filterOk(SpaceConstraint* ctr)
{
	Q_UNUSED(ctr);
	//if(ctr!=NULL) //to avoid "unused parameter" warning
	//	;

	return true;
}

void AllSpaceConstraintsForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			QString s;
			s=ctr->getDescription(gt.rules);
			visibleConstraintsList << ctr; //append
			constraintsListBox->insertItem(s);
		}
	}

	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());
}

void AllSpaceConstraintsForm::constraintChanged(int index)
{
	if(index<0)
		return;
	QString s;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList[index];
	assert(ctr!=NULL);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void AllSpaceConstraintsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList[i];
	
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
		ModifyConstraintBasicCompulsorySpaceForm form((ConstraintBasicCompulsorySpace*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
		ModifyConstraintRoomNotAvailableTimesForm form((ConstraintRoomNotAvailableTimes*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		ModifyConstraintActivityPreferredRoomForm form((ConstraintActivityPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
		ModifyConstraintActivityPreferredRoomsForm form((ConstraintActivityPreferredRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
		ModifyConstraintStudentsSetHomeRoomForm form((ConstraintStudentsSetHomeRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ModifyConstraintStudentsSetHomeRoomsForm form((ConstraintStudentsSetHomeRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
		ModifyConstraintTeacherHomeRoomForm form((ConstraintTeacherHomeRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ModifyConstraintTeacherHomeRoomsForm form((ConstraintTeacherHomeRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
		ModifyConstraintSubjectPreferredRoomForm form((ConstraintSubjectPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
		ModifyConstraintSubjectPreferredRoomsForm form((ConstraintSubjectPreferredRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintSubjectActivityTagPreferredRoomForm form((ConstraintSubjectActivityTagPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintSubjectActivityTagPreferredRoomsForm form((ConstraintSubjectActivityTagPreferredRooms*)ctr);
		form.exec();
	}
	///6 apr 2009
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintActivityTagPreferredRoomForm form((ConstraintActivityTagPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintActivityTagPreferredRoomsForm form((ConstraintActivityTagPreferredRooms*)ctr);
		form.exec();
	}
	///
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm form((ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsMaxBuildingChangesPerDayForm form((ConstraintStudentsMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm form((ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsMaxBuildingChangesPerWeekForm form((ConstraintStudentsMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm form((ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsMinGapsBetweenBuildingChangesForm form((ConstraintStudentsMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}

	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeacherMaxBuildingChangesPerDayForm form((ConstraintTeacherMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeachersMaxBuildingChangesPerDayForm form((ConstraintTeachersMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeacherMaxBuildingChangesPerWeekForm form((ConstraintTeacherMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeachersMaxBuildingChangesPerWeekForm form((ConstraintTeachersMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeacherMinGapsBetweenBuildingChangesForm form((ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm form((ConstraintTeachersMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else{
		QMessageBox::critical(this, QObject::tr("FET critical"), QObject::tr("You have found a bug in FET. Please report it. This kind of constraint"
		 " is not correctly recongnized in all space constraints dialog. FET will skip this error, so that you can continue work."
		 ". Probably the constraint can be modified from the specific constraint dialog."));
//		assert(0);
//		exit(1);
	}

	filterChanged();

	constraintsListBox->setCurrentItem(i);
}

void AllSpaceConstraintsForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList[i];
	/*if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("You cannot erase the basic space constraints"));
		return;
	}*/
	QString s;
	s=QObject::tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	//s+=QObject::tr("\nAre you sure?");
	
	bool recompute;

	switch( LongTextMessageBox::confirmation( this, QObject::tr("FET confirmation"),
		s, QObject::tr("Yes"), QObject::tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			recompute=true;
		}
		else{
			recompute=false;
		}
	
		gt.rules.removeSpaceConstraint(ctr);
		
		if(recompute){
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
			LockUnlock::increaseCommunicationSpinBox();
		}
		
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i)>=constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
