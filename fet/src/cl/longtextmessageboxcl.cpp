/*
File longtextmessageboxcl.cpp
*/

/***************************************************************************
                          longtextmessageboxcl.cpp  -  description
                             -------------------
    begin                : 2026
    copyright            : (C) 2026 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "longtextmessageboxcl.h"

#include "timetable_defs.h"

#include "messageboxes.h"

const int MIN_WIDTH=420;  //golden ratio 1.618 for min dimensions :-)
const int MAX_WIDTH=1000;
const int MIN_HEIGHT=260;
const int MAX_HEIGHT=650;

const int LARGE_MIN_WIDTH=590;
const int LARGE_MAX_WIDTH=1000;
const int LARGE_MIN_HEIGHT=380;
const int LARGE_MAX_HEIGHT=650;

const int MEDIUM_MIN_WIDTH=461;
const int MEDIUM_MAX_WIDTH=1000;
const int MEDIUM_MIN_HEIGHT=285;
const int MEDIUM_MAX_HEIGHT=650;

int LongTextMessageBox::confirmationWithDimensions
 (QWidget* parent, const QString& title, const QString& text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton, int MINW, int MAXW, int MINH, int MAXH,
 bool isWarning, bool isError )
{
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text, button0Text, button1Text, button2Text, defaultButton, escapeButton,
	 isWarning, isError);
	return defaultButton;
}

int LongTextMessageBox::confirmation
 (QWidget* parent, const QString& title, const QString& text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton,
 bool isWarning, bool isError)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		MIN_WIDTH, MAX_WIDTH, MIN_HEIGHT, MAX_HEIGHT,
		isWarning, isError);
}

int LongTextMessageBox::largeConfirmation
 (QWidget* parent, const QString& title, const QString& text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton,
 bool isWarning, bool isError)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		LARGE_MIN_WIDTH, LARGE_MAX_WIDTH, LARGE_MIN_HEIGHT, LARGE_MAX_HEIGHT,
		isWarning, isError);
}

int LongTextMessageBox::mediumConfirmation
 (QWidget* parent, const QString& title, const QString& text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton,
 bool isWarning, bool isError)
{
	return LongTextMessageBox::confirmationWithDimensions(parent, title, text,
		button0Text, button1Text, button2Text,
		defaultButton, escapeButton,
		MEDIUM_MIN_WIDTH, MEDIUM_MAX_WIDTH, MEDIUM_MIN_HEIGHT, MEDIUM_MAX_HEIGHT,
		isWarning, isError);
}

void LongTextMessageBox::informationWithDimensions
 (QWidget* parent, const QString& title, const QString& text, int MINW, int MAXW, int MINH, int MAXH,
 bool isWarning, bool isError)
{
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text,
	 isWarning, isError);
}

void LongTextMessageBox::information
 (QWidget* parent, const QString& title, const QString& text,
 bool isWarning, bool isError)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, MIN_WIDTH, MAX_WIDTH, MIN_HEIGHT, MAX_HEIGHT,
	 isWarning, isError);
}

void LongTextMessageBox::largeInformation
 (QWidget* parent, const QString& title, const QString& text,
 bool isWarning, bool isError)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, LARGE_MIN_WIDTH, LARGE_MAX_WIDTH, LARGE_MIN_HEIGHT, LARGE_MAX_HEIGHT,
	 isWarning, isError);
}

void LongTextMessageBox::mediumInformation
 (QWidget* parent, const QString& title, const QString& text,
 bool isWarning, bool isError)
{
	LongTextMessageBox::informationWithDimensions(parent, title, text, MEDIUM_MIN_WIDTH, MEDIUM_MAX_WIDTH, MEDIUM_MIN_HEIGHT, MEDIUM_MAX_HEIGHT,
	 isWarning, isError);
}
