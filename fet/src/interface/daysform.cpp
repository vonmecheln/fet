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
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "lockunlock.h"

#include "daysform.h"
#include "modifydayform.h"
#include "modifyrealdayform.h"

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>

#include <QInputDialog>

#include <QHash>

extern Timetable gt;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

DaysForm::DaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		realDaysLabel->setEnabled(false);
		realDaysListWidget->setEnabled(false);
		modifyRealDayPushButton->setEnabled(false);
	}

	okPushButton->setDefault(true);

	daysListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
	connect(cancelPushButton, &QPushButton::clicked, this, &DaysForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &DaysForm::ok);
	connect(insertDayPushButton, &QPushButton::clicked, this, &DaysForm::insertDay);
	connect(modifyDayPushButton, &QPushButton::clicked, this, &DaysForm::modifyDay);
	connect(modifyRealDayPushButton, &QPushButton::clicked, this, &DaysForm::modifyRealDay);
	connect(removeDayPushButton, &QPushButton::clicked, this, &DaysForm::removeDay);
	
	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{insertDayPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	insertDayPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyDayPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyDayPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeDayPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeDayPushButton->setToolTip(QString("⌦"));
	}

	connect(daysListWidget, &QListWidget::itemDoubleClicked, this, &DaysForm::modifyDay);
	connect(realDaysListWidget, &QListWidget::itemDoubleClicked, this, &DaysForm::modifyRealDay);

	disconnect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
	/*if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		nDaysSpinBox->setMinimum(1);
	else
		nDaysSpinBox->setMinimum(2);*/
	nDaysSpinBox->setMinimum(1);
	nDaysSpinBox->setMaximum(MAX_DAYS_PER_WEEK);
	nDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
	
	realNamesForDays.clear();
	realLongNamesForDays.clear();
	for(int i=0; i<gt.rules.nDaysPerWeek; i++){
		realNamesForDays.append(gt.rules.daysOfTheWeek[i]);
		realLongNamesForDays.append(gt.rules.daysOfTheWeek_longNames[i]);
		daysListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
		 .arg(i+1).arg(gt.rules.daysOfTheWeek[i]).arg(gt.rules.daysOfTheWeek_longNames[i]));
	}
	if(daysListWidget->count()>=1)
		daysListWidget->setCurrentRow(0);
	connect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		realNamesForRealDays.clear();
		realLongNamesForRealDays.clear();
		for(int i=0; i<gt.rules.nRealDaysPerWeek; i++){
			realNamesForRealDays.append(gt.rules.realDaysOfTheWeek[i]);
			realLongNamesForRealDays.append(gt.rules.realDaysOfTheWeek_longNames[i]);
			realDaysListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the real day index, %2 is the (short) real day name, %3 is the long real day name")
			 .arg(i+1).arg(gt.rules.realDaysOfTheWeek[i]).arg(gt.rules.realDaysOfTheWeek_longNames[i]));
		}
	}
	numberOfRealDaysChanged();

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
	
	realNamesForDays.insert(i, tr("%1").arg(i+1));
	realLongNamesForDays.insert(i, tr("Day %1").arg(i+1));
	daysListWidget->insertItem(i, tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
	 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Day %1").arg(i+1)));
	for(int j=i+1; j<daysListWidget->count(); j++)
		daysListWidget->item(j)->setText(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
		 .arg(j+1).arg(realNamesForDays.at(j)).arg(realLongNamesForDays.at(j)));

	disconnect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
	int j=nDaysSpinBox->value();
	nDaysSpinBox->setValue(j+1);
	connect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
	
	numberOfRealDaysChanged();
}

void DaysForm::modifyDay()
{
	int i=daysListWidget->currentRow();
	if(daysListWidget->count()<=0 || i<0 || i>=daysListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected day"));
		return;
	}

	QString oldName=realNamesForDays.at(i);
	QString oldLongName=realLongNamesForDays.at(i);
	
	/*
	bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename day"), tr("Please enter the new name of this day (must be unique and nonempty)"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		bool ok2=false;
		QString newLongName=QInputDialog::getText(this, tr("Rename day"), tr("Please enter the new long name of this day (may not be unique and may be empty)"),
		 QLineEdit::Normal, oldLongName, &ok2);
		
		if(ok2){
			daysListWidget->item(i)->setText(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
			 .arg(i+1).arg(newName).arg(newLongName));
			realNamesForDays[i]=newName;
			realLongNamesForDays[i]=newLongName;
		}
	}*/
	
	ModifyDayForm form(this, oldName, oldLongName);
	int res=form.exec();
	if(res==QDialog::Accepted){
		QString newName=form.name;
		QString newLongName=form.longName;
		
		daysListWidget->item(i)->setText(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
		 .arg(i+1).arg(newName).arg(newLongName));
		realNamesForDays[i]=newName;
		realLongNamesForDays[i]=newLongName;
	}
}

void DaysForm::modifyRealDay()
{
	int i=realDaysListWidget->currentRow();
	if(realDaysListWidget->count()<=0 || i<0 || i>=realDaysListWidget->count()){
		QMessageBox::warning(this, tr("FET information"), tr("Invalid selected real day"));
		return;
	}

	QString oldRealName=realNamesForRealDays.at(i);
	QString oldRealLongName=realLongNamesForRealDays.at(i);
	
	/*
	bool ok=false;
	QString newName=QInputDialog::getText(this, tr("Rename real day"), tr("Please enter the new name of this real day"),
	 QLineEdit::Normal, oldName, &ok);
	if(ok && !newName.isEmpty()){
		realDaysListWidget->item(i)->setText(tr("%1. %2", "%1 is the real day index, %2 is the real day name").arg(i+1).arg(newName));
		realNamesForRealDays[i]=newName;
	}*/

	ModifyRealDayForm form(this, oldRealName, oldRealLongName);
	int res=form.exec();
	if(res==QDialog::Accepted){
		QString newRealName=form.name;
		QString newRealLongName=form.longName;
		
		realDaysListWidget->item(i)->setText(tr("%1. N: %2, LN: %3", "%1 is the real day index, %2 is the (short) real day name, %3 is the long real day name")
		 .arg(i+1).arg(newRealName).arg(newRealLongName));
		realNamesForRealDays[i]=newRealName;
		realLongNamesForRealDays[i]=newRealLongName;
	}
}

void DaysForm::removeDay()
{
	int i=daysListWidget->currentRow();
	/*if(gt.rules.mode==MORNINGS_AFTERNOONS && daysListWidget->count()<=2){
		QMessageBox::warning(this, tr("FET information"), tr("The number of days must be at least 2, so that the number of real days will be at least 1"));
		return;
	}*/
	if(i>=0 && i<daysListWidget->count() && daysListWidget->count()>=2){
		daysListWidget->setCurrentRow(-1);
		QListWidgetItem* item=daysListWidget->takeItem(i);
		delete item;
		realNamesForDays.removeAt(i);
		realLongNamesForDays.removeAt(i);
		if(i<daysListWidget->count())
			daysListWidget->setCurrentRow(i);
		else
			daysListWidget->setCurrentRow(daysListWidget->count()-1);
		for(int j=i; j<daysListWidget->count(); j++)
			daysListWidget->item(j)->setText(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
			 .arg(j+1).arg(realNamesForDays[j]).arg(realLongNamesForDays[j]));

		disconnect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
		int j=nDaysSpinBox->value();
		nDaysSpinBox->setValue(j-1);
		connect(nDaysSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &DaysForm::numberOfDaysChanged);
		
		numberOfRealDaysChanged();
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
			realNamesForDays.removeLast();
			realLongNamesForDays.removeLast();
		}
		daysListWidget->setCurrentRow(daysListWidget->count()-1);
	}
	else if(nv>cnt){
		for(int i=cnt; i<nv; i++){
			realNamesForDays.append(tr("%1").arg(i+1));
			realLongNamesForDays.append(tr("Day %1").arg(i+1));
			daysListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the day index, %2 is the (short) day name, %3 is the long day name")
			 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Day %1").arg(i+1)));
		}
	}
	
	numberOfRealDaysChanged();
}

void DaysForm::numberOfRealDaysChanged()
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		return;

	int nv=nDaysSpinBox->value();
	int rnv=nv/2;
	realDaysLabel->setText(tr("Real days: %1", "%1 is the number of real days per week").arg(rnv));
	int rcnt=realDaysListWidget->count();
	if(rnv<rcnt){
		realDaysListWidget->setCurrentRow(-1);
		for(int i=rcnt-1; i>=rnv; i--){
			QListWidgetItem* item=realDaysListWidget->takeItem(i);
			delete item;
			realNamesForRealDays.removeLast();
			realLongNamesForRealDays.removeLast();
		}
		realDaysListWidget->setCurrentRow(realDaysListWidget->count()-1);
	}
	else if(rnv>rcnt){
		for(int i=rcnt; i<rnv; i++){
			realNamesForRealDays.append(tr("%1").arg(i+1));
			realLongNamesForRealDays.append(tr("Real day %1").arg(i+1));
			realDaysListWidget->addItem(tr("%1. N: %2, LN: %3", "%1 is the real day index, %2 is the (short) real day name, %3 is the long real day name")
			 .arg(i+1).arg(tr("%1").arg(i+1)).arg(tr("Real day %1").arg(i+1)));
		}
	}
}

void DaysForm::ok()
{
	int nDays=daysListWidget->count();

	QHash<QString, int> hashDayIndex;
	for(int i=0; i<nDays; i++){
		QString s=realNamesForDays.at(i);
		
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
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		QHash<QString, int> hashRealDayIndex;
		for(int i=0; i<nDays/2; i++){
			QString s=realNamesForRealDays.at(i);
			
			if(s.isEmpty()){
				QMessageBox::warning(this, tr("FET information"), tr("Empty names not allowed (the real day number %1 has an empty name).").arg(i+1));
				return;
			}
			
			if(hashRealDayIndex.contains(s)){
				int j=hashRealDayIndex.value(s);
				QMessageBox::warning(this, tr("FET information"),
				 tr("Duplicate names not allowed (the real day number %1 has the same name as the real day number %2).").arg(i+1).arg(j+1));
				return;
			}
			else{
				hashRealDayIndex.insert(s, i);
			}
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
		rooms_buildings_schedule_ready=false;
		
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
		rooms_buildings_schedule_ready=false;
	}
	////////////
	
	QString od=tr("Old number of days: %1").arg(gt.rules.nDaysPerWeek);
	od+=QString("\n");
	od+=tr("Old days names:\n%1").arg(gt.rules.daysOfTheWeek.join("\n"));
	od+=QString("\n");
	od+=tr("Old days long names:\n%1").arg(gt.rules.daysOfTheWeek_longNames.join("\n"));

	gt.rules.nDaysPerWeek=nDays;
	gt.rules.daysOfTheWeek.clear();
	gt.rules.daysOfTheWeek_longNames.clear();
	for(int i=0; i<nDays; i++){
		gt.rules.daysOfTheWeek.append(realNamesForDays.at(i));
		gt.rules.daysOfTheWeek_longNames.append(realLongNamesForDays.at(i));
	}
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		od+=QString("\n\n");
		od+=tr("Old number of real days: %1").arg(gt.rules.nRealDaysPerWeek);
		od+=QString("\n");
		od+=tr("Old real days names:\n%1").arg(gt.rules.realDaysOfTheWeek.join("\n"));
		od+=QString("\n");
		od+=tr("Old real days long names:\n%1").arg(gt.rules.realDaysOfTheWeek_longNames.join("\n"));

		gt.rules.nRealDaysPerWeek=nDays/2;
		gt.rules.realDaysOfTheWeek.clear();
		gt.rules.realDaysOfTheWeek_longNames.clear();
		for(int i=0; i<nDays/2; i++){
			gt.rules.realDaysOfTheWeek.append(realNamesForRealDays.at(i));
			gt.rules.realDaysOfTheWeek_longNames.append(realLongNamesForRealDays.at(i));
		}
	}

	QString nd=tr("New number of days: %1").arg(gt.rules.nDaysPerWeek);
	nd+=QString("\n");
	nd+=tr("New days names:\n%1").arg(gt.rules.daysOfTheWeek.join("\n"));
	nd+=QString("\n");
	nd+=tr("New days long names:\n%1").arg(gt.rules.daysOfTheWeek_longNames.join("\n"));

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		nd+=QString("\n\n");
		nd+=tr("New number of real days: %1").arg(gt.rules.nRealDaysPerWeek);
		nd+=QString("\n");
		nd+=tr("New real days names:\n%1").arg(gt.rules.realDaysOfTheWeek.join("\n"));
		nd+=QString("\n");
		nd+=tr("New real days long names:\n%1").arg(gt.rules.realDaysOfTheWeek_longNames.join("\n"));
	}
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		gt.rules.addUndoPoint(tr("The number and/or the names/long names of the days were changed from:\n\n%1\n\nto\n\n%2").arg(od).arg(nd));
	else
		gt.rules.addUndoPoint(tr("The number and/or the names/long names of the days/real days were changed from:\n\n%1\n\nto\n\n%2").arg(od).arg(nd));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void DaysForm::cancel()
{
	this->close();
}
