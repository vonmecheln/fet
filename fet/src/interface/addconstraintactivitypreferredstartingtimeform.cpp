/***************************************************************************
                          addconstraintactivitypreferredstartingtimeform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#include "addconstraintactivitypreferredstartingtimeform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

#include <QSet>
#include "lockunlock.h"
//extern QSet<int> idsOfLockedTime;
//extern QSet<int> idsOfPermanentlyLockedTime;

AddConstraintActivityPreferredStartingTimeForm::AddConstraintActivityPreferredStartingTimeForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	//permTextLabel->setWordWrap(true);
	
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

	studentsComboBox->insertItem("");
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}
	studentsComboBox->setCurrentItem(0);
	
	updatePeriodGroupBox();
	updateActivitiesComboBox();
}

AddConstraintActivityPreferredStartingTimeForm::~AddConstraintActivityPreferredStartingTimeForm()
{
}

bool AddConstraintActivityPreferredStartingTimeForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
//	if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}


void AddConstraintActivityPreferredStartingTimeForm::updateActivitiesComboBox(){
	activitiesComboBox->clear();
	activitiesList.clear();
	
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		
		if(filterOk(act)){
			activitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->activitiesList.append(act->id);
		}
	}

	constraintChanged();
}

void AddConstraintActivityPreferredStartingTimeForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void AddConstraintActivityPreferredStartingTimeForm::updatePeriodGroupBox(){
	startHourComboBox->clear();
	//startHourComboBox->insertItem(QObject::tr("Any"));
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->insertItem(gt.rules.hoursOfTheDay[i]);

	dayComboBox->clear();
	//dayComboBox->insertItem(QObject::tr("Any"));
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		dayComboBox->insertItem(gt.rules.daysOfTheWeek[i]);
}

void AddConstraintActivityPreferredStartingTimeForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";*/

	s+=QObject::tr("Activity preferred starting time");
	s+="\n";
	int tmp2=activitiesComboBox->currentItem();
	assert(tmp2<activitiesList.size());
	assert(tmp2<gt.rules.activitiesList.size());
	if(tmp2<0){
		s+=QObject::tr("Invalid activity");
		s+="\n";
	}
	else{
		int id=activitiesList.at(tmp2);
		s+=QObject::tr("Activity id=%1").arg(id);
		s+="\n";
	}

	int day=dayComboBox->currentItem();
	if(day<0 || day>=gt.rules.nDaysPerWeek){
		s+=QObject::tr("Invalid day");
		s+="\n";
	}
	else{
		s+=QObject::tr("Day:%1").arg(dayComboBox->currentText());
		s+="\n";
	}

	int startHour=startHourComboBox->currentItem();
	if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
		s+=QObject::tr("Invalid start hour");
		s+="\n";
	}
	else{
		s+=QObject::tr("Start hour:%1").arg(startHourComboBox->currentText());
		s+="\n";
	}
	
	if(permLockedCheckBox->isChecked()){
		s+=QObject::tr("Permanently locked (cannot be unlocked from the 'Timetable' menu)");
	}
	else{
		s+=QObject::tr("Not permanently locked (can be unlocked from the 'Timetable' menu)");
	}
	s+="\n";
	
	currentConstraintTextEdit->setText(s);
}

void AddConstraintActivityPreferredStartingTimeForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int day=dayComboBox->currentItem();
	if(day<0 || day>=gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid day"));
		return;
	}
	int startHour=startHourComboBox->currentItem();
	if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid start hour"));
		return;
	}

/*	if(startHour==0 && day==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Please specify at least a day or an hour"));
		return;
	}*/

	int id;
	int tmp2=activitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<activitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid activity"));
		return;
	}
	else
		id=activitiesList.at(tmp2);
	
	ctr=new ConstraintActivityPreferredStartingTime(weight, /*compulsory,*/ id, day, startHour, permLockedCheckBox->isChecked());

	bool tmp3=gt.rules.addTimeConstraint(ctr);
	if(tmp3){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Constraint added"));
			
		/*
		if(day-1>=0 && startHour-1>=0){
			if(permLockedCheckBox->isChecked())
				idsOfPermanentlyLockedTime.insert(id);
			else
				idsOfLockedTime.insert(id);
			LockUnlock::increaseCommunicationSpinBox();
		} wrong, must take care also of weight==100.0 */
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - duplicate"));
		delete ctr;
	}
}
