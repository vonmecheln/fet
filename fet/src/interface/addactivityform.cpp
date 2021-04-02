/***************************************************************************
                          addactivityform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
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

#include "addactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include <qlabel.h>
#include <qtabwidget.h>
#include <q3listbox.h>

#include <QDesktopWidget>

#define subTab(i)	subactivitiesTabWidget->page(i)
#define prefDay(i)	(i==0?preferredDay1ComboBox:				\
			(i==1?preferredDay2ComboBox:				\
			(i==2?preferredDay3ComboBox:				\
			(i==3?preferredDay4ComboBox:				\
			(i==4?preferredDay5ComboBox:				\
			(i==5?preferredDay6ComboBox:				\
			(i==6?preferredDay7ComboBox:				\
			(preferredDay8ComboBox))))))))
#define prefHour(i)	(i==0?preferredHour1ComboBox:				\
			(i==1?preferredHour2ComboBox:				\
			(i==2?preferredHour3ComboBox:				\
			(i==3?preferredHour4ComboBox:				\
			(i==4?preferredHour5ComboBox:				\
			(i==5?preferredHour6ComboBox:				\
			(i==6?preferredHour7ComboBox:				\
			(preferredHour8ComboBox))))))))
#define par(i)		(i==0?parity1CheckBox:					\
			(i==1?parity2CheckBox:					\
			(i==2?parity3CheckBox:					\
			(i==3?parity4CheckBox:					\
			(i==4?parity5CheckBox:					\
			(i==5?parity6CheckBox:					\
			(i==6?parity7CheckBox:					\
			(parity8CheckBox))))))))
#define dur(i)		(i==0?duration1SpinBox:					\
			(i==1?duration2SpinBox:					\
			(i==2?duration3SpinBox:					\
			(i==3?duration4SpinBox:					\
			(i==4?duration5SpinBox:					\
			(i==5?duration6SpinBox:					\
			(i==6?duration7SpinBox:					\
			(duration1SpinBox))))))))
#define activ(i)		(i==0?active1CheckBox:					\
			(i==1?active2CheckBox:					\
			(i==2?active3CheckBox:					\
			(i==3?active4CheckBox:					\
			(i==4?active5CheckBox:					\
			(i==5?active6CheckBox:					\
			(i==6?active7CheckBox:					\
			(active8CheckBox))))))))

AddActivityForm::AddActivityForm()
{
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
					
	updatePreferredDaysComboBox();
	updatePreferredHoursComboBox();
	updateStudentsListBox();
	updateTeachersListBox();
	updateSubjectsComboBox();
	updateSubjectTagsComboBox();

	minDayDistanceSpinBox->setMaxValue(gt.rules.nDaysPerWeek-1);
	minDayDistanceSpinBox->setMinValue(0);
	minDayDistanceSpinBox->setValue(1);

	int nSplit=splitSpinBox->value();
	for(int i=0; i<8; i++)
		if(i<nSplit)
			subTab(i)->setEnabled(true);
		else
			subTab(i)->setDisabled(true);
}

AddActivityForm::~AddActivityForm()
{
}

void AddActivityForm::updateTeachersListBox()
{
	allTeachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		allTeachersListBox->insertItem(tch->name);
	}
		
	selectedTeachersListBox->clear();

	activityChanged();
}

void AddActivityForm::addTeacher()
{
	if(allTeachersListBox->currentItem()<0 || (uint)(allTeachersListBox->currentItem())>=allTeachersListBox->count())
		return;
	
	for(uint i=0; i<selectedTeachersListBox->count(); i++)
		if(selectedTeachersListBox->text(i)==allTeachersListBox->currentText())
			return;
			
	selectedTeachersListBox->insertItem(allTeachersListBox->currentText());

	activityChanged();
}

void AddActivityForm::removeTeacher()
{
	if(selectedTeachersListBox->count()<=0 || selectedTeachersListBox->currentItem()<0 ||
	 (uint)(selectedTeachersListBox->currentItem())>=selectedTeachersListBox->count())
		return;
		
	selectedTeachersListBox->removeItem(selectedTeachersListBox->currentItem());

	activityChanged();
}

void AddActivityForm::addStudents()
{
	if(allStudentsListBox->currentItem()<0 || (uint)(allStudentsListBox->currentItem())>=allStudentsListBox->count())
		return;
	
	for(uint i=0; i<selectedStudentsListBox->count(); i++)
		if(selectedStudentsListBox->text(i)==allStudentsListBox->currentText())
			return;
			
	selectedStudentsListBox->insertItem(allStudentsListBox->currentText());
	
	activityChanged();
}

void AddActivityForm::removeStudents()
{
	if(selectedStudentsListBox->count()<=0 || selectedStudentsListBox->currentItem()<0 ||
	 (uint)(selectedStudentsListBox->currentItem())>=selectedStudentsListBox->count())
		return;
		
	selectedStudentsListBox->removeItem(selectedStudentsListBox->currentItem());

	activityChanged();
}

void AddActivityForm::updateSubjectsComboBox()
{
	subjectsComboBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sbj->name);
	}

	subjectChanged(subjectsComboBox->currentText());
}

void AddActivityForm::updateSubjectTagsComboBox()
{
	subjectTagsComboBox->clear();
	subjectTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* sbt=gt.rules.subjectTagsList[i];
		subjectTagsComboBox->insertItem(sbt->name);
	}
		
	subjectTagsComboBox->setCurrentItem(0);

	subjectTagChanged(subjectTagsComboBox->currentText());
}

void AddActivityForm::showYearsChanged()
{
	updateStudentsListBox();
}

void AddActivityForm::showGroupsChanged()
{
	updateStudentsListBox();
}

void AddActivityForm::showSubgroupsChanged()
{
	updateStudentsListBox();
}

void AddActivityForm::updateStudentsListBox()
{
	bool showYears=yearsCheckBox->isChecked();	
	bool showGroups=groupsCheckBox->isChecked();	
	bool showSubgroups=subgroupsCheckBox->isChecked();	

	allStudentsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(showYears)
			allStudentsListBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(showGroups)
				allStudentsListBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(showSubgroups)
					allStudentsListBox->insertItem(sts->name);
			}
		}
	}
	
	selectedStudentsListBox->clear();

	activityChanged();
}

void AddActivityForm::updatePreferredDaysComboBox()
{
	for(int j=0; j<8; j++){
		prefDay(j)->clear();
		prefDay(j)->insertItem(QObject::tr("Any"));
		for(int i=0; i<gt.rules.nDaysPerWeek; i++)
			prefDay(j)->insertItem(gt.rules.daysOfTheWeek[i]);
	}
}

void AddActivityForm::updatePreferredHoursComboBox()
{
	for(int j=0; j<8; j++){
		prefHour(j)->clear();
		prefHour(j)->insertItem(QObject::tr("Any"));
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			prefHour(j)->insertItem(gt.rules.hoursOfTheDay[i]);
	}
}

void AddActivityForm::subjectChanged(const QString& dummy)
{
	if(dummy=="")
		;

	activityChanged();
}

void AddActivityForm::subjectTagChanged(const QString& dummy)
{
	if(dummy=="")
		;

	activityChanged();
}

void AddActivityForm::splitChanged()
{
	int nSplit=splitSpinBox->value();

	minDayDistanceTextLabel->setEnabled(nSplit>=2);
	minDayDistanceSpinBox->setEnabled(nSplit>=2);

	for(int i=0; i<8; i++)
		if(i<nSplit)
			subTab(i)->setEnabled(true);
		else
			subTab(i)->setDisabled(true);

	activityChanged();
}

void AddActivityForm::activityChanged()
{
	QString s;
	s+=QObject::tr("Current activity:");s+="\n";
	for(uint i=0; i<selectedTeachersListBox->count(); i++){
		s+=QObject::tr("Teacher=%1").arg(selectedTeachersListBox->text(i));
		s+=selectedTeachersListBox->text(i);
		s+="\n";
	}

	s+=QObject::tr("Subject=%1").arg(subjectsComboBox->currentText());
	s+="\n";
	if(subjectTagsComboBox->currentText()!=""){
		s+=QObject::tr("Subject tag=%1").arg(subjectTagsComboBox->currentText());
		s+="\n";
	}
	for(uint i=0; i<selectedStudentsListBox->count(); i++){
		s+=QObject::tr("Students=%1").arg(selectedStudentsListBox->text(i));
		s+="\n";
	}

	if(nStudentsSpinBox->value()>=0){
		s+=QObject::tr("Number of students=%1").arg(nStudentsSpinBox->value());
		s+="\n";
	}
	else{
		s+=QObject::tr("Number of students: automatically computed from component students sets");
		s+="\n";
	}

	if(splitSpinBox->value()==1){
		s+=QObject::tr("Duration=%1").arg(dur(0)->value());
		s+="\n";
		if(par(0)->isChecked()){
			s+=QObject::tr("Fortnightly activity");
			s+="\n";
		}
		if(prefDay(0)->currentItem()>0){
			s+=QObject::tr("Preferred day=%1").arg(prefDay(0)->currentText());
			s+="\n";
		}
		if(prefHour(0)->currentItem()>0){
			s+=QObject::tr("Preferred hour=%1").arg(prefHour(0)->currentText());
			s+="\n";
		}
		if(activ(0)->isChecked()){
			s+=QObject::tr("Active activity");
			s+="\n";
		}
		else{
			s+=QObject::tr("Non-active activity");
			s+="\n";
		}
	}
	else{
		s+=QObject::tr("This activity will be split into %1 lessons per week").arg(splitSpinBox->value());
		s+="\n";
		if(minDayDistanceSpinBox->value()>0){
			s+=QObject::tr("The distance between any pair of subactivities must be at least %1 days").arg(minDayDistanceSpinBox->value());
			s+="\n";
		}
		s+="\n";

		for(int i=0; i<splitSpinBox->value(); i++){
			s+=QObject::tr("Componenent %1:").arg(i+1);
			s+="\n";
			s+=QObject::tr("Duration=%1").arg(dur(i)->value());
			s+="\n";
			if(par(i)->isChecked()){
				s+=QObject::tr("Fortnightly activity");
				s+="\n";
			}
			if(prefDay(i)->currentItem()>0){
				s+=QObject::tr("Preferred day=%1").arg(prefDay(i)->currentText());
				s+="\n";
			}
			if(prefHour(i)->currentItem()>0){
				s+=QObject::tr("Preferred hour=%1").arg(prefHour(i)->currentText());
				s+="\n";
			}
			if(activ(i)->isChecked()){
				s+=QObject::tr("Active activity");
				s+="\n";
			}
			else{
				s+=QObject::tr("Non-active activity");
				s+="\n";
			}
			s+="\n";
		}
	}

	currentActivityTextEdit->setText(s);
}

void AddActivityForm::addActivity()
{
	//teachers
	QStringList teachers_names;
	if(selectedTeachersListBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid teacher(s)"));
		return;
	}
	else if(selectedTeachersListBox->count()>(uint)(MAX_TEACHERS_PER_ACTIVITY)){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Too many teachers for an activity. The current maximum is %1.\n"
			"If you really need more teachers per activity, please talk to the author").
			arg(MAX_TEACHERS_PER_ACTIVITY));
		return;
	}
	else{
		for(uint i=0; i<selectedTeachersListBox->count(); i++){
			assert(gt.rules.searchTeacher(selectedTeachersListBox->text(i))>=0);
			teachers_names.append(selectedTeachersListBox->text(i));
		}
	}

	//subject
	QString subject_name=subjectsComboBox->currentText();
	int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject"));
		return;
	}

	//subject tag
	QString subject_tag_name=subjectTagsComboBox->currentText();
	int subject_tag_index=gt.rules.searchSubjectTag(subject_tag_name);
	if(subject_tag_index<0 && subject_tag_name!=""){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject tag"));
		return;
	}

	//students
	QStringList students_names;
	if(selectedStudentsListBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid students set(s)"));
		return;
	}
	else{
		for(uint i=0; i<selectedStudentsListBox->count(); i++){
			assert(gt.rules.searchStudentsSet(selectedStudentsListBox->text(i))!=NULL);
			students_names.append(selectedStudentsListBox->text(i));
		}
	}

	if(splitSpinBox->value()==1){ //indivisible activity
		int duration=duration1SpinBox->value();
		if(duration<0){
			QMessageBox::warning(this, QObject::tr("FET information"),
				QObject::tr("Invalid duration"));
			return;
		}
		int parity=PARITY_WEEKLY;
		if(parity1CheckBox->isChecked())
			parity=PARITY_FORTNIGHTLY;

		int preferred_day=preferredDay1ComboBox->currentItem()-1;
		if(preferred_day<-1 || preferred_day>=gt.rules.nDaysPerWeek){
			QMessageBox::warning(this, QObject::tr("FET information"),
				QObject::tr("Invalid preferred day"));
			return;
		}
		int preferred_hour=preferredHour1ComboBox->currentItem()-1;
		if(preferred_hour<-1 || preferred_hour>=gt.rules.nHoursPerDay){
			QMessageBox::warning(this, QObject::tr("FET information"),
				QObject::tr("Invalid preferred hour"));
			return;
		}

		bool active=false;
		if(active1CheckBox->isChecked())
			active=true;

		int activityid=0; //We set the id of this newly added activity = (the largest existing id + 1)
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > activityid)
				activityid = act->id;
		}
		activityid++;
		Activity a(gt.rules, activityid, 0, teachers_names, subject_name, subject_tag_name, students_names,
			duration, duration, parity, active, (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());

		bool already_existing=false;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if((*act)==a)
				already_existing=true;
		}

		if(already_existing){
			int t=QMessageBox::question(this, QObject::tr("FET question"), 
				QObject::tr("This activity already exists. Insert it again?"),
				QObject::tr("Yes"),QObject::tr("No"));
			assert(t==0 || t==1 ||t==-1);
			if(t==1) //no pressed
				return;
			if(t==-1) //Esc pressed
				return;
		}

		bool tmp=gt.rules.addSimpleActivity(activityid, 0, teachers_names, subject_name, subject_tag_name,
			students_names,	duration, duration, parity, active, preferred_day, preferred_hour,
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
		if(tmp)
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Activity added"));
		else
			QMessageBox::critical(this, QObject::tr("FET information"), QObject::tr("Activity NOT added - please report error"));
	}
	else{ //split activity
		int totalduration;
		int durations[8];
		int parities[8];
		int preferred_days[8];
		int preferred_hours[8];
		bool active[8];
		int nsplit=splitSpinBox->value();

		totalduration=0;
		for(int i=0; i<nsplit; i++){
			durations[i]=dur(i)->value();
			parities[i]=PARITY_WEEKLY;
			if(par(i)->isChecked())
				parities[i]=PARITY_FORTNIGHTLY;
			active[i]=false;
			if(activ(i)->isChecked())
				active[i]=true;
			preferred_days[i]=prefDay(i)->currentItem()-1;
			preferred_hours[i]=prefHour(i)->currentItem()-1;

			totalduration+=durations[i];
		}

		//the group id of this split activity and the id of the first partial activity
		//it is the maximum already existing id + 1
		int firstactivityid=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > firstactivityid)
				firstactivityid = act->id;
		}
		firstactivityid++;

		int minD=minDayDistanceSpinBox->value();
		bool tmp=gt.rules.addSplitActivity(firstactivityid, firstactivityid,
			teachers_names, subject_name, subject_tag_name, students_names,
			nsplit, totalduration, durations,
			parities, active, minD, preferred_days, preferred_hours,
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
		if(tmp)
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Split activity added"));
		else
			QMessageBox::critical(this, QObject::tr("FET information"), QObject::tr("Split activity NOT added - error???"));
	}
}

void AddActivityForm::clearTeachers()
{
	selectedTeachersListBox->clear();
	activityChanged();
}

void AddActivityForm::clearStudents()
{
	selectedStudentsListBox->clear();
	activityChanged();
}

#undef prefDay
#undef prefHour
#undef subTab
#undef activ
