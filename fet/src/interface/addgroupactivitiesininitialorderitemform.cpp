/***************************************************************************
                          addgroupactivitiesininitialorderitemform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addgroupactivitiesininitialorderitemform.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

#include <QBrush>
#include <QPalette>

#include <QSettings>

extern const QString COMPANY;
extern const QString PROGRAM;

AddGroupActivitiesInInitialOrderItemForm::AddGroupActivitiesInInitialOrderItemForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addItemPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, &QPushButton::clicked, this, &AddGroupActivitiesInInitialOrderItemForm::close);
	connect(addItemPushButton, &QPushButton::clicked, this, &AddGroupActivitiesInInitialOrderItemForm::addItem);
	connect(allActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddGroupActivitiesInInitialOrderItemForm::addActivity);
	connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &AddGroupActivitiesInInitialOrderItemForm::addAllActivities);
	connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddGroupActivitiesInInitialOrderItemForm::removeActivity);
	connect(clearPushButton, &QPushButton::clicked, this, &AddGroupActivitiesInInitialOrderItemForm::clear);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);
	showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related"), "false").toBool());

	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &AddGroupActivitiesInInitialOrderItemForm::studentsFilterChanged);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

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

	selectedActivitiesListWidget->clear();
	this->selectedActivitiesList.clear();

	filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddGroupActivitiesInInitialOrderItemForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddGroupActivitiesInInitialOrderItemForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddGroupActivitiesInInitialOrderItemForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddGroupActivitiesInInitialOrderItemForm::filterChanged);
}

AddGroupActivitiesInInitialOrderItemForm::~AddGroupActivitiesInInitialOrderItemForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/show-related"), showRelatedCheckBox->isChecked());
}

bool AddGroupActivitiesInInitialOrderItemForm::filterOk(Activity* act)
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

void AddGroupActivitiesInInitialOrderItemForm::studentsFilterChanged()
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

void AddGroupActivitiesInInitialOrderItemForm::filterChanged()
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

void AddGroupActivitiesInInitialOrderItemForm::addItem()
{
	if(this->selectedActivitiesList.count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity"));
		return;
	}

	QList<int> ids;
	int i;
	QList<int>::const_iterator it;
	ids.clear();
	for(i=0, it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); it++, i++){
		ids.append(*it);
	}
	
	GroupActivitiesInInitialOrderItem* item=new GroupActivitiesInInitialOrderItem();
	item->ids=ids;
	item->recomputeActivitiesSet();
	gt.rules.groupActivitiesInInitialOrderList.append(item);
	
	gt.rules.addUndoPoint(tr("Added the 'group activities in the initial order' item:\n\n%1").arg(item->getDetailedDescription(gt.rules)));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	QString s=tr("Added 'group activities in the initial order' item");
	s+="\n\n";
	s+=item->getDetailedDescription(gt.rules);
	LongTextMessageBox::information(this, tr("FET information"), s);
}

void AddGroupActivitiesInInitialOrderItemForm::addActivity()
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

void AddGroupActivitiesInInitialOrderItemForm::addAllActivities()
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

void AddGroupActivitiesInInitialOrderItemForm::removeActivity()
{
	if(selectedActivitiesListWidget->currentRow()<0 || selectedActivitiesListWidget->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget->currentRow();
	
	selectedActivitiesList.removeAt(tmp);
	
	selectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget->count())
		selectedActivitiesListWidget->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void AddGroupActivitiesInInitialOrderItemForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}
