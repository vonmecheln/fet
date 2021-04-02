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

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

void centerWidgetOnScreen(QWidget* widget)
{
	widget->setWindowFlags(widget->windowFlags() | Qt::WindowMinMaxButtonsHint);

	QRect rect = QApplication::desktop()->availableGeometry(widget);
	 
	widget->move(rect.center() - widget->rect().center());
}
