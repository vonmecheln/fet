/***************************************************************************
                          modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm(ConstraintStudentsActivityTagMaxHoursContinuously* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(cancel()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
//    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateActivityTagsComboBox();

	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(ctr->maxHoursContinuously);

	constraintChanged();
}

ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::~ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
}

void ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::updateActivityTagsComboBox()
{
	activityTagsComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		activityTagsComboBox->insertItem(at->name);
		if(at->name==this->_ctr->activityTagName)
			j=i;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);
	
	constraintChanged();
}

void ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students activity tag max hours continuously");
	s+="\n";
	s+=tr("Activity tag=%1").arg(activityTagsComboBox->currentText());
	s+="\n";

	s+=tr("Max hours:%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage)"));
		return;
	}

	QString activityTagName=activityTagsComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->activityTagName=activityTagName;
	this->_ctr->maxHoursContinuously=maxHoursSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm::cancel()
{
	this->close();
}
