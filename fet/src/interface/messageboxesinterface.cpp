//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2013 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "messageboxes.h"

#include <QMessageBox>

#include <QProgressDialog>

#include "longtextmessagebox.h"

//Rules

int RulesConstraintIgnored::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

void RulesImpossible::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

void RulesReconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

int RulesReconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

int RulesReconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

void RulesReconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

int RulesReconcilableMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

void RulesIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

/*int RulesIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton);
}*/

void RulesUsualInformation::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

void RulesReadingWrongConstraint::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

void IrreconcilableCriticalMessage::critical(QWidget* parent, const QString& title, const QString& message)
{
	QMessageBox::critical(parent, title, message);
}

//GeneratePre

int GeneratePreReconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

int GeneratePreReconcilableMessage::largeConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::largeConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

void GeneratePreReconcilableMessage::largeInformation(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::largeInformation(parent, title, message, true, false);
}

void GeneratePreIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

int GeneratePreIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, false, true);
}

void GeneratePreIrreconcilableMessage::mediumInformation(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

int GeneratePreIrreconcilableMessage::mediumConfirmation(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, false, true);
}

//TimetableExport

int TimetableExportMessage::information(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

int TimetableExportMessage::warning(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton)
{
	return LongTextMessageBox::mediumConfirmation(parent, title, message, button0Text, button1Text, button2Text,
	 defaultButton, escapeButton, true, false);
}

void TimetableExportMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

void TimetableExportMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

//TimeConstraint

void TimeConstraintIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

void TimeConstraintIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

//SpaceConstraint

void SpaceConstraintIrreconcilableMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

void SpaceConstraintIrreconcilableMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, false, true);
}

//Fet

void FetMessage::information(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}

void FetMessage::warning(QWidget* parent, const QString& title, const QString& message)
{
	LongTextMessageBox::mediumInformation(parent, title, message, true, false);
}
