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

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>

void centerWidgetOnScreen(QWidget* widget)
{
	widget->setWindowFlags(widget->windowFlags() | Qt::WindowMinMaxButtonsHint);

	/*QRect rect = QApplication::desktop()->availableGeometry(widget);
	 
	widget->move(rect.center() - widget->rect().center());*/
	
	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());
}

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
