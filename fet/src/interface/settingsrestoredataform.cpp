/***************************************************************************
                          settingsrestoredataform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
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

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "settingsrestoredataform.h"

#include <QDate>
#include <QTime>
#include <QPair>

extern int cntUndoRedoStackIterator;
extern std::list<QByteArray> oldRulesArchived; //.front() is oldest, .back() is newest
extern std::list<QString> operationWhichWasDone; //as above
extern std::list<QPair<QDate, QTime>> operationDateTime; //as above
extern std::list<int> unarchivedSizes; //as above
//extern std::list<QString> stateFileName; //as above

extern int savedStateIterator;

SettingsRestoreDataForm::SettingsRestoreDataForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
	
	enableHistoryCheckBox->setChecked(USE_UNDO_REDO);
	
	maxStatesLabel->setText(tr("Number of previous states to record:"));
	maxStatesLabel2->setText(tr("(maximum allowed is %1, to avoid using too much memory.)", "%1 is the maximum number of states to record").arg(1000));
	
	maxStatesSpinBox->setMinimum(1);
	maxStatesSpinBox->setMaximum(1000);
	maxStatesSpinBox->setValue(UNDO_REDO_STEPS);
	
	/*compressionLevelSpinBox->setMinimum(-1);
	compressionLevelSpinBox->setMaximum(9);
	compressionLevelSpinBox->setValue(UNDO_REDO_COMPRESSION_LEVEL);*/
}

SettingsRestoreDataForm::~SettingsRestoreDataForm()
{
	saveFETDialogGeometry(this);
}

void SettingsRestoreDataForm::on_enableHistoryCheckBox_toggled()
{
	bool t=enableHistoryCheckBox->isChecked();

	maxStatesLabel->setEnabled(t);
	maxStatesLabel2->setEnabled(t);
	maxStatesSpinBox->setEnabled(t);
	//compressionLevelLabel->setEnabled(t);
	//compressionLevelSpinBox->setEnabled(t);
}

void SettingsRestoreDataForm::ok()
{
	if(USE_UNDO_REDO==enableHistoryCheckBox->isChecked() && UNDO_REDO_STEPS==maxStatesSpinBox->value()){
		//do nothing
	}
	else{
		USE_UNDO_REDO=enableHistoryCheckBox->isChecked();

		UNDO_REDO_STEPS=maxStatesSpinBox->value();

		//UNDO_REDO_COMPRESSION_LEVEL=compressionLevelSpinBox->value();

		clearHistory();
		if(gt.rules.initialized && USE_UNDO_REDO){
			gt.rules.addUndoPoint(tr("Cleared the history, because the history settings were modified."));
			if(!gt.rules.modified)
				savedStateIterator=cntUndoRedoStackIterator;
		}
	}

	this->close();
}

void SettingsRestoreDataForm::cancel()
{
	this->close();
}
