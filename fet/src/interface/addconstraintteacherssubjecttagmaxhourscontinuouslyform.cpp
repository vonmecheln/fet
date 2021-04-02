/***************************************************************************
                          addconstraintteacherssubjecttagmaxhourscontinuouslyform.cpp  -  description
                             -------------------
    begin                : Apr 29, 2005
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

#include "addconstraintteacherssubjecttagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm::AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
						
	updateMaxHoursSpinBox();
	
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* s=gt.rules.subjectTagsList[i];
		subjectTagsComboBox->insertItem(s->name);
	}
}

AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm::~AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm()
{
}

void AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);
}

void AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr(QString("Weight=%1").arg(weight));
	s+="\n";

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";

	s+=QObject::tr("Teachers subject tag max hours continuously");
	s+="\n";

	s+=QObject::tr("Max hours continuously=%1").arg(maxHoursSpinBox->value());
	s+="\n";
	
	if(subjectTagsComboBox->currentItem()<0)
		s+=QObject::tr("Invalid subject tag");
	else
		s+=QObject::tr("Subject tag=%1").arg(subjectTagsComboBox->currentText());
	s+="\n";

	currentConstraintTextEdit->setText(s);
}

void AddConstraintTeachersSubjectTagMaxHoursContinuouslyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;

	int max_hours=maxHoursSpinBox->value();
	
	if(subjectTagsComboBox->currentItem()<0 || subjectTagsComboBox->count()<=0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject tag"));
		return;
	}

	ctr=new ConstraintTeachersSubjectTagMaxHoursContinuously(weight, compulsory, max_hours, subjectTagsComboBox->currentText());

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Constraint added"));
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
