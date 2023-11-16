//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "daysform.h"

#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>

#include <QInputDialog>

#include <QHash>

extern Timetable gt;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

DaysForm::DaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	daysListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(insertDayPushButton, SIGNAL(clicked()), this, SLOT(insertDay()));
	connect(modifyDayPushButton, SIGNAL(clicked()), this, SLOT(modifyDay()));
	connect(removeDayPushButton, SIGNAL(clicked()), this, SLOT(removeDay()));
	connect(daysListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyDay()));

	disconnect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
	nDaysSpinBox->setMinimum(1);
	nDaysSpinBox->setMaximum(MAX_DAYS_PER_WEEK);
	nDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
	realNames.clear();
	for(int i=0; i<gt.rules.nDaysPerWeek; i++){
		realNames.append(gt.rules.daysOfTheWeek[i]);
		daysListWidget->addItem(tr("%1. %2", "%1 is the day index, %2 is the day name").arg(i+1).arg(gt.rules.daysOfTheWeek[i]));
	}
	if(daysListWidget->count()>=1)
		daysListWidget->setCurrentRow(0);
	connect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

DaysForm::~DaysForm()
{
	saveFETDialogGeometry(this);
}

void DaysForm::insertDay()
{
	if(nDaysSpinBox->value()==nDaysSpinBox->maximum()){
		QMessageBox::warning(this, tr("FET information"), tr("Maximum number of days reached"));
		return;
	}
	int i=daysListWidget->currentRow();
	if(i<0)
		i=daysListWidget->count();
	
	realNames.insert(i, tr("Day %1").arg(i+1));
	daysListWidget->insertItem(i, tr("%1. %2", "%1 is the day index, %2 is the day name").arg(i+1).arg(tr("Day %1").arg(i+1)));
	for(int j=i+1; j<daysListWidget->count(); j++)
		daysListWidget->item(j)->setText(tr("%1. %2", "%1 is the day index, %2 is the day name").arg(j+1).arg(realNames.at(j)));

	disconnect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
	int j=nDaysSpinBox->value();
	nDaysSpinBox->setValue(j+1);
	connect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
}

void DaysForm::modifyDay()
{
	int i=daysListWidget->currentRow();
	if(daysListWidget->count()<=0 || i<0 || i>=daysListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected day"));
		return;
	}
	QString oldName=realNames.at(i);
	bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename day"), tr("Please enter the new name of this day"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		daysListWidget->item(i)->setText(tr("%1. %2", "%1 is the day index, %2 is the day name").arg(i+1).arg(newName));
		realNames[i]=newName;
	}
}

void DaysForm::removeDay()
{
	int i=daysListWidget->currentRow();
	if(i>=0 && i<daysListWidget->count() && daysListWidget->count()>=2){
		daysListWidget->setCurrentRow(-1);
		QListWidgetItem* item=daysListWidget->takeItem(i);
		delete item;
		realNames.removeAt(i);
		if(i<daysListWidget->count())
			daysListWidget->setCurrentRow(i);
		else
			daysListWidget->setCurrentRow(daysListWidget->count()-1);
		for(int j=i; j<daysListWidget->count(); j++)
			daysListWidget->item(j)->setText(tr("%1. %2", "%1 is the day index, %2 is the day name").arg(j+1).arg(realNames[j]));

		disconnect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
		int j=nDaysSpinBox->value();
		nDaysSpinBox->setValue(j-1);
		connect(nDaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfDaysChanged()));
	}
	else if(daysListWidget->count()==1){
		QMessageBox::warning(this, tr("FET information"), tr("The number of days must be at least 1"));
	}
	else{
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected day"));
	}
}

void DaysForm::numberOfDaysChanged()
{
	int nv=nDaysSpinBox->value();
	int cnt=daysListWidget->count();
	if(nv<cnt){
		daysListWidget->setCurrentRow(-1);
		for(int i=cnt-1; i>=nv; i--){
			QListWidgetItem* item=daysListWidget->takeItem(i);
			delete item;
			realNames.removeLast();
		}
		daysListWidget->setCurrentRow(daysListWidget->count()-1);
	}
	else if(nv>cnt){
		for(int i=cnt; i<nv; i++){
			realNames.append(tr("Day %1").arg(i+1));
			daysListWidget->addItem(tr("%1. %2", "%1 is the day index, %2 is the day name").arg(i+1).arg(tr("Day %1").arg(i+1)));
		}
	}
}

void DaysForm::ok()
{
	int nDays=daysListWidget->count();

	QHash<QString, int> hashDayIndex;
	for(int i=0; i<nDays; i++){
		QString s=realNames.at(i);
		
		if(s.isEmpty()){
			QMessageBox::warning(this, tr("FET information"), tr("Empty names not allowed (the day number %1 has an empty name).").arg(i+1));
			return;
		}
		
		if(hashDayIndex.contains(s)){
			int j=hashDayIndex.value(s);
			QMessageBox::warning(this, tr("FET information"),
			 tr("Duplicate names not allowed (the day number %1 has the same name as the day number %2).").arg(i+1).arg(j+1));
			return;
		}
		else{
			hashDayIndex.insert(s, i);
		}
	}
	
	//2011-10-18
	int cnt_mod=0;
	int cnt_rem=0;
	int oldDays=gt.rules.nDaysPerWeek;
	gt.rules.nDaysPerWeek=nDays;
	
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
		if(tc->hasWrongDayOrHour(gt.rules)){
			if(tc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}

	for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList))
		if(sc->hasWrongDayOrHour(gt.rules)){
			if(sc->canRepairWrongDayOrHour(gt.rules))
				cnt_mod++;
			else
				cnt_rem++;
		}
	
	gt.rules.nDaysPerWeek=oldDays;
	
	if(cnt_mod>0 || cnt_rem>0){
		QString s=QString("");
		if(cnt_rem>0){
			s+=tr("%1 constraints will be removed.", "%1 is the number of constraints").arg(cnt_rem);
			s+=" ";
		}
		if(cnt_mod>0){
			s+=tr("%1 constraints will be modified.", "%1 is the number of constraints").arg(cnt_mod);
			s+=" ";
		}
		s+=tr("Do you want to continue?");

		int res=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::Cancel);
		
		if(res==QMessageBox::Cancel)
			return;

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
		
		int _oldDays=gt.rules.nDaysPerWeek;
		gt.rules.nDaysPerWeek=nDays;

		//time
		QList<TimeConstraint*> toBeRemovedTime;
		for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
			if(tc->hasWrongDayOrHour(gt.rules)){
				bool tmp=tc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=tc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedTime.append(tc);
				}
			}
		}
		bool recomputeTime=false;

		if(toBeRemovedTime.count()>0){
			for(TimeConstraint* tc : std::as_const(toBeRemovedTime)){
				if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
				bool tmp=gt.rules.removeTimeConstraint(tc);
				assert(tmp);
			}
		}
		//////

		//space
		QList<SpaceConstraint*> toBeRemovedSpace;
		for(SpaceConstraint* sc : std::as_const(gt.rules.spaceConstraintsList)){
			if(sc->hasWrongDayOrHour(gt.rules)){
				bool tmp=sc->canRepairWrongDayOrHour(gt.rules);
				if(tmp){
					int tmp2=sc->repairWrongDayOrHour(gt.rules);
					assert(tmp2);
				}
				else{
					toBeRemovedSpace.append(sc);
				}
			}
		}
		
		bool recomputeSpace=false;

		if(toBeRemovedSpace.count()>0){
			for(SpaceConstraint* sc : std::as_const(toBeRemovedSpace)){
				if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
				bool tmp=gt.rules.removeSpaceConstraint(sc);
				assert(tmp);
			}
		}
		//////

		gt.rules.nDaysPerWeek=_oldDays;

		if(recomputeTime){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		}
		if(recomputeSpace){
			assert(0);
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		}
		if(recomputeTime || recomputeSpace){
			LockUnlock::increaseCommunicationSpinBox();
		}
	}
	else{
		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_schedule_ready=false;
	}
	////////////
	
	QString od=tr("Old number of days: %1").arg(gt.rules.nDaysPerWeek);
	od+=QString("\n");
	od+=tr("Old days:\n%1").arg(gt.rules.daysOfTheWeek.join("\n"));

	gt.rules.nDaysPerWeek=nDays;
	gt.rules.daysOfTheWeek.clear();
	for(int i=0; i<nDays; i++)
		gt.rules.daysOfTheWeek.append(realNames.at(i));

	QString nd=tr("New number of days: %1").arg(gt.rules.nDaysPerWeek);
	nd+=QString("\n");
	nd+=tr("New days:\n%1").arg(gt.rules.daysOfTheWeek.join("\n"));
	
	gt.rules.addUndoPoint(tr("The number and/or names of the days were changed from:\n\n%1\n\nto\n\n%2").arg(od).arg(nd));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void DaysForm::cancel()
{
	this->close();
}
