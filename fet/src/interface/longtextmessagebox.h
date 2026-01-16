/*
File longtextmessagebox.h
*/

/***************************************************************************
                          longtextmessagebox.h  -  description
                             -------------------
    begin                : 27 June 2009
    copyright            : (C) 2009 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef LONGTEXTMESSAGEBOX_H
#define LONGTEXTMESSAGEBOX_H

#include <QObject>

class QString;
class QWidget;

class LongTextMessageBox: public QObject
{
	Q_OBJECT
	
public:
	static int confirmation
	 (QWidget* parent, const QString& title, const QString& text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton,
	 bool isWarning=false, bool isError=false);

	static int largeConfirmation
	 (QWidget* parent, const QString& title, const QString& text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton,
	 bool isWarning=false, bool isError=false);

	static int mediumConfirmation
	 (QWidget* parent, const QString& title, const QString& text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton,
	 bool isWarning=false, bool isError=false);

	static void information
	 (QWidget* parent, const QString& title, const QString& text,
	 bool isWarning=false, bool isError=false);

	static void largeInformation
	 (QWidget* parent, const QString& title, const QString& text,
	 bool isWarning=false, bool isError=false);

	static void mediumInformation
	 (QWidget* parent, const QString& title, const QString& text,
	 bool isWarning=false, bool isError=false);

	static int confirmationWithDimensions
	 (QWidget* parent, const QString& title, const QString& text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton, int MINW, int MAXW, int MINH, int MAXH,
	 bool isWarning=false, bool isError=false);

	static void informationWithDimensions
	 (QWidget* parent, const QString& title, const QString& text, int MINW, int MAXW, int MINH, int MAXH,
	 bool isWarning=false, bool isError=false);

	//Used only in modifystudentsyearform.cpp
	static int largeConfirmationWithDimensionsThreeButtonsYesNoCancel
	 (QWidget* parent, const QString& title, const QString& text,
	 const QString& button0Text, const QString& button1Text, const QString& button2Text,
	 int defaultButton, int escapeButton,
	 bool isWarning=false, bool isError=false);
	 //Yes, No, Cancel, in this order
};

#endif
