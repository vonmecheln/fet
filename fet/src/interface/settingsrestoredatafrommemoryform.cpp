/***************************************************************************
                          settingsrestoredatafrommemoryform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

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

#include "settingsrestoredatafrommemoryform.h"

/*#include <QDate>
#include <QTime>
#include <QPair>*/

extern int cntUndoRedoStackIterator;
/*
extern std::list<QByteArray> oldRulesArchived; //.front() is oldest, .back() is newest
extern std::list<QString> operationWhichWasDone; //as above
extern std::list<QPair<QDate, QTime>> operationDateTime; //as above
extern std::list<int> unarchivedSizes; //as above
*/
//extern std::list<QString> stateFileName; //as above

extern int savedStateIterator;

void clearHistory();

SettingsRestoreDataFromMemoryForm::SettingsRestoreDataFromMemoryForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsRestoreDataFromMemoryForm::ok);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsRestoreDataFromMemoryForm::cancel);
	
	maxStatesLabel->setText(tr("Number of previous states to record in the memory:"));
	maxStatesLabel2->setText(tr("(Default is %1. Maximum allowed is %2, to avoid using too much memory.)",
	 "%1 is the default number of states to record in the memory, and %2 is the maximum number of states to record in the memory").arg(100).arg(1000));
	
	maxStatesSpinBox->setMinimum(1);
	maxStatesSpinBox->setMaximum(1000);
	maxStatesSpinBox->setValue(UNDO_REDO_STEPS);
	
	/*compressionLevelSpinBox->setMinimum(-1);
	compressionLevelSpinBox->setMaximum(9);
	compressionLevelSpinBox->setValue(UNDO_REDO_COMPRESSION_LEVEL);*/

	enableHistoryInMemoryCheckBox->setChecked(USE_UNDO_REDO);

	connect(enableHistoryInMemoryCheckBox, &QCheckBox::toggled, this, &SettingsRestoreDataFromMemoryForm::enableHistoryInMemoryCheckBox_toggled);

	enableHistoryInMemoryCheckBox_toggled();
}

SettingsRestoreDataFromMemoryForm::~SettingsRestoreDataFromMemoryForm()
{
	saveFETDialogGeometry(this);
}

void SettingsRestoreDataFromMemoryForm::enableHistoryInMemoryCheckBox_toggled()
{
	bool t=enableHistoryInMemoryCheckBox->isChecked();

	maxStatesLabel->setEnabled(t);
	maxStatesLabel2->setEnabled(t);
	maxStatesSpinBox->setEnabled(t);
	//compressionLevelLabel->setEnabled(t);
	//compressionLevelSpinBox->setEnabled(t);
}

void SettingsRestoreDataFromMemoryForm::ok()
{
	if(USE_UNDO_REDO==enableHistoryInMemoryCheckBox->isChecked() && UNDO_REDO_STEPS==maxStatesSpinBox->value()){
		//do nothing
	}
	else{
		USE_UNDO_REDO=enableHistoryInMemoryCheckBox->isChecked();

		UNDO_REDO_STEPS=maxStatesSpinBox->value();

		//UNDO_REDO_COMPRESSION_LEVEL=compressionLevelSpinBox->value();

		clearHistory();
		if(gt.rules.initialized && USE_UNDO_REDO){
			gt.rules.addUndoPoint(tr("Cleared the memory history, because the memory history settings were modified.",
			 "It refers to the history to be kept in the computer memory"), false, false);
			if(!gt.rules.modified)
				savedStateIterator=cntUndoRedoStackIterator;
		}
	}

	this->close();
}

void SettingsRestoreDataFromMemoryForm::cancel()
{
	this->close();
}
