/***************************************************************************
                          modifyconstraintactivitiesbeginstudentsdayform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "modifyconstraintactivitiesbeginstudentsdayform.h"
#include "timeconstraint.h"

ModifyConstraintActivitiesBeginStudentsDayForm::ModifyConstraintActivitiesBeginStudentsDayForm(QWidget* parent, ConstraintActivitiesBeginStudentsDay* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;

	updateTeachersComboBox();
	updateStudentsComboBox(parent);
	updateSubjectsComboBox();
	updateActivityTagsComboBox();

	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
}

ModifyConstraintActivitiesBeginStudentsDayForm::~ModifyConstraintActivitiesBeginStudentsDayForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivitiesBeginStudentsDayForm::updateTeachersComboBox(){
	int i=0, j=-1;
	teachersComboBox->clear();
	teachersComboBox->addItem("");
	if(this->_ctr->teacherName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.teachersList.size(); k++){
		Teacher* t=gt.rules.teachersList[k];
		teachersComboBox->addItem(t->name);
		if(t->name==this->_ctr->teacherName)
			j=i;
		i++;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivitiesBeginStudentsDayForm::updateStudentsComboBox(QWidget* parent){
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->studentsName, true);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->studentsName);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivitiesBeginStudentsDayForm::updateSubjectsComboBox(){
	int i=0, j=-1;
	subjectsComboBox->clear();
	subjectsComboBox->addItem("");
	if(this->_ctr->subjectName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* s=gt.rules.subjectsList[k];
		subjectsComboBox->addItem(s->name);
		if(s->name==this->_ctr->subjectName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivitiesBeginStudentsDayForm::updateActivityTagsComboBox(){
	int i=0, j=-1;
	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	if(this->_ctr->activityTagName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.activityTagsList.size(); k++){
		ActivityTag* s=gt.rules.activityTagsList[k];
		activityTagsComboBox->addItem(s->name);
		if(s->name==this->_ctr->activityTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentIndex(j);
}

void ModifyConstraintActivitiesBeginStudentsDayForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->studentsName);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString teacher=teachersComboBox->currentText();
	if(teacher!="")
		assert(gt.rules.searchTeacher(teacher)>=0);

	QString students=studentsComboBox->currentText();
	if(students!="")
		assert(gt.rules.searchStudentsSet(students)!=nullptr);

	QString subject=subjectsComboBox->currentText();
	if(subject!="")
		assert(gt.rules.searchSubject(subject)>=0);
		
	QString activityTag=activityTagsComboBox->currentText();
	if(activityTag!="")
		assert(gt.rules.searchActivityTag(activityTag)>=0);
		
	this->_ctr->weightPercentage=weight;
	this->_ctr->teacherName=teacher;
	this->_ctr->studentsName=students;
	this->_ctr->subjectName=subject;
	this->_ctr->activityTagName=activityTag;

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintActivitiesBeginStudentsDayForm::cancel()
{
	this->close();
}
