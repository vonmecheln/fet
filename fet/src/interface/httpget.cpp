/***************************************************************************
                          httpget.cpp  -  description
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

#include "httpget.h"
#include "timetable_defs.h"

#include <iostream>
using namespace std;

#include <QMessageBox>

HttpGet::HttpGet(QObject* parent): QObject(parent)
{
	connect(&http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
}

bool HttpGet::getFile(const QUrl& url)
{
	if(!url.isValid()){
		assert(0);
		return false;
	}
		
	if(url.scheme()!="http"){
		assert(0);
		return false;
	}
	
	cout<<"New version checking host: "<<qPrintable(url.host())<<endl;
	cout<<"New version checking path: "<<qPrintable(url.path())<<endl;
		
	assert(!url.path().isEmpty());
	
	http.setHost(url.host(), url.port(80));
	buffer.open(QIODevice::WriteOnly);
	http.get(url.path(), &buffer);
	//http.close(); - does not work on new server
	return true;
}

void HttpGet::httpDone(bool error)
{
	buffer.close();
	if(error){
		/*cout<<"Http error"<<qPrintable(http.errorString())<<endl;
		
		QString s;
		
		s=tr("FET could not search for updates, error message is:\n%1").arg(http.errorString());
		
		QMessageBox::warning(NULL, tr("FET warning"), s);*/
	}
	else{
		if(buffer.size()>256){
			//error - the server returned file not found (????)
			emit(done(true));
			return;
		}

		buffer.open(QIODevice::ReadOnly);
	
		internetVersion="";
		
		char c;
		for(;;){
			bool t=buffer.getChar(&c);
			if(c!='\n' && c!='\t' && c!=' ' && c!=13)
				break;
			if(!t){
				c=' ';
				break;
			}
		}
		
		if(c==' '){ //only whitespaces in file - impossible
			emit(done(true));
			return;
		}
		
		for(;;){
			internetVersion+=c;
			bool t=buffer.getChar(&c);
			if(!t)
				break;
			if(c=='\n' || c=='\t' || c==' ' || c==13)
				break;
		}
		cout<<"Internet version read as: '";
		cout<<qPrintable(internetVersion)<<"'"<<endl;

		buffer.close();
	}
	emit(done(error));
}
