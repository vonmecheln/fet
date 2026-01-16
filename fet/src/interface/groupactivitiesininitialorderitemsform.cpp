/***************************************************************************
                          groupactivitiesininitialorderitemsform.cpp  -  description
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

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include "longtextmessagebox.h"

#include "groupactivitiesininitialorderitemsform.h"
#include "addgroupactivitiesininitialorderitemform.h"
#include "modifygroupactivitiesininitialorderitemform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QBrush>
#include <QPalette>

#include <QSettings>

#include <algorithm>
//using namespace std;

extern const QString COMPANY;
extern const QString PROGRAM;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

GroupActivitiesInInitialOrderItemsForm::GroupActivitiesInInitialOrderItemsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);
	
	currentItemTextEdit->setReadOnly(true);
	
	modifyItemPushButton->setDefault(true);

	itemsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(itemsListWidget, &QListWidget::currentRowChanged, this, &GroupActivitiesInInitialOrderItemsForm::itemChanged);
	connect(addItemPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::addItem);
	connect(closePushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::close);
	connect(removeItemPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::removeItem);
	
	connect(modifyItemPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::modifyItem);
	connect(itemsListWidget, &QListWidget::itemDoubleClicked, this, &GroupActivitiesInInitialOrderItemsForm::modifyItem);

	connect(helpPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::help);

	connect(filterCheckBox, &QCheckBox::toggled, this, &GroupActivitiesInInitialOrderItemsForm::filter);

	connect(moveItemUpPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::moveItemUp);
	connect(moveItemDownPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::moveItemDown);

	connect(activatePushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::activateItem);
	connect(deactivatePushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::deactivateItem);

	connect(activateAllPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::activateAllItems);
	connect(deactivateAllPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::deactivateAllItems);

	connect(commentsPushButton, &QPushButton::clicked, this, &GroupActivitiesInInitialOrderItemsForm::itemComments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addItemPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addItemPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyItemPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyItemPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeItemPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeItemPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activatePushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivatePushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveItemUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveItemUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveItemDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveItemDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related"), "false").toBool());

	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &GroupActivitiesInInitialOrderItemsForm::studentsFilterChanged);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
/////////////
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
///////////////

	QString settingsName="GroupActivitiesInInitialOrderItemsAdvancedFilterForm";
	
	all=settings.value(settingsName+"/all-conditions", "true").toBool();
	
	descrDetDescr.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescr.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());
	
	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());
	
	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());

	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();
	
	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);

	filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroupActivitiesInInitialOrderItemsForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroupActivitiesInInitialOrderItemsForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroupActivitiesInInitialOrderItemsForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroupActivitiesInInitialOrderItemsForm::filterChanged);
}

GroupActivitiesInInitialOrderItemsForm::~GroupActivitiesInInitialOrderItemsForm()
{
	saveFETDialogGeometry(this);

	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/show-related"), showRelatedCheckBox->isChecked());

	QString settingsName="GroupActivitiesInInitialOrderItemsAdvancedFilterForm";
	
	settings.setValue(settingsName+"/all-conditions", all);
	
	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescr.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescr.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescr.at(i));
	
	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));
	
	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));
	
	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool GroupActivitiesInInitialOrderItemsForm::filterOk(GroupActivitiesInInitialOrderItem* item)
{
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
	if(!useFilter && (tn=="" && sbn=="" && atn=="" && stn==""))
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
	
	for(int i=0; i<item->ids.count(); i++){
		int id=item->ids.at(i);
		/*Activity* act=nullptr;
		for(Activity* a : std::as_const(gt.rules.activitiesList))
			if(a->id==id)
				act=a;*/
		Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);
		
		if(act!=nullptr){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				foundSubject=true;
		
			//activity tag
			if(atn!="" && !act->activityTagsNames.contains(atn))
				;
			else
				foundActivityTag=true;
		
			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
					//if(*it == stn){
					if(showedStudents.contains(*it)){
						ok2=true;
						break;
					}
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
	}
	
	bool ok=foundTeacher && foundStudents && foundSubject && foundActivityTag;
	
	if(!useFilter || !ok)
		return ok;
	
	////////////////////////////////////////////////
	assert(descrDetDescr.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<int> perm;
	for(int i=0; i<descrDetDescr.count(); i++)
		perm.append(i);
	//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries.
	std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescr.at(a)<descrDetDescr.at(b);});
	for(int i=0; i<perm.count()-1; i++)
		assert(descrDetDescr.at(perm.at(i))<=descrDetDescr.at(perm.at(i+1)));
	
	int firstPosWithDescr=-1;
	int firstPosWithDetDescr=-1;
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescr.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
			firstPosWithDescr=i;
		}
		else if(descrDetDescr.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
			firstPosWithDetDescr=i;
		}
	}
	
	QString s=QString("");
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescr.at(perm.at(i))==DESCRIPTION){
			assert(firstPosWithDescr>=0);
			
			if(i==firstPosWithDescr)
				s=item->getDescription(gt.rules);
		}
		else{
			assert(descrDetDescr.at(perm.at(i))==DETDESCRIPTION);
			
			assert(firstPosWithDetDescr>=0);
			
			if(i==firstPosWithDetDescr)
				s=item->getDetailedDescription(gt.rules);
		}

		bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.
		
		QString t=text.at(perm.at(i));
		if(contains.at(perm.at(i))==CONTAINS){
			okPartial=s.contains(t, csens);
		}
		else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
			okPartial=!(s.contains(t, csens));
		}
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		else if(contains.at(perm.at(i))==REGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=(regExp.match(s)).hasMatch();
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=!(regExp.match(s)).hasMatch();
		}
#else
		else if(contains.at(perm.at(i))==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)>=0);
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)<0);
		}
#endif
		else
			assert(0);
			
		if(all && !okPartial)
			return false;
		else if(!all && okPartial)
			return true;
	}
	
	return all;
}

void GroupActivitiesInInitialOrderItemsForm::moveItemUp()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a 'group activities in the initial order' item up, the 'Filter' check box must not be checked."));
		return;
	}
	
	if(itemsListWidget->count()<=1)
		return;
	int i=itemsListWidget->currentRow();
	if(i<0 || i>=itemsListWidget->count())
		return;
	if(i==0)
		return;
	
	QString s1=itemsListWidget->item(i)->text();
	QString s2=itemsListWidget->item(i-1)->text();
	
	assert(gt.rules.groupActivitiesInInitialOrderList.count()==visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* it1=gt.rules.groupActivitiesInInitialOrderList.at(i);
	assert(it1==visibleItemsList.at(i));
	GroupActivitiesInInitialOrderItem* it2=gt.rules.groupActivitiesInInitialOrderList.at(i-1);
	assert(it2==visibleItemsList.at(i-1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	itemsListWidget->item(i)->setText(s2);
	itemsListWidget->item(i-1)->setText(s1);
	
	gt.rules.groupActivitiesInInitialOrderList[i]=it2;
	gt.rules.groupActivitiesInInitialOrderList[i-1]=it1;
	
	visibleItemsList[i]=it2;
	visibleItemsList[i-1]=it1;
	
	gt.rules.addUndoPoint(tr("Moved a 'group activities in the initial order' item up:\n\n%1", "%1 is the detailed description of the item").arg(it1->getDetailedDescription(gt.rules)));

	if(it2->active){
		itemsListWidget->item(i)->setBackground(QBrush());
		itemsListWidget->item(i)->setForeground(QBrush());
	}
	else{
		itemsListWidget->item(i)->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
		itemsListWidget->item(i)->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
	}

	if(it1->active){
		itemsListWidget->item(i-1)->setBackground(QBrush());
		itemsListWidget->item(i-1)->setForeground(QBrush());
	}
	else{
		itemsListWidget->item(i-1)->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
		itemsListWidget->item(i-1)->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
	}

	itemsListWidget->setCurrentRow(i-1);
	itemChanged(i-1);
}

void GroupActivitiesInInitialOrderItemsForm::moveItemDown()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a 'group activities in the initial order' item down, the 'Filter' check box must not be checked."));
		return;
	}
	
	if(itemsListWidget->count()<=1)
		return;
	int i=itemsListWidget->currentRow();
	if(i<0 || i>=itemsListWidget->count())
		return;
	if(i==itemsListWidget->count()-1)
		return;
	
	QString s1=itemsListWidget->item(i)->text();
	QString s2=itemsListWidget->item(i+1)->text();
	
	assert(gt.rules.groupActivitiesInInitialOrderList.count()==visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* it1=gt.rules.groupActivitiesInInitialOrderList.at(i);
	assert(it1==visibleItemsList.at(i));
	GroupActivitiesInInitialOrderItem* it2=gt.rules.groupActivitiesInInitialOrderList.at(i+1);
	assert(it2==visibleItemsList.at(i+1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	itemsListWidget->item(i)->setText(s2);
	itemsListWidget->item(i+1)->setText(s1);
	
	gt.rules.groupActivitiesInInitialOrderList[i]=it2;
	gt.rules.groupActivitiesInInitialOrderList[i+1]=it1;
	
	visibleItemsList[i]=it2;
	visibleItemsList[i+1]=it1;
	
	gt.rules.addUndoPoint(tr("Moved a 'group activities in the initial order' item down:\n\n%1", "%1 is the detailed description of the item").arg(it1->getDetailedDescription(gt.rules)));

	if(it2->active){
		itemsListWidget->item(i)->setBackground(QBrush());
		itemsListWidget->item(i)->setForeground(QBrush());
	}
	else{
		itemsListWidget->item(i)->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
		itemsListWidget->item(i)->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
	}

	if(it1->active){
		itemsListWidget->item(i+1)->setBackground(QBrush());
		itemsListWidget->item(i+1)->setForeground(QBrush());
	}
	else{
		itemsListWidget->item(i+1)->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
		itemsListWidget->item(i+1)->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
	}

	itemsListWidget->setCurrentRow(i+1);
	itemChanged(i+1);
}

void GroupActivitiesInInitialOrderItemsForm::studentsFilterChanged()
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

void GroupActivitiesInInitialOrderItemsForm::filterChanged()
{
	visibleItemsList.clear();
	itemsListWidget->clear();
	int n_active=0;
	for(GroupActivitiesInInitialOrderItem* item : std::as_const(gt.rules.groupActivitiesInInitialOrderList))
		if(filterOk(item))
			visibleItemsList.append(item);
	
	for(GroupActivitiesInInitialOrderItem* item : std::as_const(visibleItemsList)){
		assert(filterOk(item));
		itemsListWidget->addItem(item->getDescription(gt.rules));

		if(item->active){
			n_active++;
		}
		else{
			itemsListWidget->item(itemsListWidget->count()-1)->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			itemsListWidget->item(itemsListWidget->count()-1)->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	
	if(itemsListWidget->count()<=0)
		currentItemTextEdit->setPlainText(QString(""));
	else
		itemsListWidget->setCurrentRow(0);

	itemsTextLabel->setText(tr("%1 / %2 items",
	 "%1 represents the number of visible active 'group activities in the initial order' items, %2 represents the total number of visible items")
	 .arg(n_active).arg(visibleItemsList.count()));
}

void GroupActivitiesInInitialOrderItemsForm::itemChanged(int index)
{
	if(index<0){
		currentItemTextEdit->setPlainText("");
	
		return;
	}
	assert(index<this->visibleItemsList.size());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(index);
	QString s=item->getDetailedDescription(gt.rules);
	currentItemTextEdit->setPlainText(s);
}

void GroupActivitiesInInitialOrderItemsForm::addItem()
{
	AddGroupActivitiesInInitialOrderItemForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	itemsListWidget->setCurrentRow(itemsListWidget->count()-1);
}

void GroupActivitiesInInitialOrderItemsForm::modifyItem()
{
	int valv=itemsListWidget->verticalScrollBar()->value();
	int valh=itemsListWidget->horizontalScrollBar()->value();

	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	GroupActivitiesInInitialOrderItem* item=visibleItemsList[i];

	ModifyGroupActivitiesInInitialOrderItemForm form(this, item);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	itemsListWidget->verticalScrollBar()->setValue(valv);
	itemsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(i>=itemsListWidget->count())
		i=itemsListWidget->count()-1;

	if(i>=0)
		itemsListWidget->setCurrentRow(i);
	else
		this->itemChanged(-1);
}

void GroupActivitiesInInitialOrderItemsForm::removeItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	QString s;
	s=tr("Remove item?");
	s+="\n\n";
	s+=item->getDetailedDescription(gt.rules);
	
	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		for(int j=0; j<gt.rules.groupActivitiesInInitialOrderList.count(); j++)
			if(visibleItemsList.at(i) == gt.rules.groupActivitiesInInitialOrderList[j]){
				QString itd=item->getDetailedDescription(gt.rules);
			
				gt.rules.groupActivitiesInInitialOrderList.removeAt(j);
				
				gt.rules.addUndoPoint(tr("Removed a 'group activities in the initial order' item:\n\n%1", "%1 is the detailed description of the item").arg(itd));
				
				gt.rules.internalStructureComputed=false;
				setRulesModifiedAndOtherThings(&gt.rules);

				break;
			}

		visibleItemsList.removeAt(i);
		itemsListWidget->setCurrentRow(-1);
		itemsListWidget->takeItem(i);
		delete item;

		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}
	
	if(i>=itemsListWidget->count())
		i=itemsListWidget->count()-1;
	if(i>=0)
		itemsListWidget->setCurrentRow(i);
	else
		this->itemChanged(-1);
}

void GroupActivitiesInInitialOrderItemsForm::help()
{
	QString s=tr("This option is an advanced one. It is intended for experienced users."
	 " It is useful in cases of peculiar uses of constraints. In most cases, FET will care"
	 " automatically about a good initial order of the activities before the generation will"
	 " begin, but in some very rare cases you can help FET. An example is: if you use constraints activities"
	 " occupy max time slots from selection in such a way that some activities are clearly meant to be"
	 " near each other when the generation starts, but FET is not clever enough to deduce that."
	 " Other usage might be that you want to leave some activities at the end and, in case the timetable"
	 " cannot be found, you can use a partial one. In this case, you need to begin to generate once"
	 " (you can interrupt generation after only a few activities were placed), see the initial order of the"
	 " activities, and use the option to group the first activities with the ones you wish to"
	 " bring forward. The group of activities will be put near the earliest one in the group.");
	s+="\n\n";
	s+=tr("Note: Each activity id must appear at most once in all the active 'group activities in the initial order' items.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void GroupActivitiesInInitialOrderItemsForm::activateItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	
	if(!item->active){
		QString sb=item->getDetailedDescription(gt.rules);

		item->active=true;
		
		gt.rules.addUndoPoint(tr("Activated a 'group activities in the initial order' item:\n\n%1", "%1 is the detailed description of the item").arg(sb));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		itemsListWidget->currentItem()->setBackground(QBrush());
		itemsListWidget->currentItem()->setForeground(QBrush());
		itemChanged(itemsListWidget->currentRow());
	
		int n_active=0;
		for(GroupActivitiesInInitialOrderItem* item2 : std::as_const(gt.rules.groupActivitiesInInitialOrderList))
			if(filterOk(item2)){
				if(item2->active)
					n_active++;
			}
	
		itemsTextLabel->setText(tr("%1 / %2 items",
		 "%1 represents the number of visible active 'group activities in the initial order' items, %2 represents the total number of visible items")
		 .arg(n_active).arg(visibleItemsList.count()));
	}
}

void GroupActivitiesInInitialOrderItemsForm::deactivateItem()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);
	
	if(item->active){
		QString sb=item->getDetailedDescription(gt.rules);

		item->active=false;

		gt.rules.addUndoPoint(tr("Deactivated a 'group activities in the initial order' item:\n\n%1", "%1 is the detailed description of the item").arg(sb));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		itemsListWidget->currentItem()->setBackground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
		itemsListWidget->currentItem()->setForeground(itemsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		itemChanged(itemsListWidget->currentRow());

		int n_active=0;
		for(GroupActivitiesInInitialOrderItem* item2 : std::as_const(gt.rules.groupActivitiesInInitialOrderList))
			if(filterOk(item2)){
				if(item2->active)
					n_active++;
			}
	
		itemsTextLabel->setText(tr("%1 / %2 items",
		 "%1 represents the number of visible active 'group activities in the initial order' items, %2 represents the total number of visible items")
		 .arg(n_active).arg(visibleItemsList.count()));
	}
}

/*static int itemsAscendingByComments(const GroupActivitiesInInitialOrderItem* item1, const GroupActivitiesInInitialOrderItem* item2)
{
	return item1->comments < item2->comments;
}

void GroupActivitiesInInitialOrderItemsForm::sortItemsByComments()
{
	QMessageBox::StandardButton t=QMessageBox::question(this, tr("Sort items?"),
	 tr("This will sort the 'group activities in the initial order' items list ascending according to their comments. You can obtain "
	 "a custom ordering by adding comments to some or all items, for example 'rank #1 ... other comments', "
	 "'rank #2 ... other different comments'.")
	 +" "+tr("Are you sure you want to continue?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	
	if(t==QMessageBox::Cancel)
		return;
	
	//qStableSort(gt.rules.groupActivitiesInInitialOrderList.begin(), gt.rules.groupActivitiesInInitialOrderList.end(), itemsAscendingByComments);
	std::stable_sort(gt.rules.groupActivitiesInInitialOrderList.begin(), gt.rules.groupActivitiesInInitialOrderList.end(), itemsAscendingByComments);

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	filterChanged();
}*/

void GroupActivitiesInInitialOrderItemsForm::itemComments()
{
	int i=itemsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected item"));
		return;
	}
	
	assert(i<visibleItemsList.count());
	GroupActivitiesInInitialOrderItem* item=visibleItemsList.at(i);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Group activities in the initial order item comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QTextEdit* commentsPT=new QTextEdit();
	commentsPT->setPlainText(item->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("GroupActivitiesInInitialOrderItemCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oldid=item->getDetailedDescription(gt.rules);
		
		item->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed a 'group activities in the initial order' item's comments:\n\n%1\nnow has the comments:\n\n%2",
		 "%1 is the detailed description of the item, %2 are the new comments")
		 .arg(oldid).arg(item->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		itemsListWidget->currentItem()->setText(item->getDescription(gt.rules));
		itemChanged(itemsListWidget->currentRow());
	}
}

void GroupActivitiesInInitialOrderItemsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for group activities in the initial order items"), false, all, descrDetDescr, contains, text, caseSensitive, "GroupActivitiesInInitialOrderItemsAdvancedFilterForm");
	
	int t=filterForm->exec();
	
	if(t==QDialog::Accepted){
		assert(useFilter==false);
		useFilter=true;
	
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
		
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
		
		descrDetDescr.clear();
		contains.clear();
		text.clear();
		
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescr.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &GroupActivitiesInInitialOrderItemsForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &GroupActivitiesInInitialOrderItemsForm::filter);
	}
	
	delete filterForm;
}

void GroupActivitiesInInitialOrderItemsForm::activateAllItems()
{
	QMessageBox::StandardButton ret=QMessageBox::No;
	QString s=tr("Are you sure you want to activate all the listed items?");
	ret=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if(ret==QMessageBox::No){
		itemsListWidget->setFocus();
		return;
	}

	QString su;
	int cnt=0;
	for(GroupActivitiesInInitialOrderItem* item : std::as_const(visibleItemsList)){
		if(!item->active){
			su+=tr("Item:\n\n%1").arg(item->getDetailedDescription(gt.rules))+QString("\n");
		
			cnt++;
			item->active=true;
		}
	}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated all the filtered 'group activities in the initial order' items:\n\n%1",
		 "%1 is the list of detailed descriptions of the activated items").arg(su));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		filterChanged();
		
		QMessageBox::information(this, tr("FET information"), tr("Activated %1 group activities in the initial order items").arg(cnt));
	}
	
	itemsListWidget->setFocus();
}

void GroupActivitiesInInitialOrderItemsForm::deactivateAllItems()
{
	QMessageBox::StandardButton ret=QMessageBox::No;
	QString s=tr("Are you sure you want to deactivate all the listed items?");
	ret=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if(ret==QMessageBox::No){
		itemsListWidget->setFocus();
		return;
	}

	QString su;
	int cnt=0;
	for(GroupActivitiesInInitialOrderItem* item : std::as_const(visibleItemsList)){
		if(item->active){
			su+=tr("Item:\n\n%1").arg(item->getDetailedDescription(gt.rules))+QString("\n");
		
			cnt++;
			item->active=false;
		}
	}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated all the filtered 'group activities in the initial order' items:\n\n%1",
		 "%1 is the list of detailed descriptions of the activated items").arg(su));
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		filterChanged();
		
		QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 group activities in the initial order items").arg(cnt));
	}
	
	itemsListWidget->setFocus();
}
