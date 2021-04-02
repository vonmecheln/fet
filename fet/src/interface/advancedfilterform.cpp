/***************************************************************************
                          advancedfilterform.cpp  -  description
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

#include "advancedfilterform.h"

#include "timetable_defs.h"

#include "centerwidgetonscreen.h"


const int MAX_ROWS=8;
const int MIN_ROWS=1;

AdvancedFilterForm::AdvancedFilterForm(bool all, QList<int> descrDetDescr, QList<int> contNCont, QStringList text, bool caseSensitive)
{
	assert(descrDetDescr.count()==contNCont.count());
	assert(contNCont.count()==text.count());
	assert(text.count()>=MIN_ROWS && text.count()<=MAX_ROWS);
	
	rows=descrDetDescr.count();
	
	setWindowTitle(tr("Advanced filter for constraints"));
	
	allRadio=new QRadioButton(tr("Match all of the following:"));
	anyRadio=new QRadioButton(tr("Match any of the following:"));
	//allRadio->setChecked(false);
	//anyRadio->setChecked(false);
	if(all)
		allRadio->setChecked(true);
	else
		anyRadio->setChecked(true);
	
	caseSensitiveCheckBox=new QCheckBox(tr("Case sensitive"));
	caseSensitiveCheckBox->setChecked(caseSensitive);
	
	caseSensitiveLayout=new QHBoxLayout();
	caseSensitiveLayout->addStretch();
	caseSensitiveLayout->addWidget(caseSensitiveCheckBox);
	
	allAnyLayout=new QVBoxLayout();
	allAnyLayout->addWidget(allRadio);
	allAnyLayout->addWidget(anyRadio);
	
	for(int i=0; i<MAX_ROWS; i++){
		QComboBox* cb1=new QComboBox();
		cb1->insertItem(tr("Description"));
		cb1->insertItem(tr("Detailed description"));
		if(i<rows)
			cb1->setCurrentIndex(descrDetDescr.at(i));
		else
			cb1->setCurrentIndex(0);
		
		QComboBox* cb2=new QComboBox();
		cb2->insertItem(tr("Contains", "A text string contains other substring"));
		cb2->insertItem(tr("Does not contain", "A text string does not contain other substring"));
		cb2->insertItem(tr("Matches regular expr.", "A text string matches a regular expression (regexp)."
			" Regular expressions are a complex notion, see definition on the internet if you don't know about them or how to translate the words.  "
			"\nDefinition from Wikipedia: 'In computing, regular expressions, also referred to as regex or regexp, provide a concise and flexible means "
			"for identifying strings of text, such as particular characters, words, or patterns of characters. A regular expression is written in "
			"a formal language that can be interpreted by a regular expression processor, a program that either serves as a parser generator or "
			"examines text and identifies parts that match the provided specification."));
		cb2->insertItem(tr("Does not match reg. expr.", "A text string does not match a regular expression (regexp)."
			" Regular expressions are a complex notion, see definition on the internet if you don't know about them or how to translate the words.  "
			"\nDefinition from Wikipedia: 'In computing, regular expressions, also referred to as regex or regexp, provide a concise and flexible means "
			"for identifying strings of text, such as particular characters, words, or patterns of characters. A regular expression is written in "
			"a formal language that can be interpreted by a regular expression processor, a program that either serves as a parser generator or "
			"examines text and identifies parts that match the provided specification."));
		if(i<rows)
			cb2->setCurrentItem(contNCont.at(i));
		else
			cb2->setCurrentItem(0);
		
		QLineEdit* ln1=new QLineEdit();
		if(i<rows)
			ln1->setText(text.at(i));
		else
			ln1->setText(QString(""));
		
		descrDetDescrComboBoxList.append(cb1);
		contNContReNReComboBoxList.append(cb2);
		textLineEditList.append(ln1);
	}
	
	filtersLayout=new QGridLayout();
	/*QComboBox* cb1=new QComboBox();
	cb1->insertItem(tr("Description"));
	cb1->insertItem(tr("Detailed description"));
	QComboBox* cb2=new QComboBox();
	cb2->insertItem(tr("Contains"));
	cb2->insertItem(tr("Does not contain"));
	cb2->insertItem(tr("Matches regular expr."));
	cb2->insertItem(tr("Does not match reg. expr."));
	QLineEdit* ln1=new QLineEdit(QString(""));
	descrDetDescrComboBoxList.append(cb1);
	contNContReNReComboBoxList.append(cb2);
	textLineEditList.append(ln1);*/

	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	assert(descrDetDescrComboBoxList.count()==MAX_ROWS);
	for(int i=0; i<MAX_ROWS; i++){
		QComboBox* cb1=descrDetDescrComboBoxList.at(i);
		QComboBox* cb2=contNContReNReComboBoxList.at(i);
		QLineEdit* ln1=textLineEditList.at(i);
		
		filtersLayout->addWidget(cb1, i, 0);
		filtersLayout->addWidget(cb2, i, 1);
		filtersLayout->addWidget(ln1, i, 2);
	}
	
	morePushButton=new QPushButton(tr("More"));
	fewerPushButton=new QPushButton(tr("Fewer"));
	moreFewerLayout=new QHBoxLayout();
	moreFewerLayout->addWidget(morePushButton);
	moreFewerLayout->addWidget(fewerPushButton);
	moreFewerLayout->addStretch();
	morePushButton->setEnabled(rows<MAX_ROWS);
	fewerPushButton->setEnabled(rows>MIN_ROWS);
	
	resetPushButton=new QPushButton(tr("Reset"));
	okPushButton=new QPushButton(tr("OK"));
	okPushButton->setDefault(true);
	cancelPushButton=new QPushButton(tr("Cancel"));
	buttonsLayout=new QHBoxLayout();
	buttonsLayout->addWidget(resetPushButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okPushButton);
	buttonsLayout->addWidget(cancelPushButton);
	
	layout=new QVBoxLayout(this);
	layout->addLayout(allAnyLayout);
	layout->addLayout(filtersLayout);
	layout->addLayout(moreFewerLayout);
	layout->addLayout(caseSensitiveLayout);
	//layout->addSpacing(100);
	layout->addStretch();
	layout->addLayout(buttonsLayout);
	
	connect(resetPushButton, SIGNAL(clicked()), this, SLOT(reset()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(morePushButton, SIGNAL(clicked()), this, SLOT(more()));
	connect(fewerPushButton, SIGNAL(clicked()), this, SLOT(fewer()));
	
	int w=this->sizeHint().width();
	int h=this->sizeHint().height();
	//if(h<450)
	//	h=450;
	this->setGeometry(0, 0, w, h);
	centerWidgetOnScreen(this);
	
	for(int i=0; i<MAX_ROWS; i++){
		descrDetDescrComboBoxList.at(i)->setVisible(i<rows);
		contNContReNReComboBoxList.at(i)->setVisible(i<rows);
		textLineEditList.at(i)->setVisible(i<rows);
	}
}

AdvancedFilterForm::~AdvancedFilterForm()
{

}

void AdvancedFilterForm::reset()
{
	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	assert(descrDetDescrComboBoxList.count()==MAX_ROWS);
	
	rows=1;

	fewerPushButton->setEnabled(rows>MIN_ROWS);
	morePushButton->setEnabled(rows<MAX_ROWS);
	
	for(int i=0; i<MAX_ROWS; i++){
		if(i<rows){
			descrDetDescrComboBoxList.at(i)->setVisible(true);
		}
		else{
			descrDetDescrComboBoxList.at(i)->setVisible(false);
		}
		descrDetDescrComboBoxList.at(i)->setCurrentIndex(0);

		if(i<rows){
			contNContReNReComboBoxList.at(i)->setVisible(true);
		}
		else{
			contNContReNReComboBoxList.at(i)->setVisible(false);
		}
		contNContReNReComboBoxList.at(i)->setCurrentIndex(0);

		if(i<rows){
			textLineEditList.at(i)->setVisible(true);
		}
		else{
			textLineEditList.at(i)->setVisible(false);
		}
		textLineEditList.at(i)->setText(QString(""));
	}
	
	//allRadio->setChecked(false);
	//anyRadio->setChecked(false);
	allRadio->setChecked(true);
	
/*	descrDetDescrComboBoxList.at(0)->setCurrentItem(0);
	contNContReNReComboBoxList.at(0)->setCurrentItem(0);
	textLineEditList.at(0)->setText(QString(""));*/
	
	caseSensitiveCheckBox->setChecked(false);
}

void AdvancedFilterForm::more()
{
/*
	QComboBox* cb1=new QComboBox();
	cb1->insertItem(tr("Description"));
	cb1->insertItem(tr("Detailed description"));
	QComboBox* cb2=new QComboBox();
	cb2->insertItem(tr("Contains"));
	cb2->insertItem(tr("Does not contain"));
	cb2->insertItem(tr("Matches regular expr."));
	cb2->insertItem(tr("Does not match reg. expr."));
	QLineEdit* ln1=new QLineEdit(QString(""));
	descrDetDescrComboBoxList.append(cb1);
	contNContReNReComboBoxList.append(cb2);
	textLineEditList.append(ln1);

	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	for(int i=0; i<descrDetDescrComboBoxList.count(); i++){
		QComboBox* cb1=descrDetDescrComboBoxList.at(i);
		QComboBox* cb2=contNContReNReComboBoxList.at(i);
		QLineEdit* ln1=textLineEditList.at(i);
		
		filtersLayout->addWidget(cb1, i, 0);
		filtersLayout->addWidget(cb2, i, 1);
		filtersLayout->addWidget(ln1, i, 2);
	}

	fewerPushButton->setEnabled(descrDetDescrComboBoxList.count()>MIN_ROWS);
	morePushButton->setEnabled(descrDetDescrComboBoxList.count()<MAX_ROWS);
	*/

	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	assert(descrDetDescrComboBoxList.count()==MAX_ROWS);
	
	assert(rows<MAX_ROWS);
	
	descrDetDescrComboBoxList.at(rows)->setCurrentIndex(0);
	contNContReNReComboBoxList.at(rows)->setCurrentIndex(0);
	textLineEditList.at(rows)->setText(QString(""));
	
	descrDetDescrComboBoxList.at(rows)->setVisible(true);
	contNContReNReComboBoxList.at(rows)->setVisible(true);
	textLineEditList.at(rows)->setVisible(true);
	
	rows++;
	
	fewerPushButton->setEnabled(rows>MIN_ROWS);
	morePushButton->setEnabled(rows<MAX_ROWS);
}

void AdvancedFilterForm::fewer()
{
	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	assert(descrDetDescrComboBoxList.count()==MAX_ROWS);
	
	assert(rows>MIN_ROWS);
	
	rows--;
	
	descrDetDescrComboBoxList.at(rows)->setCurrentIndex(0);
	contNContReNReComboBoxList.at(rows)->setCurrentIndex(0);
	textLineEditList.at(rows)->setText(QString(""));
	
	descrDetDescrComboBoxList.at(rows)->setVisible(false);
	contNContReNReComboBoxList.at(rows)->setVisible(false);
	textLineEditList.at(rows)->setVisible(false);
	
	fewerPushButton->setEnabled(rows>MIN_ROWS);
	morePushButton->setEnabled(rows<MAX_ROWS);

/*
	delete descrDetDescrComboBoxList.takeLast();
	delete contNContReNReComboBoxList.takeLast();
	delete textLineEditList.takeLast();

	assert(descrDetDescrComboBoxList.count()==contNContReNReComboBoxList.count());
	assert(descrDetDescrComboBoxList.count()==textLineEditList.count());
	for(int i=0; i<descrDetDescrComboBoxList.count(); i++){
		QComboBox* cb1=descrDetDescrComboBoxList.at(i);
		QComboBox* cb2=contNContReNReComboBoxList.at(i);
		QLineEdit* ln1=textLineEditList.at(i);
		
		filtersLayout->addWidget(cb1, i, 0);
		filtersLayout->addWidget(cb2, i, 1);
		filtersLayout->addWidget(ln1, i, 2);
	}

	fewerPushButton->setEnabled(descrDetDescrComboBoxList.count()>MIN_ROWS);
	morePushButton->setEnabled(descrDetDescrComboBoxList.count()<MAX_ROWS);
*/
}
