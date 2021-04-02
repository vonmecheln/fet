/***************************************************************************
                          addconstraintteachermaxhourscontinuouslyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
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

#include "longtextmessagebox.h"

#include "addconstraintteachermaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTeacherMaxHoursContinuouslyForm::AddConstraintTeacherMaxHoursContinuouslyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(close()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateMaxHoursSpinBox();

	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}

	constraintChanged();
}

AddConstraintTeacherMaxHoursContinuouslyForm::~AddConstraintTeacherMaxHoursContinuouslyForm()
{
}

void AddConstraintTeacherMaxHoursContinuouslyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);
}

void AddConstraintTeacherMaxHoursContinuouslyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher max hours continuously");
	s+="\n";

	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=tr("Max hours continuously=%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeacherMaxHoursContinuouslyForm::addCurrentConstraint()
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
	/*if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}*/

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

	int max_hours=maxHoursSpinBox->value();

	ctr=new ConstraintTeacherMaxHoursContinuously(weight, /*compulsory,*/ max_hours, teacher_name);

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
