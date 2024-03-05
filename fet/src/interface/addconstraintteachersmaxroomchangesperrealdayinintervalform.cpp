/***************************************************************************
                          addconstraintteachersmaxroomchangesperrealdayinintervalform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#include "addconstraintteachersmaxroomchangesperrealdayinintervalform.h"

AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(0);

	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
	startHourComboBox->setCurrentIndex(0);

	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
	endHourComboBox->addItem(tr("End of real day"));
	endHourComboBox->setCurrentIndex(2);
}

AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::~AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::addCurrentConstraint()
{
	SpaceConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	int startHour=startHourComboBox->currentIndex();
	int endHour=endHourComboBox->currentIndex();
	if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour invalid"));
		return;
	}
	if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour invalid"));
		return;
	}
	if(endHour-startHour<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
		return;
	}

	ctr=new ConstraintTeachersMaxRoomChangesPerRealDayInInterval(weight, maxChangesSpinBox->value(), startHour, endHour);

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
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

void AddConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm::addCurrentConstraints()
{
	QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
	 tr("This operation will add multiple constraints, one for each teacher. Do you want to continue?"),
	 QMessageBox::Cancel | QMessageBox::Yes);
	if(res==QMessageBox::Cancel)
		return;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	int startHour=startHourComboBox->currentIndex();
	int endHour=endHourComboBox->currentIndex();
	if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour invalid"));
		return;
	}
	if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour invalid"));
		return;
	}
	if(endHour-startHour<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
		return;
	}

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		SpaceConstraint *ctr=new ConstraintTeacherMaxRoomChangesPerRealDayInInterval(weight, tch->name, maxChangesSpinBox->value(), startHour, endHour);
		bool tmp2=gt.rules.addSpaceConstraint(ctr);
		assert(tmp2);

		ctrs+=ctr->getDetailedDescription(gt.rules);
		ctrs+="\n";
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 space constraints. Please note that these constraints"
	 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
						  .arg(gt.rules.teachersList.count()).arg(ctrs));
}
