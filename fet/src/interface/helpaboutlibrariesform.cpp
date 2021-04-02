/***************************************************************************
                          helpaboutlibrariesform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QCoreApplication>

#include <QString>

#include "helpaboutlibrariesform.h"

#include "timetable_defs.h"

HelpAboutLibrariesForm::HelpAboutLibrariesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	aboutLibrariesTextBrowser->setReadOnly(true);
	
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	aboutLibrariesTextBrowser->setOpenExternalLinks(true);
	
	QString about=QString("");
	about+=tr("In addition to Qt (see Help -> About Qt), depending on the platform and compiler,"
	 " this program may use libraries from:");
	about+="<br /><br />";
	about+=QString("&nbsp;&nbsp;&nbsp;&nbsp;");
	about+=QString("gcc, Copyright (C) Free Software Foundation, Inc.");
	about+="<br />";
	about+=QString("&nbsp;&nbsp;&nbsp;&nbsp;");
	about+=tr("Homepage: %1").arg("<a href=\"https://gcc.gnu.org/\">https://gcc.gnu.org/</a>");
	about+="<br /><br />";
	about+=QString("&nbsp;&nbsp;&nbsp;&nbsp;");
	about+=QString("mingw-w64, Copyright (c) by the mingw-w64 project.");
	about+="<br />";
	about+=QString("&nbsp;&nbsp;&nbsp;&nbsp;");
	about+=tr("Homepage: %1").arg("<a href=\"http://mingw-w64.org/\">http://mingw-w64.org/</a>");
	about+="<br />";
	
	aboutLibrariesTextBrowser->setHtml(about);
}

HelpAboutLibrariesForm::~HelpAboutLibrariesForm()
{
	saveFETDialogGeometry(this);
}
