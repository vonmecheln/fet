/***************************************************************************
                          addconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.cpp  -  description
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

#include "addconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h"
#include "timeconstraint.h"

AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);

	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
}

AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::~AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
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
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid teacher"));
		return;
	}

	ctr=new ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3(weight, teacher_name);

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

void AddConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form::addCurrentConstraints()
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
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3(weight, tch->name);
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
