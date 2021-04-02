/***************************************************************************
                          addconstraintstudentsactivitytagmaxhoursdailyform.cpp  -  description
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

#include "addconstraintstudentsactivitytagmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <QDesktopWidget>

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

AddConstraintStudentsActivityTagMaxHoursDailyForm::AddConstraintStudentsActivityTagMaxHoursDailyForm()
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

AddConstraintStudentsActivityTagMaxHoursDailyForm::~AddConstraintStudentsActivityTagMaxHoursDailyForm()
{
}

void AddConstraintStudentsActivityTagMaxHoursDailyForm::updateActivityTagsComboBox()
{
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagsComboBox->insertItem(at->name);

	constraintChanged();
}

void AddConstraintStudentsActivityTagMaxHoursDailyForm::constraintChanged()
{
	//nothing
}

void AddConstraintStudentsActivityTagMaxHoursDailyForm::addCurrentConstraint()
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

	ctr=new ConstraintStudentsActivityTagMaxHoursDaily(weight, maxHours, activityTagName);

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
