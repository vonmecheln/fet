/***************************************************************************
                          splityearform.cpp  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QtGlobal>

#include "splityearform.h"

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QMessageBox>

#include <QListWidget>
#include <QAbstractItemView>

#include <QInputDialog>

#include <QSet>
#include <QHash>

#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "longtextmessagebox.h"

SplitYearForm::SplitYearForm(QWidget* parent, const QString& _year): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(copyFromAnotherYearPushButton, &QPushButton::clicked, this, &SplitYearForm::copyFromAnotherYear);
	
	connect(tabWidget, &QTabWidget::currentChanged, this, &SplitYearForm::tabIndexChanged);

	connect(okPushButton, &QPushButton::clicked, this, &SplitYearForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &SplitYearForm::close);
	connect(categoriesSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SplitYearForm::numberOfCategoriesChanged);
	
	listWidgets[0]=listWidget1;
	listWidgets[1]=listWidget2;
	listWidgets[2]=listWidget3;
	listWidgets[3]=listWidget4;
	listWidgets[4]=listWidget5;
	listWidgets[5]=listWidget6;
	listWidgets[6]=listWidget7;
	listWidgets[7]=listWidget8;
	listWidgets[8]=listWidget9;
	listWidgets[9]=listWidget10;
	listWidgets[10]=listWidget11;
	listWidgets[11]=listWidget12;
	listWidgets[12]=listWidget13;
	listWidgets[13]=listWidget14;
	
	for(int i=0; i<MAX_CATEGORIES; i++){
		listWidgets[i]->clear();
		listWidgets[i]->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	connect(listWidgets[0], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list0DoubleClicked);
	connect(listWidgets[1], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list1DoubleClicked);
	connect(listWidgets[2], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list2DoubleClicked);
	connect(listWidgets[3], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list3DoubleClicked);
	connect(listWidgets[4], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list4DoubleClicked);
	connect(listWidgets[5], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list5DoubleClicked);
	connect(listWidgets[6], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list6DoubleClicked);
	connect(listWidgets[7], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list7DoubleClicked);
	connect(listWidgets[8], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list8DoubleClicked);
	connect(listWidgets[9], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list9DoubleClicked);
	connect(listWidgets[10], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list10DoubleClicked);
	connect(listWidgets[11], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list11DoubleClicked);
	connect(listWidgets[12], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list12DoubleClicked);
	connect(listWidgets[13], &QListWidget::itemDoubleClicked, this, &SplitYearForm::list13DoubleClicked);
	
	connect(addPushButton, &QPushButton::clicked, this, &SplitYearForm::addClicked);
	connect(modifyPushButton, &QPushButton::clicked, this, &SplitYearForm::modifyClicked);
	connect(removePushButton, &QPushButton::clicked, this, &SplitYearForm::removeClicked);
	connect(removeAllPushButton, &QPushButton::clicked, this, &SplitYearForm::removeAllClicked);
	
	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removePushButton->setToolTip(QString("⌦"));
	}
	
	connect(helpPushButton, &QPushButton::clicked, this, &SplitYearForm::help);
	connect(resetPushButton, &QPushButton::clicked, this, &SplitYearForm::reset);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	//2020-09-03
	StudentsSet* ss=gt.rules.searchStudentsSet(_year);
	assert(ss!=nullptr);
	assert(ss->type==STUDENTS_YEAR);
	StudentsYear* sy=(StudentsYear*)ss;

	_firstCategoryIsPermanent=sy->firstCategoryIsPermanent;

	_sep=sy->separator;
	_nCategories=sy->divisions.count();
	for(int i=0; i<_nCategories; i++){
		_nDivisions[i]=sy->divisions.at(i).count();
		_divisions[i]=sy->divisions.at(i);
	}

	year=_year;

	QString s=tr("Splitting year: %1").arg(year);
	splitYearTextLabel->setText(s);
	
	//restore saved values
	firstCategoryPermanentCheckBox->setChecked(_firstCategoryIsPermanent);

	separatorLineEdit->setText(_sep);
	
	for(int i=0; i<_nCategories; i++)
		for(int j=0; j<_nDivisions[i]; j++)
			listWidgets[i]->addItem(_divisions[i].at(j));
	
	maxSubgroupsPerYearLabel->setText(tr("Max subgroups per year: %1").arg(MAX_TOTAL_SUBGROUPS));
	maxTotalSubgroupsLabel->setText(tr("Max total subgroups: %1").arg(MAX_TOTAL_SUBGROUPS));
	
	disconnect(categoriesSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SplitYearForm::numberOfCategoriesChanged);
	categoriesSpinBox->setValue(_nCategories);
	connect(categoriesSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SplitYearForm::numberOfCategoriesChanged);
	
	numberOfCategoriesChanged();
	
	if(_nCategories>0)
		tabIndexChanged(0);
}

SplitYearForm::~SplitYearForm()
{
	saveFETDialogGeometry(this);
}

void SplitYearForm::tabIndexChanged(int i)
{
	if(i>=0){
		assert(i<MAX_CATEGORIES);
		if(listWidgets[i]->count()>0)
			listWidgets[i]->setCurrentRow(0);
	}
}

void SplitYearForm::numberOfCategoriesChanged()
{
	for(int i=0; i<MAX_CATEGORIES; i++)
		if(i<categoriesSpinBox->value())
			tabWidget->setTabEnabled(i, true);
		else
			tabWidget->setTabEnabled(i, false);
	
	updateNumberOfSubgroups();
	updateDivisionsLabel();
	
	if(categoriesSpinBox->value()==0){
		tabWidget->setCurrentIndex(0);

		addPushButton->setEnabled(false);
		modifyPushButton->setEnabled(false);
		removePushButton->setEnabled(false);
		removeAllPushButton->setEnabled(false);
		
		separatorGroupBox->setEnabled(false);
	}
	else{
		addPushButton->setEnabled(true);
		modifyPushButton->setEnabled(true);
		removePushButton->setEnabled(true);
		removeAllPushButton->setEnabled(true);
		
		separatorGroupBox->setEnabled(true);
	}
}

void SplitYearForm::addClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	bool ok;
	QString text=QInputDialog::getText(this, tr("FET - Add division to category %1").arg(i+1),
	 tr("Please input division name:"), QLineEdit::Normal, QString(""), &ok);
	if(ok && !text.isEmpty()){
		for(int k=0; k<categoriesSpinBox->value(); k++)
			for(int j=0; j<listWidgets[k]->count(); j++)
				if(listWidgets[k]->item(j)->text()==text){
					QMessageBox::information(this, tr("FET information"), tr("Duplicate names are not allowed (the current string is found in category number %1, division number %2).")
					 .arg(k+1).arg(j+1));
					return;
				}
	
		listWidgets[i]->addItem(text);
		listWidgets[i]->setCurrentRow(listWidgets[i]->count()-1);

		updateNumberOfSubgroups();
		updateDivisionsLabel();
	}
}

void SplitYearForm::modifyClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	modifyDoubleClicked(i);
}

void SplitYearForm::modifyDoubleClicked(int i)
{
	if(listWidgets[i]->currentRow()>=0 && listWidgets[i]->currentRow()<listWidgets[i]->count()){
		QString ts=listWidgets[i]->currentItem()->text();

		bool ok;
		QString text=QInputDialog::getText(this, tr("FET - Modify division to category %1").arg(i+1),
		 tr("Please input the new division name:"), QLineEdit::Normal, ts, &ok);
		if(ok && !text.isEmpty()){
			if(text!=ts){
				for(int k=0; k<categoriesSpinBox->value(); k++)
					for(int j=0; j<listWidgets[k]->count(); j++)
						if(listWidgets[k]->item(j)->text()==text){
							QMessageBox::information(this, tr("FET information"), tr("Duplicate names are not allowed (the current string is found in category number %1, division number %2).")
							 .arg(k+1).arg(j+1));
							return;
						}
						
				listWidgets[i]->currentItem()->setText(text);
			}
		}
	}
}

void SplitYearForm::list0DoubleClicked()
{
	modifyDoubleClicked(0);
}

void SplitYearForm::list1DoubleClicked()
{
	modifyDoubleClicked(1);
}

void SplitYearForm::list2DoubleClicked()
{
	modifyDoubleClicked(2);
}

void SplitYearForm::list3DoubleClicked()
{
	modifyDoubleClicked(3);
}

void SplitYearForm::list4DoubleClicked()
{
	modifyDoubleClicked(4);
}

void SplitYearForm::list5DoubleClicked()
{
	modifyDoubleClicked(5);
}

void SplitYearForm::list6DoubleClicked()
{
	modifyDoubleClicked(6);
}

void SplitYearForm::list7DoubleClicked()
{
	modifyDoubleClicked(7);
}

void SplitYearForm::list8DoubleClicked()
{
	modifyDoubleClicked(8);
}

void SplitYearForm::list9DoubleClicked()
{
	modifyDoubleClicked(9);
}

void SplitYearForm::list10DoubleClicked()
{
	modifyDoubleClicked(10);
}

void SplitYearForm::list11DoubleClicked()
{
	modifyDoubleClicked(11);
}

void SplitYearForm::list12DoubleClicked()
{
	modifyDoubleClicked(12);
}

void SplitYearForm::list13DoubleClicked()
{
	modifyDoubleClicked(13);
}

void SplitYearForm::removeClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	if(listWidgets[i]->currentRow()>=0 && listWidgets[i]->currentRow()<listWidgets[i]->count()){
		QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
		 tr("Do you want to remove division %1 from category %2?").arg(listWidgets[i]->currentItem()->text())
		 .arg(i+1), QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	
		int j=listWidgets[i]->currentRow();
		listWidgets[i]->setCurrentRow(-1);
		QListWidgetItem* item;
		item=listWidgets[i]->takeItem(j);
		delete item;
		
		if(j>=listWidgets[i]->count())
			j=listWidgets[i]->count()-1;
		if(j>=0)
			listWidgets[i]->setCurrentRow(j);

		updateNumberOfSubgroups();
		updateDivisionsLabel();
	}
}

void SplitYearForm::removeAllClicked()
{
	int i=tabWidget->currentIndex();
	if(i<0 || i>=tabWidget->count())
		return;

	QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
	 tr("Do you really want to remove all divisions from category %1?").arg(i+1),
	 QMessageBox::Yes|QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;
	
	listWidgets[i]->clear();

	updateNumberOfSubgroups();
	updateDivisionsLabel();
}

void SplitYearForm::ok()
{
	if(categoriesSpinBox->value()>4){
		QMessageBox::StandardButton ret=QMessageBox::warning(this, tr("FET warning"),
		 tr("You want to divide the year by %1 categories. The recommended number of categories"
		 " is 2, 3 or maximum 4 (to ensure the timetable generation speed and feasibility). Are you sure?")
		 .arg(categoriesSpinBox->value()),
		 QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	}

	qint64 product=1;
	
	for(int i=0; i<categoriesSpinBox->value(); i++){
		product*=listWidgets[i]->count();

		if(product>MAX_SUBGROUPS_PER_YEAR){
			QMessageBox::information(this, tr("FET information"), tr("The current number of subgroups for this year is too large"
			 " (the maximum allowed value is %1, but computing up to category %2 gives %3 subgroups)")
			 .arg(MAX_SUBGROUPS_PER_YEAR).arg(i+1).arg(product));
			return;
		}
	}
	
	if(product==0){
		QMessageBox::information(this, tr("FET information"), tr("Each category must contain at least one division"));
		return;
	}
	
	//warn if there are too many total subgroups - suggested by Volker Dirr
	QSet<QString> tmpSet;
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
		if(sty->name!=year){
			if(sty->groupsList.count()==0){
				tmpSet.insert(sty->name);
			}
			else{
				for(StudentsGroup* stg : std::as_const(sty->groupsList)){
					if(stg->subgroupsList.count()==0){
						tmpSet.insert(stg->name);
					}
					else{
						for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList))
							tmpSet.insert(sts->name);
					}
				}
			}
		}
	}
	int totalEstimated=tmpSet.count()+int(product);
	if(totalEstimated>MAX_TOTAL_SUBGROUPS){
		QMessageBox::StandardButton ret=QMessageBox::warning(this, tr("FET warning"),
		 tr("Please note that the current configuration will lead you to %1 total number of subgroups."
		 " The file format supports any number of students sets, but for the timetable generation to be"
		 " possible the maximum allowed total number of subgroups is %2.").arg(totalEstimated).arg(MAX_TOTAL_SUBGROUPS)
		 +QString("\n\n")+tr("Are you sure you want to continue?"),
		 QMessageBox::Yes|QMessageBox::Cancel);
		if(ret==QMessageBox::Cancel)
			return;
	}
	
	QString separator=separatorLineEdit->text();
	
	QSet<QString> tmp;
	for(int i=0; i<categoriesSpinBox->value(); i++)
		for(int j=0; j<listWidgets[i]->count(); j++){
			QString ts=listWidgets[i]->item(j)->text();
			if(tmp.contains(ts)){
				QMessageBox::information(this, tr("FET information"), tr("Duplicate names not allowed (%1 appears a second time in category number %2, division number %3).",
				 "%1 is the name of a division (of a year).")
				 .arg(ts).arg(i+1).arg(j+1));
				return;
			}
			else if(ts.isEmpty()){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed (the entry in category number %1, division number %2 has an empty name).").arg(i+1).arg(j+1));
				return;
			}
			tmp.insert(ts);
		}
		
	QSet<QString> existingNames;
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
		assert(!existingNames.contains(sty->name));
		existingNames.insert(sty->name);
		if(sty->name!=year){
			for(StudentsGroup* group : std::as_const(sty->groupsList)){
				if(!existingNames.contains(group->name))
					existingNames.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
					if(!existingNames.contains(subgroup->name))
						existingNames.insert(subgroup->name);
				}
			}
		}
	}

	QSet<QString> newStudentsSets;
	for(int j=0; j<listWidgets[0]->count(); j++){
		QString ts=year+separator+listWidgets[0]->item(j)->text();
		if(existingNames.contains(ts)){
			QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with the same name exists."
			 " Please choose another name or remove the old set").arg(ts));
			return;
		}
		newStudentsSets.insert(ts);
	}

	if(firstCategoryPermanentCheckBox->isChecked() && categoriesSpinBox->value()>=3){
		for(int fc=0; fc<listWidgets[0]->count(); fc++){
			for(int i=1; i<categoriesSpinBox->value(); i++){
				for(int j=0; j<listWidgets[i]->count(); j++){
					QString ts=year+separator+listWidgets[0]->item(fc)->text()+separator+listWidgets[i]->item(j)->text();
					if(existingNames.contains(ts)){
						QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with the same name exists."
						 " Please choose another name or remove the old set").arg(ts));
						return;
					}
					newStudentsSets.insert(ts);
				}
			}
		}
	}

	for(int i=1; i<categoriesSpinBox->value(); i++){
		for(int j=0; j<listWidgets[i]->count(); j++){
			QString ts=year+separator+listWidgets[i]->item(j)->text();
			if(existingNames.contains(ts)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with the same name exists."
				 " Please choose another name or remove the old set").arg(ts));
				return;
			}
			newStudentsSets.insert(ts);
		}
	}

	//As in Knuth TAOCP vol 4A section 7.2.1.1, generate all n-tuples
	int b[MAX_CATEGORIES];
	int ii;
	
	if(categoriesSpinBox->value()>=2){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QString sb=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sb+=separator+listWidgets[i]->item(b[i])->text();
			if(existingNames.contains(sb)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add subgroup %1, because a set with the same name exists. "
				 "Please choose another name or remove the old set").arg(sb));
				return;
			}
			newStudentsSets.insert(sb);
			ii=categoriesSpinBox->value()-1;
again_here_1:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_1;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}

	//check for new conflicts
	QSet<QString> alreadyExistingNewSets;
	for(int fc=0; fc<listWidgets[0]->count(); fc++){
		QString ts=year+separator+listWidgets[0]->item(fc)->text();
		
		if(alreadyExistingNewSets.contains(ts)){
			QMessageBox::information(this, tr("FET information"), tr("Cannot add the group %1, because it would have the same name as another set in this same year."
			 " Please review your divisions' names and consider that each final group and subgroup must have different names.").arg(ts));
			return;
		}
		
		alreadyExistingNewSets.insert(ts);
		
		if(firstCategoryPermanentCheckBox->isChecked() && categoriesSpinBox->value()>=3){
			for(int i=1; i<categoriesSpinBox->value(); i++){
				for(int j=0; j<listWidgets[i]->count(); j++){
					QString ts2=year+separator+listWidgets[0]->item(fc)->text()+separator+listWidgets[i]->item(j)->text();

					if(alreadyExistingNewSets.contains(ts2)){
						QMessageBox::information(this, tr("FET information"), tr("Cannot add the group %1, because it would have the same name as another set in this same year."
						 " Please review your divisions' names and consider that each final group and subgroup must have different names.").arg(ts2));
						return;
					}

					alreadyExistingNewSets.insert(ts2);
				}
			}
		}
	}
	if(categoriesSpinBox->value()>=2){
		for(int i=1; i<categoriesSpinBox->value(); i++){
			for(int j=0; j<listWidgets[i]->count(); j++){
				QString ts=year+separator+listWidgets[i]->item(j)->text();

			if(alreadyExistingNewSets.contains(ts)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add the group %1, because it would have the same name as another set in this same year."
				 " Please review your divisions' names and consider that each final group and subgroup must have different names.").arg(ts));
				return;
			}

				alreadyExistingNewSets.insert(ts);
			}
		}
	}
	
	if(categoriesSpinBox->value()>=2){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QString sbn=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sbn+=separator+listWidgets[i]->item(b[i])->text();

			if(alreadyExistingNewSets.contains(sbn)){
				QMessageBox::information(this, tr("FET information"), tr("Cannot add the subgroup %1, because it would have the same name as another set in this same year."
				 " Please review your divisions' names and consider that each final group and subgroup must have different names.").arg(sbn));
				return;
			}

			alreadyExistingNewSets.insert(sbn);

			ii=categoriesSpinBox->value()-1;
again_here_2:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_2;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}

	QHash<QString, StudentsGroup*> groupsHash;
	
	StudentsYear* yearPointer=nullptr;
	int yearIndex=-1;
	for(int i=0; i<gt.rules.yearsList.count(); i++)
		if(gt.rules.yearsList[i]->name==year){
			yearPointer=gt.rules.yearsList[i];
			yearIndex=i;
			break;
		}
	assert(yearPointer!=nullptr);
	assert(yearIndex>=0);
	
	QSet<QString> notExistingGroupsSet;
	QSet<QString> notExistingSubgroupsSet;
	QStringList notExistingGroupsList;
	QStringList notExistingSubgroupsList;
	for(StudentsGroup* group : std::as_const(yearPointer->groupsList)){
		if(!existingNames.contains(group->name) && !newStudentsSets.contains(group->name) && !notExistingGroupsSet.contains(group->name)){
			notExistingGroupsSet.insert(group->name);
			notExistingGroupsList.append(group->name);
		}
		for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
			if(!existingNames.contains(subgroup->name) && !newStudentsSets.contains(subgroup->name) && !notExistingSubgroupsSet.contains(subgroup->name)){
				notExistingSubgroupsSet.insert(subgroup->name);
				notExistingSubgroupsList.append(subgroup->name);
			}
		}
	}
	
	bool removeGroupsOrSubgroups = notExistingGroupsList.count()>0 || notExistingSubgroupsList.count()>0;
	
	QString description=QString("");
	if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()>0)
		description+=tr("WARNING: There are groups and subgroups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");
	else if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()==0)
		description+=tr("WARNING: There are groups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");
	else if(notExistingGroupsList.count()==0 && notExistingSubgroupsList.count()>0)
		description+=tr("WARNING: There are subgroups which will no longer be available and which will be removed, along with the associated"
		 " activities and constraints. Are you sure? See the list below.");

	if(notExistingGroupsList.count()>0 || notExistingSubgroupsList.count()>0){
		description+=" (";
		if(notExistingGroupsList.count()>0 && notExistingSubgroupsList.count()>0)
			description+=tr("Notes:");
		else
			description+=tr("Note:");
		description+=" ";
		if(notExistingGroupsList.count()>0){
			description+=tr("To keep a group, you need to keep the corresponding division name and use the same separator(s) character(s).");
			description+=" ";
		}
		if(notExistingSubgroupsList.count()>0){
			description+=tr("Probably you can safely ignore the warning about the removal of the subgroups.");
			description+=" ";
		}
		description+=tr("Read the divide year dialog Help for details.");
		description+=")";
	}
	
	if(!description.isEmpty())
		description+="\n\n";
	
	if(notExistingGroupsList.count()>0){
		description+=tr("The following groups will no longer be available:");
		description+="\n\n";
		description+=notExistingGroupsList.join("\n");
	}
	if(notExistingSubgroupsList.count()>0){
		if(notExistingGroupsList.count()>0)
			description+="\n\n";
	
		description+=tr("The following subgroups will no longer be available:");
		description+="\n\n";
		description+=notExistingSubgroupsList.join("\n");
	}
	
	if(!description.isEmpty()){
		int lres=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
		 description, tr("Yes"), tr("No"), QString(), 0, 1);
		if(lres!=0){
			return;
		}
		
		QMessageBox::StandardButton t=QMessageBox::warning(this, tr("FET warning"), tr("Year %1 will be split again."
		 " All groups and subgroups of this year which will no longer exist (listed before) and the associated activities and constraints"
		 " will be removed. Are you absolutely sure?").arg(year), QMessageBox::Yes|QMessageBox::Cancel);
		
		if(t==QMessageBox::Cancel)
			return;
	}
	
	QString sb;

	sb+=tr("Separator: %1", "The separator character").arg(yearPointer->separator);
	sb+=QString("\n");
	QString tsb;
	if(yearPointer->firstCategoryIsPermanent)
		tsb=tr("yes");
	else
		tsb=tr("no");
	sb+=tr("First category is permanent: %1", "%1 is yes or no").arg(tsb);
	sb+=QString("\n");

	sb+=tr("Number of categories: %1").arg(yearPointer->divisions.count());
	sb+=QString("\n");
	for(int i=0; i<yearPointer->divisions.count(); i++){
		sb+=tr("Category %1: %2").arg(i+1).arg(yearPointer->divisions.at(i).join(translatedCommaSpace()));
		sb+=QString("\n");
	}
	/*if(yearPointer->divisions.count()==0){
		sb+=tr("No categories");
		sb+=QString("\n");
	}*/
	
	StudentsYear* newYear=new StudentsYear;
	newYear->name=yearPointer->name;
	newYear->longName=yearPointer->longName;
	newYear->code=yearPointer->code;
	newYear->numberOfStudents=yearPointer->numberOfStudents;
	
	newYear->comments=yearPointer->comments;
	
	newYear->indexInAugmentedYearsList=yearPointer->indexInAugmentedYearsList;
	
	QHash<QString, int> numberOfStudents;
	for(StudentsGroup* group : std::as_const(yearPointer->groupsList)){
		numberOfStudents.insert(group->name, group->numberOfStudents);
		for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
			numberOfStudents.insert(subgroup->name, subgroup->numberOfStudents);
	}
	
	//add groups and subgroups
	for(int fc=0; fc<listWidgets[0]->count(); fc++){
		QString ts=year+separator+listWidgets[0]->item(fc)->text();
		StudentsGroup* gr=new StudentsGroup;
		gr->name=ts;
		if(numberOfStudents.contains(gr->name))
			gr->numberOfStudents=numberOfStudents.value(gr->name);
		bool t=gt.rules.addGroupFast(newYear, gr);
		
		assert(t);
		
		assert(!groupsHash.contains(gr->name));
		groupsHash.insert(gr->name, gr);
		
		if(firstCategoryPermanentCheckBox->isChecked() && categoriesSpinBox->value()>=3){
			for(int i=1; i<categoriesSpinBox->value(); i++){
				for(int j=0; j<listWidgets[i]->count(); j++){
					QString ts2=year+separator+listWidgets[0]->item(fc)->text()+separator+listWidgets[i]->item(j)->text();
					StudentsGroup* gr=new StudentsGroup;
					gr->name=ts2;
					if(numberOfStudents.contains(gr->name))
						gr->numberOfStudents=numberOfStudents.value(gr->name);
					bool t=gt.rules.addGroupFast(newYear, gr);
					
					assert(t);
					
					assert(!groupsHash.contains(gr->name));
					groupsHash.insert(gr->name, gr);
				}
			}
		}
	}
	if(categoriesSpinBox->value()>=2){
		for(int i=1; i<categoriesSpinBox->value(); i++){
			for(int j=0; j<listWidgets[i]->count(); j++){
				QString ts=year+separator+listWidgets[i]->item(j)->text();
				StudentsGroup* gr=new StudentsGroup;
				gr->name=ts;
				if(numberOfStudents.contains(gr->name))
					gr->numberOfStudents=numberOfStudents.value(gr->name);
				bool t=gt.rules.addGroupFast(newYear, gr);
				
				assert(t);
				
				assert(!groupsHash.contains(gr->name));
				groupsHash.insert(gr->name, gr);
			}
		}
	}
	
	QHash<QString, StudentsSubgroup*> newSubgroupsHash;

	if(categoriesSpinBox->value()>=2){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QStringList groups;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				groups.append(year+separator+listWidgets[i]->item(b[i])->text());
			
			QString sbn=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sbn+=separator+listWidgets[i]->item(b[i])->text();
			
			StudentsSubgroup* sb=new StudentsSubgroup;
			sb->name=sbn;
			if(numberOfStudents.contains(sb->name))
				sb->numberOfStudents=numberOfStudents.value(sb->name);
			
			if(firstCategoryPermanentCheckBox->isChecked()){
				assert(!newSubgroupsHash.contains(sbn));
				newSubgroupsHash.insert(sbn, sb);
			}

			for(int i=0; i<categoriesSpinBox->value(); i++){
				assert(groupsHash.contains(groups.at(i)));
				bool t=gt.rules.addSubgroupFast(newYear, groupsHash.value(groups.at(i)), sb);
				assert(t);
			}

			ii=categoriesSpinBox->value()-1;
again_here_3:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_3;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}

	if(firstCategoryPermanentCheckBox->isChecked() && categoriesSpinBox->value()>=3){
		for(int i=0; i<categoriesSpinBox->value(); i++)
			b[i]=0;
		
		for(;;){
			QStringList groups;
			for(int i=1; i<categoriesSpinBox->value(); i++)
				groups.append(year+separator+listWidgets[0]->item(b[0])->text()+separator+listWidgets[i]->item(b[i])->text());
			
			QString sbn=year;
			for(int i=0; i<categoriesSpinBox->value(); i++)
				sbn+=separator+listWidgets[i]->item(b[i])->text();
			
			assert(newSubgroupsHash.contains(sbn));
			StudentsSubgroup* sb=newSubgroupsHash.value(sbn);
			/*StudentsSubgroup* sb=new StudentsSubgroup;
			sb->name=sbn;
			if(numberOfStudents.contains(sb->name))
				sb->numberOfStudents=numberOfStudents.value(sb->name);*/

			for(int i=1; i<categoriesSpinBox->value(); i++){
				assert(groupsHash.contains(groups.at(i-1)));
				bool t=gt.rules.addSubgroupFast(newYear, groupsHash.value(groups.at(i-1)), sb);
				assert(t);
			}

			ii=categoriesSpinBox->value()-1;
again_here_4:
			if(b[ii]>=listWidgets[ii]->count()-1){
				ii--;
				if(ii<0)
					break;
				goto again_here_4;
			}
			else{
				b[ii]++;
				for(int i=ii+1; i<categoriesSpinBox->value(); i++)
					b[i]=0;
			}
		}
	}

	assert(yearIndex>=0 && yearIndex<gt.rules.yearsList.count());
	assert(gt.rules.yearsList[yearIndex]==yearPointer);
	gt.rules.yearsList[yearIndex]=newYear;
	
	QString s=QString("");
	
	int nActivitiesBefore=gt.rules.activitiesList.count();
	int nTimeConstraintsBefore=gt.rules.timeConstraintsList.count();
	int nSpaceConstraintsBefore=gt.rules.spaceConstraintsList.count();
	int nGroupActivitiesInInitialOrderItemsBefore=gt.rules.groupActivitiesInInitialOrderList.count();
	
	gt.rules.computePermanentStudentsHash();
	gt.rules.removeYearPointerAfterSplit(yearPointer);

	int nActivitiesAfter=gt.rules.activitiesList.count();
	int nTimeConstraintsAfter=gt.rules.timeConstraintsList.count();
	int nSpaceConstraintsAfter=gt.rules.spaceConstraintsList.count();
	int nGroupActivitiesInInitialOrderItemsAfter=gt.rules.groupActivitiesInInitialOrderList.count();
	
	if(removeGroupsOrSubgroups){
		s+="\n\n";
		s+=tr("There were removed %1 activities, %2 time constraints and %3 space constraints.")
		 .arg(nActivitiesBefore-nActivitiesAfter)
		 .arg(nTimeConstraintsBefore-nTimeConstraintsAfter)
		 .arg(nSpaceConstraintsBefore-nSpaceConstraintsAfter);
	
		if(nGroupActivitiesInInitialOrderItemsBefore!=nGroupActivitiesInInitialOrderItemsAfter)
			s+=QString(" ")+tr("There were removed %1 'group activities in the initial order' items.")
			 .arg(nGroupActivitiesInInitialOrderItemsBefore-nGroupActivitiesInInitialOrderItemsAfter);
	}
	else{
		assert(nActivitiesBefore==nActivitiesAfter);
		assert(nTimeConstraintsBefore==nTimeConstraintsAfter);
		assert(nSpaceConstraintsBefore==nSpaceConstraintsAfter);
		assert(nGroupActivitiesInInitialOrderItemsBefore==nGroupActivitiesInInitialOrderItemsAfter);
	}
	
	QMessageBox::information(this, tr("FET information"), tr("Split of the year complete, please check the groups and subgroups"
	 " of the year to make sure that everything is OK.")+s);
	
	//saving page
	_firstCategoryIsPermanent=firstCategoryPermanentCheckBox->isChecked();
	
	if(categoriesSpinBox->value()>0)
		_sep=separatorLineEdit->text();
	else
		_sep=QString(" ");
	
	_nCategories=categoriesSpinBox->value();
	
	for(int i=0; i<_nCategories; i++){
		_nDivisions[i]=listWidgets[i]->count();
		
		_divisions[i].clear();
		for(int j=0; j<listWidgets[i]->count(); j++)
			_divisions[i].append(listWidgets[i]->item(j)->text());
	}

	//2020-09-03
	newYear->firstCategoryIsPermanent=_firstCategoryIsPermanent;
	newYear->separator=_sep;

	QString sa;

	sa+=tr("Separator: %1").arg(_sep);
	sa+=QString("\n");
	QString tsa;
	if(_firstCategoryIsPermanent)
		tsa=tr("yes");
	else
		tsa=tr("no");
	sa+=tr("First category is permanent: %1", "%1 is yes or no").arg(tsa);
	sa+=QString("\n");

	sa+=tr("Number of categories: %1").arg(_nCategories);
	sa+=QString("\n");
	for(int i=0; i<_nCategories; i++){
		newYear->divisions.append(_divisions[i]);

		sa+=tr("Category %1: %2").arg(i+1).arg(_divisions[i].join(translatedCommaSpace()));
		sa+=QString("\n");
	}
	/*if(_nCategories==0){
		sa+=tr("No categories");
		sa+=QString("\n");
	}*/

	gt.rules.addUndoPoint(tr("Modified the categories and the divisions of the year %1 from\n\n%2\ninto\n\n%3")
	 .arg(newYear->name).arg(sb).arg(sa));
	
	//No need for gt.rules.internalStructureComputed=false and the rest of it, because there was invoked gt.rules.removeYearPointerAfterSplit(yearPointer)
	//(and in case the number of categories is nonzero, there was also invoked addGroupFast and addSubgroupFast).
	
	this->close();
}

void SplitYearForm::help()
{
	QString s;

	s+=tr("You might first want to consider if dividing a year is necessary and on what options. Please remember"
	 " that FET can handle activities with multiple teachers/students sets. If you have say students set 9a, which is split"
	 " into 2 parts: English (teacher TE) and French (teacher TF), and language activities must be simultaneous, then you might not want to divide"
	 " according to this category, but add more larger activities, with students set 9a and teachers TE+TF.");
	
	s+="\n\n";
	
	s+=tr("Please choose a number of categories and in each category the number of divisions. You can choose for instance"
	 " 3 categories, 5 divisions for the first category: a, b, c, d and e, 2 divisions for the second category: boys and girls,"
	 " and 3 divisions for the third: English, German and French.");

	s+="\n\n";

	s+=tr("If your number of subgroups is reasonable, probably you need not worry about empty subgroups (regarding speed of generation)."
		" But more tests need to be done. You just need to know that for the moment the maximum total number of subgroups is %1 (which can be changed,"
		" but nobody needed larger values)").arg(MAX_TOTAL_SUBGROUPS);

	s+="\n\n";

	s+=tr("If you intend to divide again a year by categories and you want to keep (the majority of) the existing groups in this year,"
		" you will need to use the exact same separator character(s) for dividing this year as you used when previously dividing this year,"
		" and the same division names (any old division which is no longer entered means a group which will be removed from this year).");
		
	s+="\n\n";
	
	s+=tr("When dividing again a year, you might get a warning about subgroups which will be removed. If you didn't explicitly use"
		" (these) subgroups in your activities or constraints, probably you can safely ignore this warning. Generally, if you use years' division"
		" by categories, it is groups that matter.");
	
	s+="\n\n";

	s+=tr("Separator character(s) is of your choice (default is space)");
	
	s+="\n\n";
	
	s+=tr("If you have many subgroups and you don't explicitly use them, it is recommended to use the three global settings: hide subgroups"
		" in combo boxes, hide subgroups in activity planning, and do not write subgroups timetables on hard disk.");
	s+="\n";
	s+=tr("Note that if you are only working to get a feasible timetable, without the need to obtain the students timetable (XML or HTML) on"
		" the disk at all, and if you have many total subgroups, a good idea is to disable writing the subgroups, groups AND years timetables"
		" to the hard disk, as these take a long time to compute (not only subgroups, but also groups and years!).");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is large.)");
	s+=" ";
	s+=tr("After that, you can enable the writing of the students timetables on the disk, and regenerate.");
	
	s+="\n\n";
	s+=tr("About using a large number of categories, divisions per category and subgroups: it is highly recommended to"
		" keep these to a minimum, especially the number of categories, by using any kind of tricks. Otherwise the timetable"
		" might become impossible (taking too much time to generate).");
	s+=" ";
	s+=tr("Maybe a reasonable number of categories could be 2, 3 or maximum 4. The divide year dialog allows much higher values, but"
		" these are not at all recommended.");
	s+=" ";
	s+=tr("The generation time of the timetable is directly proportional with the total number of subgroups!");
	s+=" ";
	s+=tr("For instance, by reducing the total number of subgroups from 10000 to 100 the generation time might decrease 100 times!");
	s+="\n";
	s+=tr("Maybe an alternative to dividing a year into many categories/subgroups would be to enter individual students as FET subgroups and add into"
		" each group the corresponding subgroups. But this is hard to do from the FET interface - maybe a solution would be to use an automatic"
		" tool to convert your institution data into a file in .fet format.");
	s+=" ";
	s+=tr("Or you might use the FET feature to import students sets from comma separated values (CSV) files.");
	s+=" ";
	s+=tr("In such cases (individual students as FET subgroups), remember that a smaller number of total subgroups means faster generation time, so"
		" you might want to consider a single subgroup for two or more students who have the exact same activities and constraints.");
	
	s+="\n\n";
	s+=tr("The option 'Consider the first category/divisions as permanent': it has effect only if the number of categories is greater or equal to 3,"
		" and will generate additional groups, considering the first category and each of the other categories.");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on dividing a year"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
}

void SplitYearForm::reset() //reset to defaults
{
	QMessageBox::StandardButton ret=QMessageBox::question(this, tr("FET confirmation"),
	 tr("Do you really want to reset the form values to defaults (empty)?"),
	 QMessageBox::Yes|QMessageBox::Cancel);
	if(ret==QMessageBox::Cancel)
		return;

	separatorLineEdit->setText(" ");
	
	for(int i=0; i<MAX_CATEGORIES; i++)
		listWidgets[i]->clear();
	
	categoriesSpinBox->setValue(0);

	firstCategoryPermanentCheckBox->setChecked(false);
}

void SplitYearForm::updateNumberOfSubgroups()
{
	qint64 n=1;
	for(int i=0; i<categoriesSpinBox->value(); i++)
		n*=listWidgets[i]->count();
	currentSubgroupsLabel->setText(tr("Subgroups: %1", "%1 is the number of subgroups").arg(n));
}

void SplitYearForm::updateDivisionsLabel()
{
	QString ts;
	
	if(categoriesSpinBox->value()>0){
		ts=CustomFETString::number(listWidgets[0]->count());
		for(int i=1; i<categoriesSpinBox->value(); i++)
			ts+=QString(2, ' ')+CustomFETString::number(listWidgets[i]->count());
	}
	else{
		ts=QString("");
	}

	divisionsLabel->setText(ts);
}

void SplitYearForm::copyFromAnotherYear()
{
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("Copy categories and divisions from another year"));
	
	QComboBox* yearsComboBox=new QComboBox;
	for(StudentsYear* sy : std::as_const(gt.rules.yearsList))
		if(sy->name!=year)
			yearsComboBox->addItem(sy->name);
	if(yearsComboBox->count()>=1)
		yearsComboBox->setCurrentIndex(0);
	
	QLabel* label=new QLabel(tr("Please select another year to copy categories and divisions from:"));
	
	QDialogButtonBox* buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	
	QVBoxLayout* layout=new QVBoxLayout;
	
	layout->addWidget(label);
	layout->addWidget(yearsComboBox);
	layout->addWidget(buttonBox);
	
	dialog.setLayout(layout);

	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, "CopyCategoriesAndDivisionsFromAnotherYearForm");

	int res=dialog.exec();
	if(res==QDialog::Accepted){
		int i=gt.rules.searchYear(yearsComboBox->currentText());
		if(i>=0){
			StudentsYear* y=gt.rules.yearsList.at(i);
			
			separatorLineEdit->setText(y->separator);
			
			categoriesSpinBox->setValue(y->divisions.count());

			for(int i=0; i<MAX_CATEGORIES; i++)
				listWidgets[i]->clear();
			
			for(int i=0; i < y->divisions.count(); i++)
				for(int j=0; j < y->divisions.at(i).count(); j++)
					listWidgets[i]->addItem(y->divisions.at(i).at(j));

			updateNumberOfSubgroups();
			updateDivisionsLabel();

			firstCategoryPermanentCheckBox->setChecked(y->firstCategoryIsPermanent);
		}
	}

	saveFETDialogGeometry(&dialog, "CopyCategoriesAndDivisionsFromAnotherYearForm");
}
