/***************************************************************************
                          modifyconstraintteachermaxbuildingchangesperweekform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "modifyconstraintteachermaxbuildingchangesperweekform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::ModifyConstraintTeacherMaxBuildingChangesPerWeekForm(ConstraintTeacherMaxBuildingChangesPerWeek* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(cancel()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxChangesSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
		
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateTeachersComboBox();

	maxChangesSpinBox->setMinValue(0);
	maxChangesSpinBox->setMaxValue(100);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerWeek);	
		
	constraintChanged();
}

ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::~ModifyConstraintTeacherMaxBuildingChangesPerWeekForm()
{
}

void ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::updateTeachersComboBox()
{
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->insertItem(tch->name);
		if(tch->name==this->_ctr->teacherName)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentItem(j);

	constraintChanged();
}

void ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr(QString("Weight (percentage)=%1\%").arg(weight));
	s+="\n";

	s+=tr("Teacher max building changes per week");
	s+="\n";
	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";
	
	s+=tr("Max building changes per week=%1").arg(maxChangesSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int t=gt.rules.searchTeacher(teacher_name);
	if(t==-1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->teacherName=teacher_name;
	this->_ctr->maxBuildingChangesPerWeek=maxChangesSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeacherMaxBuildingChangesPerWeekForm::cancel()
{
	this->close();
}
