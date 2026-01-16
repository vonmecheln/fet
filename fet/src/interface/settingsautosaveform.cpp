/***************************************************************************
                          settingsautosaveform.cpp  -  description
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

#include "settingsautosaveform.h"

#include <QFileDialog>
#include <QDir>

#include <QMessageBox>

SettingsAutosaveForm::SettingsAutosaveForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//connect(buttonBox, SIG NAL(accepted()), this, SL OT(ok()));
	//connect(buttonBox, SIG NAL(rejected()), this, SL OT(cancel()));
	connect(okPushButton, &QPushButton::clicked, this, &SettingsAutosaveForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &SettingsAutosaveForm::cancel);
	
	connect(chooseDirectoryPushButton, &QPushButton::clicked, this, &SettingsAutosaveForm::chooseDirectory);
	
	///////////////
	minutesSpinBox->setRange(1, 15);
	minutesSpinBox->setValue(MINUTES_AUTOSAVE);
	QString s=tr("After %1 minutes");
	QStringList sl=s.split("%1");
	QString prefix=sl.at(0);
	QString suffix;
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	minutesSpinBox->setPrefix(prefix);
	minutesSpinBox->setSuffix(suffix);
	
	minutesSpinBox->setSizePolicy(QSizePolicy::Expanding, minutesSpinBox->sizePolicy().verticalPolicy());
	////////////////
	
	///////////////
	operationsSpinBox->setRange(1, 100);
	operationsSpinBox->setValue(OPERATIONS_AUTOSAVE);
	s=tr("After %1 operations");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
	operationsSpinBox->setPrefix(prefix);
	operationsSpinBox->setSuffix(suffix);
	
	operationsSpinBox->setSizePolicy(QSizePolicy::Expanding, minutesSpinBox->sizePolicy().verticalPolicy());
	////////////////
	
	directoryLineEdit->setText(QDir::toNativeSeparators(DIRECTORY_AUTOSAVE));
	fileNameSuffixLineEdit->setText(SUFFIX_FILENAME_AUTOSAVE);
	fileNameSuffixLineEdit->setReadOnly(true);

	enableAutosaveCheckBox->setChecked(USE_AUTOSAVE);

	connect(enableAutosaveCheckBox, &QCheckBox::toggled, this, &SettingsAutosaveForm::enableAutosaveCheckBox_toggled);

	enableAutosaveCheckBox_toggled();
}

SettingsAutosaveForm::~SettingsAutosaveForm()
{
	saveFETDialogGeometry(this);
}

void SettingsAutosaveForm::enableAutosaveCheckBox_toggled()
{
	bool t=enableAutosaveCheckBox->isChecked();

	whenToDoAutosaveGroupBox->setEnabled(t);
	whereToDoAutosaveGroupBox->setEnabled(t);
	label->setEnabled(t);
	label2->setEnabled(t);
	label3->setEnabled(t);
	label4->setEnabled(t);
}

void SettingsAutosaveForm::chooseDirectory()
{
	DIRECTORY_AUTOSAVE=QFileDialog::getExistingDirectory(this, tr("Choose the directory for autosave"), QDir::fromNativeSeparators(DIRECTORY_AUTOSAVE),
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	directoryLineEdit->setText(QDir::toNativeSeparators(DIRECTORY_AUTOSAVE));
}

void SettingsAutosaveForm::ok()
{
	if(!QDir(QDir::fromNativeSeparators(directoryLineEdit->text())).exists()){
		QMessageBox::information(this, tr("FET information"), tr("The chosen directory does not exist."
		 " Please choose an existing directory or let its name be empty (in that case, the autosave will"
		 " happen in the working directory)."));
		return;
	}

	USE_AUTOSAVE=enableAutosaveCheckBox->isChecked();
	MINUTES_AUTOSAVE=minutesSpinBox->value();
	OPERATIONS_AUTOSAVE=operationsSpinBox->value();
	if(directoryLineEdit->text().isEmpty())
		DIRECTORY_AUTOSAVE=QString("");
	else
		DIRECTORY_AUTOSAVE=QDir(QDir::fromNativeSeparators(directoryLineEdit->text())).canonicalPath();
	SUFFIX_FILENAME_AUTOSAVE=fileNameSuffixLineEdit->text();

	this->close();
}

void SettingsAutosaveForm::cancel()
{
	this->close();
}
