/***************************************************************************
                          addremovemultipleconstraintsactivitiessamestartinghourform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addremovemultipleconstraintsactivitiessamestartinghourform.h"

#include "timetable.h"
#include "fet.h"

#include "longtextmessagebox.h"

#include <tuple>
#include <algorithm>

#include <QMap>

#include <QMessageBox>

AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::AddRemoveMultipleConstraintsActivitiesSameStartingHourForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	connect(addAllConstraintsPushButton, &QPushButton::clicked, this, &AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::addAllConstraints);
	connect(removeAllConstraintsPushButton, &QPushButton::clicked, this, &AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::removeAllConstraints);
	connect(helpPushButton, &QPushButton::clicked, this, &AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::help);
	connect(closePushButton, &QPushButton::clicked, this, &AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::close);
	
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
	QMap<std::tuple<QString, QStringList, QStringList, QStringList>, QList<int>> mp; //It could have been a QHash, but it seems there is no hash function for tuple.

	QStringList descr;

	if(sameSubjectCheckBox->isChecked())
		descr.append(tr("same subject"));
	if(sameActivityTagsCheckBox->isChecked())
		descr.append(tr("same activity tags"));
	if(sameStudentsCheckBox->isChecked())
		descr.append(tr("same students"));
	if(sameTeachersCheckBox->isChecked())
		descr.append(tr("same teachers"));

	for(Activity* act : std::as_const(gt.rules.activitiesList)){
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
	
	QList<QList<int>> aal;
	
	QMap<std::tuple<QString, QStringList, QStringList, QStringList>, QList<int>>::const_iterator it=mp.constBegin();
	while(it!=mp.constEnd()){
		QList<int> tl=it.value();
		if(tl.count()>=2)
			aal.append(tl);
		it++;
	}
	
	std::stable_sort(aal.begin(), aal.end());
	for(const QList<int>& tl : std::as_const(aal)){
		ConstraintActivitiesSameStartingHour* tc=new ConstraintActivitiesSameStartingHour(100.0, tl.count(), tl);
		gt.rules.addTimeConstraint(tc);
		cnt++;
	}

	QMessageBox::information(this, tr("FET information"), tr("There were added %1 time constraints").arg(cnt));
	
	if(cnt>0)
		gt.rules.addUndoPoint(tr("Added %1 time constraints of type activities same starting hour, with weight=%2%, conditions=%3.")
		 .arg(cnt).arg(100.0).arg(descr.join(translatedCommaSpace())));
}

void AddRemoveMultipleConstraintsActivitiesSameStartingHourForm::removeAllConstraints()
{
	int ret=QMessageBox::warning(this, tr("FET question"), tr("Are you sure you want to remove all the constraints of type "
	 "activities same starting hour from your data?"),
	 QMessageBox::Yes | QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
	
	QList<TimeConstraint*> tcl;
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
		if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR)
			tcl.append(tc);
	int t=tcl.count();

	gt.rules.removeTimeConstraints(tcl);
	
	QMessageBox::information(this, tr("FET information"), tr("There were removed %1 time constraints").arg(t));

	if(tcl.count()>0)
		gt.rules.addUndoPoint(tr("Removed all the %1 time constraints of type activities same starting hour.")
		 .arg(tcl.count()));
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
