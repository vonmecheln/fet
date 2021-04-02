/***************************************************************************
                          advancedfilterform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADVANCEDFILTERFORM_H
#define ADVANCEDFILTERFORM_H

#include <QtGui>


class AdvancedFilterForm : public QDialog {
	Q_OBJECT
	
public:
	int rows;
	
	QRadioButton* allRadio;
	QRadioButton* anyRadio;
	
	QCheckBox* caseSensitiveCheckBox;
	QHBoxLayout* caseSensitiveLayout;

	QPushButton* morePushButton;
	QPushButton* fewerPushButton;
	QList<QComboBox*> descrDetDescrComboBoxList;
	QList<QComboBox*> contNContReNReComboBoxList;
	QList<QLineEdit*> textLineEditList;
	QPushButton* resetPushButton;
	QPushButton* okPushButton;
	QPushButton* cancelPushButton;
	
	QVBoxLayout* allAnyLayout;
	
	QGridLayout* filtersLayout;
	QHBoxLayout* moreFewerLayout;
	QHBoxLayout* buttonsLayout;
	
	QVBoxLayout* layout;
	
public:
	AdvancedFilterForm(bool all, QList<int> descrDetDescr, QList<int> contNCont, QStringList text, bool caseSensitive);
	~AdvancedFilterForm();
	
public slots:
	void reset();
	void more();
	void fewer();
};

#endif
