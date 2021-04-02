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

#include <qlabel.h>
#include <qtabwidget.h>

#include <QDesktopWidget>

#define subTab(i)	subactivitiesTabWidget->page(i)
#define dur(i)		(i==0?duration1SpinBox:			\
			(i==1?duration2SpinBox:					\
			(i==2?duration3SpinBox:					\
			(i==3?duration4SpinBox:					\
			(i==4?duration5SpinBox:					\
			(i==5?duration6SpinBox:					\
			(i==6?duration7SpinBox:					\
			(i==7?duration8SpinBox:					\
			(i==8?duration9SpinBox:					\
			(duration10SpinBox))))))))))
#define activ(i)		(i==0?active1CheckBox:			\
			(i==1?active2CheckBox:					\
			(i==2?active3CheckBox:					\
			(i==3?active4CheckBox:					\
			(i==4?active5CheckBox:					\
			(i==5?active6CheckBox:					\
			(i==6?active7CheckBox:					\
			(i==7?active8CheckBox:					\
			(i==8?active9CheckBox:					\
			(active10CheckBox))))))))))

ModifyActivityForm::ModifyActivityForm(int id, int activityGroupId)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_id=id;
	this->_activityGroupId=activityGroupId;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->activityGroupId==this->_activityGroupId && act->id==this->_id)
			this->_activity=act;
	}
			
	this->_teachers=this->_activity->teachersNames;
	this->_subject = this->_activity->subjectName;
	this->_subjectTag = this->_activity->subjectTagName;
	this->_students=this->_activity->studentsNames;
	
	int nSplit;
	
	if(this->_activityGroupId!=0){
		nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->activityGroupId==this->_activityGroupId){
				if(nSplit>=10){
					assert(0);
				}
				else{
					if(this->_id==act->id)
						subactivitiesTabWidget->setCurrentPage(nSplit);
					//par(nSplit)->setChecked(act->parity==PARITY_FORTNIGHTLY);
					dur(nSplit)->setValue(act->duration);
					activ(nSplit)->setChecked(act->active);
					nSplit++;
				}
			}
		}
	}
	else{
		nSplit=1;
		//par(0)->setChecked(this->_activity->parity==PARITY_FORTNIGHTLY);
		dur(0)->setValue(this->_activity->duration);
		activ(0)->setChecked(this->_activity->active);
	}

	splitSpinBox->setMinValue(nSplit);
	splitSpinBox->setMaxValue(nSplit);	
	splitSpinBox->setValue(nSplit);	
	
	if(nSplit==1)
		currentActivityTextLabel->setText(tr("Current activity"));
	else
		currentActivityTextLabel->setText(tr("Current activities"));
	
	if(this->_activity->computeNTotalStudents==false)
		nStudentsSpinBox->setValue(this->_activity->nTotalStudents);
	
	updateStudentsListBox();
	updateTeachersListBox();
	updateSubjectsComboBox();
	updateSubjectTagsComboBox();

	for(int i=0; i<10; i++)
		if(i<nSplit)
			subTab(i)->setEnabled(true);
		else
			subTab(i)->setDisabled(true);
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

void ModifyActivityForm::updateSubjectTagsComboBox()
{
	int i=0, j=-1;
	subjectTagsComboBox->clear();
	subjectTagsComboBox->insertItem("");
	if(this->_subjectTag=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectTagsList.size(); k++, i++){
		SubjectTag* sbt=gt.rules.subjectTagsList[k];
		subjectTagsComboBox->insertItem(sbt->name);
		if(sbt->name==this->_subjectTag)
			j=i;
	}
	assert(j!=-1);
	subjectTagsComboBox->setCurrentItem(j);

	subjectTagChanged("");
}

void ModifyActivityForm::updateStudentsListBox()
{
	allStudentsListBox->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		allStudentsListBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			allStudentsListBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				allStudentsListBox->insertItem(sts->name);
			}
		}
	}
	
	selectedStudentsListBox->clear();
	for(QStringList::Iterator it=this->_students.begin(); it!=this->_students.end(); it++)
		selectedStudentsListBox->insertItem(*it);

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

void ModifyActivityForm::addStudents()
{
	if(allStudentsListBox->currentItem()<0 || (uint)(allStudentsListBox->currentItem())>=allStudentsListBox->count())
		return;
	
	for(uint i=0; i<selectedStudentsListBox->count(); i++)
		if(selectedStudentsListBox->text(i)==allStudentsListBox->currentText())
			return;
			
	selectedStudentsListBox->insertItem(allStudentsListBox->currentText());
	
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

void ModifyActivityForm::subjectTagChanged(const QString& dummy)
{
	Q_UNUSED(dummy);
	//if(dummy=="")
	//	;

	activityChanged();
}

void ModifyActivityForm::activityChanged()
{
	QString s;
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
	if(subjectTagsComboBox->currentText()!=""){
		s+=tr("Subject tag=%1").arg(subjectTagsComboBox->currentText());
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
		/*if(par(0)->isChecked()){
			s+=tr("Fortnightly activity");
			s+="\n";
		}*/
		
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
			/*if(par(i)->isChecked()){
				s+=tr("Fortnightly activity");
				s+="\n";
			}*/
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
}

void ModifyActivityForm::cancel()
{
	this->close();
}

void ModifyActivityForm::ok()
{
	//teachers
	QStringList teachers_names;
	if(selectedTeachersListBox->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the activity with no teacher(s)?"),
		 QMessageBox::Yes, QMessageBox::Cancel);

		if(t==QMessageBox::Cancel)
			return;
	}
	/*else if(selectedTeachersListBox->count()>(uint)(MAX_TEACHERS_PER_ACTIVITY)){
		QMessageBox::warning(this, tr("FET information"),
			tr("Too many teachers for an activity. The current maximum is %1.\n"
			"If you really need more teachers per activity, please talk to the author").
			arg(MAX_TEACHERS_PER_ACTIVITY));
		return;
	}*/
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

	//subject tag
	QString subject_tag_name=subjectTagsComboBox->currentText();
	int subject_tag_index=gt.rules.searchSubjectTag(subject_tag_name);
	if(subject_tag_index<0 && subject_tag_name!=""){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject tag"));
		return;
	}

	//students
	QStringList students_names;
	if(selectedStudentsListBox->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to have the activity with no student set(s)?"),
		 QMessageBox::Yes, QMessageBox::Cancel);

		if(t==QMessageBox::Cancel)
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
	int durations[10];
	//int parities[8];
	bool active[10];
	int nsplit=splitSpinBox->value();

	totalduration=0;
	for(int i=0; i<nsplit; i++){
		durations[i]=dur(i)->value();
		//parities[i]=PARITY_WEEKLY;
		//if(par(i)->isChecked())
		//	parities[i]=PARITY_FORTNIGHTLY;
		active[i]=activ(i)->isChecked();

		totalduration+=durations[i];
	}

	if(nStudentsSpinBox->value()==-1){
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 subject_tag_name,students_names, nsplit, totalduration, durations, /*parities,*/ active,
		 (nStudentsSpinBox->value()==-1), total_number_of_students);
	}
	else{
		gt.rules.modifyActivity(this->_id, this->_activityGroupId, teachers_names, subject_name,
		 subject_tag_name,students_names, nsplit, totalduration, durations, /*parities,*/ active,
		 (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value());
	}
	
	this->close();
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

//#undef prefDay
//#undef prefHour
#undef subTab
#undef activ
#undef dur
