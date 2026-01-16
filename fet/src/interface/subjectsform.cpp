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

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "subjectsform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"

#include <QInputDialog>

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QListWidget>
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

SubjectsForm::SubjectsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentSubjectTextEdit->setReadOnly(true);
	
	renameSubjectPushButton->setDefault(true);

	subjectsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, &QPushButton::clicked, this, &SubjectsForm::close);
	connect(addSubjectPushButton, &QPushButton::clicked, this, &SubjectsForm::addSubject);
	connect(removeSubjectPushButton, &QPushButton::clicked, this, &SubjectsForm::removeSubject);
	
	connect(renameSubjectPushButton, &QPushButton::clicked, this, &SubjectsForm::renameSubject);

	connect(moveSubjectUpPushButton, &QPushButton::clicked, this, &SubjectsForm::moveSubjectUp);
	connect(moveSubjectDownPushButton, &QPushButton::clicked, this, &SubjectsForm::moveSubjectDown);

	connect(sortSubjectsPushButton, &QPushButton::clicked, this, &SubjectsForm::sortSubjects);
	connect(subjectsListWidget, &QListWidget::currentRowChanged, this, &SubjectsForm::subjectChanged);
	connect(activateSubjectPushButton, &QPushButton::clicked, this, &SubjectsForm::activateSubject);
	connect(deactivateSubjectPushButton, &QPushButton::clicked, this, &SubjectsForm::deactivateSubject);
	connect(subjectsListWidget, &QListWidget::itemDoubleClicked, this, &SubjectsForm::renameSubject);

	connect(longNamePushButton, &QPushButton::clicked, this, &SubjectsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &SubjectsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &SubjectsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addSubjectPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addSubjectPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{renameSubjectPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	renameSubjectPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeSubjectPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeSubjectPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activateSubjectPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activateSubjectPushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivateSubjectPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivateSubjectPushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveSubjectUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveSubjectUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveSubjectDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveSubjectDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	subjectsListWidget->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsListWidget->addItem(sbj->name);
	}
	
	if(subjectsListWidget->count()>0)
		subjectsListWidget->setCurrentRow(0);
}


SubjectsForm::~SubjectsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void SubjectsForm::addSubject()
{
	bool ok = false;
	Subject* sbj=new Subject();
	sbj->name = QInputDialog::getText( this, tr("Add subject"), tr("Please enter subject's name") ,
	 QLineEdit::Normal, QString(), &ok );

	if ( ok && !((sbj->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addSubject(sbj)){
			QMessageBox::information( this, tr("Subject insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete sbj;
		}
		else{
			subjectsListWidget->addItem(sbj->name);
			subjectsListWidget->setCurrentRow(subjectsListWidget->count()-1);

			gt.rules.addUndoPoint(tr("Added the subject %1.").arg(sbj->name));
		}
	}
	else{
		if(ok){ //the user entered nothing
			QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		}
		delete sbj;// user entered nothing or pressed Cancel
	}
}

void SubjectsForm::removeSubject()
{
	int i=subjectsListWidget->currentRow();
	if(subjectsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	QString text=subjectsListWidget->currentItem()->text();
	int subject_ID=gt.rules.searchSubject(text);
	if(subject_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete this subject and all related activities and constraints?"),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/
	if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete this subject and all related activities and constraints?"),
	 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
		return;

	int tmp=gt.rules.removeSubject(text);
	if(tmp){
		subjectsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=subjectsListWidget->takeItem(i);
		delete item;

		if(i>=subjectsListWidget->count())
			i=subjectsListWidget->count()-1;
		if(i>=0)
			subjectsListWidget->setCurrentRow(i);
		else
			currentSubjectTextEdit->setPlainText(QString(""));

		gt.rules.addUndoPoint(tr("Removed the subject %1.").arg(text));
	}
}

void SubjectsForm::renameSubject()
{
	int i=subjectsListWidget->currentRow();
	if(subjectsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString initialSubjectName=subjectsListWidget->currentItem()->text();

	int subject_ID=gt.rules.searchSubject(initialSubjectName);
	if(subject_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}

	bool ok = false;
	QString finalSubjectName;
	finalSubjectName = QInputDialog::getText( this, tr("Modify subject"), tr("Please enter new subject's name") ,
	 QLineEdit::Normal, initialSubjectName, &ok);

	if ( ok && !(finalSubjectName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchSubject(finalSubjectName)>=0){
			QMessageBox::information( this, tr("Subject insertion dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifySubject(initialSubjectName, finalSubjectName);
			subjectsListWidget->item(i)->setText(finalSubjectName);
			subjectChanged(subjectsListWidget->currentRow());

			gt.rules.addUndoPoint(tr("Renamed the subject from %1 to %2.").arg(initialSubjectName).arg(finalSubjectName));
		}
	}
}

void SubjectsForm::moveSubjectUp()
{
	if(subjectsListWidget->count()<=1)
		return;
	int i=subjectsListWidget->currentRow();
	if(i<0 || i>=subjectsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=subjectsListWidget->item(i)->text();
	QString s2=subjectsListWidget->item(i-1)->text();
	
	Subject* sbj1=gt.rules.subjectsList.at(i);
	Subject* sbj2=gt.rules.subjectsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	subjectsListWidget->item(i)->setText(s2);
	subjectsListWidget->item(i-1)->setText(s1);
	
	gt.rules.subjectsList[i]=sbj2;
	gt.rules.subjectsList[i-1]=sbj1;
	
	gt.rules.addUndoPoint(tr("Moved the subject %1 up.").arg(s1));

	subjectsListWidget->setCurrentRow(i-1);
	subjectChanged(i-1);
}

void SubjectsForm::moveSubjectDown()
{
	if(subjectsListWidget->count()<=1)
		return;
	int i=subjectsListWidget->currentRow();
	if(i<0 || i>=subjectsListWidget->count())
		return;
	if(i==subjectsListWidget->count()-1)
		return;
		
	QString s1=subjectsListWidget->item(i)->text();
	QString s2=subjectsListWidget->item(i+1)->text();
	
	Subject* sbj1=gt.rules.subjectsList.at(i);
	Subject* sbj2=gt.rules.subjectsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	subjectsListWidget->item(i)->setText(s2);
	subjectsListWidget->item(i+1)->setText(s1);
	
	gt.rules.subjectsList[i]=sbj2;
	gt.rules.subjectsList[i+1]=sbj1;

	gt.rules.addUndoPoint(tr("Moved the subject %1 down.").arg(s1));

	subjectsListWidget->setCurrentRow(i+1);
	subjectChanged(i+1);
}

void SubjectsForm::sortSubjects()
{
	gt.rules.sortSubjectsAlphabetically();
	
	gt.rules.addUndoPoint(tr("Sorted the subjects."));

	subjectsListWidget->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsListWidget->addItem(sbj->name);
	}
	
	if(subjectsListWidget->count()>0)
		subjectsListWidget->setCurrentRow(0);
}

void SubjectsForm::subjectChanged(int index)
{
	if(index<0){
		currentSubjectTextEdit->setPlainText(QString(""));
		return;
	}
	
	Subject* sb=gt.rules.subjectsList.at(index);
	assert(sb!=nullptr);
	QString s=sb->getDetailedDescriptionWithConstraints(gt.rules);
	currentSubjectTextEdit->setPlainText(s);
}

void SubjectsForm::activateSubject()
{
	if(subjectsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString subjectName=subjectsListWidget->currentItem()->text();
	
	int count=gt.rules.activateSubject(subjectName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Activated the subject %1 (%2 activities).", "%2 is the number of activated activities").arg(subjectName).arg(count));
}

void SubjectsForm::deactivateSubject()
{
	if(subjectsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	QString subjectName=subjectsListWidget->currentItem()->text();
	
	int count=gt.rules.deactivateSubject(subjectName);
	QMessageBox::information(this, tr("FET information"), tr("Deactivated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Deactivated subject %1 (%2 activities).", "%2 is the number of deactivated activities").arg(subjectName).arg(count));
}

void SubjectsForm::comments()
{
	int ind=subjectsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	Subject* sbj=gt.rules.subjectsList[ind];
	assert(sbj!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Subject comments"));
	
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
	commentsPT->setPlainText(sbj->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("SubjectCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=sbj->comments;

		sbj->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed the comments for the subject %1 from\n%2\nto\n%3.").arg(sbj->name).arg(oc).arg(sbj->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subjectChanged(ind);
	}
}

void SubjectsForm::longName()
{
	int ind=subjectsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	Subject* sbj=gt.rules.subjectsList[ind];
	assert(sbj!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Subject long name"));
	
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
	longNameLE->setText(sbj->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("SubjectLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=sbj->longName;
	
		sbj->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the subject %1 from\n%2\nto\n%3.").arg(sbj->name).arg(oln).arg(sbj->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subjectChanged(ind);
	}
}

void SubjectsForm::code()
{
	int ind=subjectsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subject"));
		return;
	}
	
	Subject* sbj=gt.rules.subjectsList[ind];
	assert(sbj!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Subject code"));
	
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
	codeLE->setText(sbj->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("SubjectCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=sbj->code;
	
		sbj->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the subject %1 from\n%2\nto\n%3.").arg(sbj->name).arg(oc).arg(sbj->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subjectChanged(ind);
	}
}
