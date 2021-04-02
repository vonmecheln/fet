/***************************************************************************
                          modifyconstraintteacherminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "modifyconstraintteacherminhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeacherMinHoursDailyForm::ModifyConstraintTeacherMinHoursDailyForm(ConstraintTeacherMinHoursDaily* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeacherMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMinHoursDailyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMinHoursDailyForm_template*/, SLOT(cancel()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeacherMinHoursDailyForm_template*/, SLOT(constraintChanged()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintTeacherMinHoursDailyForm_template*/, SLOT(constraintChanged()));


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
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursDaily);

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

ModifyConstraintTeacherMinHoursDailyForm::~ModifyConstraintTeacherMinHoursDailyForm()
{
}

void ModifyConstraintTeacherMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinValue(2);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeacherMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);

	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher min hours daily ");
	s+="\n";

	s+=tr("Min hours daily=%1").arg(minHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeacherMinHoursDailyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursDaily=min_hours;
	this->_ctr->teacherName=teacher_name;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeacherMinHoursDailyForm::cancel()
{
	this->close();
}
