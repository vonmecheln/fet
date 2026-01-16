/*
File timetable_defs.cpp
*/

/***************************************************************************
                          timetable_defs.cpp  -  description
                             -------------------
    begin                : Sat Mar 15 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"

//#include <ctime>
#include <chrono>

#include <QHash>

#include <QLocale>

#include <QCoreApplication>

#include <QChar>
#include <QString>

const QString IL1=QString(2, ' '); //indentation levels
const QString IL2=QString(4, ' ');
const QString IL3=QString(6, ' ');
const QString IL4=QString(8, ' ');
const QString IL5=QString(10, ' ');

bool USE_UNDO_REDO=true;
int UNDO_REDO_STEPS=100;
//int UNDO_REDO_COMPRESSION_LEVEL=-1;

bool USE_UNDO_REDO_SAVE=false;
int UNDO_REDO_STEPS_SAVE=20;
QString SUFFIX_FILENAME_SAVE_HISTORY=".his";

bool USE_AUTOSAVE=false;
int MINUTES_AUTOSAVE=3;
int OPERATIONS_AUTOSAVE=1;
QString DIRECTORY_AUTOSAVE="";
QString SUFFIX_FILENAME_AUTOSAVE="_AUTOSAVE";

bool checkForUpdates;

QString internetVersion;

int STUDENTS_COMBO_BOXES_STYLE=STUDENTS_COMBO_BOXES_STYLE_SIMPLE;

/**
The FET version
*/
const QString FET_VERSION="7.5.5";

/**
The version number of the data format, useful when saving/restoring the history to/from the disk.
On any change of the data format, no matter how small or large, this number must be updated;
older or newer data history versions are not compatible.
*/
const QString FET_DATA_FORMAT_VERSION="7.5.4"; //started from "6.15.1", since FET-6.15.1.

/**
The FET language
*/
QString FET_LANGUAGE="en_US";

QString FET_LANGUAGE_WITH_LOCALE="en_US";

/**
The output directory. Please be careful when editing it,
because the functions add a FILE_SEP sign at the end of it
and then the name of a file. If you make OUTPUT_DIR="",
there will be problems.
*/
QString OUTPUT_DIR;

QString CURRENT_OUTPUT_DIRECTORY;
QString HIGHEST_OUTPUT_DIRECTORY;

QString MULTIPLE_OUTPUT_DIRECTORY;

bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

bool PRINT_RTL=false;

bool PRINT_FROM_INTERFACE=false;

QString LANGUAGE_FOR_HTML;

bool OVERWRITE_SINGLE_GENERATION_FILES=false;

/**
Timetable HTML CSS JavaScript level, by Volker Dirr
*/
int TIMETABLE_HTML_LEVEL;

bool TIMETABLE_HTML_PRINT_ACTIVITY_TAGS;

bool TIMETABLE_HTML_PRINT_SUBJECTS;
bool TIMETABLE_HTML_PRINT_TEACHERS;
bool TIMETABLE_HTML_PRINT_STUDENTS;
bool TIMETABLE_HTML_PRINT_ROOMS;

bool PRINT_DETAILED_HTML_TIMETABLES;

bool PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS;

bool PRINT_NOT_AVAILABLE_TIME_SLOTS;

bool PRINT_BREAK_TIME_SLOTS;

bool PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME;

bool DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS;

bool TIMETABLE_HTML_REPEAT_NAMES;

bool VERBOSE;

//these hashes are needed to get the IDs for HTML and CSS in timetableexport and statistics
/*QHash<QString, QString> hashSubjectIDs;
QHash<QString, QString> hashActivityTagIDs;
QHash<QString, QString> hashStudentIDs;
QHash<QString, QString> hashTeacherIDs;
QHash<QString, QString> hashRoomIDs;
QHash<QString, QString> hashDayIDs;*/

/**
A log file explaining how the XML input file was parsed
*/
const QString XML_PARSING_LOG_FILENAME="file_open.log";

/**
The predefined names of the days of the week
*/
/*
const QString PREDEFINED_DAYS_OF_THE_WEEK[]={"Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday", "Sunday", "Monday2",
	"Tuesday2", "Wednesday2", "Thursday2", "Friday2", "Saturday2", "Sunday2",
	"Monday3", "Tuesday3", "Wednesday3",
	"Thursday3", "Friday3", "Saturday3", "Sunday3", "Monday4",
	"Tuesday4", "Wednesday4", "Thursday4", "Friday4", "Saturday4", "Sunday4"};
*/

/**
File and directory separator
*/
const QString FILE_SEP="/";

QString protect(const QString& str) //used for XML
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	p.replace("'", "&apos;");
	return p;
}

QString protect2(const QString& str) //used for HTML
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&#39;"); (note that "&apos;" does not work for HTML 4)
	return p;
}

QString protect2vert(const QString& str) //used for HTML
{
	QString p=str;
	p.replace("&", "&amp;");
	p.replace("\"", "&quot;");
	p.replace(">", "&gt;");
	p.replace("<", "&lt;");
	//p.replace("'", "&#39;"); (note that "&apos;" does not work for HTML 4)

	QString returnstring;
	for(int i=0; i<p.size();i++){
		QString a=p.at(i);
		QString b="<br />";
		returnstring.append(a);
		returnstring.append(b);
	}
	return returnstring;
}

QString protect3(const QString& str) //used for CSS
{
	//This should take care of wrongly nested comments in the CSS file. The situation that the returned string begins or ends with a "/" is
	//assumed nonproblematic, because protect3(...) is supposed to be preceded and followed by a whitespace character (so we cannot have
	//an ugly "/*/" at the beginning or at the end of the resulted string).
	QString p=str;
	p.replace("/*", "/_*");
	p.replace("*/", "*_/");
	return p;
}

QString translatedComma()
{
	return QCoreApplication::translate("PunctuationMarks", ",", "Comma");
}

QString translatedCommaSpace()
{
	return translatedComma()+QString(" ");
}

QString translatedSemicolon()
{
	return QCoreApplication::translate("PunctuationMarks", ";", "Semicolon");
}

QString translatedSemicolonSpace()
{
	return translatedSemicolon()+QString(" ");
}

QString translatedDot()
{
	return QCoreApplication::translate("PunctuationMarks", ".", "Dot");
}

QString translatedQuestionMark()
{
	return QCoreApplication::translate("PunctuationMarks", "?", "Question mark");
}

QString translatedThreeQuestionMarks()
{
	return translatedQuestionMark()+translatedQuestionMark()+translatedQuestionMark();
}

///////begin tricks
void weight_sscanf(const QString& str, const char* fmt, double* result)
{
	assert(QString(fmt)==QString("%lf"));

	bool ok;
	double myres=customFETStrToDouble(str, &ok);
	if(!ok)
		(*result)=-2.5; //any value that does not belong to {>=0.0 and <=100.0} or {-1.0}
						//not -1.0 because of modify multiple constraints min days between activities,
						//-1 there represents any weight
						//potential bug found by Volker Dirr
	else
		(*result)=myres;
}

QString CustomFETString::number(int n)
{
	return QString::number(n);
}

QString CustomFETString::number(double x)
{
	QString tmp=QString::number(x, 'f', CUSTOM_DOUBLE_PRECISION);
	
	//remove trailing zeroes AFTER the decimal points
	if(tmp.contains('.')){
		int n=tmp.length()-1;
		int del=0;
		while(tmp.at(n)=='0'){
			n--;
			del++;
		}
		if(tmp.at(n)=='.'){
			n--;
			del++;
		}
		tmp.chop(del);
	}

	return tmp;
}

QString CustomFETString::numberPlusTwoDigitsPrecision(double x)
{
	QString tmp=QString::number(x, 'f', CUSTOM_DOUBLE_PRECISION+2);
	
	//remove trailing zeroes AFTER the decimal points
	if(tmp.contains('.')){
		int n=tmp.length()-1;
		int del=0;
		while(tmp.at(n)=='0'){
			n--;
			del++;
		}
		if(tmp.at(n)=='.'){
			n--;
			del++;
		}
		tmp.chop(del);
	}

	return tmp;
}

double customFETStrToDouble(const QString& str, bool* ok)
{
	QLocale c(QLocale::C);

	//tricks to convert numbers like 97.123456789 to 97.123457, to CUSTOM_DOUBLE_PRECISION (6) decimal digits after the decimal point
	double tmpd=c.toDouble(str, ok);
	if(ok!=0)
		if((*ok)==false)
			return tmpd;
	QString tmps=CustomFETString::number(tmpd);
	return c.toDouble(tmps, ok);
}
///////end tricks

bool BEEP_AT_END_OF_GENERATION=true;
bool ENABLE_COMMAND_AT_END_OF_GENERATION=false;
QString commandAtEndOfGeneration=QString("");

bool BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE=false;
bool ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE=false;
QString commandAtEndOfGenerationEachTimetable=QString("");

//bool DETACHED_NOTIFICATION=false;
//int terminateCommandAfterSeconds=0;
//int killCommandAfterSeconds=0;

/*
int XX;
int YY;
int ZZ;

//random routines
void initRandomKnuth()
{
	assert(MM==2147483647);
	assert(AA==48271);
	assert(QQ==44488);
	assert(RR==3399);
	
	assert(MMM==2147483399);
	assert(MMM==MM-248);
	assert(AAA==40692);
	assert(QQQ==52774);
	assert(RRR==3791);
	
	//a few tests
	XX=123; YY=123;
	int tttt=randomKnuth1MM1();
	assert(XX==5937333);
	assert(YY==5005116);
	assert(tttt==932217);

	XX=4321; YY=54321;
	tttt=randomKnuth1MM1();
	assert(XX==208578991);
	assert(YY==62946733);
	assert(tttt==145632258);

	XX=87654321; YY=987654321;
	tttt=randomKnuth1MM1();
	assert(XX==618944401);
	assert(YY==1625301246);
	assert(tttt==1141126801);

	XX=1; YY=1;
	tttt=randomKnuth1MM1();
	assert(XX==48271);
	assert(YY==40692);
	assert(tttt==7579);

	XX=MM-1; YY=MMM-1;
	tttt=randomKnuth1MM1();
	assert(XX==2147435376);
	assert(YY==2147442707);
	assert(tttt==2147476315);

	XX=100; YY=1000;
	tttt=randomKnuth1MM1();
	assert(XX==4827100);
	assert(YY==40692000);
	assert(tttt==2111618746);
	//////////
	
	//unsigned tt=unsigned(time(nullptr));
	qint64 tt=qint64(time(nullptr));
	
	//XX is the current time
	//XX = 1 + ( (unsigned(tt)) % (unsigned(MM-1)) );
	XX = 1 + int( tt%(qint64(MM-1)) );
	assert(XX>0);
	assert(XX<MM);

	//YY is the next random, after initializing YY with the current time
	//YY = 1 + ( (unsigned(tt)) % (unsigned(MMM-1)) );
	YY = 1 + int( tt%(qint64(MMM-1)) );
	assert(YY>0);
	assert(YY<MMM);
	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;
	assert(YY>0);
	assert(YY<MMM);
	
	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).
}

int randomKnuth1MM1()
{
	assert(XX>0);
	assert(XX<MM);

	XX=AA*(XX%QQ)-RR*(XX/QQ);
	if(XX<0)
		XX+=MM;

	assert(XX>0);
	assert(XX<MM);

	assert(YY>0);
	assert(YY<MMM);

	YY=AAA*(YY%QQQ)-RRR*(YY/QQQ);
	if(YY<0)
		YY+=MMM;
	
	assert(YY>0);
	assert(YY<MMM);

	ZZ=XX-YY;
	if(ZZ<=0)
		ZZ+=MM-1; //-1 is not written in Knuth TAOCP vol. 2 third edition; I think it would be an improvement. (Later edit: yes, the author confirmed that).
	assert(ZZ>0);
	assert(ZZ<MM); //again, modified from Knuth TAOCP vol. 2 third edition, ZZ is strictly lower than MM (the author confirmed that, too).
	
	return ZZ;
}

int randomKnuth(int k)
{
	//like in Knuth TAOCP vol.2, reject some numbers (very few), so that the distribution is perfectly uniform
	for(;;){
		int U=randomKnuth1MM1();
		if( U <= k * ((MM-1)/k) )
			return U%k;
	}
}
*/

//MRG32k3a rng;

const qint64 MRG32k3a::m1 = Q_INT64_C(4294967087);
const qint64 MRG32k3a::m2 = Q_INT64_C(4294944443);
const qint64 MRG32k3a::a12 = Q_INT64_C(1403580);
const qint64 MRG32k3a::a13n = Q_INT64_C(810728);
const qint64 MRG32k3a::a21 = Q_INT64_C(527612);
const qint64 MRG32k3a::a23n = Q_INT64_C(1370589);

MRG32k3a::MRG32k3a()
{
	//not permitted values, so we check that we don't forget to init this RNG in another place.
	s10=s11=s12=0;
	s20=s21=s22=0;
}

MRG32k3a::~MRG32k3a()
{
}

void MRG32k3a::initializeMRG32k3a(qint64 _s10, qint64 _s11, qint64 _s12,
	qint64 _s20, qint64 _s21, qint64 _s22)
{
	assert(m1==Q_INT64_C(4294967296)-Q_INT64_C(209));
	assert(m1==Q_INT64_C(4294967087));
	
	assert(m2==Q_INT64_C(4294967296)-Q_INT64_C(22853));
	assert(m2==Q_INT64_C(4294944443));
	
	assert(a12==Q_INT64_C(1403580));
	assert(a13n==Q_INT64_C(810728));
	
	assert(a21==Q_INT64_C(527612));
	assert(a23n==Q_INT64_C(1370589));

	assert(_s10>=0);
	assert(_s11>=0);
	assert(_s12>=0);

	assert(_s20>=0);
	assert(_s21>=0);
	assert(_s22>=0);

	assert(_s10 < m1);
	assert(_s11 < m1);
	assert(_s12 < m1);

	assert(_s20 < m2);
	assert(_s21 < m2);
	assert(_s22 < m2);

	assert(_s10>0 || _s11>0 || _s12>0);
	assert(_s20>0 || _s21>0 || _s22>0);
	
	s10=_s10;
	s11=_s11;
	s12=_s12;

	s20=_s20;
	s21=_s21;
	s22=_s22;
}

void MRG32k3a::initializeMRG32k3a()
{
	qint64 _s10, _s11, _s12, _s20, _s21, _s22;

	//qint64 tt=qint64(time(nullptr));
	
	std::chrono::seconds s=std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
	qint64 si=static_cast<qint64>(s.count());
	//cout<<"si=="<<si<<endl;
	assert(si>=0);
	_s10=si%m1;
	if(_s10==0) //just in case :-)  . It could even be allowed to be 0, but then we wouldn't have a certain guarantee that not all seeds in component 1 are 0.
		_s10=1;

	_s20=si%m2;
	if(_s20==0) //just in case :-)  . It could even be allowed to be 0, but then we wouldn't have a certain guarantee that not all seeds in component 2 are 0.
		_s20=1;

	/*_s10 = 1 + tt%(m1-1);
	assert(_s10>=1);
	assert(_s10<m1);

	_s20 = 1 + tt%(m2-1);
	assert(_s20>=1);
	assert(_s20<m2);*/
	
	//Using ideas and code from https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
	//and https://stackoverflow.com/questions/31255486/c-how-do-i-convert-a-stdchronotime-point-to-long-and-back
	//and https://stackoverflow.com/questions/18022927/convert-high-resolution-clock-time-into-an-integer-chrono/18023064
	std::chrono::nanoseconds ns1=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	qint64 nsi1=static_cast<qint64>(ns1.count());
	//cout<<"nsi1=="<<nsi1<<endl;
	assert(nsi1>=0);

	_s11=nsi1%1000000000;

	std::chrono::nanoseconds ns2=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	qint64 nsi2=static_cast<qint64>(ns2.count());
	//cout<<"nsi2=="<<nsi2<<endl;
	assert(nsi2>=0);

	_s21=nsi2%1000000000;
	
	_s12=0; //We could try other better methods, but they need to be portable.
	_s22=0;
	
	initializeMRG32k3a(_s10, _s11, _s12, _s20, _s21, _s22);
}

unsigned int MRG32k3a::uiMRG32k3a()
{
	assert(s10>0 || s11>0 || s12>0);
	assert(s20>0 || s21>0 || s22>0);

	qint64 p, p1, p2;

	/* Component 1 */
	p1 = a12*s11 - a13n*s10;
	p1%=m1;
	if(p1<0)
		p1+=m1;
	assert(p1>=0 && p1<m1);
	s10 = s11;
	s11 = s12;
	s12 = p1;

	/* Component 2 */
	p2 = a21*s22 - a23n*s20;
	p2%=m2;
	if(p2<0)
		p2+=m2;
	assert(p2>=0 && p2<m2);
	s20 = s21;
	s21 = s22;
	s22 = p2;
	
	/* Combination */
	p=p1-p2;
	if(p<0)
		p+=m1;
	assert(p>=0 && p<m1);

	return (unsigned int)(p);
}

/*double MRG32k3a::dMRG32k3a()
{
	double p=double(uiMRG32k3a())/double(m1);

	assert(p>=0.0);
	assert(p<1.0);

	return p;
}*/

int MRG32k3a::intMRG32k3a(int k)
{
	qint64 q=(qint64(uiMRG32k3a())*qint64(k))/m1;
	assert(q<qint64(k));
	int r=int(q);
	
	return r;
}
