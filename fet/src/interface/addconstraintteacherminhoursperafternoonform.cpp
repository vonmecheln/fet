/***************************************************************************
                          addconstraintteacherminhoursperafternoonform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintteacherminhoursperafternoonform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinHoursPerAfternoonForm::AddConstraintTeacherMinHoursPerAfternoonForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateMinHoursSpinBox();

	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
}

AddConstraintTeacherMinHoursPerAfternoonForm::~AddConstraintTeacherMinHoursPerAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMinHoursPerAfternoonForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
}

void AddConstraintTeacherMinHoursPerAfternoonForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	
	if(!allowEmptyAfternoonsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for a teacher, "
			"please use the constraint teacher min afternoons per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeacherMinHoursPerAfternoon(weight, min_hours, teacher_name, allowEmptyAfternoonsCheckBox->isChecked());

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

void AddConstraintTeacherMinHoursPerAfternoonForm::on_allowEmptyAfternoonsCheckBox_toggled()
{
	bool k=allowEmptyAfternoonsCheckBox->isChecked();

	if(!k){
		allowEmptyAfternoonsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for this teacher,"
			" please use constraint teacher min afternoons per week."));
	}
}