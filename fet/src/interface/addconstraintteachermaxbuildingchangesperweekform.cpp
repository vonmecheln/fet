/***************************************************************************
                          addconstraintteachermaxbuildingchangesperweekform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintteachermaxbuildingchangesperweekform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTeacherMaxBuildingChangesPerWeekForm::AddConstraintTeacherMaxBuildingChangesPerWeekForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(close()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));
//    connect(maxChangesSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeacherMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
		
	maxChangesSpinBox->setMinValue(0);
	maxChangesSpinBox->setMaxValue(100);
	maxChangesSpinBox->setValue(3);

	updateTeachersComboBox();
}

AddConstraintTeacherMaxBuildingChangesPerWeekForm::~AddConstraintTeacherMaxBuildingChangesPerWeekForm()
{
}

void AddConstraintTeacherMaxBuildingChangesPerWeekForm::updateTeachersComboBox()
{
	teachersComboBox->clear();
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersComboBox->insertItem(tch->name);

	constraintChanged();
}

void AddConstraintTeacherMaxBuildingChangesPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher max building changes per week");
	s+="\n";
	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";
	
	s+=tr("Max building changes per week=%1").arg(maxChangesSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeacherMaxBuildingChangesPerWeekForm::addCurrentConstraint()
{
	SpaceConstraint *ctr=NULL;

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

	ctr=new ConstraintTeacherMaxBuildingChangesPerWeek(weight, teacher_name, maxChangesSpinBox->value());

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
