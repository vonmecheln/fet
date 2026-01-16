/*
File longtextmessagebox_auxiliary.h
*/

/***************************************************************************
                          longtextmessagebox_auxiliary.h  -  description
                             -------------------
    begin                : 6 October 2015
    copyright            : (C) 2015 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef LONGTEXTMESSAGEBOX_AUXILIARY_H
#define LONGTEXTMESSAGEBOX_AUXILIARY_H

#include <QObject>

#include <QDialog>

class QWidget;

class MyDialogWithThreeButtons: public QDialog
{
	Q_OBJECT
	
public:
	int clickedButton;
	
	MyDialogWithThreeButtons(QWidget* parent);
	~MyDialogWithThreeButtons();
	
public Q_SLOTS:
	void setYes();
	void setNo();
	void setCancel();
};

#endif
