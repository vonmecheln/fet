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

#include "fetmainform.h"
#include "timetableviewroomsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "lockunlock.h"

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

/*#include <iostream>
#include <fstream>
using namespace std;*/

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern bool simulation_running;

extern Solution best_solution;
extern SpaceChromosome best_space_chromosome;

extern double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern QSet <int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet <int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet <int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet <int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern QSpinBox* pcommunicationSpinBox;	//small hint to sync the forms

/////////////TODO
/*
#include <sys/time.h>

#include <iostream>
using namespace std;

double get_time(){
   struct timeval t;
   gettimeofday(&t, NULL);
   double d = t.tv_sec + (double) t.tv_usec/1000000;
   return d;
}*/
/////////////

TimetableViewRoomsForm::TimetableViewRoomsForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

//////////TODO
/*    double time_start = get_time();
*/
//////////

/////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//these 2 lines are not really needed - we just keep them to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	LockUnlock::increaseCommunicationSpinBox();

//////////TODO
/*
    double time_end = get_time();
    cout<<"This program has run for: "<<time_end-time_start<<" seconds."<<endl;
*/
//////////

	roomsListBox->clear();
	for(int i=0; i<gt.rules.nInternalRooms; i++)
		roomsListBox->insertItem(gt.rules.internalRoomsList[i]->name);
	roomChanged(roomsListBox->currentText());

	//added by Volker Dirr
	connect(pcommunicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateRoomsTimetableTable()));
}

TimetableViewRoomsForm::~TimetableViewRoomsForm()
{
}

void TimetableViewRoomsForm::roomChanged(const QString &roomName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	if(roomName==QString::null){
		return;
	}

	int roomIndex=gt.rules.searchRoom(roomName);
	if(roomIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms timetable dialog"));
		return;
	}

	updateRoomsTimetableTable();
}

void TimetableViewRoomsForm::updateRoomsTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable "
		"or close the timetable view rooms dialog"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable "
		"or close the timetable view rooms dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	QString roomName;

	if(roomsListBox->currentItem() < 0)
		return;

	roomName = roomsListBox->currentText();
	s = roomName;
	roomNameTextLabel->setText(s);
	
	if(gt.rules.searchRoom(roomName)<0){
		QMessageBox::warning(this, tr("FET warning"), tr("You have an old timetable view rooms dialog opened - please close it"));
		return;
	}

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
				
				QString ats="";
				foreach(QString crt, act->activityTagsNames)
					ats+=" "+crt;
				
				s += act->subjectName + ats; //+ " " + act->activityTagName;
				//added by Volker Dirr (start)
				QString descr="";
				QString t="";
				if(idsOfPermanentlyLockedTime.contains(act->id)){
					descr+=tr("permanently locked time");
					t=", ";
				}
				else if(idsOfLockedTime.contains(act->id)){
					descr+=tr("locked time");
					t=", ";
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id)){
					descr+=t+tr("permanently locked space");
				}
				else if(idsOfLockedSpace.contains(act->id)){
					descr+=t+tr("locked space");
				}
				if(descr!=""){
					descr.prepend("\n(");
					descr.append(")");
				}
				s+=descr;
				//added by Volker Dirr (end)
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
	//cout<<"timetableviewroomsform updated form."<<endl;
	
	detailActivity(roomsTimetableTable->currentRow(), roomsTimetableTable->currentColumn());
}

void TimetableViewRoomsForm::detailActivity(int row, int col){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
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

	if(roomIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms dialog"));
		return;
	}

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
			//added by Volker Dirr (start)
			QString descr="";
			QString t="";
			if(idsOfPermanentlyLockedTime.contains(act->id)){
				descr+=tr("permanently locked time");
				t=", ";
			}
			else if(idsOfLockedTime.contains(act->id)){
				descr+=tr("locked time");
				t=", ";
			}
			if(idsOfPermanentlyLockedSpace.contains(act->id)){
				descr+=t+tr("permanently locked space");
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=t+tr("locked space");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
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
	//cout<<"rooms begin: internalStructureComputed=="<<gt.rules.internalStructureComputed<<endl;

	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	//find room index
	QString roomName;
	if(roomsListBox->currentText()==QString::null){
		QMessageBox::information(this, tr("FET information"), tr("Please select a room"));
		return;
	}
	roomName = roomsListBox->currentText();
	int i=gt.rules.searchRoom(roomName);

	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(rooms_schedule_ready);
	//SpaceChromosome* c=&best_space_chromosome;
	
	if(i<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms timetable dialog"));
		return;
	}
	
	Solution* c=&best_solution;
	
	bool report=true;
	
	int added=0, unlocked=0;

	//lock selected activities
	QSet <int> careAboutIndex;		//added by Volker Dirr. Needed, because of activities with duration > 1
	careAboutIndex.clear();
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(roomsTimetableTable->isSelected(j, k)){
				int ai=rooms_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY && !careAboutIndex.contains(ai)){	//modified, because of activities with duration > 1
					careAboutIndex.insert(ai);					//Needed, because of activities with duration > 1
					int a_tim=c->times[ai];
					int hour=a_tim/gt.rules.nDaysPerWeek;
					int day=a_tim%gt.rules.nDaysPerWeek;

					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(lockTime){
						ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false);
						bool t=gt.rules.addTimeConstraint(ctr);
						QString s;
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							added++;
							idsOfLockedTime.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<TimeConstraint*> tmptc;
							tmptc.clear();
							int count=0;
							foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
								if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
									ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) tc;
									if(c->activityId==act->id && tc->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
										count++;
										if(c->permanentlyLocked){
											if(idsOfLockedTime.contains(c->activityId) || !idsOfPermanentlyLockedTime.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												 );
											}
											else{
												s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
											}
										}
										else{
											if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												 );
											}
											else
												tmptc.append(tc);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expects to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

							foreach(TimeConstraint* deltc, tmptc){
								s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeTimeConstraint(deltc);
								idsOfLockedTime.remove(act->id);
								unlocked++;
								//delete deltc; - done by rules.remove...
							}
							tmptc.clear();
							//gt.rules.internalStructureComputed=false;
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							int k;
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
		 					if(k==0)
								report=false;
						}
					}
					
					int ri=c->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100.0, act->id, (gt.rules.internalRoomsList[ri])->name, false);
						bool t=gt.rules.addSpaceConstraint(ctr);
						
						QString s;
						
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							added++;
							idsOfLockedSpace.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<SpaceConstraint*> tmpsc;
							tmpsc.clear();
							int count=0;
							foreach(SpaceConstraint* sc, gt.rules.spaceConstraintsList){
								if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
									ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) sc;
									if(c->activityId==act->id && sc->weightPercentage==100.0){
										count++;
										if(c->permanentlyLocked){
											if(idsOfLockedSpace.contains(c->activityId) || !idsOfPermanentlyLockedSpace.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												 );
											}
											else{
												s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
											}
										} else {
											if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												 );
											}
											else
												tmpsc.append(sc);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expects to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

							foreach(SpaceConstraint* delsc, tmpsc){
								s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeSpaceConstraint(delsc);
								idsOfLockedSpace.remove(act->id);
								unlocked++;
								//delete delsc; - done by rules.removeSpa...
							}
							tmpsc.clear();
							//gt.rules.internalStructureComputed=false;
						}  //modified by Volker Dirr, so you can also unlock (end)

						if(report){
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
								
		 					if(k==0)
								report=false;
						}
					}
				}
			}
		}
	}
	
	QMessageBox::information(this, tr("FET information"), tr("Added %1 locking constraints and deleted %2 locking constraints").arg(added).arg(unlocked));

////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //not really needed, just to test
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
/////////

	LockUnlock::increaseCommunicationSpinBox(); //this is needed
	
	//cout<<"rooms end: internalStructureComputed=="<<gt.rules.internalStructureComputed<<endl;
	//cout<<endl;
}
