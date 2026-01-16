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

#ifndef FET_COMMAND_LINE
#include <QMessageBox>
#else
#include <iostream>
using namespace std;

#include <QDir>
#include <QFile>
#include <QTextStream>
#endif

#include "longtextmessagebox.h"

#ifdef FET_COMMAND_LINE
extern QString logsDir; //computed in fet.cpp

void commandLineMessage(QWidget* parent, const QString& title, const QString& message,
 bool isWarning, bool isError)
{
	Q_UNUSED(parent);
	
	assert((isWarning && !isError) || (!isWarning && isError));
	
	QString filename;
	if(isWarning)
		filename=logsDir+"warnings.txt";
	else
		filename=logsDir+"errors.txt";
	
	bool begin;
	if(QFile::exists(filename))
		begin=false;
	else
		begin=true;

	QFile file(filename);
	
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::Append)){
#else
	if(!file.open(QIODevice::Append)){
#endif
		cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(filename))<<")."
			 " If this is a bug - please report it."<<endl;
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	if(begin){
		tos.setGenerateByteOrderMark(true);
	}
	
	tos<<FetCommandLine::tr("Title: %1").arg(title)<<Qt::endl;
	tos<<FetCommandLine::tr("Message: %1").arg(message)<<Qt::endl;
	tos<<Qt::endl;
	
	if(file.error()!=QFileDevice::NoError){
		cout<<"FET critical - writing in the file "<<qPrintable(QDir::toNativeSeparators(filename))<<" gave the error message "
			 <<qPrintable(file.errorString())<<" which means the writing is compromised. Please check your disk's free space."<<endl;
	}
	file.close();
}

int commandLineMessage(QWidget* parent, const QString& title, const QString& message,
 const QString& button0Text, const QString& button1Text, const QString& button2Text, int defaultButton, int escapeButton,
 bool isWarning, bool isError)
{
	Q_UNUSED(parent);

	assert((isWarning && !isError) || (!isWarning && isError));
	
	QString filename;
	if(isWarning)
		filename=logsDir+"warnings.txt";
	else
		filename=logsDir+"errors.txt";
	
	bool begin;
	if(QFile::exists(filename))
		begin=false;
	else
		begin=true;

	QFile file(filename);
	
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::Append)){
#else
	if(!file.open(QIODevice::Append)){
#endif
		cout<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(QDir::toNativeSeparators(filename))<<")."
			 " If this is a bug - please report it."<<endl;
		return defaultButton;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	if(begin){
		tos.setGenerateByteOrderMark(true);
	}
	
	tos<<FetCommandLine::tr("Title: %1").arg(title)<<Qt::endl;
	tos<<FetCommandLine::tr("Message: %1").arg(message)<<Qt::endl;

	if(button0Text!=QString())
		tos<<FetCommandLine::tr("Button 0 text: %1").arg(button0Text)<<Qt::endl;
	if(button1Text!=QString())
		tos<<FetCommandLine::tr("Button 1 text: %1").arg(button1Text)<<Qt::endl;
	if(button2Text!=QString())
		tos<<FetCommandLine::tr("Button 2 text: %1").arg(button2Text)<<Qt::endl;

	tos<<FetCommandLine::tr("Default button: %1").arg(defaultButton)<<Qt::endl;
	tos<<FetCommandLine::tr("Escape button: %1").arg(escapeButton)<<Qt::endl;

	tos<<FetCommandLine::tr("Pressing default button %1").arg(defaultButton)<<Qt::endl;
	
	tos<<Qt::endl;
	
	if(file.error()!=QFileDevice::NoError){
		cout<<"FET critical - writing in the file "<<qPrintable(QDir::toNativeSeparators(filename))<<" gave the error message "
			 <<qPrintable(file.errorString())<<" which means the writing is compromised. Please check your disk's free space."<<endl;
	}
	file.close();
	
	return defaultButton;
}
#endif

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
#ifndef FET_COMMAND_LINE
	QMessageBox::critical(parent, title, message);
#else
	commandLineMessage(parent, title, message, false, true);
#endif
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

//QProgressDialog

#ifdef FET_COMMAND_LINE

QProgressDialog::QProgressDialog(QWidget* parent)
{
	Q_UNUSED(parent);
}

void QProgressDialog::setWindowTitle(const QString& title)
{
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress title: %1").arg(title))<<endl;
	}
}

void QProgressDialog::setLabelText(const QString& label)
{
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress label: %1").arg(label))<<endl;
	}
}

void QProgressDialog::setRange(int a, int b)
{
	if(VERBOSE){
		cout<<qPrintable(FetCommandLine::tr("Progress range: %1..%2").arg(a).arg(b))<<endl;
	}
}

void QProgressDialog::setModal(bool m)
{
	if(VERBOSE){
		if(m)
			cout<<qPrintable(FetCommandLine::tr("Progress setModal(true)"))<<endl;
		else
			cout<<qPrintable(FetCommandLine::tr("Progress setModal(false)"))<<endl;
	}
}

void QProgressDialog::setValue(int v)
{
	Q_UNUSED(v);
	//cout<<qPrintable(FetCommandLine::tr("Progress value: %1").arg(v))<<endl;
}

bool QProgressDialog::wasCanceled()
{
	return false;
}

#endif
