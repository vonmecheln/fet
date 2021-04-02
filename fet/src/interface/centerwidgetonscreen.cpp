/*
File centerwidgetonscreen.cpp
*/

/***************************************************************************
                          centerwidgetonscreen.cpp  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "centerwidgetonscreen.h"

#include "fetmainform.h"

#include "rules.h"
#include "timetable.h"

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QSize>
#include <QPoint>

#include <QSettings>

#include <QObject>
#include <QMetaObject>

#include <QString>

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>

extern const QString COMPANY;
extern const QString PROGRAM;

extern FetMainForm* pFetMainForm;

extern Timetable gt;

void centerWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);

	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
/*	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());*/
}

void forceCenterWidgetOnScreen(QWidget* widget)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());
}

/*void centerWidgetOnParent(QWidget* widget, QWidget* parent)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	assert(parent!=NULL);
	
	QRect frect=widget->geometry();
	frect.moveCenter(parent->geometry().center());
	widget->move(frect.topLeft());
}*/

int maxScreenWidth(QWidget* widget)
{
	QRect rect = QApplication::desktop()->availableGeometry(widget);

	return rect.width();
}

int maxRecommendedWidth(QWidget* widget)
{
	int d=maxScreenWidth(widget);
	
	if(d<800)
		d=800;
		
	//if(d>1000)
	//	d=1000;
	
	return d;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	
	QRect rect=widget->geometry();
	settings.setValue(name+QString("/geometry"), rect);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name=QString(widget->metaObject()->className());
	if(!alternativeName.isEmpty())
		name=alternativeName;
	if(settings.contains(name+QString("/geometry"))){
		QRect rect=settings.value(name+QString("/geometry")).toRect();
		if(rect.isValid())
			widget->setGeometry(rect);
	}
}

void setParentAndOtherThings(QWidget* widget, QWidget* parent)
{
	Q_UNUSED(widget);
	Q_UNUSED(parent);

/*	if(!widget->parentWidget()){
		widget->setParent(parent, Qt::Dialog);
	
		QRect rect=widget->geometry();
		rect.translate(widget->geometry().x() - widget->frameGeometry().x(), widget->geometry().y() - widget->frameGeometry().y());
		widget->setGeometry(rect);
	}*/
}

void setStretchAvailabilityTableNicely(QTableWidget* notAllowedTimesTable)
{
	notAllowedTimesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	int q=notAllowedTimesTable->horizontalHeader()->defaultSectionSize();
	notAllowedTimesTable->horizontalHeader()->setMinimumSectionSize(q);

	notAllowedTimesTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
	q=-1;
	for(int i=0; i<notAllowedTimesTable->verticalHeader()->count(); i++)
		if(q<notAllowedTimesTable->verticalHeader()->sectionSizeHint(i))
			q=notAllowedTimesTable->verticalHeader()->sectionSizeHint(i);
	notAllowedTimesTable->verticalHeader()->setMinimumSectionSize(q);
	
	//2011-09-23
	for(int i=0; i<notAllowedTimesTable->rowCount(); i++){
		for(int j=0; j<notAllowedTimesTable->columnCount(); j++){
			QFont font=notAllowedTimesTable->item(i,j)->font();
			font.setBold(true);
			notAllowedTimesTable->item(i,j)->setFont(font);
		}
	}
	notAllowedTimesTable->setCornerButtonEnabled(false);
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	if(!rules->modified){
		rules->modified=true;
	
		if(rules==&gt.rules && pFetMainForm!=NULL)
			if(!pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(true);
	}
}

void setRulesUnmodifiedAndOtherThings(Rules* rules)
{
	if(rules->modified){
		rules->modified=false;
	
		if(rules==&gt.rules && pFetMainForm!=NULL)
			if(pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(false);
	}
}
