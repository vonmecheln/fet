/***************************************************************************
                          addconstraintteacherminhoursdailyrealdaysform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "addconstraintteacherminhoursdailyrealdaysform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinHoursDailyRealDaysForm::AddConstraintTeacherMinHoursDailyRealDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinHoursDailyRealDaysForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinHoursDailyRealDaysForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinHoursDailyRealDaysForm::close);

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

	connect(allowEmptyDaysCheckBox, &QCheckBox::toggled, this, &AddConstraintTeacherMinHoursDailyRealDaysForm::allowEmptyDaysCheckBox_toggled);
	
	allowEmptyDaysCheckBox_toggled();
}

AddConstraintTeacherMinHoursDailyRealDaysForm::~AddConstraintTeacherMinHoursDailyRealDaysForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMinHoursDailyRealDaysForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
}

void AddConstraintTeacherMinHoursDailyRealDaysForm::addCurrentConstraint()
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
	
	if(!allowEmptyDaysCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
			"please use the constraint teacher min days per week"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeacherMinHoursDailyRealDays(weight, min_hours, teacher_name, allowEmptyDaysCheckBox->isChecked());

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintTeacherMinHoursDailyRealDaysForm::addCurrentConstraints()
{
	QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
	 tr("This operation will add multiple constraints, one for each teacher, regardless of the one selected in the combo box. Do you want to continue?"),
	 QMessageBox::Cancel | QMessageBox::Yes);
	if(res==QMessageBox::Cancel)
		return;

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

	if(!allowEmptyDaysCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
			"please use the constraint teachers min days per week"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMinHoursDailyRealDays(weight, min_hours, tch->name, allowEmptyDaysCheckBox->isChecked());
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);

		ctrs+=ctr->getDetailedDescription(gt.rules);
		ctrs+="\n";
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
						  .arg(gt.rules.teachersList.count()).arg(ctrs));
}

void AddConstraintTeacherMinHoursDailyRealDaysForm::allowEmptyDaysCheckBox_toggled()
{
	bool k=allowEmptyDaysCheckBox->isChecked();

	if(!k){
		allowEmptyDaysCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
			" please use constraint teacher min days per week"));
	}
}
