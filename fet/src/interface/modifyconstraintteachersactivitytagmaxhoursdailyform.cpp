/***************************************************************************
                          modifyconstraintteachersactivitytagmaxhoursdailyform.cpp  -  description
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

#include "modifyconstraintteachersactivitytagmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeachersActivityTagMaxHoursDailyForm::ModifyConstraintTeachersActivityTagMaxHoursDailyForm(ConstraintTeachersActivityTagMaxHoursDaily* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm_template*/, SLOT(cancel()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);

	activityTagsComboBox->clear();
	int j=-1;
	for(int k=0; k<gt.rules.activityTagsList.count(); k++){
		ActivityTag* at=gt.rules.activityTagsList.at(k);
		activityTagsComboBox->insertItem(at->name);
		if(at->name==this->_ctr->activityTagName)
			j=k;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);

	constraintChanged();
}

ModifyConstraintTeachersActivityTagMaxHoursDailyForm::~ModifyConstraintTeachersActivityTagMaxHoursDailyForm()
{
}

void ModifyConstraintTeachersActivityTagMaxHoursDailyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeachersActivityTagMaxHoursDailyForm::constraintChanged()
{
	//nothing
}

void ModifyConstraintTeachersActivityTagMaxHoursDailyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int max_hours=maxHoursSpinBox->value();

	QString activityTagName=activityTagsComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxHoursDaily=max_hours;
	this->_ctr->activityTagName=activityTagName;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersActivityTagMaxHoursDailyForm::cancel()
{
	this->close();
}
