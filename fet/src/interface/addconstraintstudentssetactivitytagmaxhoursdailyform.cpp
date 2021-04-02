/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhoursdailyform.cpp  -  description
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

#include "addconstraintstudentssetactivitytagmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <QDesktopWidget>

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

AddConstraintStudentsSetActivityTagMaxHoursDailyForm::AddConstraintStudentsSetActivityTagMaxHoursDailyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(close()));
//    connect(studentsComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
//    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);

	updateStudentsSetComboBox();
	
	updateActivityTagsComboBox();
}

AddConstraintStudentsSetActivityTagMaxHoursDailyForm::~AddConstraintStudentsSetActivityTagMaxHoursDailyForm()
{
}

void AddConstraintStudentsSetActivityTagMaxHoursDailyForm::updateStudentsSetComboBox()
{
	studentsComboBox->clear();	
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}

	constraintChanged();
}

void AddConstraintStudentsSetActivityTagMaxHoursDailyForm::updateActivityTagsComboBox()
{
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagsComboBox->insertItem(at->name);

	constraintChanged();
}

void AddConstraintStudentsSetActivityTagMaxHoursDailyForm::constraintChanged()
{
	//nothing
}

void AddConstraintStudentsSetActivityTagMaxHoursDailyForm::addCurrentConstraint()
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid students set"));
		return;
	}
	
	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	ctr=new ConstraintStudentsSetActivityTagMaxHoursDaily(weight, maxHours, students_name, activityTagName);

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
