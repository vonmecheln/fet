/***************************************************************************
                          modifyconstraintteacherssubjecttagmaxhourscontinuouslyform.cpp  -  description
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

#include "modifyconstraintteacherssubjecttagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

#include <QDesktopWidget>

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm(ConstraintTeachersSubjectTagMaxHoursContinuously* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weight));
	
	int i=0, j=-1;
	for(int k=0; k<gt.rules.subjectTagsList.size(); k++){
		SubjectTag* s=gt.rules.subjectTagsList[k];
		subjectTagsComboBox->insertItem(s->name);
		if(s->name==ctr->subjectTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectTagsComboBox->setCurrentItem(j);
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursContinuously);
}

ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::~ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm()
{
}

void ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight=%1").arg(weight);
	s+="\n";

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";

	s+=QObject::tr("Teachers subject tag max hours continuously ");
	s+="\n";

	s+=QObject::tr("Max hours continuously=%1").arg(maxHoursSpinBox->value());
	s+="\n";

	s+=QObject::tr("Subject tag=%1").arg(subjectTagsComboBox->currentText());
	s+="\n";

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	if(subjectTagsComboBox->currentItem()<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid subject tag"));
		return;
	}

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;

	int max_hours=maxHoursSpinBox->value();

	this->_ctr->weight=weight;
	this->_ctr->compulsory=compulsory;
	this->_ctr->maxHoursContinuously=max_hours;
	this->_ctr->subjectTagName=subjectTagsComboBox->currentText();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersSubjectTagMaxHoursContinuouslyForm::cancel()
{
	this->close();
}
