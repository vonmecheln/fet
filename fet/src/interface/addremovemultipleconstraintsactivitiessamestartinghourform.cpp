/***************************************************************************
                          addremovemultipleconstraintsactivitiessamestartinghourform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "addremovemultipleconstraintsactivitiessamestartinghourform.h"

#include "timetable.h"
#include "fet.h"

#include "longtextmessagebox.h"

#include <tuple>

#include <QMap>

#include <QMessageBox>

AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::AddRemoveMultipleConstraintsActivitiesSameStartingHourForm(QWidget* parent): QDialog(parent)
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
}

AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::~AddRemoveMultipleConstraintsActivitiesSameStartingHourForm()
{
	saveFETDialogGeometry(this);
}

void AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::addAllConstraints()
{
	int ret=QMessageBox::warning(this, tr("FET question"), tr("Are you sure you want to add multiple constraints of type "
	 "activities same starting hour to your data, according to the filters you selected?"),
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
			ConstraintActivitiesSameStartingHour* tc=new ConstraintActivitiesSameStartingHour(100.0, tl.count(), tl);
			gt.rules.addTimeConstraint(tc);
			cnt++;
		}
		it++;
	}

	QMessageBox::information(this, tr("FET information"), tr("There were added %1 time constraints").arg(cnt));
}

void AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::removeAllConstraints()
{
	int ret=QMessageBox::warning(this, tr("FET question"), tr("Are you sure you want to remove all the constraints of type "
	 "activities same starting hour from your data?"),
	 QMessageBox::Yes | QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
	
	QList<TimeConstraint*> tcl;
	for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList))
		if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR)
			tcl.append(tc);
	int t=tcl.count();

	gt.rules.removeTimeConstraints(tcl);
	
	QMessageBox::information(this, tr("FET information"), tr("There were removed %1 time constraints").arg(t));
}

void AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::help()
{
	QString s;
	
	s+=tr("These functions (add or remove multiple constraints activities same starting hour) were suggested by the user %1 on the forum (%2)."
	 " They could be useful for institutions using the credit hour system (American system), in which the activities with the same subject and students"
	 " should start at the same hour (any days). In this case, select the option 'Same subject' and 'Same students', leaving 'Same activity tags'"
	 " and 'Same teachers' unselected (this is the default setting), and click on 'Add all constraints'. There is also a 'Remove all constraints', which"
	 " removes all the constraints of type activities same starting hour.").arg("Imad").arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=5387.0");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}
