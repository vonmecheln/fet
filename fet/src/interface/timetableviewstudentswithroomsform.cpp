/***************************************************************************
                          timetableviewstudentswithroomsform.cpp  -  description
                             -------------------
    begin                : 5 Feb. 2005
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

#include "timetableviewstudentswithroomsform.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fetmainform.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3table.h>
#include <qapplication.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <q3table.h>
#include <q3textedit.h>
#include <qstring.h>

#include <QDesktopWidget>

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern GeneticTimetable gt;

extern bool simulation_running;

extern TimeChromosome best_time_chromosome;
extern SpaceChromosome best_space_chromosome;

TimetableViewStudentsWithRoomsForm::TimetableViewStudentsWithRoomsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	yearsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		yearsListBox->insertItem(sty->name);
	}
	yearChanged(yearsListBox->currentText());
}

TimetableViewStudentsWithRoomsForm::~TimetableViewStudentsWithRoomsForm()
{
}

void TimetableViewStudentsWithRoomsForm::yearChanged(const QString &yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		if(gt.rules.yearsList.size()>0)
			yearIndex=0;
		else
			return;
	}

	groupsListBox->clear();
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}

	groupChanged(groupsListBox->currentText());
}

void TimetableViewStudentsWithRoomsForm::groupChanged(const QString &groupName)
{
	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		if(gt.rules.yearsList.size()>0){
			yearIndex=0;
			yearName=gt.rules.yearsList.at(0)->name;
		}
		else
			return;
	}

	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	if(groupIndex<0){
		if(sty->groupsList.size()>0)
			groupIndex=0;		
		else
			return;
	}

	subgroupsListBox->clear();
	
	StudentsGroup* stg=sty->groupsList.at(groupIndex);
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListBox->insertItem(sts->name);
	}
}

void TimetableViewStudentsWithRoomsForm::subgroupChanged(const QString &subgroupName)
{
	if(subgroupName!="")
		;	

	updateStudentsTimetableTable();
}

void TimetableViewStudentsWithRoomsForm::updateStudentsTimetableTable(){
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QString s;
	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null)
		return;
	if(groupsListBox->currentText()==QString::null)
		return;
	if(subgroupsListBox->currentText()==QString::null)
		return;

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	s = QObject::tr("Students");
	s += ": ";
	s += yearname;
	s += ", ";
	s += groupname;
	s += ", ";
	s += subgroupname;

	classNameTextLabel->setText(s);

	assert(gt.rules.initialized);
	studentsTimetableTable->setNumRows(gt.rules.nHoursPerDay);
	studentsTimetableTable->setNumCols(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		//studentsTimetableTable->setText(0, j+1, gt.rules.daysOfTheWeek[j]);
		studentsTimetableTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		//studentsTimetableTable->setText(i+1, 0, gt.rules.hoursOfTheDay[i]);
		studentsTimetableTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);
		
	assert(rooms_schedule_ready);
	SpaceChromosome& c=best_space_chromosome;

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchStudentsSet(subgroupname);
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			s="";
			int ai=students_timetable_week1[i][k][j]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				s+=act->subjectName + " " + act->subjectTagName;
				
				s+="\n";
				int ri=c.rooms[ai]; //room index;
				QString roomName;
				if(ri==UNALLOCATED_SPACE)
					roomName="UNALLOCATED_SPACE";
				else
					roomName=gt.rules.internalRoomsList[ri]->name;
				s+=roomName;
			}
			ai=students_timetable_week2[i][k][j]; //activity index
			//act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				s += "\n/\n" + act->subjectName + " " + act->subjectTagName;

				s+="\n";
				int ri=c.rooms[ai]; //room index;
				QString roomName;
				if(ri==UNALLOCATED_SPACE)
					roomName="UNALLOCATED_SPACE";
				else
					roomName=gt.rules.internalRoomsList[ri]->name;
				s+=roomName;
			}
			studentsTimetableTable->setText(j, k, s);
		}
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		studentsTimetableTable->adjustRow(i); //added in version 3_9_16, on 16 Oct. 2004
}

void TimetableViewStudentsWithRoomsForm::detailActivity(int row, int col)
{
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QString s;
	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null)
		return;
	if(groupsListBox->currentText()==QString::null)
		return;
	if(subgroupsListBox->currentText()==QString::null)
		return;

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	assert(rooms_schedule_ready);
	SpaceChromosome& c=best_space_chromosome;

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchStudentsSet(subgroupname);
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);
	int j=row;
	int k=col;
	s="";
	if(j>=0 && k>=0){
		int ai=students_timetable_week1[i][k][j]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			s+=act->getDetailedDescriptionWithConstraints(gt.rules);

			int ri=c.rooms[ai]; //room index;
			QString roomName;
			if(ri==UNALLOCATED_SPACE)
				roomName="UNALLOCATED_SPACE";
			else
				roomName=gt.rules.internalRoomsList[ri]->name;
			s+=QObject::tr("Room=%1").arg(roomName);
			s+="\n";
		}
		ai=students_timetable_week2[i][k][j]; //activity index
		//act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			s+="/\n";
			s+=act->getDetailedDescriptionWithConstraints(gt.rules);

			int ri=c.rooms[ai]; //room index;
			QString roomName;
			if(ri==UNALLOCATED_SPACE)
				roomName="UNALLOCATED_SPACE";
			else
				roomName=gt.rules.internalRoomsList[ri]->name;
			s+=QObject::tr("Room=%1").arg(roomName);
			s+="\n";
		}
	}
	detailsTextEdit->setText(s);
}

void TimetableViewStudentsWithRoomsForm::lockTime()
{
	this->lock(true, false);
}

void TimetableViewStudentsWithRoomsForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewStudentsWithRoomsForm::lock()
{
	this->lock(true, true);
}

void TimetableViewStudentsWithRoomsForm::lock(bool lockTime, bool lockSpace)
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	//find subgroup index
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null)
		return;
	if(groupsListBox->currentText()==QString::null)
		return;
	if(subgroupsListBox->currentText()==QString::null)
		return;

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	assert(rooms_schedule_ready);
	SpaceChromosome* c=&best_space_chromosome;
	TimeChromosome* tc=&best_time_chromosome;

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchStudentsSet(subgroupname);
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);

	//lock selected activities
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(studentsTimetableTable->isSelected(j, k)){
				int ai=students_timetable_week1[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY){
					int time=tc->times[ai];
					int hour=time/gt.rules.nDaysPerWeek;
					int day=time%gt.rules.nDaysPerWeek;
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(lockTime){
						ConstraintActivityPreferredTime* ctr=new ConstraintActivityPreferredTime(1, true, act->id, day, hour);
						bool t=gt.rules.addTimeConstraint(ctr);
						if(t)
							QMessageBox::information(this, QObject::tr("FET information"), 
							 QObject::tr("Added the following constraint:\n"+ctr->getDetailedDescription(gt.rules)));
						else{
							QMessageBox::warning(this, QObject::tr("FET information"), 
							 QObject::tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules)));
							delete ctr;
						}
					}
					
					int ri=c->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && lockSpace){
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(1, true, act->id, (gt.rules.internalRoomsList[ri])->name);
						bool t=gt.rules.addSpaceConstraint(ctr);
						if(t)
							QMessageBox::information(this, QObject::tr("FET information"), 
							 QObject::tr("Added the following constraint:\n"+ctr->getDetailedDescription(gt.rules)));
						else{
							QMessageBox::warning(this, QObject::tr("FET information"), 
							 QObject::tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules)));
							delete ctr;
						}
					}
				}
				
				ai=students_timetable_week2[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY){
					int time=tc->times[ai];
					int hour=time/gt.rules.nDaysPerWeek;
					int day=time%gt.rules.nDaysPerWeek;
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(lockTime){
						ConstraintActivityPreferredTime* ctr=new ConstraintActivityPreferredTime(1, true, act->id, day, hour);
						bool t=gt.rules.addTimeConstraint(ctr);
						if(t)
							QMessageBox::information(this, QObject::tr("FET information"), 
							 QObject::tr("Added the following constraint:\n%1").arg(ctr->getDetailedDescription(gt.rules)));
						else{
							QMessageBox::warning(this, QObject::tr("FET information"), 
							 QObject::tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules)));
							delete ctr;
						}
					}
					
					int ri=c->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && lockSpace){
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(1, true, act->id, (gt.rules.internalRoomsList[ri])->name);
						bool t=gt.rules.addSpaceConstraint(ctr);
						if(t)
							QMessageBox::information(this, QObject::tr("FET information"), 
							 QObject::tr("Added the following constraint:\n%1").arg(ctr->getDetailedDescription(gt.rules)));
						else{
							QMessageBox::warning(this, QObject::tr("FET information"), 
							 QObject::tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules)));
							delete ctr;
						}
					}
				}
			}
		}
	}
}
