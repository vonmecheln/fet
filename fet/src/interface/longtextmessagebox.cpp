/*
File longtextmessagebox.cpp
*/

/***************************************************************************
                          longtextmessagebox.cpp  -  description
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

#include "longtextmessagebox.h"

#include "timetable_defs.h"

#ifndef FET_COMMAND_LINE

#include "longtextmessagebox_auxiliary.h"

#include <QMessageBox>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#else

#include "messageboxes.h"

#endif

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
#ifndef FET_COMMAND_LINE
	Q_UNUSED(isWarning);
	Q_UNUSED(isError);

	if(button0Text==QString() || button1Text==QString() || button2Text!=QString()){
		QMessageBox::critical(parent, tr("FET critical"), tr("You have encountered a FET bug. The problem is in file"
		 " %1 line %2, the reason is that a confirmation dialog box does not get exactly 2 arguments. Please report bug. FET will now continue."
		 " You probably don't have any problems with your data file - you can save it.").arg(__FILE__).arg(__LINE__));
	}

	QDialog dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb0=nullptr;
	if(button0Text!=QString()){
		pb0=new QPushButton(button0Text);
		if(defaultButton==0)
			connect(pb0, &QPushButton::clicked, &dialog, &QDialog::accept);
		if(escapeButton==0)
			connect(pb0, &QPushButton::clicked, &dialog, &QDialog::reject);
	}
	
	QPushButton* pb1=nullptr;
	if(button1Text!=QString()){
		pb1=new QPushButton(button1Text);
		if(defaultButton==1)
			connect(pb1, &QPushButton::clicked, &dialog, &QDialog::accept);
		if(escapeButton==1)
			connect(pb1, &QPushButton::clicked, &dialog, &QDialog::reject);
	}
	
	QPushButton* pb2=nullptr;
	if(button2Text!=QString()){
		pb2=new QPushButton(button2Text);
		if(defaultButton==2)
			connect(pb2, &QPushButton::clicked, &dialog, &QDialog::accept);
		if(escapeButton==2)
			connect(pb2, &QPushButton::clicked, &dialog, &QDialog::reject);
	}
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	if(pb0!=nullptr)
		hl->addWidget(pb0);
	if(pb1!=nullptr)
		hl->addWidget(pb1);
	if(pb2!=nullptr)
		hl->addWidget(pb2);
		
	vl->addWidget(te);
	vl->addLayout(hl);
	
	if(pb0!=nullptr && defaultButton==0){
		pb0->setDefault(true);
		pb0->setFocus();
	}
	else if(pb1!=nullptr && defaultButton==1){
		pb1->setDefault(true);
		pb1->setFocus();
	}
	else if(pb2!=nullptr && defaultButton==2){
		pb2->setDefault(true);
		pb2->setFocus();
	}
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	int b=dialog.exec();
	
	if(b==QDialog::Accepted){
		//cout<<"accepted"<<endl;
		return defaultButton;
	}
	else{
		//cout<<"rejected"<<endl;
		return escapeButton;
	}
#else
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text, button0Text, button1Text, button2Text, defaultButton, escapeButton,
	 isWarning, isError);
	return defaultButton;
#endif
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
#ifndef FET_COMMAND_LINE
	Q_UNUSED(isWarning);
	Q_UNUSED(isError);

	QDialog dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb=new QPushButton(tr("OK"));
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::accept);
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);
		
	vl->addWidget(te);
	vl->addLayout(hl);
	
	pb->setDefault(true);
	pb->setFocus();
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	dialog.exec();
#else
	Q_UNUSED(MINW);
	Q_UNUSED(MAXW);
	Q_UNUSED(MINH);
	Q_UNUSED(MAXH);
	
	commandLineMessage(parent, title, text,
	 isWarning, isError);
#endif
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

#ifndef FET_COMMAND_LINE
MyDialogWithThreeButtons::MyDialogWithThreeButtons(QWidget* parent): QDialog(parent)
{
}

MyDialogWithThreeButtons::~MyDialogWithThreeButtons()
{
}

void MyDialogWithThreeButtons::setYes()
{
	clickedButton=QMessageBox::Yes;
	accept();
}

void MyDialogWithThreeButtons::setNo()
{
	clickedButton=QMessageBox::No;
	accept();
}

void MyDialogWithThreeButtons::setCancel()
{
	clickedButton=QMessageBox::Cancel;
	reject();
}

int LongTextMessageBox::largeConfirmationWithDimensionsThreeButtonsYesNoCancel
 (QWidget* parent, const QString& title, const QString& text,
 const QString& button0Text, const QString& button1Text, const QString& button2Text,
 int defaultButton, int escapeButton,
 bool isWarning, bool isError)
 //Yes, No, Cancel, in this order.
{
	Q_UNUSED(isWarning);
	Q_UNUSED(isError);

	int MINW=LARGE_MIN_WIDTH;
	int MAXW=LARGE_MAX_WIDTH;
	int MINH=LARGE_MIN_HEIGHT;
	int MAXH=LARGE_MAX_HEIGHT;

	if(button0Text==QString() || button1Text==QString() || button2Text==QString()){
		assert(0);
	}

	MyDialogWithThreeButtons dialog(parent);
	dialog.setWindowTitle(title);
	
	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(text);
	te->setReadOnly(true);
	
	QPushButton* pb0=nullptr;
	if(button0Text!=QString()){
		pb0=new QPushButton(button0Text);
		if(defaultButton==0)
			connect(pb0, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::accept);
		if(escapeButton==0)
			connect(pb0, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::reject);
	}
	
	QPushButton* pb1=nullptr;
	if(button1Text!=QString()){
		pb1=new QPushButton(button1Text);
		if(defaultButton==1)
			connect(pb1, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::accept);
		if(escapeButton==1)
			connect(pb1, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::reject);
	}
	
	QPushButton* pb2=nullptr;
	if(button2Text!=QString()){
		pb2=new QPushButton(button2Text);
		if(defaultButton==2)
			connect(pb2, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::accept);
		if(escapeButton==2)
			connect(pb2, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::reject);
	}
	
	assert(defaultButton==0);
	assert(escapeButton==2);
	
	connect(pb0, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::setYes);
	connect(pb1, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::setNo);
	connect(pb2, &QPushButton::clicked, &dialog, &MyDialogWithThreeButtons::setCancel);
	
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	if(pb0!=nullptr)
		hl->addWidget(pb0);
	if(pb1!=nullptr)
		hl->addWidget(pb1);
	if(pb2!=nullptr)
		hl->addWidget(pb2);
	
	vl->addWidget(te);
	vl->addLayout(hl);
	
	if(pb0!=nullptr && defaultButton==0){
		pb0->setDefault(true);
		pb0->setFocus();
	}
	else if(pb1!=nullptr && defaultButton==1){
		pb1->setDefault(true);
		pb1->setFocus();
	}
	else if(pb2!=nullptr && defaultButton==2){
		pb2->setDefault(true);
		pb2->setFocus();
	}
	
	int w=dialog.sizeHint().width();
	int h=dialog.sizeHint().height();
	if(w>MAXW) w=MAXW;
	if(w<MINW) w=MINW;
	if(h>MAXH) h=MAXH;
	if(h<MINH) h=MINH;
	dialog.resize(w, h);
	if(parent==0)
		forceCenterWidgetOnScreen(&dialog);
	
	dialog.clickedButton=-1;
	int b=dialog.exec();
	
	if(dialog.clickedButton==-1){
		if(b==QDialog::Accepted){
			//return defaultButton;
			dialog.clickedButton=QMessageBox::Yes;
		}
		else{
			//return escapeButton;
			dialog.clickedButton=QMessageBox::Cancel;
		}
	}
	
	assert(dialog.clickedButton>=0);
	return dialog.clickedButton;
}
#endif
