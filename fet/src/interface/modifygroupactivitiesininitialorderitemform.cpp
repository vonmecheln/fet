/***************************************************************************
                          modifygroupactivitiesininitialorderitemform.cpp  -  description
                             -------------------
    begin                : 2014
    copyright            : (C) 2014 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "modifygroupactivitiesininitialorderitemform.h"

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

#include <QBrush>
#include <QPalette>

#include <QSettings>

extern const QString COMPANY;
extern const QString PROGRAM;

ModifyGroupActivitiesInInitialOrderItemForm::ModifyGroupActivitiesInInitialOrderItemForm(QWidget* parent, GroupActivitiesInInitialOrderItem* item): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::ok);
	connect(allActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::addActivity);
	connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::addAllActivities);
	connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::removeActivity);

	connect(clearPushButton, &QPushButton::clicked, this, &ModifyGroupActivitiesInInitialOrderItemForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);
	showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related"), "false").toBool());

	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &ModifyGroupActivitiesInInitialOrderItemForm::studentsFilterChanged);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_item=item;

	selectedActivitiesList.clear();
	selectedActivitiesListWidget->clear();
	for(int i=0; i<item->ids.count(); i++){
		int actId=item->ids[i];
		this->selectedActivitiesList.append(actId);
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		this->selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
		if(!act->active){
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	
	////////////////
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

	populateStudentsComboBox(studentsComboBox, QString(""), true);
	studentsComboBox->setCurrentIndex(0);

	filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyGroupActivitiesInInitialOrderItemForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyGroupActivitiesInInitialOrderItemForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyGroupActivitiesInInitialOrderItemForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyGroupActivitiesInInitialOrderItemForm::filterChanged);
}

ModifyGroupActivitiesInInitialOrderItemForm::~ModifyGroupActivitiesInInitialOrderItemForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/show-related"), showRelatedCheckBox->isChecked());
}

void ModifyGroupActivitiesInInitialOrderItemForm::studentsFilterChanged()
{
	bool showRelated=showRelatedCheckBox->isChecked();
	
	showedStudents.clear();
	
	if(!showRelated){
		showedStudents.insert(studentsComboBox->currentText());
	}
	else{
		if(studentsComboBox->currentText()=="")
			showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(studentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=studentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							showedStudents.insert(year->name);
							showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}
	
	filterChanged();
}

void ModifyGroupActivitiesInInitialOrderItemForm::filterChanged()
{
	allActivitiesListWidget->clear();
	this->activitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			allActivitiesListWidget->addItem(ac->getDescription(gt.rules));
			if(!ac->active){
				allActivitiesListWidget->item(allActivitiesListWidget->count()-1)->setBackground(allActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
				allActivitiesListWidget->item(allActivitiesListWidget->count()-1)->setForeground(allActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
			this->activitiesList.append(ac->id);
		}
	}
	
	int q=allActivitiesListWidget->verticalScrollBar()->minimum();
	allActivitiesListWidget->verticalScrollBar()->setValue(q);
}

bool ModifyGroupActivitiesInInitialOrderItemForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	bool ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
	if(atn!="" && !act->activityTagsNames.contains(atn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
			//if(*it == stn){
			if(showedStudents.contains(*it)){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void ModifyGroupActivitiesInInitialOrderItemForm::ok()
{
	if(this->selectedActivitiesList.size()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.size()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity"));
		return;
	}
	
	QString od=_item->getDetailedDescription(gt.rules);

	QList<int> ids=selectedActivitiesList;
	
	_item->ids=ids;
	_item->recomputeActivitiesSet();

	QString nd=_item->getDetailedDescription(gt.rules);
	
	gt.rules.addUndoPoint(tr("Modified the 'group activities in the initial order' item from\n\n%1\ninto\n\n%2").arg(od).arg(nd));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyGroupActivitiesInInitialOrderItemForm::cancel()
{
	this->close();
}

void ModifyGroupActivitiesInInitialOrderItemForm::addActivity()
{
	if(allActivitiesListWidget->currentRow()<0)
		return;
	int tmp=allActivitiesListWidget->currentRow();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=allActivitiesListWidget->currentItem()->text();
	assert(actName!="");
	
	//duplicate?
	if(this->selectedActivitiesList.contains(_id))
		return;
	
	selectedActivitiesListWidget->addItem(actName);
	Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
	if(act!=nullptr){
		if(!act->active){
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);

	this->selectedActivitiesList.append(_id);
}

void ModifyGroupActivitiesInInitialOrderItemForm::addAllActivities()
{
	QSet<int> ts(selectedActivitiesList.constBegin(), selectedActivitiesList.constEnd());

	for(int tmp=0; tmp<allActivitiesListWidget->count(); tmp++){
		int _id=this->activitiesList.at(tmp);
	
		QString actName=allActivitiesListWidget->item(tmp)->text();
		assert(actName!="");
		
		if(ts.contains(_id))
			continue;
		
		selectedActivitiesListWidget->addItem(actName);
		Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
		if(act!=nullptr){
			if(!act->active){
				selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
				selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
		}
		this->selectedActivitiesList.append(_id);
		ts.insert(_id);
	}
	
	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void ModifyGroupActivitiesInInitialOrderItemForm::removeActivity()
{
	if(selectedActivitiesListWidget->currentRow()<0 || selectedActivitiesListWidget->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget->currentRow();
	
	this->selectedActivitiesList.removeAt(tmp);

	selectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget->count())
		selectedActivitiesListWidget->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void ModifyGroupActivitiesInInitialOrderItemForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}
