/***************************************************************************
                          addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.cpp  -  description
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

#include "addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h"

#include "timetable.h"
#include "fet.h"

#include "longtextmessagebox.h"

#include <tuple>

#include <QMap>

#include <QMessageBox>

AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm::AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	connect(addAllConstraintsPushButton, SIGNAL(clicked()), this, SLOT(addAllConstraints()));
	connect(removeAllConstraintsPushButton, SIGNAL(clicked()), this, SLOT(removeAllConstraints()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	sameSubjectCheckBox->setChecked(true);
	sameActivityTagsCheckBox->setChecked(false);
	sameStudentsCheckBox->setChecked(true);
	sameTeachersCheckBox->setChecked(false);
	
	maxDifferentRoomsSpinBox->setMinimum(1);
	maxDifferentRoomsSpinBox->setMaximum(MAX_ROOMS);
	maxDifferentRoomsSpinBox->setValue(1);
}

AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm::~AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm()
{
	saveFETDialogGeometry(this);
}

void AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm::addAllConstraints()
{
	int ret=QMessageBox::warning(this, tr("FET question"), tr("Are you sure you want to add multiple constraints of type "
	 "activities occupy max different rooms to your data, according to the filters you selected?"),
	 QMessageBox::Yes | QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
		
	int cnt=0;
	QMap<std::tuple<QString, QStringList, QStringList, QStringList>, QList<int>> mp;
	
	for(Activity* act : qAsConst(gt.rules.activitiesList)){
		QString sbj=QString();
		if(sameSubjectCheckBox->isChecked())
			sbj=act->subjectName;

		QStringList atl=QStringList();
		if(sameActivityTagsCheckBox->isChecked())
			atl=act->activityTagsNames;
	
		QStringList stl=QStringList();
		if(sameStudentsCheckBox->isChecked())
			stl=act->studentsNames;
	
		QStringList tnl=QStringList();
		if(sameTeachersCheckBox->isChecked())
			tnl=act->teachersNames;
	
		QList<int> tl=mp[std::make_tuple(sbj, atl, stl, tnl)];
		tl.append(act->id);
		mp[std::make_tuple(sbj, atl, stl, tnl)]=tl;
	}
	
	QMap<std::tuple<QString, QStringList, QStringList, QStringList>, QList<int>>::const_iterator it=mp.constBegin();
	while(it!=mp.constEnd()){
		QList<int> tl=it.value();
		if(tl.count()>=2){
			ConstraintActivitiesOccupyMaxDifferentRooms* tc=new ConstraintActivitiesOccupyMaxDifferentRooms(100.0, tl, maxDifferentRoomsSpinBox->value());
			gt.rules.addSpaceConstraint(tc);
			cnt++;
		}
		it++;
	}

	QMessageBox::information(this, tr("FET information"), tr("There were added %1 space constraints").arg(cnt));
}

void AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm::removeAllConstraints()
{
	int ret=QMessageBox::warning(this, tr("FET question"), tr("Are you sure you want to remove all the constraints of type "
	 "activities occupy max different rooms from your data?"),
	 QMessageBox::Yes | QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
	
	QList<SpaceConstraint*> scl;
	for(SpaceConstraint* sc : qAsConst(gt.rules.spaceConstraintsList))
		if(sc->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS)
			scl.append(sc);
	int t=scl.count();

	gt.rules.removeSpaceConstraints(scl);
	
	QMessageBox::information(this, tr("FET information"), tr("There were removed %1 space constraints").arg(t));
}

void AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm::help()
{
	QString s;
	
	s+=tr("These functions (add or remove multiple constraints activities occupy max different rooms) were suggested by the user %1 on the forum (%2)."
	 " They could be useful for institutions using the credit hour system (American system), in which the activities with the same subject and students"
	 " should take place in the same room. In this case, select the options 'Same subject' and 'Same students', leaving 'Same activity tags'"
	 " and 'Same teachers' unselected (this is the default setting), and click on 'Add all constraints' (the max different rooms should be 1, of course)."
	 " There is also a button 'Remove all constraints', which removes all the constraints of type activities occupy max different rooms.")
	 .arg("Imad").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=5387.0");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}