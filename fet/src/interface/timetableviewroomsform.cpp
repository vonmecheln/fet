/***************************************************************************
                          timetableviewroomsform.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include "timetableviewroomsform.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

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
#include <q3textedit.h>
#include <qstring.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <q3table.h>

#include <QDesktopWidget>

#include <QMessageBox>

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern bool simulation_running;

extern Solution best_solution;
extern SpaceChromosome best_space_chromosome;

extern double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

TimetableViewRoomsForm::TimetableViewRoomsForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	roomsListBox->clear();
	for(int i=0; i<gt.rules.nInternalRooms; i++)
		roomsListBox->insertItem(gt.rules.internalRoomsList[i]->name);
	roomChanged(roomsListBox->currentText());
}

TimetableViewRoomsForm::~TimetableViewRoomsForm()
{
}

void TimetableViewRoomsForm::roomChanged(const QString &roomName)
{
	if(roomName==QString::null)
		return;

	int roomIndex=gt.rules.searchRoom(roomName);
	if(roomIndex<0)
		return;

	updateRoomsTimetableTable();
}

void TimetableViewRoomsForm::updateRoomsTimetableTable(){
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	QString roomName;

	if(roomsListBox->currentItem() < 0)
		return;

	roomName = roomsListBox->currentText();
	s = roomName;
	roomNameTextLabel->setText(s);

	assert(gt.rules.initialized);
	roomsTimetableTable->setNumRows(gt.rules.nHoursPerDay);
	roomsTimetableTable->setNumCols(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		//roomsTimetableTable->setText(0, j+1, gt.rules.daysOfTheWeek[j]);
		roomsTimetableTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		//roomsTimetableTable->setText(i+1, 0, gt.rules.hoursOfTheDay[i]);
		roomsTimetableTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);

	int roomIndex=gt.rules.searchRoom(roomName);
	assert(roomIndex>=0);
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			s = "";
			int ai=rooms_timetable_weekly[roomIndex][k][j]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				s += act->subjectName + " " + act->activityTagName;
			}
			else{
				if((notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]>=0 || breakDayHour[k][j])
				 && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					s+="-x-";
				/*if(!breakDayHour[k][j] && notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]>=0 && notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]<100.0){
					s+="\n";
					s+=QString::number(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]);
					s+="%";
				}*/
			}
			roomsTimetableTable->setText(j, k, s);
		}
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		roomsTimetableTable->adjustRow(i); //added in version 3_12_20
}

void TimetableViewRoomsForm::detailActivity(int row, int col){
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	QString roomName;

	if(roomsListBox->currentText()==QString::null)
		return;

	roomName = roomsListBox->currentText();
	s = roomName;
	roomNameTextLabel->setText(s);

	int roomIndex=gt.rules.searchRoom(roomName);
	assert(roomIndex>=0);
	int j=row;
	int k=col;
	s = "";
	if(j>=0 && k>=0){
		int ai=rooms_timetable_weekly[roomIndex][k][j]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			s += act->getDetailedDescriptionWithConstraints(gt.rules);
		}
		else{
			if(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]>=0){
				s+=tr("Room is not available with weight %1%").arg(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]);
				s+="\n";
			}
			if(breakDayHour[k][j]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setText(s);
}

void TimetableViewRoomsForm::lock()
{
	this->lock(true, true);
}

void TimetableViewRoomsForm::lockTime()
{
	this->lock(true, false);
}

void TimetableViewRoomsForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewRoomsForm::lock(bool lockTime, bool lockSpace)
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	//find room index
	QString roomName;
	if(roomsListBox->currentText()==QString::null)
		return;
	roomName = roomsListBox->currentText();
	int i=gt.rules.searchRoom(roomName);

	assert(rooms_schedule_ready);
	//SpaceChromosome* c=&best_space_chromosome;
	Solution* c=&best_solution;
	
	bool report=true;
	
	int added=0, duplicates=0;

	//lock selected activities
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(roomsTimetableTable->isSelected(j, k)){
				int ai=rooms_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY){
					int time=c->times[ai];
					int hour=time/gt.rules.nDaysPerWeek;
					int day=time%gt.rules.nDaysPerWeek;

					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(lockTime){
						ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour);
						bool t=gt.rules.addTimeConstraint(ctr);
						
						if(t)
							added++;
						else
							duplicates++;
						
						QString s;
						
						if(t)
							s=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						else{
							s=tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules));
							delete ctr;
						}
						
						if(report){
							int k;
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::warning(this, tr("FET warning"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
		 					if(k==0)
								report=false;
						}
					}
					
					int ri=c->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && lockSpace){
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100.0, act->id, (gt.rules.internalRoomsList[ri])->name);
						bool t=gt.rules.addSpaceConstraint(ctr);
						
						if(t)
							added++;
						else
							duplicates++;

						QString s;
						
						if(t)
							s=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						else{
							s=tr("Constraint\n%1 NOT added - duplicate").arg(ctr->getDetailedDescription(gt.rules));
							delete ctr;
						}

						if(report){
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::warning(this, tr("FET warning"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
		 					if(k==0)
								report=false;
						}
					}
				}
			}
		}
	}
	
	QMessageBox::information(this, tr("FET information"), tr("Added %1 locking constraints, ignored %2 duplicates").arg(added).arg(duplicates));
}
