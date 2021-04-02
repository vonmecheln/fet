/***************************************************************************
                          addconstraintstudentsactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <QDesktopWidget>

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

AddConstraintStudentsActivityTagMaxHoursContinuouslyForm::AddConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(constraintChanged()));
//    connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);

	updateActivityTagsComboBox();
}

AddConstraintStudentsActivityTagMaxHoursContinuouslyForm::~AddConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
}

void AddConstraintStudentsActivityTagMaxHoursContinuouslyForm::updateActivityTagsComboBox()
{
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagsComboBox->insertItem(at->name);

	constraintChanged();
}

void AddConstraintStudentsActivityTagMaxHoursContinuouslyForm::constraintChanged()
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

	int maxHours=maxHoursSpinBox->value();
	if(maxHours>=0){
		s+=tr("Max. hours:%1").arg(maxHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsActivityTagMaxHoursContinuouslyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int maxHours=maxHoursSpinBox->value();

	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	ctr=new ConstraintStudentsActivityTagMaxHoursContinuously(weight, maxHours, activityTagName);

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
