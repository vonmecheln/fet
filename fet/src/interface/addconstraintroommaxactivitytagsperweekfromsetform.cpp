/***************************************************************************
                          addconstraintroommaxactivitytagsperweekfromsetform.cpp  -  description
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

#include "addconstraintroommaxactivitytagsperweekfromsetform.h"
#include "spaceconstraint.h"

AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::AddConstraintRoomMaxActivityTagsPerWeekFromSetForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::close);

	connect(allActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::addActivityTag);
	connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::removeActivityTag);

	connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::clearActivityTags);

	connect(filterCheckBox, &QCheckBox::toggled, this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::filter);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	roomsComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* room=gt.rules.roomsList[i];
		//roomsComboBox->addItem(room->getDescription());
		roomsComboBox->addItem(room->name);
	}

	connect(roomsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::filter);
	
	filter();

	selectedActivityTagsListWidget->clear();
}

AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::~AddConstraintRoomMaxActivityTagsPerWeekFromSetForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::filter()
{
	if(!filterCheckBox->isChecked() || roomsComboBox->count()==0 || roomsComboBox->currentIndex()<0 || roomsComboBox->currentIndex()>=gt.rules.roomsList.count()){
		allActivityTagsListWidget->clear();
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			ActivityTag* at=gt.rules.activityTagsList[i];
			allActivityTagsListWidget->addItem(at->name);
		}
	}
	else{
		QSet<QString> tagsSet;
	
		QString room=roomsComboBox->currentText();
		Room* rm=gt.rules.roomsList.at(roomsComboBox->currentIndex());
		//Hmmm... for other than activity preferred room(s) it might be quite inefficient (slow).
		for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList)){
			switch(ctr->type){
				//3
				case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
					{
						ConstraintActivityPreferredRoom* sc=(ConstraintActivityPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm)){
							Activity* act=gt.rules.activitiesPointerHash.value(sc->activityId, nullptr);
							//assert(act!=nullptr);
							if(act!=nullptr)
								for(const QString& at : act->activityTagsNames)
									tagsSet.insert(at);
						}
						break;
					}
				//4
				case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
					{
						ConstraintActivityPreferredRooms* sc=(ConstraintActivityPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm)){
							Activity* act=gt.rules.activitiesPointerHash.value(sc->activityId, nullptr);
							//assert(act!=nullptr);
							if(act!=nullptr)
								for(const QString& at : act->activityTagsNames)
									tagsSet.insert(at);
						}
						break;
					}
				//5
				case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
					{
						ConstraintStudentsSetHomeRoom* sc=(ConstraintStudentsSetHomeRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//6
				case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
					{
						ConstraintStudentsSetHomeRooms* sc=(ConstraintStudentsSetHomeRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//7
				case CONSTRAINT_TEACHER_HOME_ROOM:
					{
						ConstraintTeacherHomeRoom* sc=(ConstraintTeacherHomeRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//8
				case CONSTRAINT_TEACHER_HOME_ROOMS:
					{
						ConstraintTeacherHomeRooms* sc=(ConstraintTeacherHomeRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//9
				case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
					{
						ConstraintSubjectPreferredRoom* sc=(ConstraintSubjectPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//10
				case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
					{
						ConstraintSubjectPreferredRooms* sc=(ConstraintSubjectPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//11
				case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
					{
						ConstraintSubjectActivityTagPreferredRoom* sc=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//12
				case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
					{
						ConstraintSubjectActivityTagPreferredRooms* sc=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				///6 apr 2009
				//13
				case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
					{
						ConstraintActivityTagPreferredRoom* sc=(ConstraintActivityTagPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//14
				case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
					{
						ConstraintActivityTagPreferredRooms* sc=(ConstraintActivityTagPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				////
				default:
					//do nothing
					break;
			}
		}
		
		allActivityTagsListWidget->clear();
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			ActivityTag* at=gt.rules.activityTagsList[i];
			if(tagsSet.contains(at->name))
				allActivityTagsListWidget->addItem(at->name);
		}
	}
}

void AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;
	
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;
	
	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::removeActivityTag()
{
	if(selectedActivityTagsListWidget->count()<=0 || selectedActivityTagsListWidget->currentRow()<0 ||
	 selectedActivityTagsListWidget->currentRow()>=selectedActivityTagsListWidget->count())
		return;
	
	int i=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(i);
	delete item;
	if(i<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(i);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void AddConstraintRoomMaxActivityTagsPerWeekFromSetForm::addCurrentConstraint()
{
	SpaceConstraint *ctr=nullptr;

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

	if(selectedActivityTagsListWidget->count()<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("Please select at least two activity tags"));
		return;
	}

	int i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	Room* room=gt.rules.roomsList.at(i);

	QStringList selectedActivityTagsList;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

	ctr=new ConstraintRoomMaxActivityTagsPerWeekFromSet(weight, room->name, maxActivityTagsSpinBox->value(), selectedActivityTagsList);

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
