/***************************************************************************
                          httpget.h  -  description
                             -------------------
    begin                : July 24 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef HTTPGET_H
#define HTTPGET_H

#include <QHttp>
#include <QBuffer>
#include <QUrl>

class HttpGet: public QObject
{
	Q_OBJECT
	
public:
	HttpGet(QObject* parent=0);
	bool getFile(const QUrl& url);

signals:
	void done(bool error);

private slots:
	void httpDone(bool error);
	
public:
	QHttp http;
	
private:
	QBuffer buffer;
};

#endif
