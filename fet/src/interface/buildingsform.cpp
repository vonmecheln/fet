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
#include "fet.h"
#include "buildingsform.h"
#include "addbuildingform.h"
#include "modifybuildingform.h"

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

BuildingsForm::BuildingsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentBuildingTextEdit->setReadOnly(true);

	modifyBuildingPushButton->setDefault(true);

	buildingsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addBuildingPushButton, &QPushButton::clicked, this, &BuildingsForm::addBuilding);
	connect(removeBuildingPushButton, &QPushButton::clicked, this, &BuildingsForm::removeBuilding);
	
	connect(buildingsListWidget, &QListWidget::currentRowChanged, this, &BuildingsForm::buildingChanged);
	connect(closePushButton, &QPushButton::clicked, this, &BuildingsForm::close);
	connect(modifyBuildingPushButton, &QPushButton::clicked, this, &BuildingsForm::modifyBuilding);

	connect(moveBuildingUpPushButton, &QPushButton::clicked, this, &BuildingsForm::moveBuildingUp);
	connect(moveBuildingDownPushButton, &QPushButton::clicked, this, &BuildingsForm::moveBuildingDown);

	connect(sortBuildingsPushButton, &QPushButton::clicked, this, &BuildingsForm::sortBuildings);
	connect(buildingsListWidget, &QListWidget::itemDoubleClicked, this, &BuildingsForm::modifyBuilding);

	connect(longNamePushButton, &QPushButton::clicked, this, &BuildingsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &BuildingsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &BuildingsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addBuildingPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addBuildingPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyBuildingPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyBuildingPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeBuildingPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeBuildingPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveBuildingUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveBuildingUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveBuildingDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveBuildingDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}

BuildingsForm::~BuildingsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool BuildingsForm::filterOk(Building* bu)
{
	Q_UNUSED(bu);

	bool ok=true;

	return ok;
}

void BuildingsForm::filterChanged()
{
	QString s;
	buildingsListWidget->clear();
	visibleBuildingsList.clear();
	for(int i=0; i<gt.rules.buildingsList.size(); i++){
		Building* bu=gt.rules.buildingsList[i];
		if(this->filterOk(bu)){
			s=bu->name;
			visibleBuildingsList.append(bu);
			buildingsListWidget->addItem(s);
		}
	}
	
	if(buildingsListWidget->count()>0)
		buildingsListWidget->setCurrentRow(0);
	else
		buildingChanged(-1);
}

void BuildingsForm::addBuilding()
{
	AddBuildingForm addBuildingForm(this);
	setParentAndOtherThings(&addBuildingForm, this);
	addBuildingForm.exec();
	
	filterChanged();
	
	buildingsListWidget->setCurrentRow(buildingsListWidget->count()-1);
}

void BuildingsForm::removeBuilding()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ind);
	assert(bu!=nullptr);

	if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete this building?"),
	 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
		return;
	
	QString on=bu->name;
	
	bool tmp=gt.rules.removeBuilding(bu->name);
	assert(tmp);
	
	gt.rules.addUndoPoint(tr("Removed the building %1.").arg(on));

	visibleBuildingsList.removeAt(ind);
	buildingsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=buildingsListWidget->takeItem(ind);
	delete item;
	
	if(ind>=buildingsListWidget->count())
		ind=buildingsListWidget->count()-1;
	if(ind>=0)
		buildingsListWidget->setCurrentRow(ind);
	else
		currentBuildingTextEdit->setPlainText(QString(""));
}

void BuildingsForm::buildingChanged(int index)
{
	if(index<0){
		currentBuildingTextEdit->setPlainText("");
		return;
	}

	QString s;
	Building* building=visibleBuildingsList.at(index);

	assert(building!=nullptr);
	s=building->getDetailedDescriptionWithConstraints(gt.rules);
	currentBuildingTextEdit->setPlainText(s);
}

void BuildingsForm::moveBuildingUp()
{
	if(buildingsListWidget->count()<=1)
		return;
	int i=buildingsListWidget->currentRow();
	if(i<0 || i>=buildingsListWidget->count())
		return;
	if(i==0)
		return;
	
	QString s1=buildingsListWidget->item(i)->text();
	QString s2=buildingsListWidget->item(i-1)->text();
	
	Building* bu1=gt.rules.buildingsList.at(i);
	Building* bu2=gt.rules.buildingsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	buildingsListWidget->item(i)->setText(s2);
	buildingsListWidget->item(i-1)->setText(s1);
	
	gt.rules.buildingsList[i]=bu2;
	gt.rules.buildingsList[i-1]=bu1;
	
	gt.rules.addUndoPoint(tr("Moved the building %1 up.").arg(bu1->name));
	
	//Begin bug fix on 2017-08-29
	Building* vb1=visibleBuildingsList[i];
	Building* vb2=visibleBuildingsList[i-1];
	visibleBuildingsList[i]=vb2;
	visibleBuildingsList[i-1]=vb1;
	//End bug fix
	
	buildingsListWidget->setCurrentRow(i-1);
	buildingChanged(i-1);
}

void BuildingsForm::moveBuildingDown()
{
	if(buildingsListWidget->count()<=1)
		return;
	int i=buildingsListWidget->currentRow();
	if(i<0 || i>=buildingsListWidget->count())
		return;
	if(i==buildingsListWidget->count()-1)
		return;
		
	QString s1=buildingsListWidget->item(i)->text();
	QString s2=buildingsListWidget->item(i+1)->text();
	
	Building* bu1=gt.rules.buildingsList.at(i);
	Building* bu2=gt.rules.buildingsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	buildingsListWidget->item(i)->setText(s2);
	buildingsListWidget->item(i+1)->setText(s1);
	
	gt.rules.buildingsList[i]=bu2;
	gt.rules.buildingsList[i+1]=bu1;
	
	gt.rules.addUndoPoint(tr("Moved the building %1 down.").arg(bu1->name));
	
	//Begin bug fix on 2017-08-29
	Building* vb1=visibleBuildingsList[i];
	Building* vb2=visibleBuildingsList[i+1];
	visibleBuildingsList[i]=vb2;
	visibleBuildingsList[i+1]=vb1;
	//End bug fix
	
	buildingsListWidget->setCurrentRow(i+1);
	buildingChanged(i+1);
}

void BuildingsForm::sortBuildings()
{
	gt.rules.sortBuildingsAlphabetically();

	gt.rules.addUndoPoint(tr("Sorted the buildings."));

	filterChanged();
}

void BuildingsForm::modifyBuilding()
{
	int valv=buildingsListWidget->verticalScrollBar()->value();
	int valh=buildingsListWidget->horizontalScrollBar()->value();

	int ci=buildingsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ci);
	ModifyBuildingForm form(this, bu->name);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	buildingsListWidget->verticalScrollBar()->setValue(valv);
	buildingsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=buildingsListWidget->count())
		ci=buildingsListWidget->count()-1;

	if(ci>=0)
		buildingsListWidget->setCurrentRow(ci);
}

void BuildingsForm::comments()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=gt.rules.buildingsList[ind];
	assert(bu!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Building comments"));
	
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
	commentsPT->setPlainText(bu->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("BuildingsCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=bu->comments;
	
		bu->comments=commentsPT->toPlainText();
		
		gt.rules.addUndoPoint(tr("Changed the comments for the building %1 from\n%2\nto\n%3.").arg(bu->name).arg(oc).arg(bu->comments));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		buildingChanged(ind);
	}
}

void BuildingsForm::longName()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=gt.rules.buildingsList[ind];
	assert(bu!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Building long name"));
	
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
	longNameLE->setText(bu->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("BuildingLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=bu->longName;
	
		bu->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the building %1 from\n%2\nto\n%3.").arg(bu->name).arg(oln).arg(bu->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		buildingChanged(ind);
	}
}

void BuildingsForm::code()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=gt.rules.buildingsList[ind];
	assert(bu!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Building code"));
	
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
	codeLE->setText(bu->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("BuildingCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=bu->code;
	
		bu->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the building %1 from\n%2\nto\n%3.").arg(bu->name).arg(oc).arg(bu->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		buildingChanged(ind);
	}
}
