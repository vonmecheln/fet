/***************************************************************************
                          listofrelatedtimeconstraintsform.cpp  -  description
                             -------------------
    begin                : 2025
    copyright            : (C) 2025 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "longtextmessagebox.h"

#include "listofrelatedtimeconstraintsform.h"

#include "addormodifytimeconstraint.h"

#include "lockunlock.h"

#include "advancedfilterform.h"

#include <Qt>

#include <QMessageBox>

#include <QInputDialog>

#include <QTextEdit>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QBrush>
#include <QPalette>

#include <algorithm>
//using namespace std;

extern const QString COMPANY;
extern const QString PROGRAM;

const int FILTER_IS_UNSPECIFIED=0;
const int FILTER_IS_TEACHER=1;
const int FILTER_IS_STUDENTS_SET=2;
const int FILTER_IS_ROOM=3;
const int FILTER_IS_ACTIVITY=4;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

int timeConstraintsAscendingByDescription(TimeConstraint* t1, TimeConstraint* t2); //defined in alltimeconstraints.cpp

ListOfRelatedTimeConstraintsForm::ListOfRelatedTimeConstraintsForm(QWidget* parent, int _filterBy, const QList<int>& _filterId, const QString& _filterName, const QList<TimeConstraint*>& _allRelatedTimeConstraintsList):
 QDialog(parent)
{
	setupUi(this);
	
	filterBy=_filterBy;
	filterId=_filterId;
	filterName=_filterName;
	allRelatedTimeConstraintsList=_allRelatedTimeConstraintsList;
	
	currentItemLabel->setText(filterName);

	filterCheckBox->setChecked(false);
	sortedCheckBox->setChecked(false);
	
	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ListOfRelatedTimeConstraintsForm::constraintChanged);

	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListOfRelatedTimeConstraintsForm::selectionChanged);

	connect(closePushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::close);
	connect(removeConstraintsPushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::removeConstraints);
	
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ListOfRelatedTimeConstraintsForm::modifyConstraint);
	connect(filterCheckBox, &QCheckBox::toggled, this, &ListOfRelatedTimeConstraintsForm::filter);

	connect(sortedCheckBox, &QCheckBox::toggled, this, &ListOfRelatedTimeConstraintsForm::sortedChanged);
	connect(activatePushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::activateConstraints);
	connect(deactivatePushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::deactivateConstraints);

	connect(commentsPushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::constraintComments);

	connect(weightsPushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::changeWeights);

	if(filterBy==FILTER_IS_ACTIVITY){
		connect(addConstraintPushButton, &QPushButton::clicked, this, &ListOfRelatedTimeConstraintsForm::addConstraint);

		if(SHORTCUT_PLUS){
			QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
			connect(addShortcut, &QShortcut::activated, [=]{addConstraintPushButton->animateClick();});
			//if(SHOW_TOOL_TIPS)
			//	addPushButton->setToolTip(QString("+"));
		}
	}
	else{
		addConstraintPushButton->setVisible(false);
	}

	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyConstraintPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyConstraintPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeConstraintsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeConstraintsPushButton->setToolTip(QString("⌦"));
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
	if(SHORTCUT_W){
		QShortcut* weightsShortcut=new QShortcut(QKeySequence(Qt::Key_W), this);
		connect(weightsShortcut, &QShortcut::activated, [=]{weightsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	weightsPushButton->setToolTip(QString("W"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	QString settingsName="ListOfRelatedTimeConstraintsAdvancedFilterForm";
	
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
	assert(sortedCheckBox->isChecked()==false);
	
	filterChanged();
}

ListOfRelatedTimeConstraintsForm::~ListOfRelatedTimeConstraintsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	QString settingsName="ListOfRelatedTimeConstraintsAdvancedFilterForm";
	
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

bool ListOfRelatedTimeConstraintsForm::filterOk(TimeConstraint* ctr)
{
	if(!useFilter)
		return true;

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
				s=ctr->getDescription(gt.rules);
		}
		else{
			assert(descrDetDescr.at(perm.at(i))==DETDESCRIPTION);
			
			assert(firstPosWithDetDescr>=0);
			
			if(i==firstPosWithDetDescr)
				s=ctr->getDetailedDescription(gt.rules);
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

void ListOfRelatedTimeConstraintsForm::sortedChanged(bool checked)
{
	Q_UNUSED(checked);

	filterChanged();
	
	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::filterChanged()
{
	disconnect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListOfRelatedTimeConstraintsForm::selectionChanged);

	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	for(TimeConstraint* ctr : std::as_const(allRelatedTimeConstraintsList))
		if(filterOk(ctr))
			visibleTimeConstraintsList.append(ctr);
		
	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleTimeConstraintsList.begin(), visibleTimeConstraintsList.end(), timeConstraintsAscendingByDescription);

	for(TimeConstraint* ctr : std::as_const(visibleTimeConstraintsList)){
		assert(filterOk(ctr));
		constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		
		if(ctr->active){
			n_active++;
		}
		else{
			constraintsListWidget->item(constraintsListWidget->count()-1)->setBackground(constraintsListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			constraintsListWidget->item(constraintsListWidget->count()-1)->setForeground(constraintsListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}

	if(constraintsListWidget->count()<=0)
		currentConstraintTextEdit->setPlainText("");
	else
		constraintsListWidget->setCurrentRow(0);
	
	constraintsTextLabel->setText(tr("No: %1 / %2",
	 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
	 .arg(n_active).arg(visibleTimeConstraintsList.count()));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));
	
	selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &ListOfRelatedTimeConstraintsForm::selectionChanged);
}

void ListOfRelatedTimeConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;
	
	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void ListOfRelatedTimeConstraintsForm::addConstraint()
{
	int oldRow=constraintsListWidget->currentRow();

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int initialNumberOfTimeConstraints=gt.rules.timeConstraintsList.count();

	AddOrModifyTimeConstraint aomtc(this, CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS, nullptr,
									QString(), QString(), QString(), QString(), QString(), filterId);

	int finalNumberOfTimeConstraints=gt.rules.timeConstraintsList.count();

	if(initialNumberOfTimeConstraints!=finalNumberOfTimeConstraints){
		assert(finalNumberOfTimeConstraints>initialNumberOfTimeConstraints);
		for(int i=initialNumberOfTimeConstraints; i<finalNumberOfTimeConstraints; i++)
			allRelatedTimeConstraintsList.append(gt.rules.timeConstraintsList.at(i));
	}

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);
	
	int newRow=oldRow;
	if(initialNumberOfTimeConstraints!=finalNumberOfTimeConstraints){
		newRow=constraintsListWidget->count()-1;
		
		if(sortedCheckBox->isChecked()){
			assert(finalNumberOfTimeConstraints>=1);
			for(int i=0; i<visibleTimeConstraintsList.count(); i++)
				if(visibleTimeConstraintsList.at(i)==gt.rules.timeConstraintsList.constLast()){
					newRow=i;
					break;
				}
		}
	}
	
	constraintsListWidget->setCurrentRow(newRow);
	constraintChanged();

	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	//185
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_DAY){
		QMessageBox::warning(this, tr("FET warning"), tr("The constraints of type activity preferred day cannot be edited. They can only be added/removed"
		 " from the students/teachers timetable view time horizontal dialog or they can be removed from this dialog."));
		return;
	}

	AddOrModifyTimeConstraint(this, ctr->type, ctr);
	
	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	
	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::removeConstraints()
{
	bool recompute=false;

	QList<TimeConstraint*> tl;

	bool firstBasic=true;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			tl.append(ctr);

			if(firstBasic && ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
				firstBasic=false;

				QMessageBox::StandardButton wr;

				QString s=tr("Your selection contains the basic compulsory time constraint(s). Do you really want to remove all the selected "
				 "constraints, including this (these) one(s)? You cannot generate a timetable without this (these) constraint(s).");
				s+="\n\n";
				s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
					"Miscellaneous -> Basic compulsory time constraints.");

				wr=QMessageBox::warning(this, tr("FET warning"), s,
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

				if(wr==QMessageBox::No){
					constraintsListWidget->setFocus();
					return;
				}
			}
			else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				recompute=true;
			}
		}

	QString s;
	s=tr("Remove these selected time constraints?");
	s+="\n\n";
	for(TimeConstraint* ctr : std::as_const(tl))
		s+=ctr->getDetailedDescription(gt.rules)+"\n";
	int lres=LongTextMessageBox::confirmation(this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );

	if(lres!=0){
		constraintsListWidget->setFocus();
		return;
	}

	QString su;
	if(!tl.isEmpty()){
		su=tr("Removed %1 time constraints:").arg(tl.count());
		su+=QString("\n\n");
		for(TimeConstraint* ctr : std::as_const(tl))
			su+=ctr->getDetailedDescription(gt.rules)+"\n";
	}

	//The user clicked the OK button or pressed Enter
	gt.rules.removeTimeConstraints(tl);
	
	if(!tl.isEmpty()){
		//
		QSet<TimeConstraint*> rtcs(tl.constBegin(), tl.constEnd());
		QList<TimeConstraint*> ftcl;
		for(TimeConstraint* tc : std::as_const(allRelatedTimeConstraintsList))
			if(!rtcs.contains(tc))
				ftcl.append(tc);
		allRelatedTimeConstraintsList=ftcl;
		//

		gt.rules.addUndoPoint(su);
	}

	if(recompute){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int cr=constraintsListWidget->currentRow();

	filterChanged();

	if(cr>=0){
		if(cr<constraintsListWidget->count())
			constraintsListWidget->setCurrentRow(cr);
		else if(constraintsListWidget->count()>0)
			constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
	}

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for time constraints"), false, all, descrDetDescr, contains, text, caseSensitive, "AllTimeConstraintsAdvancedFilterForm");

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

		constraintsListWidget->setFocus();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &ListOfRelatedTimeConstraintsForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &ListOfRelatedTimeConstraintsForm::filter);
	}
	
	delete filterForm;
}

void ListOfRelatedTimeConstraintsForm::activateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected time constraints?");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(!ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=true;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated %1 time constraints:", "%1 is the number of activated time constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
		
		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Activated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::deactivateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected time constraints? "
		 "(Note that the basic compulsory time constraints will not be deactivated, even if they are selected.)");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
				continue;
			if(ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=false;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated %1 time constraints:", "%1 is the number of deactivated time constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
		
		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}

void ListOfRelatedTimeConstraintsForm::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Constraint comments"));
	
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
	commentsPT->setPlainText(ctr->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("TimeConstraintCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString cb=ctr->getDetailedDescription(gt.rules);

		ctr->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed a constraint's comments. Constraint before:\n\n%1\nComments after:\n\n%2").arg(cb).arg(ctr->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		if(!filterOk(ctr)){ //Maybe the constraint is no longer visible in the list widget, because of the filter.
			visibleTimeConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			QListWidgetItem* item=constraintsListWidget->takeItem(i);
			delete item;

			if(i>=constraintsListWidget->count())
				i=constraintsListWidget->count()-1;
			if(i>=0)
				constraintsListWidget->setCurrentRow(i);
			else
				currentConstraintTextEdit->setPlainText(QString(""));

			int n_active=0;
			for(TimeConstraint* ctr2 : std::as_const(visibleTimeConstraintsList))
				if(ctr2->active)
					n_active++;
	
			constraintsTextLabel->setText(tr("%1 / %2 time constraints",
			 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
			 .arg(n_active).arg(visibleTimeConstraintsList.count()));
		}
		else{
			constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
			constraintChanged();
		}
	}
}

void ListOfRelatedTimeConstraintsForm::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(constraintsListWidget->count()==visibleTimeConstraintsList.count());
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleTimeConstraintsList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected time constraints, %1 is the number of active"
	 " selected time constraints, %2 is the total number of selected time constraints").arg(nActive).arg(nTotal));
}

void ListOfRelatedTimeConstraintsForm::changeWeights()
{
	int cnt_pre=0;
	int cnt_unchanged=0;
	double nw=100.0;
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->canHaveAnyWeight())
				cnt_pre++;
			else
				cnt_unchanged++;
		}
	if(cnt_pre==0){
		QMessageBox::information(this, tr("FET information"), tr("No constraints from your selection can change their weight"
		 " (remember that some types of constraints are allowed to have only 100% weight)."));
		
		return;
	}
	else{
		bool ok;
		if(cnt_unchanged==0)
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 selected time constraints.\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (3 or even 4), if needed. %1 is the number of constraints which will change.")
			 .arg(cnt_pre),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		else
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 time constraints from your\n"
			 "selection (remember that some types of constraints\n"
			 "are only allowed to have 100% weight, so %2\n"
			 "constraints out of the %3 selected will not change).\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (6 or even 7), if needed. %1 is the number of constraints which will change, %2 is the number of constraints"
			 " which will not change, and %3 is the number of all selected constraints.")
			 .arg(cnt_pre).arg(cnt_unchanged).arg(cnt_pre+cnt_unchanged),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		
		if(!ok)
			return;
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->canHaveAnyWeight()){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->weightPercentage=nw;
				if(ctr->active && ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}
		
	assert(cnt>0);
	assert(cnt==cnt_pre);

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Changed the weights of the following %1 selected time constraints to %2%:",
		 "%1 is the number of time constraints for which the user has changed the weight, %2 is the new weight for all the selected constraints")
		 .arg(cnt).arg(CustomFETString::number(nw))+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}
