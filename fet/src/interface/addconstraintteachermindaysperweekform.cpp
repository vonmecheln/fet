/***************************************************************************
                          addconstraintteachermindaysperweekform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintteachermindaysperweekform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinDaysPerWeekForm::AddConstraintTeacherMinDaysPerWeekForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeacherMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxDaysPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxDaysPerWeekForm_template*/, SLOT(close()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxDaysSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeacherMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateMinDaysSpinBox();
	updateTeachersComboBox();
}

AddConstraintTeacherMinDaysPerWeekForm::~AddConstraintTeacherMinDaysPerWeekForm()
{
}

void AddConstraintTeacherMinDaysPerWeekForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}

	//constraintChanged();
}

void AddConstraintTeacherMinDaysPerWeekForm::updateMinDaysSpinBox(){
	minDaysSpinBox->setMinValue(1);
	minDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	minDaysSpinBox->setValue(1);
}

void AddConstraintTeacherMinDaysPerWeekForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

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
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int min_days=minDaysSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	ctr=new ConstraintTeacherMinDaysPerWeek(weight, /*compulsory,*/ min_days, teacher_name);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
