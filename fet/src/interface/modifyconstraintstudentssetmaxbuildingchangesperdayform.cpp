/***************************************************************************
                          modifyconstraintstudentssetmaxbuildingchangesperdayform.cpp  -  description
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

#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm(ConstraintStudentsSetMaxBuildingChangesPerDay* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm_template*/, SLOT(cancel()));
//    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm_template*/, SLOT(constraintChanged()));
//    connect(maxChangesSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateStudentsComboBox();

	maxChangesSpinBox->setMinValue(0);
	maxChangesSpinBox->setMaxValue(10);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerDay);	
		
	constraintChanged();
}

ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::~ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm()
{
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::updateStudentsComboBox(){
	studentsComboBox->clear();
	int i=0, j=-1;
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->insertItem(sty->name);
		if(sty->name==this->_ctr->studentsName)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->insertItem(stg->name);
			if(stg->name==this->_ctr->studentsName)
				j=i;
			i++;
			for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->insertItem(sts->name);
				if(sts->name==this->_ctr->studentsName)
					j=i;
				i++;
			}
		}
	}
	assert(j>=0);
	studentsComboBox->setCurrentItem(j);																

	constraintChanged();
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr(QString("Weight (percentage)=%1\%").arg(weight));
	s+="\n";

	s+=tr("Students set max building changes per day");
	s+="\n";
	s+=tr("Students set=%1").arg(studentsComboBox->currentText());
	s+="\n";
	
	s+=tr("Max building changes per day=%1").arg(maxChangesSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==NULL){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->studentsName=students_name;
	this->_ctr->maxBuildingChangesPerDay=maxChangesSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm::cancel()
{
	this->close();
}
