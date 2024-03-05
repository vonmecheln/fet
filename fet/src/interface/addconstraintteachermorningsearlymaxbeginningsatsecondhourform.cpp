/***************************************************************************
                          addconstraintteachermorningsearlymaxbeginningsatsecondhourform.cpp  -  description
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

#include "addconstraintteachermorningsearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teachersComboBox->addItem(tch->name);

	maxBeginningsSpinBox->setMinimum(0);
	maxBeginningsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	maxBeginningsSpinBox->setValue(gt.rules.nDaysPerWeek/2);
}

AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::~AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int i=gt.rules.searchTeacher(teacher_name);
	if(i<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	ctr=new ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour(weight, maxBeginningsSpinBox->value(), teacher_name);

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

void AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraints()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour(weight, maxBeginningsSpinBox->value(), tch->name);
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
