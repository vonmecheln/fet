/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          advancedlockunlockform.cpp  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (http://lalescu.ro/liviu/) and Volker Dirr (http://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   FET program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
#include <iostream>
using namespace std;*/

#include <QtGui>

#include <QMessageBox>

//extern CommunicationSpinBox communicationSpinBox;

#include "lockunlock.h"
#include "advancedlockunlockform.h"
#include "fetmainform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;

extern Timetable gt;

extern QSet<int> idsOfLockedTime;
extern QSet<int> idsOfLockedSpace;
extern QSet<int> idsOfPermanentlyLockedTime;
extern QSet<int> idsOfPermanentlyLockedSpace;

/*#include <iostream>
using namespace std;*/

/*
LockUnlockToggleDay::LockUnlockToggleDay()
{
}

LockUnlockToggleDay::~LockUnlockToggleDay()
{
}*/

const int MIN_WIDTH=400;
const int MIN_HEIGHT=200;

/*void LockUnlock::computeLockedUnlockedActivitiesTimeSpace()
{
	//by Volker Dirr
	idsOfLockedTime.clear();
	idsOfLockedSpace.clear();
	idsOfPermanentlyLockedTime.clear();
	idsOfPermanentlyLockedSpace.clear();

	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME && tc->weightPercentage==100.0){
			ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) tc;
			if(c->day >= 0  &&  c->hour >= 0) {
				if(c->permanentlyLocked)
					idsOfPermanentlyLockedTime.insert(c->activityId);
				else
					idsOfLockedTime.insert(c->activityId);
			}
		}
	}
	
	foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM && tc->weightPercentage==100.0){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) tc;

			if(c->permanentlyLocked)
				idsOfPermanentlyLockedSpace.insert(c->activityId);
			else
				idsOfLockedSpace.insert(c->activityId);
		}
	}
}

void LockUnlock::computeLockedUnlockedActivitiesOnlyTime()
{
	//by Volker Dirr
	idsOfLockedTime.clear();
	idsOfPermanentlyLockedTime.clear();

	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME && tc->weightPercentage==100.0){
			ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) tc;
			if(c->day >= 0  &&  c->hour >= 0) {
				if(c->permanentlyLocked)
					idsOfPermanentlyLockedTime.insert(c->activityId);
				else
					idsOfLockedTime.insert(c->activityId);
			}
		}
	}
}

void LockUnlock::computeLockedUnlockedActivitiesOnlySpace()
{
	//by Volker Dirr
	idsOfLockedSpace.clear();
	idsOfPermanentlyLockedSpace.clear();

	foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM && tc->weightPercentage==100.0){
			ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) tc;

			if(c->permanentlyLocked)
				idsOfPermanentlyLockedSpace.insert(c->activityId);
			else
				idsOfLockedSpace.insert(c->activityId);
		}
	}
}

void LockUnlock::increaseCommunicationSpinBox()
{
	assert(pcommunicationSpinBox!=NULL);
	
	int q=pcommunicationSpinBox->value();	//needed to display locked and unlocked times and rooms
	//cout<<"communication spin box old value: "<<pcommunicationSpinBox->value()<<", ";
	q++;
	assert(pcommunicationSpinBox->maximum()>pcommunicationSpinBox->minimum());
	if(q > pcommunicationSpinBox->maximum())
		q=pcommunicationSpinBox->minimum();
	pcommunicationSpinBox->setValue(q);
	//cout<<"changed to new value: "<<pcommunicationSpinBox->value()<<endl;
}*/

void AdvancedLockUnlockForm::lockDay()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	QStringList days;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		days<<gt.rules.daysOfTheWeek[j];
	assert(days.size()!=0);

	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Lock activities of a day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the selected day will be locked")+"\n\n"+tr("Please select the day to lock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp=taLW->minimumSizeHint();
	Q_UNUSED(tmp);
	
	taLW->addItems(days);
	taLW->setCurrentItem(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Lock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	int selectedDayInt=taLW->currentIndex();
	assert(selectedDayInt>=0 && selectedDayInt<gt.rules.nDaysPerWeek);

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=NULL;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=NULL;
			
			if(d==selectedDayInt && lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(d==selectedDayInt && lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, false);
				}
			}
			
			if(newTimeCtr!=NULL){
				bool add=true;
				foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
						if((*((ConstraintActivityPreferredStartingTime*)tc)) == (*newTimeCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
	 				
	 				addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=NULL){
				bool add=true;
				foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
						if((*((ConstraintActivityPreferredRoom*)tc)) == (*newSpaceCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, addedTimeConstraints){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	foreach(TimeConstraint* tc, notAddedTimeConstraints){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	foreach(SpaceConstraint* sc, addedSpaceConstraints){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	foreach(SpaceConstraint* sc, notAddedSpaceConstraints){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(NULL, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::unlockDay()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	QStringList days;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		days<<gt.rules.daysOfTheWeek[j];
	assert(days.size()!=0);

	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Unlock activities of a day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the selected day will be unlocked (those which are not permanently locked)")+"\n\n"+tr("Please select the day to unlock:"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QComboBox* taLW=new QComboBox();
	
	QSize tmp2=taLW->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	taLW->addItems(days);
	taLW->setCurrentItem(0);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Unlock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addWidget(taLW);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int selectedDayInt=taLW->currentIndex();
	assert(selectedDayInt>=0 && selectedDayInt<gt.rules.nDaysPerWeek);

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QSet<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			//int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			if(d==selectedDayInt){
				lockedActivitiesIds.insert(gt.rules.internalActivitiesList[i].id);
			}
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<ConstraintActivityPreferredStartingTime*> removedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<ConstraintActivityPreferredRoom*> removedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0 && lockedActivitiesIds.contains(c->activityId)){
					if(c->day!=selectedDayInt){
						//QMessageBox::warning(NULL, tr("FET warning"), tr("Incorrect data - time constraint is incorrect - please regenerate the timetable. Please report possible bug."));
						//above test is no good???
					}
					//assert(c->day==selectedDayInt);
					
					if(!c->permanentlyLocked){
						removedTimeConstraints.append(c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append(c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)tc;
				if(c->weightPercentage==100.0 && lockedActivitiesIds.contains(c->activityId)){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append(c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append(c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, removedTimeConstraints)
		gt.rules.removeTimeConstraint(tc);
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	foreach(SpaceConstraint* sc, removedSpaceConstraints)
		gt.rules.removeSpaceConstraint(sc);
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(NULL, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::lockEndStudentsDay()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Lock all activities which end each students set's day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which end each students set's day will be locked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Lock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;
	
	QList<int> activitiesIdsList;
	QList<int> activitiesIndexList;
	QSet<int> activitiesIdsSet;
	for(int sg=0; sg<gt.rules.nInternalSubgroups; sg++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=gt.rules.nHoursPerDay-1; h>=0; h--){
				int ai=students_timetable_weekly[sg][d][h];
				
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(!activitiesIdsSet.contains(act->id)){
						activitiesIdsList.append(act->id);
						activitiesIdsSet.insert(act->id);
						activitiesIndexList.append(ai);
					}
				
					break;
				}
			}
		}
	}

	assert(activitiesIdsList.count()==activitiesIndexList.count());
	for(int q=0; q<activitiesIdsList.count(); q++){
		int id=activitiesIdsList.at(q);
		int ai=activitiesIndexList.at(q);
		assert(gt.rules.internalActivitiesList[ai].id==id);

		assert(best_solution.times[ai]!=UNALLOCATED_TIME);
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			assert(best_solution.times[ai]>=0 && best_solution.times[ai]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=NULL;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=NULL;
			
			if(lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, id, d, h, false);
			}
			
			if(lockSpace){
				if(best_solution.rooms[ai]!=UNALLOCATED_SPACE && best_solution.rooms[ai]!=UNSPECIFIED_ROOM){
					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, id, gt.rules.internalRoomsList[best_solution.rooms[ai]]->name, false);
				}
			}
			
			if(newTimeCtr!=NULL){
				bool add=true;
				foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
						if((*((ConstraintActivityPreferredStartingTime*)tc)) == (*newTimeCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
	 				
	 				addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=NULL){
				bool add=true;
				foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
						if((*((ConstraintActivityPreferredRoom*)tc)) == (*newSpaceCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, addedTimeConstraints){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	foreach(TimeConstraint* tc, notAddedTimeConstraints){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	foreach(SpaceConstraint* sc, addedSpaceConstraints){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	foreach(SpaceConstraint* sc, notAddedSpaceConstraints){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(NULL, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::unlockEndStudentsDay()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Unlock all activities which end each students set's day"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities which end each students set's day will be unlocked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Unlock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	

	QSet<int> activitiesIdsSet;
	for(int sg=0; sg<gt.rules.nInternalSubgroups; sg++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=gt.rules.nHoursPerDay-1; h>=0; h--){
				int ai=students_timetable_weekly[sg][d][h];
				
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(!activitiesIdsSet.contains(act->id)){
						activitiesIdsSet.insert(act->id);
					}
				
					break;
				}
			}
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<ConstraintActivityPreferredStartingTime*> removedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<ConstraintActivityPreferredRoom*> removedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0 && activitiesIdsSet.contains(c->activityId)){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append(c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append(c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)tc;
				if(c->weightPercentage==100.0 && activitiesIdsSet.contains(c->activityId)){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append(c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append(c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, removedTimeConstraints)
		gt.rules.removeTimeConstraint(tc);
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	foreach(SpaceConstraint* sc, removedSpaceConstraints)
		gt.rules.removeSpaceConstraint(sc);
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(NULL, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
}

void AdvancedLockUnlockForm::lockAll()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Lock all activities in the timetable"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities in the current timetable will be locked"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Lock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool lockTime=timeCheckBox->isChecked();
	bool lockSpace=spaceCheckBox->isChecked();

	QString addedTimeConstraintsString;
	QString notAddedTimeConstraintsString;
	
	QList<ConstraintActivityPreferredStartingTime*> addedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notAddedTimeConstraints;
	
	QString addedSpaceConstraintsString;
	QString notAddedSpaceConstraintsString;
	
	QList<ConstraintActivityPreferredRoom*> addedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notAddedSpaceConstraints;
	
	int addedTime=0, notAddedTime=0;
	int addedSpace=0, notAddedSpace=0;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			
			ConstraintActivityPreferredStartingTime* newTimeCtr=NULL;
			
			ConstraintActivityPreferredRoom* newSpaceCtr=NULL;
			
			if(lockTime){
				newTimeCtr=new ConstraintActivityPreferredStartingTime(100.0, gt.rules.internalActivitiesList[i].id, d, h, false);
			}
			
			if(lockSpace){
				if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
					newSpaceCtr=new ConstraintActivityPreferredRoom(100.0, gt.rules.internalActivitiesList[i].id, gt.rules.internalRoomsList[best_solution.rooms[i]]->name, false);
				}
			}
			
			if(newTimeCtr!=NULL){
				bool add=true;
				foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
						if((*((ConstraintActivityPreferredStartingTime*)tc)) == (*newTimeCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					addedTimeConstraints.append(newTimeCtr);
	 				
	 				addedTime++;
				}
				else{
					notAddedTimeConstraintsString+=newTimeCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedTimeConstraints.append(newTimeCtr);
					
					notAddedTime++;
				}
			}

			if(newSpaceCtr!=NULL){
				bool add=true;
				foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
					if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
						if((*((ConstraintActivityPreferredRoom*)tc)) == (*newSpaceCtr)){
							add=false;
							break;
						}
					}
				}
				
				if(add){
					addedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					addedSpaceConstraints.append(newSpaceCtr);
	 				
	 				addedSpace++;
				}
				else{
					notAddedSpaceConstraintsString+=newSpaceCtr->getDetailedDescription(gt.rules)+"\n";
					notAddedSpaceConstraints.append(newSpaceCtr);
					
					notAddedSpace++;
				}
			}
		}
	}
	
	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* addTim=new QTextEdit();
	addTim->setReadOnly(true);
	addTim->setText(addedTimeConstraintsString);
	QLabel* labAddTim=new QLabel(tr("These time constraints will be added"));
	labAddTim->setWordWrap(true);
	labAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labAddTim);
	left->addWidget(addTim);

	QTextEdit* notAddTim=new QTextEdit();
	notAddTim->setReadOnly(true);
	notAddTim->setText(notAddedTimeConstraintsString);
	QLabel* labNotAddTim=new QLabel(tr("These time constraints will NOT be added"));
	labNotAddTim->setWordWrap(true);
	labNotAddTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotAddTim);
	left->addWidget(notAddTim);

	QTextEdit* addSpa=new QTextEdit();
	addSpa->setReadOnly(true);
	addSpa->setText(addedSpaceConstraintsString);
	QLabel* labAddSpa=new QLabel(tr("These space constraints will be added"));
	labAddSpa->setWordWrap(true);
	labAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labAddSpa);
	right->addWidget(addSpa);
	
	QTextEdit* notAddSpa=new QTextEdit();
	notAddSpa->setReadOnly(true);
	notAddSpa->setText(notAddedSpaceConstraintsString);
	QLabel* labNotAddSpa=new QLabel(tr("These space constraints will NOT be added"));
	labNotAddSpa->setWordWrap(true);
	labNotAddSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotAddSpa);
	right->addWidget(notAddSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, addedTimeConstraints){
		bool t=gt.rules.addTimeConstraint(tc);
		assert(t);
	}
	addedTimeConstraints.clear();
	foreach(TimeConstraint* tc, notAddedTimeConstraints){
		delete tc;
	}
	notAddedTimeConstraints.clear();
	
	foreach(SpaceConstraint* sc, addedSpaceConstraints){
		bool t=gt.rules.addSpaceConstraint(sc);
		assert(t);
	}
	addedSpaceConstraints.clear();
	foreach(SpaceConstraint* sc, notAddedSpaceConstraints){
		delete sc;
	}
	notAddedSpaceConstraints.clear();
	
	QMessageBox::information(NULL, tr("FET information"), tr("There were added %1 locking time constraints and"
		" %2 locking space constraints. There were not added %3 locking time constraints and %4 locking space constraints, because"
		" these activities were already locked").arg(addedTime).arg(addedSpace).arg(notAddedTime).arg(notAddedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();

	//cout<<"isc=="<<gt.rules.internalStructureComputed<<endl;
}

void AdvancedLockUnlockForm::unlockAll()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		return;
	}
	
	//New Dialog
	QDialog taDialog(NULL);
	taDialog.setWindowTitle(tr("FET - Unlock all activities of the current timetable"));

	QVBoxLayout* taMainLayout=new QVBoxLayout(&taDialog);

	QLabel* taLabel=new QLabel();
	taLabel->setWordWrap(true);
	taLabel->setText(tr("All activities of the current timetable will be unlocked (those which are not permanently locked)"));
	taLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
	
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	////////
	QGroupBox* selectorsGroupBox=new QGroupBox();
	selectorsGroupBox->setTitle("Unlock");
	
	QHBoxLayout* groupLayout=new QHBoxLayout(selectorsGroupBox);
	
	QCheckBox* timeCheckBox=new QCheckBox();
	timeCheckBox->setChecked(true);
	timeCheckBox->setText("Time");
	QCheckBox* spaceCheckBox=new QCheckBox();
	spaceCheckBox->setChecked(true);
	spaceCheckBox->setText("Space");

	groupLayout->addWidget(timeCheckBox);
	groupLayout->addWidget(spaceCheckBox);
	groupLayout->addStretch();
	/////////
	
	taMainLayout->addWidget(taLabel);
	taMainLayout->addStretch();
	taMainLayout->addWidget(selectorsGroupBox);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &taDialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &taDialog, SLOT(reject()));

	tapb2->setDefault(true);
	tapb2->setFocus();
	
	int w=taDialog.sizeHint().width();
	int h=taDialog.sizeHint().height();
	if(w<MIN_WIDTH)
		w=MIN_WIDTH;
	if(h<MIN_HEIGHT)
		h=MIN_HEIGHT;
	taDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&taDialog);
	
	bool ok=taDialog.exec();
	if(!ok)
		return;
		
	bool unlockTime=timeCheckBox->isChecked();
	bool unlockSpace=spaceCheckBox->isChecked();

	int removedTime=0, notRemovedTime=0;
	int removedSpace=0, notRemovedSpace=0;
	
	QSet<int> lockedActivitiesIds;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			assert(best_solution.times[i]>=0 && best_solution.times[i]<gt.rules.nHoursPerWeek);
			lockedActivitiesIds.insert(gt.rules.internalActivitiesList[i].id);
		}
	}

	QString removedTimeConstraintsString;
	QString notRemovedTimeConstraintsString;

	QList<ConstraintActivityPreferredStartingTime*> removedTimeConstraints;
	QList<ConstraintActivityPreferredStartingTime*> notRemovedTimeConstraints;
	
	QString removedSpaceConstraintsString;
	QString notRemovedSpaceConstraintsString;

	QList<ConstraintActivityPreferredRoom*> removedSpaceConstraints;
	QList<ConstraintActivityPreferredRoom*> notRemovedSpaceConstraints;
	
	if(unlockTime){
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*)tc;
				if(c->weightPercentage==100.0 && c->day>=0 && c->hour>=0 && lockedActivitiesIds.contains(c->activityId)){
					if(!c->permanentlyLocked){
						removedTimeConstraints.append(c);
						removedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedTime++;
					}
					else{
						notRemovedTimeConstraints.append(c);
						notRemovedTimeConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedTime++;
					}
				}
			}
		}
	}

	if(unlockSpace){
		foreach(SpaceConstraint* tc, gt.rules.spaceConstraintsList){
			if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*)tc;
				if(c->weightPercentage==100.0 && lockedActivitiesIds.contains(c->activityId)){
					if(!c->permanentlyLocked){
						removedSpaceConstraints.append(c);
						removedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						removedSpace++;
					}
					else{
						notRemovedSpaceConstraints.append(c);
						notRemovedSpaceConstraintsString+=c->getDetailedDescription(gt.rules)+"\n";
						notRemovedSpace++;
					}
				}
			}
		}
	}

	////////////
	//last confirmation dialog
	QDialog lastConfirmationDialog(NULL);
	lastConfirmationDialog.setWindowTitle(tr("Last confirmation needed"));

	QVBoxLayout* lastMainLayout=new QVBoxLayout(&lastConfirmationDialog);

	QPushButton* lastpb1=new QPushButton(tr("Cancel"));
	QPushButton* lastpb2=new QPushButton(tr("OK"));
	QHBoxLayout* lastButtons=new QHBoxLayout();
	lastButtons->addStretch();
	lastButtons->addWidget(lastpb1);
	lastButtons->addWidget(lastpb2);
	
	////texts
	QVBoxLayout* left=new QVBoxLayout();
	QVBoxLayout* right=new QVBoxLayout();
	QHBoxLayout* all=new QHBoxLayout();
	all->addLayout(left);
	all->addLayout(right);
	
	QTextEdit* remTim=new QTextEdit();
	remTim->setReadOnly(true);
	remTim->setText(removedTimeConstraintsString);
	QLabel* labRemTim=new QLabel(tr("These time constraints will be removed"));
	labRemTim->setWordWrap(true);
	labRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labRemTim);
	left->addWidget(remTim);

	QTextEdit* notRemTim=new QTextEdit();
	notRemTim->setReadOnly(true);
	notRemTim->setText(notRemovedTimeConstraintsString);
	QLabel* labNotRemTim=new QLabel(tr("These time constraints will NOT be removed"));
	labNotRemTim->setWordWrap(true);
	labNotRemTim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	left->addWidget(labNotRemTim);
	left->addWidget(notRemTim);

	QTextEdit* remSpa=new QTextEdit();
	remSpa->setReadOnly(true);
	remSpa->setText(removedSpaceConstraintsString);
	QLabel* labRemSpa=new QLabel(tr("These space constraints will be removed"));
	labRemSpa->setWordWrap(true);
	labRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labRemSpa);
	right->addWidget(remSpa);
	
	QTextEdit* notRemSpa=new QTextEdit();
	notRemSpa->setReadOnly(true);
	notRemSpa->setText(notRemovedSpaceConstraintsString);
	QLabel* labNotRemSpa=new QLabel(tr("These space constraints will NOT be removed"));
	labNotRemSpa->setWordWrap(true);
	labNotRemSpa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	right->addWidget(labNotRemSpa);
	right->addWidget(notRemSpa);
	////end texts
	
	////////
	lastMainLayout->addLayout(all);
	//lastMainLayout->addStretch();
	lastMainLayout->addLayout(lastButtons);

	QObject::connect(lastpb2, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(accept()));
	QObject::connect(lastpb1, SIGNAL(clicked()), &lastConfirmationDialog, SLOT(reject()));

	lastpb2->setDefault(true);
	lastpb2->setFocus();
	
	int lw=lastConfirmationDialog.sizeHint().width();
	int lh=lastConfirmationDialog.sizeHint().height();
	lastConfirmationDialog.setGeometry(0,0,lw,lh);
	centerWidgetOnScreen(&lastConfirmationDialog);
	
	ok=lastConfirmationDialog.exec();
	if(!ok)
		return;
	////////////
	
	foreach(TimeConstraint* tc, removedTimeConstraints)
		gt.rules.removeTimeConstraint(tc);
	removedTimeConstraints.clear();
	notRemovedTimeConstraints.clear();

	foreach(SpaceConstraint* sc, removedSpaceConstraints)
		gt.rules.removeSpaceConstraint(sc);
	removedSpaceConstraints.clear();
	notRemovedSpaceConstraints.clear();

	QMessageBox::information(NULL, tr("FET information"), tr("There were removed %1 locking time constraints and"
		" %2 locking space constraints. There were not removed %3 locking time constraints and %4 locking space constraints, because"
		" these activities were permanently locked").arg(removedTime).arg(removedSpace).arg(notRemovedTime).arg(notRemovedSpace));
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	//cout<<"isc=="<<gt.rules.internalStructureComputed<<endl;
}
