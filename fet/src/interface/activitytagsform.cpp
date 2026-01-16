//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
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
#include "activitytagsform.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"

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

ActivityTagsForm::ActivityTagsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentActivityTagTextEdit->setReadOnly(true);

	renameActivityTagPushButton->setDefault(true);

	activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, &QPushButton::clicked, this, &ActivityTagsForm::close);
	connect(addActivityTagPushButton, &QPushButton::clicked, this, &ActivityTagsForm::addActivityTag);
	connect(removeActivityTagPushButton, &QPushButton::clicked, this, &ActivityTagsForm::removeActivityTag);
	connect(renameActivityTagPushButton, &QPushButton::clicked, this, &ActivityTagsForm::renameActivityTag);
	
	connect(moveActivityTagUpPushButton, &QPushButton::clicked, this, &ActivityTagsForm::moveActivityTagUp);
	connect(moveActivityTagDownPushButton, &QPushButton::clicked, this, &ActivityTagsForm::moveActivityTagDown);

	connect(sortActivityTagsPushButton, &QPushButton::clicked, this, &ActivityTagsForm::sortActivityTags);
	connect(activityTagsListWidget, &QListWidget::currentRowChanged, this, &ActivityTagsForm::activityTagChanged);
	connect(activateActivityTagPushButton, &QPushButton::clicked, this, &ActivityTagsForm::activateActivityTag);
	connect(deactivateActivityTagPushButton, &QPushButton::clicked, this, &ActivityTagsForm::deactivateActivityTag);
	connect(activityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ActivityTagsForm::renameActivityTag);
	connect(helpPushButton, &QPushButton::clicked, this, &ActivityTagsForm::help);

	connect(printablePushButton, &QPushButton::clicked, this, &ActivityTagsForm::printableActivityTag);
	connect(notPrintablePushButton, &QPushButton::clicked, this, &ActivityTagsForm::notPrintableActivityTag);

	connect(longNamePushButton, &QPushButton::clicked, this, &ActivityTagsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &ActivityTagsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &ActivityTagsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addActivityTagPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addActivityTagPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{renameActivityTagPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	renameActivityTagPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeActivityTagPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeActivityTagPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activateActivityTagPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activateActivityTagPushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivateActivityTagPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivateActivityTagPushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveActivityTagUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveActivityTagUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveActivityTagDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveActivityTagDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	activityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(at->name);
	}
	
	if(activityTagsListWidget->count()>0)
		activityTagsListWidget->setCurrentRow(0);
}


ActivityTagsForm::~ActivityTagsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void ActivityTagsForm::addActivityTag()
{
	bool ok = false;
	ActivityTag* at=new ActivityTag();
	at->name = QInputDialog::getText( this, tr("Add activity tag"), tr("Please enter activity tag's name") ,
	 QLineEdit::Normal, QString(), &ok );

	if ( ok && !((at->name).isEmpty()) ){
		// user entered something and pressed OK
		if(!gt.rules.addActivityTag(at)){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not insert item. Must be a duplicate"));
			delete at;
		}
		else{
			activityTagsListWidget->addItem(at->name);
			activityTagsListWidget->setCurrentRow(activityTagsListWidget->count()-1);
			
			gt.rules.addUndoPoint(tr("Added the activity tag %1.").arg(at->name));
		}
	}
	else{
		if(ok){ //the user entered nothing
			QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		}
		delete at;// user entered nothing or pressed Cancel
	}
}

void ActivityTagsForm::removeActivityTag()
{
	int i=activityTagsListWidget->currentRow();
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int activity_tag_ID=gt.rules.searchActivityTag(text);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete this activity tag?"),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/
	if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete this activity tag?"),
	 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
		return;

	int tmp=gt.rules.removeActivityTag(text);
	if(tmp){
		activityTagsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=activityTagsListWidget->takeItem(i);
		delete item;
		
		if(i>=activityTagsListWidget->count())
			i=activityTagsListWidget->count()-1;
		if(i>=0)
			activityTagsListWidget->setCurrentRow(i);
		else
			currentActivityTagTextEdit->setPlainText(QString(""));

		gt.rules.addUndoPoint(tr("Removed the activity tag %1.").arg(text));
	}
}

void ActivityTagsForm::renameActivityTag()
{
	int i=activityTagsListWidget->currentRow();
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	QString initialActivityTagName=activityTagsListWidget->currentItem()->text();

	int activity_tag_ID=gt.rules.searchActivityTag(initialActivityTagName);
	if(activity_tag_ID<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	bool ok = false;
	QString finalActivityTagName;
	finalActivityTagName = QInputDialog::getText( this, tr("Rename activity tag"), tr("Please enter new activity tag's name") ,
	 QLineEdit::Normal, initialActivityTagName, &ok );

	if ( ok && !(finalActivityTagName.isEmpty()) ){
		// user entered something and pressed OK
		if(gt.rules.searchActivityTag(finalActivityTagName)>=0){
			QMessageBox::information( this, tr("Activity tag insertion dialog"),
				tr("Could not modify item. New name must be a duplicate"));
		}
		else{
			gt.rules.modifyActivityTag(initialActivityTagName, finalActivityTagName);
			activityTagsListWidget->item(i)->setText(finalActivityTagName);
			activityTagChanged(activityTagsListWidget->currentRow());
			
			gt.rules.addUndoPoint(tr("Renamed the activity tag from %1 to %2.").arg(initialActivityTagName).arg(finalActivityTagName));
		}
	}
}

void ActivityTagsForm::moveActivityTagUp()
{
	if(activityTagsListWidget->count()<=1)
		return;
	int i=activityTagsListWidget->currentRow();
	if(i<0 || i>=activityTagsListWidget->count())
		return;
	if(i==0)
		return;
	
	QString s1=activityTagsListWidget->item(i)->text();
	QString s2=activityTagsListWidget->item(i-1)->text();
	
	ActivityTag* at1=gt.rules.activityTagsList.at(i);
	ActivityTag* at2=gt.rules.activityTagsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;
	
	activityTagsListWidget->item(i)->setText(s2);
	activityTagsListWidget->item(i-1)->setText(s1);
	
	gt.rules.activityTagsList[i]=at2;
	gt.rules.activityTagsList[i-1]=at1;

	gt.rules.addUndoPoint(tr("Moved the activity tag %1 up.").arg(s1));
	
	activityTagsListWidget->setCurrentRow(i-1);
	activityTagChanged(i-1);
}

void ActivityTagsForm::moveActivityTagDown()
{
	if(activityTagsListWidget->count()<=1)
		return;
	int i=activityTagsListWidget->currentRow();
	if(i<0 || i>=activityTagsListWidget->count())
		return;
	if(i==activityTagsListWidget->count()-1)
		return;
		
	QString s1=activityTagsListWidget->item(i)->text();
	QString s2=activityTagsListWidget->item(i+1)->text();
	
	ActivityTag* at1=gt.rules.activityTagsList.at(i);
	ActivityTag* at2=gt.rules.activityTagsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	activityTagsListWidget->item(i)->setText(s2);
	activityTagsListWidget->item(i+1)->setText(s1);
	
	gt.rules.activityTagsList[i]=at2;
	gt.rules.activityTagsList[i+1]=at1;
	
	gt.rules.addUndoPoint(tr("Moved the activity tag %1 down.").arg(s1));

	activityTagsListWidget->setCurrentRow(i+1);
	activityTagChanged(i+1);
}

void ActivityTagsForm::sortActivityTags()
{
	gt.rules.sortActivityTagsAlphabetically();

	gt.rules.addUndoPoint(tr("Sorted the activity tags."));

	activityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		activityTagsListWidget->addItem(at->name);
	}

	if(activityTagsListWidget->count()>0)
		activityTagsListWidget->setCurrentRow(0);
}

void ActivityTagsForm::activityTagChanged(int index)
{
	if(index<0){
		currentActivityTagTextEdit->setPlainText(QString(""));
		return;
	}
	
	ActivityTag* st=gt.rules.activityTagsList.at(index);
	assert(st!=nullptr);
	QString s=st->getDetailedDescriptionWithConstraints(gt.rules);
	currentActivityTagTextEdit->setPlainText(s);
}

void ActivityTagsForm::activateActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int count=gt.rules.activateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));
	
	if(count>0)
		gt.rules.addUndoPoint(tr("Activated the activity tag %1 (%2 activities).", "%2 is the number of activated activities").arg(text).arg(count));
}

void ActivityTagsForm::deactivateActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	int count=gt.rules.deactivateActivityTag(text);
	QMessageBox::information(this, tr("FET information"), tr("Deactivated a number of %1 activities").arg(count));
	
	if(count>0)
		gt.rules.addUndoPoint(tr("Deactivated the activity tag %1 (%2 activities).", "%2 is the number of deactivated activities").arg(text).arg(count));
}

void ActivityTagsForm::printableActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	
	bool t=gt.rules.makeActivityTagPrintable(text);

	if(t){
		activityTagChanged(activityTagsListWidget->currentRow());
		gt.rules.addUndoPoint(tr("The activity tag %1 was made printable.").arg(text));
	}
}

void ActivityTagsForm::notPrintableActivityTag()
{
	if(activityTagsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}

	QString text=activityTagsListWidget->currentItem()->text();
	
	bool t=gt.rules.makeActivityTagNotPrintable(text);
	
	if(t){
		activityTagChanged(activityTagsListWidget->currentRow());
		gt.rules.addUndoPoint(tr("The activity tag %1 was made not printable.").arg(text));
	}
}

void ActivityTagsForm::help()
{
	QMessageBox::information(this, tr("FET help on activity tags"),
	 tr("Activity tag is a field which can be used or not, depending on your wish (optional field)."
	 " It is designed to help you with some constraints. Each activity has a list of activity tags"
	 " (which may be empty)."));
}

void ActivityTagsForm::comments()
{
	int ind=activityTagsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	ActivityTag* at=gt.rules.activityTagsList[ind];
	assert(at!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Activity tag comments"));
	
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
	commentsPT->setPlainText(at->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("ActivityTagCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=at->comments;
	
		at->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed the comments for the activity tag %1 from\n%2\nto\n%3.").arg(at->name).arg(oc).arg(at->comments));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		activityTagChanged(ind);
	}
}

void ActivityTagsForm::longName()
{
	int ind=activityTagsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	ActivityTag* at=gt.rules.activityTagsList[ind];
	assert(at!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Activity tag long name"));
	
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
	longNameLE->setText(at->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("ActivityTagLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=at->longName;
	
		at->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the activity tag %1 from\n%2\nto\n%3.").arg(at->name).arg(oln).arg(at->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		activityTagChanged(ind);
	}
}

void ActivityTagsForm::code()
{
	int ind=activityTagsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity tag"));
		return;
	}
	
	ActivityTag* at=gt.rules.activityTagsList[ind];
	assert(at!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Activity tag code"));
	
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
	codeLE->setText(at->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("ActivityTagCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=at->code;
	
		at->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the activity tag %1 from\n%2\nto\n%3.").arg(at->name).arg(oc).arg(at->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		activityTagChanged(ind);
	}
}
