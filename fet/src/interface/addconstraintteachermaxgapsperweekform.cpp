/***************************************************************************
                          addconstraintteachersmaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : July 6, 2007
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintteachermaxgapsperweekform.h"
#include "timeconstraint.h"

AddConstraintTeacherMaxGapsPerWeekForm::AddConstraintTeacherMaxGapsPerWeekForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeacherMaxGapsPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxGapsPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxGapsPerWeekForm_template*/, SLOT(close()));
//    connect(maxGapsSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeacherMaxGapsPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherMaxGapsPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);

	maxGapsSpinBox->setMinValue(0);
	//maxGapsSpinBox->setMaxValue(gt.rules.nHoursPerWeek);
	maxGapsSpinBox->setMaxValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(3);

	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}

	constraintChanged();
}

AddConstraintTeacherMaxGapsPerWeekForm::~AddConstraintTeacherMaxGapsPerWeekForm()
{
}

void AddConstraintTeacherMaxGapsPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=tr("Max gaps=%1").arg(maxGapsSpinBox->value());
	s+="\n";

	s+=tr("Teacher max gaps per week");
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeacherMaxGapsPerWeekForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	ctr=new ConstraintTeacherMaxGapsPerWeek(weight, teacher_name, maxGapsSpinBox->value());

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
