/***************************************************************************
                          modifyconstraintstudentsactivitytagmaxhoursdailyform.cpp  -  description
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

#include "modifyconstraintstudentsactivitytagmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintStudentsActivityTagMaxHoursDailyForm::ModifyConstraintStudentsActivityTagMaxHoursDailyForm(ConstraintStudentsActivityTagMaxHoursDaily* ctr)
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
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);

	constraintChanged();
}

ModifyConstraintStudentsActivityTagMaxHoursDailyForm::~ModifyConstraintStudentsActivityTagMaxHoursDailyForm()
{
}

void ModifyConstraintStudentsActivityTagMaxHoursDailyForm::updateActivityTagsComboBox()
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

void ModifyConstraintStudentsActivityTagMaxHoursDailyForm::constraintChanged()
{
	//nothing
}

void ModifyConstraintStudentsActivityTagMaxHoursDailyForm::ok()
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
	this->_ctr->maxHoursDaily=maxHoursSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsActivityTagMaxHoursDailyForm::cancel()
{
	this->close();
}
