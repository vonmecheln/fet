/***************************************************************************
                          activitiestagsform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "activitiestagsform.h"

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include <QMessageBox>

extern bool teachers_schedule_ready;
extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;

ActivitiesTagsForm::ActivitiesTagsForm(QWidget* parent, const QList<Activity*>& salist): QDialog(parent)
{
	setupUi(this);
	
	selectedActivitiesList=salist;
	
	instructionsLabel->setText(tr("You selected %1 individual (sub)activities. You can add the selected activity tag to all of the selected (sub)activities"
	 " which do not have it, or remove the selected activity tag from all the selected (sub)activities which have it.").arg(selectedActivitiesList.count()));
	
	connect(addActivityTagPushButton, &QPushButton::clicked, this, &ActivitiesTagsForm::addActivityTag);
	connect(removeActivityTagPushButton, &QPushButton::clicked, this, &ActivitiesTagsForm::removeActivityTag);
	connect(closePushButton, &QPushButton::clicked, this, &ActivitiesTagsForm::close);
	
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

ActivitiesTagsForm::~ActivitiesTagsForm()
{
	saveFETDialogGeometry(this);
}

void ActivitiesTagsForm::addActivityTag()
{
	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid selected activity tag."));
		return;
	}
	
	int cnt=0;
	for(Activity* act : std::as_const(selectedActivitiesList))
		if(!act->activityTagsNames.contains(activityTagName))
			cnt++;
	
	if(cnt>0){
		QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
		 tr("Add the activity tag %1 to %2 selected (sub)activities which do not have it?", "%1 is an activity tag, %2 is the count of the affected (sub)activities")
		 .arg(activityTagName).arg(cnt),
		 QMessageBox::Cancel | QMessageBox::Ok);
		if(res==QMessageBox::Cancel)
			return;
		
		QString oldDescriptions;
		
		for(Activity* act : std::as_const(selectedActivitiesList))
			if(!act->activityTagsNames.contains(activityTagName)){
				oldDescriptions+=QString("\n\n")+act->getDetailedDescription(gt.rules);
				act->activityTagsNames.append(activityTagName);
			}

		gt.rules.addUndoPoint(tr("Added the activity tag %1 to these %2 (sub)activities:", "%1 is an activity tag, %2 is the count of the affected (sub)activities")
		 .arg(activityTagName).arg(cnt)+oldDescriptions);
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
	}
	else{
		QMessageBox::information(this, tr("FET information"), tr("All the selected (sub)activities have the activity tag %1.").arg(activityTagName));
	}
}

void ActivitiesTagsForm::removeActivityTag()
{
	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid selected activity tag."));
		return;
	}
	
	int cnt=0;
	for(Activity* act : std::as_const(selectedActivitiesList))
		if(act->activityTagsNames.contains(activityTagName))
			cnt++;
	
	if(cnt>0){
		QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
		 tr("Remove the activity tag %1 from %2 selected (sub)activities which have it?", "%1 is an activity tag, %2 is the count of the affected (sub)activities")
		 .arg(activityTagName).arg(cnt),
		 QMessageBox::Cancel | QMessageBox::Ok);
		if(res==QMessageBox::Cancel)
			return;
		
		QString oldDescriptions;
		
		for(Activity* act : std::as_const(selectedActivitiesList))
			if(act->activityTagsNames.contains(activityTagName)){
				oldDescriptions+=QString("\n\n")+act->getDetailedDescription(gt.rules);
				int t=act->activityTagsNames.removeAll(activityTagName);
				assert(t==1);
			}

		gt.rules.addUndoPoint(tr("Removed the activity tag %1 from these %2 (sub)activities:", "%1 is an activity tag, %2 is the count of the affected (sub)activities")
		 .arg(activityTagName).arg(cnt)+oldDescriptions);
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
	}
	else{
		QMessageBox::information(this, tr("FET information"), tr("None of the selected (sub)activities have the activity tag %1.").arg(activityTagName));
	}
}
