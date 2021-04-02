/***************************************************************************
                          modifyactivityform.cpp  -  description
                             -------------------
    begin                : Feb 9, 2005
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

#include "modifyactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include "activityplanningform.h"

#include <QMessageBox>

#include <QList>

/*QWidget* ModifyActivityForm::subTab(int i)
{
	assert(i>=0 && i<subactivitiesTabWidget->count());
	return subactivitiesTabWidget->widget(i);
}*/

QSpinBox* ModifyActivityForm::dur(int i)
{
	assert(i>=0 && i<durList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return durList.at(i);
}

QCheckBox* ModifyActivityForm::activ(int i)
{
	assert(i>=0 && i<activList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return activList.at(i);
}

ModifyActivityForm::ModifyActivityForm(int id, int activityGroupId)
{
    setupUi(this);

	durList.clear();
	durList.append(duration1SpinBox);
	durList.append(duration2SpinBox);
	durList.append(duration3SpinBox);
	durList.append(duration4SpinBox);
	durList.append(duration5SpinBox);
	durList.append(duration6SpinBox);
	durList.append(duration7SpinBox);
	durList.append(duration8SpinBox);
	durList.append(duration9SpinBox);
	durList.append(duration10SpinBox);
	durList.append(duration11SpinBox);
	durList.append(duration12SpinBox);
	durList.append(duration13SpinBox);
	durList.append(duration14SpinBox);
	durList.append(duration15SpinBox);
	durList.append(duration16SpinBox);
	durList.append(duration17SpinBox);
	durList.append(duration18SpinBox);
	durList.append(duration19SpinBox);
	durList.append(duration20SpinBox);
	durList.append(duration21SpinBox);
	durList.append(duration22SpinBox);
	durList.append(duration23SpinBox);
	durList.append(duration24SpinBox);
	durList.append(duration25SpinBox);
	durList.append(duration26SpinBox);
	durList.append(duration27SpinBox);
	durList.append(duration28SpinBox);
	durList.append(duration29SpinBox);
	durList.append(duration30SpinBox);
	durList.append(duration31SpinBox);
	durList.append(duration32SpinBox);
	durList.append(duration33SpinBox);
	durList.append(duration34SpinBox);
	durList.append(duration35SpinBox);
	
	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++)
		dur(i)->setMaxValue(gt.rules.nHoursPerDay);
	
	activList.clear();
	activList.append(active1CheckBox);
	activList.append(active2CheckBox);
	activList.append(active3CheckBox);
	activList.append(active4CheckBox);
	activList.append(active5CheckBox);
	activList.append(active6CheckBox);
	activList.append(active7CheckBox);
	activList.append(active8CheckBox);
	activList.append(active9CheckBox);
	activList.append(active10CheckBox);
	activList.append(active11CheckBox);
	activList.append(active12CheckBox);
	activList.append(active13CheckBox);
	activList.append(active14CheckBox);
	activList.append(active15CheckBox);
	activList.append(active16CheckBox);
	activList.append(active17CheckBox);
	activList.append(active18CheckBox);
	activList.append(active19CheckBox);
	activList.append(active20CheckBox);
	activList.append(active21CheckBox);
	activList.append(active22CheckBox);
	activList.append(active23CheckBox);
	activList.append(active24CheckBox);
	activList.append(active25CheckBox);
	activList.append(active26CheckBox);
	activList.append(active27CheckBox);
	activList.append(active28CheckBox);
	activList.append(active29CheckBox);
	activList.append(active30CheckBox);
	activList.append(active31CheckBox);
	activList.append(active32CheckBox);
	activList.append(active33CheckBox);
	activList.append(active34CheckBox);
	activList.append(active35CheckBox);

    connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(subjectChanged(QString)));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(clearTeacherPushButton, SIGNAL(clicked()), this, SLOT(clearTeachers()));
    connect(clearStudentsPushButton, SIGNAL(clicked()), this, SLOT(clearStudents()));
    connect(allTeachersListBox, SIGNAL(selected(QString)), this, SLOT(addTeacher()));
    connect(selectedTeachersListBox, SIGNAL(selected(QString)), this, SLOT(removeTeacher()));
    connect(allStudentsListBox, SIGNAL(selected(QString)), this, SLOT(addStudents()));
    connect(selectedStudentsListBox, SIGNAL(selected(QString)), this, SLOT(removeStudents()));
    connect(clearActivityTagPushButton, SIGNAL(clicked()), this, SLOT(clearActivityTags()));
    connect(allActivityTagsListBox, SIGNAL(selected(QString)), this, SLOT(addActivityTag()));
    connect(selectedActivityTagsListBox, SIGNAL(selected(QString)), this, SLOT(removeActivityTag()));
    connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showYearsChanged()));
    connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showGroupsChanged()));
    connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	this->_id=id;
	this->_activityGroupId=activityGroupId;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->activityGroupId==this->_activityGroupId && act->id==this->_id)
			this->_activity=act;
	}
			
	this->_teachers=this->_activity->teachersNames;
	this->_subject = this->_activity->subjectName;
	this->_activityTags = this->_activity->activityTagsNames;
	this->_students=this->_activity->studentsNames;
	
	int nSplit;
	
	if(this->_activityGroupId!=0){
		nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->activityGroupId==this->_activityGroupId){
				if(nSplit>=MAX_SPLIT_OF_AN_ACTIVITY){
					assert(0);
				}
				else{
					if(this->_id==act->id)
						subactivitiesTabWidget->setCurrentIndex(nSplit);
					dur(nSplit)->setValue(act->duration);
					activ(nSplit)->setChecked(act->active);
					nSplit++;
				}
			}
		}
	}
	else{
		nSplit=1;
		dur(0)->setValue(this->_activity->duration);
		activ(0)->setChecked(this->_activity->active);
		subactivitiesTabWidget->setCurrentIndex(0);
	}

	splitSpinBox->setMinValue(nSplit);
	splitSpinBox->setMaxValue(nSplit);	
	splitSpinBox->setValue(nSplit);	
	
	nStudentsSpinBox->setMinValue(-1);
	nStudentsSpinBox->setMaxValue(MAX_ROOM_CAPACITY);
	nStudentsSpinBox->setValue(-1);
				
	if(this->_activity->computeNTotalStudents==false)
		nStudentsSpinBox->setValue(this->_activity->nTotalStudents);
	
	updateStudentsListBox();
	updateTeachersListBox();
	updateSubjectsComboBox();
	updateActivityTagsListBox();
	
	selectedStudentsListBox->clear();
	for(QStringList::Iterator it=this->_students.begin(); it!=this->_students.end(); it++)
		selectedStudentsListBox->insertItem(*it);

	for(int i=0; i<MAX_SPLIT_OF_AN_ACTIVITY; i++)
		if(i<nSplit)
			subactivitiesTabWidget->setTabEnabled(i, true);
		else
			subactivitiesTabWidget->setTabEnabled(i, false);
			
	okPushButton->setDefault(true);
	okPushButton->setFocus();
}

ModifyActivityForm::~ModifyActivityForm()
{
}

void ModifyActivityForm::updateTeachersListBox()
{
	allTeachersListBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		allTeachersListBox->insertItem(tch->name);
	}
		
	selectedTeachersListBox->clear();
	for(QStringList::Iterator it=this->_teachers.begin(); it!=this->_teachers.end(); it++)
		selectedTeachersListBox->insertItem(*it);
		
	activityChanged();
}

void ModifyActivityForm::updateSubjectsComboBox()
{
	int i=0, j=-1;
	subjectsComboBox->clear();
	for(int k=0; k<gt.rules.subjectsList.size(); k++, i++){
		Subject* sbj=gt.rules.subjectsList[k];
		subjectsComboBox->insertItem(sbj->name);
		if(sbj->name==this->_subject)
			j=i;
	}
	assert(j!=-1);
	subjectsComboBox->setCurrentItem(j);

	subjectChanged("");
}

void ModifyActivityForm::updateActivityTagsListBox()
{
	allActivityTagsListBox->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListBox->insertItem(at->name);
	}
		
	selectedActivityTagsListBox->clear();
	for(QStringList::Iterator it=this->_activityTags.begin(); it!=this->_activityTags.end(); it++)
		selectedActivityTagsListBox->insertItem(*it);
		
	activityChanged();
}

void ModifyActivityForm::showYearsChanged()
{
	updateStudentsListBox();
}

void ModifyActivityForm::showGroupsChanged()
{
	updateStudentsListBox();
}

void ModifyActivityForm::showSubgroupsChanged()
{
	updateStudentsListBox();
}

void ModifyActivityForm::updateStudentsListBox()
{
	const int INDENT=2;

	bool showYears=showYearsCheckBox->isChecked();
	bool showGroups=showGroupsCheckBox->isChecked();
	bool showSubgroups=showSubgroupsCheckBox->isChecked();

	allStudentsListBox->clear();
	canonicalStudentsSetsNames.clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(showYears){
			allStudentsListBox->insertItem(sty->name);
			canonicalStudentsSetsNames.append(sty->name);
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(showGroups){
				QString begin=QString("");
				QString end=QString("");
				begin=QString(INDENT, ' ');
				if(LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
					end=QString(INDENT, ' ');
				allStudentsListBox->insertItem(begin+stg->name+end);
				canonicalStudentsSetsNames.append(stg->name);
			}
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(showSubgroups){
					QString begin=QString("");
					QString end=QString("");
					begin=QString(2*INDENT, ' ');
					if(LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
						end=QString(2*INDENT, ' ');
					allStudentsListBox->insertItem(begin+sts->name+end);
					canonicalStudentsSetsNames.append(sts->name);
				}
			}
		}
	}

	activityChanged();
}

void ModifyActivityForm::addTeacher()
{
	if(allTeachersListBox->currentItem()<0 || (uint)(allTeachersListBox->currentItem())>=allTeachersListBox->count())
		return;
	
	for(uint i=0; i<selectedTeachersListBox->count(); i++)
		if(selectedTeachersListBox->text(i)==allTeachersListBox->currentText())
			return;
			
	selectedTeachersListBox->insertItem(allTeachersListBox->currentText());

	activityChanged();
}

void ModifyActivityForm::removeTeacher()
{
	if(selectedTeachersListBox->count()<=0 || selectedTeachersListBox->currentItem()<0 ||
	 (uint)(selectedTeachersListBox->currentItem())>=selectedTeachersListBox->count())
		return;
		
	selectedTeachersListBox->removeItem(selectedTeachersListBox->currentItem());

	activityChanged();
}

void ModifyActivityForm::addActivityTag()
{
	if(allActivityTagsListBox->currentItem()<0 || (uint)(allActivityTagsListBox->currentItem())>=allActivityTagsListBox->count())
		return;
	
	for(uint i=0; i<selectedActivityTagsListBox->count(); i++)
		if(selectedActivityTagsListBox->text(i)==allActivityTagsListBox->currentText())
			return;
			
	selectedActivityTagsListBox->insertItem(allActivityTagsListBox->currentText());

	activityChanged();
}

void ModifyActivityForm::removeActivityTag()
{
	if(selectedActivityTagsListBox->count()<=0 || selectedActivityTagsListBox->currentItem()<0 ||
	 (uint)(selectedActivityTagsListBox->currentItem())>=selectedActivityTagsListBox->count())
		return;
		
	selectedActivityTagsListBox->removeItem(selectedActivityTagsListBox->currentItem());

	activityChanged();
}

void ModifyActivityForm::addStudents()
{
	if(allStudentsListBox->currentItem()<0 || (uint)(allStudentsListBox->currentItem())>=allStudentsListBox->count())
		return;
	
	assert(canonicalStudentsSetsNames.count()==(int)(allStudentsListBox->count()));
	QString sn=canonicalStudentsSetsNames.at(allStudentsListBox->currentItem());
	
	for(uint i=0; i<selectedStudentsListBox->count(); i++)
		if(selectedStudentsListBox->text(i)==sn)
			return;
			
	selectedStudentsListBox->insertItem(sn);
	
	activityChanged();
}

void ModifyActivityForm::removeStudents()
{
	if(selectedStudentsListBox->count()<=0 || selectedStudentsListBox->currentItem()<0 ||
	 (uint)(selectedStudentsListBox->currentItem())>=selectedStudentsListBox->count())
		return;
		
	selectedStudentsListBox->removeItem(selectedStudentsListBox->currentItem());

	activityChanged();
}

void ModifyActivityForm::subjectChanged(const QString& dummy)
{
	Q_UNUSED(dummy);
	//if(dummy=="")
	//	;

	activityChanged();
}

void ModifyActivityForm::activityChanged()
{
/*	QString s;
	//s+=tr("Current activity:");
	//s+="\n";
	
	if(selectedTeachersListBox->count()==0){
		if(splitSpinBox->value()==1)
			s+=tr("No teachers for this activity\n");
		else
			s+=tr("No teachers for these activities\n");
	}
	else
		for(uint i=0; i<selectedTeachersListBox->count(); i++){
			s+=tr("Teacher=%1").arg(selectedTeachersListBox->text(i));
			s+="\n";
		}

	s+=tr("Subject=%1").arg(subjectsComboBox->currentText());
	s+="\n";
	if(activityTagsComboBox->currentText()!=""){
		s+=tr("Activity tag=%1").arg(activityTagsComboBox->currentText());
		s+="\n";
	}
	if(selectedStudentsListBox->count()==0){
		if(splitSpinBox->value()==1)
			s+=tr("No students for this activity\n");
		else
			s+=tr("No students for these activities\n");
	}
	else
		for(uint i=0; i<selectedStudentsListBox->count(); i++){
			s+=tr("Students=%1").arg(selectedStudentsListBox->text(i));
			s+="\n";
		}
	
	if(nStudentsSpinBox->value()==-1){
		s+=tr("Number of students: computed from corresponding students sets");
		s+="\n";	
	}
	else{
		s+=tr("Number of students=%1").arg(nStudentsSpinBox->value());
		s+="\n";	
	}

	if(splitSpinBox->value()==1){
		s+=tr("Duration=%1").arg(dur(0)->value());
		s+="\n";
		
		if(activ(0)->isChecked()){
			s+=tr("Active activity");
			s+="\n";
		}
		else{
			s+=tr("Non-active activity");
			s+="\n";
		}
	}
	else{
		s+=tr("This larger activity is split into %1 smaller activities per week").arg(splitSpinBox->value());
		s+="\n";
		s+="\n";

		for(int i=0; i<splitSpinBox->value(); i++){
			s+=tr("Component %1:").arg(i+1);
			s+="\n";
			s+=tr("Duration=%1").arg(dur(i)->value());
			s+="\n";
			if(activ(i)->isChecked()){
				s+=tr("Active activity");
				s+="\n";
			}
			else{
				s+=tr("Non-active activity");
				s+="\n";
			}
			s+="\n";
		}
	}

	currentActivityTextEdit->setText(s);
	*/
}

void ModifyActivityForm::cancel()
{
	this->reject();
}

void ModifyActivityForm::ok()
{
	//teachers
	QStringList teachers_names;
	if(selectedTeachersListBox->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the activity without teacher(s)?"),
		 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
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
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject"));
		return;
	}

	//activity tag
	QStringList activity_tags_names;
	for(uint i=0; i<selectedActivityTagsListBox->count(); i++){
		assert(gt.rules.searchActivityTag(selectedActivityTagsListBox->text(i))>=0);
		activity_tags_names.append(selectedActivityTagsListBox->text(i));
	}

	//students
	QStringList students_names;
	if(selectedStudentsListBox->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the activity without student set(s)?"),
		 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(uint i=0; i<selectedStudentsListBox->count(); i++){
			assert(gt.rules.searchStudentsSet(selectedStudentsListBox->text(i))!=NULL);
			students_names.append(selectedStudentsListBox->text(i));
		}
	}

	int total_number_of_students=0;
	for(QStringList::Iterator it=students_names.begin(); it!=students_names.end(); it++){
		StudentsSet* ss=gt.rules.searchStudentsSet(*it);
		total_number_of_students+=ss->numberOfStudents;
	}

	int totalduration;
	int durations[MAX_SPLIT_OF_AN_ACTIVITY];
	bool active[MAX_SPLIT_OF_AN_ACTIVITY];
	int nsplit=splitSpinBox->value();

	totalduration=0;
	for(int i=0; i<nsplit; i++){
		durations[i]=dur(i)->value();
		active[i]=activ(i)->isChecked();

		totalduration+=durations[i];
	}

	if(nStudentsSpinBox->value()==-1){
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, nsplit, totalduration, durations, /*parities,*/ active,
		 (nStudentsSpinBox->value()==-1), total_number_of_students);
	}
	else{
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 activity_tags_names,students_names, nsplit, totalduration, durations, /*parities,*/ active,
		 (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
	}
	
	PlanningChanged::increasePlanningCommunicationSpinBox();
	
	this->accept();
}

void ModifyActivityForm::clearTeachers()
{
	selectedTeachersListBox->clear();
	activityChanged();
}

void ModifyActivityForm::clearStudents()
{
	selectedStudentsListBox->clear();
	activityChanged();
}

void ModifyActivityForm::clearActivityTags()
{
	selectedActivityTagsListBox->clear();
	activityChanged();
}
