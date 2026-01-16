//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addstudentsyearform.h"
#include "modifystudentsyearform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "yearsform.h"
#include "studentsset.h"

#include "splityearform.h"

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;
extern bool teachers_schedule_ready;

YearsForm::YearsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	detailsTextEdit->setReadOnly(true);
	
	modifyYearPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addYearPushButton, &QPushButton::clicked, this, &YearsForm::addYear);
	connect(closePushButton, &QPushButton::clicked, this, &YearsForm::close);
	connect(removeYearPushButton, &QPushButton::clicked, this, &YearsForm::removeYear);
	
	connect(yearsListWidget, &QListWidget::currentRowChanged, this, &YearsForm::yearChanged);
	connect(modifyYearPushButton, &QPushButton::clicked, this, &YearsForm::modifyYear);

	connect(moveYearUpPushButton, &QPushButton::clicked, this, &YearsForm::moveYearUp);
	connect(moveYearDownPushButton, &QPushButton::clicked, this, &YearsForm::moveYearDown);

	connect(sortYearsPushButton, &QPushButton::clicked, this, &YearsForm::sortYears);
	connect(activateStudentsPushButton, &QPushButton::clicked, this, &YearsForm::activateStudents);
	connect(deactivateStudentsPushButton, &QPushButton::clicked, this, &YearsForm::deactivateStudents);
	connect(divisionsPushButton, &QPushButton::clicked, this, &YearsForm::divideYear);
	connect(yearsListWidget, &QListWidget::itemDoubleClicked, this, &YearsForm::modifyYear);

	connect(longNamePushButton, &QPushButton::clicked, this, &YearsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &YearsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &YearsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addYearPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addYearPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyYearPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyYearPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeYearPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeYearPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activateStudentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activateStudentsPushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivateStudentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivateStudentsPushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveYearUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveYearUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveYearDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveYearDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
}

YearsForm::~YearsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void YearsForm::addYear()
{
	AddStudentsYearForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	int i=yearsListWidget->count()-1;
	if(i>=0)
		yearsListWidget->setCurrentRow(i);
}

void YearsForm::removeYear()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete year %1 and all related groups, subgroups, activities and constraints?").arg(yearName),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/
	if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete year %1 and all related groups, subgroups, activities and constraints?").arg(yearName),
	 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
		return;

	bool tmp=gt.rules.removeYear(yearName);
	assert(tmp);
	if(tmp){
		gt.rules.addUndoPoint(tr("Removed the year %1.").arg(yearName));

		int q=yearsListWidget->currentRow();

		yearsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=yearsListWidget->takeItem(q);
		delete item;

		if(q>=yearsListWidget->count())
			q=yearsListWidget->count()-1;
		if(q>=0)
			yearsListWidget->setCurrentRow(q);
		else
			detailsTextEdit->setPlainText(QString(""));
	}
}

void YearsForm::yearChanged()
{
	if(yearsListWidget->currentRow()<0){
		detailsTextEdit->setPlainText("");
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearsListWidget->currentRow());
	detailsTextEdit->setPlainText(sty->getDetailedDescriptionWithConstraints(gt.rules));
}

void YearsForm::moveYearUp()
{
	if(yearsListWidget->count()<=1)
		return;
	int i=yearsListWidget->currentRow();
	if(i<0 || i>=yearsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=yearsListWidget->item(i)->text();
	QString s2=yearsListWidget->item(i-1)->text();
	
	StudentsYear* sy1=gt.rules.yearsList.at(i);
	StudentsYear* sy2=gt.rules.yearsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	yearsListWidget->item(i)->setText(s2);
	yearsListWidget->item(i-1)->setText(s1);
	
	gt.rules.yearsList[i]=sy2;
	gt.rules.yearsList[i-1]=sy1;
	
	gt.rules.addUndoPoint(tr("Moved the year %1 up.").arg(s1));

	yearsListWidget->setCurrentRow(i-1);
	yearChanged(/*i-1*/);
}

void YearsForm::moveYearDown()
{
	if(yearsListWidget->count()<=1)
		return;
	int i=yearsListWidget->currentRow();
	if(i<0 || i>=yearsListWidget->count())
		return;
	if(i==yearsListWidget->count()-1)
		return;
		
	QString s1=yearsListWidget->item(i)->text();
	QString s2=yearsListWidget->item(i+1)->text();
	
	StudentsYear* sy1=gt.rules.yearsList.at(i);
	StudentsYear* sy2=gt.rules.yearsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	yearsListWidget->item(i)->setText(s2);
	yearsListWidget->item(i+1)->setText(s1);
	
	gt.rules.yearsList[i]=sy2;
	gt.rules.yearsList[i+1]=sy1;
	
	gt.rules.addUndoPoint(tr("Moved the year %1 down.").arg(s1));

	yearsListWidget->setCurrentRow(i+1);
	yearChanged(/*i+1*/);
}

void YearsForm::sortYears()
{
	gt.rules.sortYearsAlphabetically();

	gt.rules.addUndoPoint(tr("Sorted the years."));

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}

	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
}

void YearsForm::modifyYear()
{
	int q=yearsListWidget->currentRow();
	int valv=yearsListWidget->verticalScrollBar()->value();
	int valh=yearsListWidget->horizontalScrollBar()->value();

	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(yearName);
	assert(studentsSet!=nullptr);
	int numberOfStudents=studentsSet->numberOfStudents;

	ModifyStudentsYearForm form(this, yearName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}
	
	yearsListWidget->verticalScrollBar()->setValue(valv);
	yearsListWidget->horizontalScrollBar()->setValue(valh);

	if(q>=yearsListWidget->count())
		q=yearsListWidget->count()-1;
	if(q>=0)
		yearsListWidget->setCurrentRow(q);
	else
		detailsTextEdit->setPlainText(QString(""));
}

void YearsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Activated the year %1 (%2 activities).", "%2 is the number of activated activities").arg(yearName).arg(count));
}

void YearsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(yearName);
	QMessageBox::information(this, tr("FET information"), tr("Deactivated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Deactivated year %1 (%2 activities).", "%2 is the number of deactivated activities").arg(yearName).arg(count));
}

void YearsForm::divideYear()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	SplitYearForm form(this, yearName);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void YearsForm::comments()
{
	int ind=yearsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(yearName);
	assert(studentsSet!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students year comments"));
	
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
	commentsPT->setPlainText(studentsSet->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("StudentsYearCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString ocs=studentsSet->comments;

		studentsSet->comments=commentsPT->toPlainText();

		gt.rules.addUndoPoint(tr("Changed the comments for the year %1 from\n%2\nto\n%3.").arg(yearName).arg(ocs).arg(studentsSet->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		yearChanged();
	}
}

void YearsForm::longName()
{
	int ind=yearsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(yearName);
	assert(studentsSet!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Year long name"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* longNameLE=new QLineEdit();
	longNameLE->setText(studentsSet->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("YearLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=studentsSet->longName;
	
		studentsSet->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the year %1 from\n%2\nto\n%3.").arg(studentsSet->name).arg(oln).arg(studentsSet->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		yearChanged();
	}
}

void YearsForm::code()
{
	int ind=yearsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(yearName);
	assert(studentsSet!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Year code"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getCodeDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCodeDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* codeLE=new QLineEdit();
	codeLE->setText(studentsSet->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("YearCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=studentsSet->code;
	
		studentsSet->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the year %1 from\n%2\nto\n%3.").arg(studentsSet->name).arg(oc).arg(studentsSet->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		yearChanged();
	}
}
