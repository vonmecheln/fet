//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
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

#include "notificationcommandform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QString>
#include <QStringList>

#include <QDir>

#include <QFileDialog>
#include <QSettings>

static QString LAST_USED_DIRECTORY;
static QString LAST_USED_DIRECTORY_EACH_TIMETABLE;

extern const QString COMPANY;
extern const QString PROGRAM;

NotificationCommandForm::NotificationCommandForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(cancelPushButton, &QPushButton::clicked, this, &NotificationCommandForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &NotificationCommandForm::ok);

	connect(helpPushButton, &QPushButton::clicked, this, &NotificationCommandForm::help);
	
	externalCommandCheckBox->setChecked(ENABLE_COMMAND_AT_END_OF_GENERATION);
	externalCommandEachTimetableCheckBox->setChecked(ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE);
	//detachedCheckBox->setChecked(DETACHED_NOTIFICATION);

	connect(externalCommandCheckBox, &QCheckBox::toggled, this, &NotificationCommandForm::externalCommandCheckBoxToggled);
	connect(externalCommandEachTimetableCheckBox, &QCheckBox::toggled, this, &NotificationCommandForm::externalCommandEachTimetableCheckBoxToggled);
	//connect(detachedCheckBox, SIG NAL(toggled(bool)), this, SL OT(detachedCheckBoxToggled()));

	connect(browsePushButton, &QPushButton::clicked, this, &NotificationCommandForm::browse);
	connect(browseEachTimetablePushButton, &QPushButton::clicked, this, &NotificationCommandForm::browseEachTimetable);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	beepCheckBox->setChecked(BEEP_AT_END_OF_GENERATION);
	externalCommandLineEdit->setText(QDir::toNativeSeparators(commandAtEndOfGeneration));

	beepEachTimetableCheckBox->setChecked(BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE);
	externalCommandEachTimetableLineEdit->setText(QDir::toNativeSeparators(commandAtEndOfGenerationEachTimetable));
	
	/*terminateAfterSecondsSpinBox->setMinimum(0);
	terminateAfterSecondsSpinBox->setMaximum(600);
	terminateAfterSecondsSpinBox->setValue(terminateCommandAfterSeconds);
	terminateAfterSecondsSpinBox->setSpecialValueText(tr("Terminate after: never"));
	
	QString s=tr("Terminate after: %1 seconds");
	QStringList sl=s.split("%1");
	QString prefix=sl.at(0);
	QString suffix;
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
		
	terminateAfterSecondsSpinBox->setPrefix(prefix);
	terminateAfterSecondsSpinBox->setSuffix(suffix);

	killAfterSecondsSpinBox->setMinimum(0);
	killAfterSecondsSpinBox->setMaximum(600);
	killAfterSecondsSpinBox->setValue(killCommandAfterSeconds);
	killAfterSecondsSpinBox->setSpecialValueText(tr("Kill after: never"));

	s=tr("Kill after: %1 seconds");
	sl=s.split("%1");
	prefix=sl.at(0);
	if(sl.count()<2)
		suffix=QString("");
	else
		suffix=sl.at(1);
		
	killAfterSecondsSpinBox->setPrefix(prefix);
	killAfterSecondsSpinBox->setSuffix(suffix);*/

	externalCommandCheckBoxToggled();
	externalCommandEachTimetableCheckBoxToggled();
	//detachedCheckBoxToggled();
	
	QSettings settings(COMPANY, PROGRAM);
	LAST_USED_DIRECTORY=settings.value(this->metaObject()->className()+QString("/last-used-directory"), QString("")).toString();
	LAST_USED_DIRECTORY_EACH_TIMETABLE=settings.value(this->metaObject()->className()+QString("/last-used-directory-each-timetable"), QString("")).toString();
}

NotificationCommandForm::~NotificationCommandForm()
{
	saveFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/last-used-directory"), LAST_USED_DIRECTORY);
	settings.setValue(this->metaObject()->className()+QString("/last-used-directory-each-timetable"), LAST_USED_DIRECTORY_EACH_TIMETABLE);
}

void NotificationCommandForm::browse()
{
	QString s=QFileDialog::getOpenFileName(this, tr("Choose an external command", "Title of a dialog to choose an external command"), LAST_USED_DIRECTORY);
	if(!s.isNull()){
		externalCommandLineEdit->setText(QDir::toNativeSeparators(s));
		int tmp=s.lastIndexOf(FILE_SEP);
		LAST_USED_DIRECTORY=s.left(tmp);
	}
}

void NotificationCommandForm::browseEachTimetable()
{
	QString s=QFileDialog::getOpenFileName(this, tr("Choose an external command", "Title of a dialog to choose an external command"), LAST_USED_DIRECTORY_EACH_TIMETABLE);
	if(!s.isNull()){
		externalCommandEachTimetableLineEdit->setText(QDir::toNativeSeparators(s));
		int tmp=s.lastIndexOf(FILE_SEP);
		LAST_USED_DIRECTORY_EACH_TIMETABLE=s.left(tmp);
	}
}

void NotificationCommandForm::externalCommandCheckBoxToggled()
{
	if(externalCommandCheckBox->isChecked()){
		//detachedCheckBox->setEnabled(true);
		externalCommandLineEdit->setEnabled(true);
		browsePushButton->setEnabled(true);
		//if(!detachedCheckBox->isChecked()){
			//terminateAfterSecondsSpinBox->setEnabled(true);
			//killAfterSecondsSpinBox->setEnabled(true);
		//}
		//else{
		//	terminateAfterSecondsSpinBox->setEnabled(false);
		//	killAfterSecondsSpinBox->setEnabled(false);
		//}
	}
	else{
		//detachedCheckBox->setEnabled(false);
		externalCommandLineEdit->setEnabled(false);
		browsePushButton->setEnabled(false);
		//terminateAfterSecondsSpinBox->setEnabled(false);
		//killAfterSecondsSpinBox->setEnabled(false);
	}
}

void NotificationCommandForm::externalCommandEachTimetableCheckBoxToggled()
{
	if(externalCommandEachTimetableCheckBox->isChecked()){
		//detachedCheckBox->setEnabled(true);
		externalCommandEachTimetableLineEdit->setEnabled(true);
		browseEachTimetablePushButton->setEnabled(true);
		//if(!detachedCheckBox->isChecked()){
			//terminateAfterSecondsSpinBox->setEnabled(true);
			//killAfterSecondsSpinBox->setEnabled(true);
		//}
		//else{
		//	terminateAfterSecondsSpinBox->setEnabled(false);
		//	killAfterSecondsSpinBox->setEnabled(false);
		//}
	}
	else{
		//detachedCheckBox->setEnabled(false);
		externalCommandEachTimetableLineEdit->setEnabled(false);
		browseEachTimetablePushButton->setEnabled(false);
		//terminateAfterSecondsSpinBox->setEnabled(false);
		//killAfterSecondsSpinBox->setEnabled(false);
	}
}

/*void NotificationCommandForm::detachedCheckBoxToggled()
{
	//externalCommandCheckBoxToggled();
	if(externalCommandCheckBox->isChecked() && !detachedCheckBox->isChecked()){
		terminateAfterSecondsSpinBox->setEnabled(true);
		killAfterSecondsSpinBox->setEnabled(true);
	}
	else{
		terminateAfterSecondsSpinBox->setEnabled(false);
		killAfterSecondsSpinBox->setEnabled(false);
	}
}*/

void NotificationCommandForm::ok()
{
	BEEP_AT_END_OF_GENERATION=beepCheckBox->isChecked();
	ENABLE_COMMAND_AT_END_OF_GENERATION=externalCommandCheckBox->isChecked();
	//DETACHED_NOTIFICATION=detachedCheckBox->isChecked();
	commandAtEndOfGeneration=QDir::fromNativeSeparators(externalCommandLineEdit->text());
	//terminateCommandAfterSeconds=terminateAfterSecondsSpinBox->value();
	//killCommandAfterSeconds=killAfterSecondsSpinBox->value();

	BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE=beepEachTimetableCheckBox->isChecked();
	ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE=externalCommandEachTimetableCheckBox->isChecked();
	commandAtEndOfGenerationEachTimetable=QDir::fromNativeSeparators(externalCommandEachTimetableLineEdit->text());

	this->close();
}

void NotificationCommandForm::cancel()
{
	this->close();
}

void NotificationCommandForm::help()
{
	QString s;
	
	s+=tr("You can choose to make FET execute an external command at the end of each single or multiple generation, in addition to or as an alternative"
	 " to the system beep.");
	s+=" ";
	s+=tr("Also, you can choose to make FET issue a system beep and/or execute an external command after each successful timetable in a multiple generation.");
	//s+="\n\n";
	//s+=tr("This external command can be terminated and/or killed after a respective specified time (expressed in seconds).");
	/*s+=tr("1) If the notification command is not detached, it can be terminated and/or killed after a respective specified time (expressed in seconds)."
	 " Also, it will be killed if you quit FET.");
	s+="\n\n";
	s+=tr("2) If the notification command is detached, it won't be killed if you quit FET.");*/

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
