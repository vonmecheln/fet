/***************************************************************************
                          settingsrestoredatafromdiskform.cpp  -  description
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

#include "settingsrestoredatafromdiskform.h"

SettingsRestoreDataFromDiskForm::SettingsRestoreDataFromDiskForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsRestoreDataFromDiskForm::ok);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsRestoreDataFromDiskForm::cancel);
	
	maxStatesLabel->setText(tr("Number of previous states to save/restore to/from disk:"));
	maxStatesLabel2->setText(tr("(Default is %1. Maximum allowed is %2, to avoid a slowdown when saving/opening a file, and using too much disk space.)",
	 "%1 is the default number of states to record on the disk, and %2 is the maximum number of states to record on the disk").arg(20).arg(100));
	
	maxStatesSpinBox->setMinimum(1);
	maxStatesSpinBox->setMaximum(100);
	maxStatesSpinBox->setValue(UNDO_REDO_STEPS_SAVE);
	
	fileNameSuffixLineEdit->setText(SUFFIX_FILENAME_SAVE_HISTORY);
	fileNameSuffixLineEdit->setReadOnly(true);

	enableHistoryOnDiskCheckBox->setChecked(USE_UNDO_REDO_SAVE);

	connect(enableHistoryOnDiskCheckBox, &QCheckBox::toggled, this, &SettingsRestoreDataFromDiskForm::enableHistoryOnDiskCheckBox_toggled);

	enableHistoryOnDiskCheckBox_toggled();
}

SettingsRestoreDataFromDiskForm::~SettingsRestoreDataFromDiskForm()
{
	saveFETDialogGeometry(this);
}

void SettingsRestoreDataFromDiskForm::enableHistoryOnDiskCheckBox_toggled()
{
	bool t=enableHistoryOnDiskCheckBox->isChecked();

	maxStatesLabel->setEnabled(t);
	maxStatesLabel2->setEnabled(t);
	maxStatesSpinBox->setEnabled(t);
	fileNameSuffixLineEdit->setEnabled(t);

	fileNameLabel->setEnabled(t);
	fileNameLabel2->setEnabled(t);
}

void SettingsRestoreDataFromDiskForm::ok()
{
	USE_UNDO_REDO_SAVE=enableHistoryOnDiskCheckBox->isChecked();
	UNDO_REDO_STEPS_SAVE=maxStatesSpinBox->value();
	SUFFIX_FILENAME_SAVE_HISTORY=fileNameSuffixLineEdit->text();
	
	this->close();
}

void SettingsRestoreDataFromDiskForm::cancel()
{
	this->close();
}
