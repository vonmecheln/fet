/***************************************************************************
                          timetable_defs.cpp  -  description
                             -------------------
    begin                : Sat Mar 15 2003
    copyright            : (C) 2003 by Lalescu Liviu
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


#include "timetable_defs.h"

#include <QByteArray>

int checkForUpdates;

QString internetVersion;

/**
FET version
*/
const QString FET_VERSION="5.5.5";

/**
FET language
*/
QString FET_LANGUAGE="en_GB";

bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

QString LANGUAGE_FOR_HTML;

/**
Timetable html css javaScript Level, by Volker Dirr
*/
int TIMETABLE_HTML_LEVEL;

QString protect(const QString& str) //used for xml
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	p.replace("'", "&apos;");
	return p;
}

QString protect2(const QString& str) //used for html
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&apos;");
	return p;
}

//protect2vert is very similar to protect2
//protect2vert code contributed by Volker Dirr

QString protect2vert(const QString& str) //used for html
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&apos;");

	QString returnstring;
	for(int i=0; i<p.size();i++){
		QString a=p.at(i);
		QString b="<br />";
		returnstring.append(a);
		returnstring.append(b);
	}
	return returnstring;
}

//protect2id is very similar to protect2
//protect2id code contributed by Volker Dirr

QString protect2id(const QString& str) //used for html
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&apos;");
	p.replace(" ", "_");		// id must be a single token
	p.replace(",", "_");		// looks like this makes trouble
	return p;
}

QString protect3(const QString& str) //used for iCal
{
	QString p=str;
	p.replace("?", "_");
	p.replace("/", "_");
	p.replace(" ", "_");
	p.replace("\\", "_");
	p.replace(":", "_");
	p.replace("'", "_");
	p.replace("*", "_");
	p.replace("\"", "_");
	return p;
}

bool isLeapYear(int year)
{
	bool leap=false;
	if(year%4==0){
		leap=true;
		if(year%100==0){
			leap=false;
			if(year%400==0)
				leap=true;
		}
	}

	return leap;
}

bool isCorrectDay(const QString sday)
{
	QByteArray day=sday.toAscii();

	if(day.length()!=8)
		return false;

	int nDays[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int y=(day[0]-'0')*1000+(day[1]-'0')*100+(day[2]-'0')*10+(day[3]-'0');
	if(y<1 || y>=10000)
		return false;
	if(isLeapYear(y))
		nDays[2]=29;
	int m=(day[4]-'0')*10+(day[5]-'0');
	if(m>12 || m<1)
		return false;
	int d=(day[6]-'0')*10+(day[7]-'0');
	if(d>nDays[m] || d<1)
		return false;

	return true;
}

bool isCorrectHour(const QString shour)
{
	QByteArray hour=shour.toAscii();

	if(hour.length()!=4)
		return false;
	int h=(hour[0]-'0')*10+(hour[1]-'0');
	if(h<0 || h>23)
		return false;
	int m=(hour[2]-'0')*10+(hour[3]-'0');
	if(m<0 || h>59)
		return false;

	return true;
}

QString nextDay(const QString sday)
{
	QByteArray day=sday.toAscii();

	int nDays[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int y=(day[0]-'0')*1000+(day[1]-'0')*100+(day[2]-'0')*10+(day[3]-'0');
	if(isLeapYear(y))
		nDays[2]=29;
	int m=(day[4]-'0')*10+(day[5]-'0');
	assert(m<=12);
	int d=(day[6]-'0')*10+(day[7]-'0');
	assert(d<=nDays[m]);
	if(d==nDays[m]){
		if(m==12){
			y++;
			m=1;
			d=1;
		}
		else{
			d=1;
			m++;
		}
	}
	else
		d++;

	char tmp[9];
	QString nday;

	tmp[0]=y/1000+'0';
	y%=1000;
	tmp[1]=y/100+'0';
	y%=100;
	tmp[2]=y/10+'0';
	y%=10;
	tmp[3]=y+'0';

	tmp[4]=m/10+'0';
	m%=10;
	tmp[5]=m+'0';

	tmp[6]=d/10+'0';
	d%=10;
	tmp[7]=d+'0';

	tmp[8]='\0';

	nday=tmp;

	return nday;
}

bool sumHours(const QString shour1, const QString shour2, QString& result)
{
	QByteArray hour1=shour1.toAscii();
	QByteArray hour2=shour2.toAscii();

	//Sums the hour1 with hour2 and outputs the sum in "result".
	//Returns true if the result is the next day
	assert(isCorrectHour(hour1));
	assert(isCorrectHour(hour2));

	int h1=(hour1[0]-'0')*10+(hour1[1]-'0');
	int m1=(hour1[2]-'0')*10+(hour1[3]-'0');

	int h2=(hour2[0]-'0')*10+(hour2[1]-'0');
	int m2=(hour2[2]-'0')*10+(hour2[3]-'0');

	int rh=h1+h2+(m1+m2)/60;
	int rm=(m1+m2)%60;

	bool nextDay;

	if(rh>=24){
		nextDay=true;
		rh-=24;
	}
	else
		nextDay=false;

	char res[5];

	res[0]=rh/10+'0';
	rh%=10;
	res[1]=rh+'0';
	res[2]=rm/10+'0';
	rm%=10;
	res[3]=rm+'0';
	res[4]='\0';

	result=res;

	assert(isCorrectHour(result));

	return nextDay;
}

QString iCalFolding(const QString s)
{
	//makes the (long) string conform tu iCalendar standard
	//by adding after each 75 characters a CRLF + SPACE
	QString t;
	for(int i=0; i<s.length(); i++){
		if(i!=0 && i%75==0){
#ifdef WIN32
			t.append(char(13));
#endif
			t.append(char(10));
			t.append(char(32));
		}
		t.append(s.at(i));
	}

	return t;
}

int XX;

//random routines
void initRandomKnuth()
{
	assert(MM==2147483647);
	assert(AA==48271);
	assert(QQ==44488);
	assert(RR==3399);
				
	XX=1+((unsigned(time(NULL)))%(MM-1));
}
	
int randomKnuth()
{
	XX=AA*(XX%QQ)-RR*(XX/QQ);
	if(XX<0)
		XX+=MM;
		
	return XX;
}
