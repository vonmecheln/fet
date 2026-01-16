/*
File export.cpp
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          export.cpp  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

// Code contributed by Volker Dirr ( https://www.timetabling.de/ )

//TODO: protect export strings. textquote must be doubled
//TODO: count skipped min days between activities constraints?
//TODO: add cancel button

#include <Qt>
#include <QtGlobal>
#include <QFile>
#include <QFileDevice>
#include <QDir>

#ifndef FET_COMMAND_LINE
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#endif

#include <QHash>
#include <QSet>
#include <QMap>

class QWidget;
void centerWidgetOnScreen(QWidget* widget);

#include "timetable_defs.h"		//needed, because of QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
#include "export.h"
#include "solution.h"

#include "messageboxes.h"

extern Timetable gt;
extern Solution best_solution;
extern bool teachers_schedule_ready;
extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;

const char CSVActivities[]="activities.csv";
const char CSVActivitiesStatistics[]="statistics_activities.csv";
const char CSVActivityTags[]="activity_tags.csv";
const char CSVRoomsAndBuildings[]="rooms_and_buildings.csv";
const char CSVSubjects[]="subjects.csv";
const char CSVTeachers[]="teachers.csv";
const char CSVStudents[]="students.csv";
const char CSVTimetable[]="timetable.csv";
QString DIRECTORY_CSV;
QString PREFIX_CSV;

#ifdef FET_COMMAND_LINE
bool EXPORT_CSV=false;

bool EXPORT_ALLOW_OVERWRITE=false;

bool EXPORT_FIRST_LINE_IS_HEADING=true;

extern const int EXPORT_DOUBLE_QUOTES; //trick so that these constants are visible in fet.cpp.
extern const int EXPORT_SINGLE_QUOTES;
extern const int EXPORT_NO_QUOTES;
const int EXPORT_DOUBLE_QUOTES=0;
const int EXPORT_SINGLE_QUOTES=1;
const int EXPORT_NO_QUOTES=2;
int EXPORT_QUOTES=EXPORT_DOUBLE_QUOTES;

extern const int EXPORT_COMMA;
extern const int EXPORT_SEMICOLON;
extern const int EXPORT_VERTICALBAR;
const int EXPORT_COMMA=0;
const int EXPORT_SEMICOLON=1;
const int EXPORT_VERTICALBAR=2;
int EXPORT_FIELD_SEPARATOR=EXPORT_COMMA;

#include <iostream>
using namespace std;
#endif

Export::Export()
{
}

Export::~Export()
{
}

#ifndef FET_COMMAND_LINE
bool Export::okToWrite(QWidget* parent, const QString& file, QMessageBox::StandardButton& msgBoxButton)
{
	if(QFile::exists(file)){
		if(msgBoxButton==QMessageBox::YesToAll){
			return true;
		}
		else if(msgBoxButton==QMessageBox::NoToAll){
			return false;
		}
		else if(msgBoxButton==QMessageBox::No ||
		 msgBoxButton==QMessageBox::Yes ||
		 msgBoxButton==QMessageBox::NoButton){
			/*QMessageBox msgBox(parent);
			msgBox.setWindowTitle(tr("FET warning"));
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setText(tr("File %1 exists - are you sure you want to overwrite existing file?").arg(file));
			msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::YesToAll|QMessageBox::NoToAll);
			msgBox.setDefaultButton(QMessageBox::Yes);
			
			msgBoxButton=(QMessageBox::StandardButton)(msgBox.exec());
			*/
			
			msgBoxButton=QMessageBox::warning(parent, tr("FET warning"),
			 tr("File %1 exists - are you sure you want to overwrite existing file?").arg(file),
			 QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
			 QMessageBox::Yes);
			
			if(msgBoxButton==QMessageBox::No || msgBoxButton==QMessageBox::NoToAll)
				return false;
			else
				return true;
		}
		else{
			assert(0);
			return false;
		}
	}
	else
		return true;
}
#else
bool Export::okToWrite(const QString& file)
{
	if(QFile::exists(file))
		return EXPORT_ALLOW_OVERWRITE;
	else
		return true;
}
#endif

#ifndef FET_COMMAND_LINE
void Export::exportCSV(QWidget* parent){
	QString fieldSeparator=",";
	QString textquote="\"";
	QString componentSeparator="|";
	QString setSeparator="+";
	bool head=true;
	bool ok=true;

	DIRECTORY_CSV=OUTPUT_DIR+FILE_SEP+"csv";

	QString s2;
	if(INPUT_FILENAME_XML=="")
		s2="unnamed";
	else{
		s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

		if(s2.right(4)==".fet")
			s2=s2.left(s2.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	DIRECTORY_CSV.append(FILE_SEP);
	DIRECTORY_CSV.append(s2);

	PREFIX_CSV=DIRECTORY_CSV+FILE_SEP;

	QDir dir;
	if(!dir.exists(OUTPUT_DIR))
		dir.mkpath(OUTPUT_DIR);
	if(!dir.exists(DIRECTORY_CSV))
		dir.mkpath(DIRECTORY_CSV);

	QDialog* newParent;
	ok=selectSeparatorAndTextQuote(parent, newParent, textquote, fieldSeparator, head);
	
	QString lastWarnings;
	if(!ok){
		lastWarnings.insert(0,Export::tr("Export aborted")+"\n");
	} else {
		bool okat, okr, oks, okt, okst, okact, okacts, oktim;

		QMessageBox::StandardButton msgBoxButton=QMessageBox::NoButton;

		okat=exportCSVActivityTags(newParent, lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator, msgBoxButton);
		okr=exportCSVRoomsAndBuildings(newParent, lastWarnings, textquote, fieldSeparator, head, msgBoxButton);
		oks=exportCSVSubjects(newParent, lastWarnings, textquote, fieldSeparator, head, componentSeparator, msgBoxButton);
		okt=exportCSVTeachers(newParent, lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator, msgBoxButton);
		okst=exportCSVStudents(newParent, lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator, msgBoxButton);
		okact=exportCSVActivities(newParent, lastWarnings, textquote, fieldSeparator, head, msgBoxButton);
		okacts=exportCSVActivitiesStatistics(newParent, lastWarnings, textquote, fieldSeparator, head, msgBoxButton);
		oktim=exportCSVTimetable(newParent, lastWarnings, textquote, fieldSeparator, head, msgBoxButton);
		
		ok=okat && okr && oks && okt && okst && okact && okacts && oktim;
		
		lastWarnings.insert(0,Export::tr("CSV files were exported to directory %1.").arg(QDir::toNativeSeparators(DIRECTORY_CSV))+"\n");
		if(ok)
			lastWarnings.insert(0,Export::tr("Export complete")+"\n");
		else
			lastWarnings.insert(0,Export::tr("Export incomplete")+"\n");
	}

	LastWarningsDialogE lwd(newParent, lastWarnings);
	int w=lwd.sizeHint().width();
	int h=lwd.sizeHint().height();
	lwd.resize(w,h);
	centerWidgetOnScreen(&lwd);
	
	ok=lwd.exec();
}
#else
void Export::exportCSV(Solution* bestOrHighest, Solution* current){
	if(!EXPORT_CSV)
		return;

	QString fieldSeparator;
	if(EXPORT_FIELD_SEPARATOR==EXPORT_COMMA)
		fieldSeparator=",";
	else if(EXPORT_FIELD_SEPARATOR==EXPORT_SEMICOLON)
		fieldSeparator=";";
	else if(EXPORT_FIELD_SEPARATOR==EXPORT_VERTICALBAR)
		fieldSeparator="|";
	else
		assert(0);
	
	QString textquote;
	if(EXPORT_QUOTES==EXPORT_DOUBLE_QUOTES)
		textquote="\"";
	else if(EXPORT_QUOTES==EXPORT_SINGLE_QUOTES)
		textquote="'";
	else if(EXPORT_QUOTES==EXPORT_NO_QUOTES)
		textquote="";
	else
		assert(0);

	QString componentSeparator="|";
	QString setSeparator="+";
	bool head=EXPORT_FIRST_LINE_IS_HEADING;
	bool ok=true;

	if(OUTPUT_DIR!="")
		DIRECTORY_CSV=OUTPUT_DIR+FILE_SEP+"csv";
	else
		DIRECTORY_CSV="csv";

	QString s2;
	if(INPUT_FILENAME_XML=="")
		s2="unnamed";
	else{
		s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

		if(s2.right(4)==".fet")
			s2=s2.left(s2.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	DIRECTORY_CSV.append(FILE_SEP);
	DIRECTORY_CSV.append(s2);

	PREFIX_CSV=DIRECTORY_CSV+FILE_SEP;

	QDir dir;
	if(OUTPUT_DIR!="")
		if(!dir.exists(OUTPUT_DIR))
			dir.mkpath(OUTPUT_DIR);
	if(!dir.exists(DIRECTORY_CSV))
		dir.mkpath(DIRECTORY_CSV);

	QString lastWarnings;

	bool okat, okr, oks, okt, okst, okact, okacts, oktim1, oktim2;

	okat=exportCSVActivityTags(lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator);
	okr=exportCSVRoomsAndBuildings(lastWarnings, textquote, fieldSeparator, head);
	oks=exportCSVSubjects(lastWarnings, textquote, fieldSeparator, head, componentSeparator);
	okt=exportCSVTeachers(lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator);
	okst=exportCSVStudents(lastWarnings, textquote, fieldSeparator, head, componentSeparator, setSeparator);
	okact=exportCSVActivities(lastWarnings, textquote, fieldSeparator, head);
	okacts=exportCSVActivitiesStatistics(lastWarnings, textquote, fieldSeparator, head);
	
	if(current==nullptr){
		best_solution.copy(gt.rules, *bestOrHighest);
		lastWarnings.append(Export::tr("Successfully finished timetable:"));
		lastWarnings.append(" ");
		oktim1=exportCSVTimetable(lastWarnings, textquote, fieldSeparator, head);

		oktim2=true;
	}
	else{
		QString oldName=DIRECTORY_CSV;
		DIRECTORY_CSV.append("-highest");
		PREFIX_CSV=DIRECTORY_CSV+FILE_SEP;
		if(!dir.exists(DIRECTORY_CSV))
			dir.mkpath(DIRECTORY_CSV);
		best_solution.copy(gt.rules, *bestOrHighest);
		lastWarnings.append(Export::tr("Highest stage timetable:"));
		lastWarnings.append(" ");
		oktim1=exportCSVTimetable(lastWarnings, textquote, fieldSeparator, head);
		
		DIRECTORY_CSV=oldName;
		DIRECTORY_CSV.append("-current");
		PREFIX_CSV=DIRECTORY_CSV+FILE_SEP;
		if(!dir.exists(DIRECTORY_CSV))
			dir.mkpath(DIRECTORY_CSV);
		best_solution.copy(gt.rules, *current);
		lastWarnings.append(Export::tr("Current stage timetable:"));
		lastWarnings.append(" ");
		oktim2=exportCSVTimetable(lastWarnings, textquote, fieldSeparator, head);

		DIRECTORY_CSV=oldName;
		PREFIX_CSV=DIRECTORY_CSV+FILE_SEP;
	}
	
	ok=okat && okr && oks && okt && okst && okact && okacts && oktim1 && oktim2;
	
	lastWarnings.insert(0,Export::tr("CSV files were exported to directory %1.").arg(QDir::toNativeSeparators(DIRECTORY_CSV))+"\n");
	if(ok)
		lastWarnings.insert(0,Export::tr("Export complete")+"\n");
	else
		lastWarnings.insert(0,Export::tr("Export incomplete")+"\n");
		
	cout<<qPrintable(tr("FET - export comments", "The comments of the exporting operation"))<<endl;
	cout<<qPrintable(lastWarnings); //no endl here - there is one already
}
#endif

QString Export::protectCSV(const QString& str){
	QString p=str;
	p.replace("\"", "\"\"");
	return p;
}

QString Export::protectCSVComments(const QString& str){
	QString p=str;
	p.replace("\"", "\"\"");
	p.replace("\n", " ");
	return p;
}

bool Export::checkSetSeparator(const QString& str, const QString& setSeparator){
	if(str.contains(setSeparator))
		return false;
	return true;
}

bool Export::checkComponentSeparator(const QString& str, const QString& componentSeparator){
	if(str.contains(componentSeparator))
		return false;
	return true;
}

bool Export::isActivityNotManuallyEditedPart1(int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents,
		QString& tl, QString& sl, QString& atl, QString& stl){ //similar to ActivitiesForm::modifyActivity() by Liviu Lalescu, but added diffActive
	diffTeachers=diffSubject=diffActivityTags=diffStudents=false;

	assert(activityIndex>=0);
	assert(activityIndex<gt.rules.activitiesList.size());

	Activity* act=gt.rules.activitiesList[activityIndex];
	assert(act!=nullptr);
	
	QStringList teachers=act->teachersNames;
	QString subject=act->subjectName;
	QStringList activityTags=act->activityTagsNames;
	QStringList students=act->studentsNames;
	
	if(act->isSplit()){
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(teachers!=act2->teachersNames){
					//return false;
					diffTeachers=true;
				}
				if(subject!=act2->subjectName){
					//return false;
					diffSubject=true;
				}
				if(activityTags!=act2->activityTagsNames){
					diffActivityTags=true;
					//return false;
				}
				if(students!=act2->studentsNames){
					diffStudents=true;
					//return false;
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	/*
	if(!diffTeachers && !diffSubject && !diffActivityTags && !diffStudents){
		//students
		stl="";
		for(int s=0; s<act->studentsNames.size(); s++){
			if(s!=0)
				stl+="+";
			stl+=protectCSV(act->studentsNames[s]);
		}
		
		//subject
		sl="";
		sl+=protectCSV(act->subjectName);
		
		//teachers
		tl="";
		for(int t=0; t<act->teachersNames.size(); t++){
			if(t!=0)
				tl+="+";
			tl+=protectCSV(act->teachersNames[t]);
		}
		
		//activity tags
		atl="";
		for(int s=0; s<act->activityTagsNames.size(); s++){
			if(s!=0)
				atl+="+";
			atl+=protectCSV(act->activityTagsNames[s]);
		}
		
		return true;
	}
	else{
		stl="";
		sl="";
		tl="";
		atl="";
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(i!=activityIndex){
					stl+="|";
					sl+="|";
					tl+="|";
					atl+="|";
				}
				
				//students
				for(int s=0; s<act2->studentsNames.size(); s++){
					if(s!=0)
						stl+="+";
					stl+=protectCSV(act2->studentsNames[s]);
				}
				
				//subject
				sl+=protectCSV(act2->subjectName);
				
				//teachers
				for(int t=0; t<act2->teachersNames.size(); t++){
					if(t!=0)
						tl+="+";
					tl+=protectCSV(act2->teachersNames[t]);
				}
				
				//activity tags
				for(int s=0; s<act2->activityTagsNames.size(); s++){
					if(s!=0)
						atl+="+";
					atl+=protectCSV(act2->activityTagsNames[s]);
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
		
		return false;
	}
	*/
	
	//students
	stl="";
	if(!diffStudents){
		for(int s=0; s<act->studentsNames.size(); s++){
			if(s!=0)
				stl+="+";
			stl+=protectCSV(act->studentsNames[s]);
		}
	}
	else{
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(i!=activityIndex){
					stl+="|";
				}
				
				for(int s=0; s<act2->studentsNames.size(); s++){
					if(s!=0)
						stl+="+";
					stl+=protectCSV(act2->studentsNames[s]);
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	
	//subject
	sl="";
	if(!diffSubject){
		sl+=protectCSV(act->subjectName);
	}
	else{
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(i!=activityIndex){
					sl+="|";
				}
				
				sl+=protectCSV(act2->subjectName);
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	
	//teachers
	tl="";
	if(!diffTeachers){
		for(int t=0; t<act->teachersNames.size(); t++){
			if(t!=0)
				tl+="+";
			tl+=protectCSV(act->teachersNames[t]);
		}
	}
	else{
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(i!=activityIndex){
					tl+="|";
				}
				
				for(int t=0; t<act2->teachersNames.size(); t++){
					if(t!=0)
						tl+="+";
					tl+=protectCSV(act2->teachersNames[t]);
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	
		//activity tags
	atl="";
	if(!diffActivityTags){
		for(int s=0; s<act->activityTagsNames.size(); s++){
			if(s!=0)
				atl+="+";
			atl+=protectCSV(act->activityTagsNames[s]);
		}
	}
	else{
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if(i!=activityIndex){
					atl+="|";
				}
				
				for(int s=0; s<act2->activityTagsNames.size(); s++){
					if(s!=0)
						atl+="+";
					atl+=protectCSV(act2->activityTagsNames[s]);
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	
	if(!diffTeachers && !diffSubject && !diffActivityTags && !diffStudents)
		return true;
	else
		return false;
}

bool Export::isActivityNotManuallyEditedPart2(int activityIndex, bool& diffCompNStud, bool& diffNStud, bool& diffActive){ //similar to ActivitiesForm::modifyActivity() by Liviu Lalescu, but added diffActive
	diffCompNStud=diffNStud=diffActive=false;

	assert(activityIndex>=0);
	assert(activityIndex<gt.rules.activitiesList.size());

	Activity* act=gt.rules.activitiesList[activityIndex];
	assert(act!=nullptr);
	
	int nTotalStudents=act->nTotalStudents;
	
	bool computeNTotalStudents=act->computeNTotalStudents;
	bool active=act->active;

	if(act->isSplit()){
		for(int i=activityIndex; i<gt.rules.activitiesList.size(); i++){	//Probably old comment: possible speed improvement: not i=0. do i=act->activityGroupId
			Activity* act2=gt.rules.activitiesList[i];			//Probably old comment: possible speed improvement: if(act2->activityGroupId changed) break;
			if(act2->activityGroupId!=0 && act2->activityGroupId==act->activityGroupId){
				if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ nTotalStudents!=act2->nTotalStudents){
					diffNStud=true;
					//return false;
				}
				if(computeNTotalStudents!=act2->computeNTotalStudents){
					diffCompNStud=true;
					//return false;
				}
				if(active!=act2->active){
					diffActive=true;
					//return false;	
				}
			}
			else
				i=gt.rules.activitiesList.size();
		}
	}
	if(!diffCompNStud && !diffNStud && !diffActive)
		return true;
	else
		return false;
}

#ifndef FET_COMMAND_LINE
bool Export::selectSeparatorAndTextQuote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head){
	assert(gt.rules.initialized);

	newParent=(QDialog*)parent;

	QStringList separators;
	QStringList textquotes;
	separators<<","<<";"<<"|";
	//textquotes<<"\""<<"'"<<Export::tr("no text quote", "The translated field must contain at least 2 characters (normally it should), otherwise the export filter does not work");
	const QString NO_TEXTQUOTE_TRANSLATED=Export::tr("no text quote", "Please use at least 2 characters for the translation of this field, so that the program works OK");
	textquotes<<"\""<<"'"<<NO_TEXTQUOTE_TRANSLATED;
	const int NO_TEXTQUOTE_POS=2; //if you modify line above, modify also this variable to be the position of the no textquote (starts from 0)
	//also, if you add textquotes longer than one character, take care of line 309 (later in the same function) (assert textquote.size()==1)
	//it is permitted for position NO_TEXTQUOTE_POS to have a string longer than 1 QChar
	
	/*if(textquotes[2].size()<=1){
		QMessageBox::warning(parent, tr("FET warning"), tr("Translation is wrong, because translation of 'no text quote' is too short - falling back to English words. Please report bug"));
		textquote=QString("no text quote");
	}
	assert(textquotes[2].size()>1);*/
	
	const QString settingsName=QString("ExportSelectSeparatorsDialog");

	newParent=new QDialog(parent);
	QDialog& separatorsDialog=(*newParent);
	
	separatorsDialog.setWindowTitle(tr("FET question"));
	QVBoxLayout* separatorsMainLayout=new QVBoxLayout(&separatorsDialog);

	QHBoxLayout* top=new QHBoxLayout();
	QLabel* topText=new QLabel();
	topText->setText(Export::tr("Please keep the default settings.\nImport of data will be easier with these settings."));
	top->addWidget(topText);

	QGroupBox* separatorsGroupBox = new QGroupBox(Export::tr("Please specify the separator between fields:"));
	QComboBox* separatorsCB=nullptr;
	if(separators.size()>1){
		QHBoxLayout* separatorBoxChoose=new QHBoxLayout();
		separatorsCB=new QComboBox();

		QLabel* separatorTextChoose=new QLabel();
		separatorTextChoose->setText(Export::tr("Use field separator:"));
		separatorsCB->insertItems(0,separators);
		separatorBoxChoose->addWidget(separatorTextChoose);
		separatorBoxChoose->addWidget(separatorsCB);
		separatorsGroupBox->setLayout(separatorBoxChoose);
	}

	QGroupBox* textquoteGroupBox = new QGroupBox(Export::tr("Please specify the text quote of text fields:"));
	QComboBox* textquoteCB=nullptr;
	if(textquotes.size()>1){
		QHBoxLayout* textquoteBoxChoose=new QHBoxLayout();
		textquoteCB=new QComboBox();
		
		QLabel* textquoteTextChoose=new QLabel();
		textquoteTextChoose->setText(Export::tr("Use textquote:"));
		textquoteCB->insertItems(0,textquotes);
		textquoteBoxChoose->addWidget(textquoteTextChoose);
		textquoteBoxChoose->addWidget(textquoteCB);
		textquoteGroupBox->setLayout(textquoteBoxChoose);
	}

	QGroupBox* firstLineGroupBox = new QGroupBox(Export::tr("Please specify the contents of the first line:"));
	QVBoxLayout* firstLineChooseBox=new QVBoxLayout();
	QRadioButton* firstLineRadio1 = new QRadioButton(Export::tr("The first line is the heading."));
	QRadioButton* firstLineRadio2 = new QRadioButton(Export::tr("The first line contains data. Don't export heading."));
	firstLineRadio1->setChecked(true);
	firstLineChooseBox->addWidget(firstLineRadio1);
	firstLineChooseBox->addWidget(firstLineRadio2);
	firstLineGroupBox->setLayout(firstLineChooseBox);

	QPushButton* pb=new QPushButton(tr("OK"));
	QPushButton* cancelpb=new QPushButton(tr("Cancel"));
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb);
	hl->addWidget(cancelpb);
	
	separatorsMainLayout->addLayout(top);
	separatorsMainLayout->addWidget(separatorsGroupBox);
	separatorsMainLayout->addWidget(textquoteGroupBox);

	separatorsMainLayout->addWidget(firstLineGroupBox);
	separatorsMainLayout->addLayout(hl);

	pb->setDefault(true);
	pb->setFocus();
	
	connect(pb, &QPushButton::clicked, &separatorsDialog, &QDialog::accept);
	connect(cancelpb, &QPushButton::clicked, &separatorsDialog, &QDialog::reject);
	
	int w=separatorsDialog.sizeHint().width();
	int h=separatorsDialog.sizeHint().height();
	separatorsDialog.resize(w,h);
	
	centerWidgetOnScreen(&separatorsDialog);
	restoreFETDialogGeometry(&separatorsDialog, settingsName);

	int ok=separatorsDialog.exec();
	saveFETDialogGeometry(&separatorsDialog, settingsName);
	if(ok!=QDialog::Accepted) return false;

	// TODO: if is always true. maybe clean source (also 2 previous if)
	if(separators.size()>1){
		assert(separatorsCB!=nullptr);
		assert(textquoteCB!=nullptr);
		fieldSeparator=separatorsCB->currentText();
		textquote=textquoteCB->currentText();
		if(textquoteCB->currentIndex()==NO_TEXTQUOTE_POS){
			assert(textquote==NO_TEXTQUOTE_TRANSLATED);
			textquote=QString("no text quote"); //must have length >= 2
		}
		else{
			assert(textquote.size()==1);
			//assert(textquote=="\"" || textquote=="'");
		}
	}
	else{
		assert(separatorsCB==nullptr);
		assert(textquoteCB==nullptr);
		fieldSeparator="";
		textquote="";
	}

	if(textquote.size()!=1)
		textquote="";

	if(firstLineRadio1->isChecked())
		head=true;
	else head=false;
	return true;
}
#endif

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivityTags(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVActivityTags(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVActivityTags;
	
#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);

	if(head)
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<"Activity Tag"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<"Activity Tag"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif

	for(ActivityTag* a : std::as_const(gt.rules.activityTagsList)){
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<protectCSV(a->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(a->comments)<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<protectCSV(a->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(a->comments)<<textquote<<endl;
#endif
		if(!checkComponentSeparator(a->name, componentSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is an activity tag's name").arg(a->name)+"\n";
		if(!checkSetSeparator(a->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.", "%1 is an activity tag's name").arg(a->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 activity tags exported.").arg(gt.rules.activityTagsList.size())+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVRoomsAndBuildings(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVRoomsAndBuildings(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head){
#endif

	bool usingVirtualRooms=false;

	for(Room* r : std::as_const(gt.rules.roomsList))
		if(r->isVirtual==true){
			usingVirtualRooms=true;
			break;
		}

	if(usingVirtualRooms)
#ifndef FET_COMMAND_LINE
		TimetableExportMessage::warning(parent, tr("FET information"), tr("Your data contains virtual rooms. These virtual rooms will be exported as normal/real rooms."
		 " The list of sets of real rooms of the virtual rooms will not be exported."));
#else
		TimetableExportMessage::warning(nullptr, tr("FET information"), tr("Your data contains virtual rooms. These virtual rooms will be exported as normal/real rooms."
		 " The list of sets of real rooms of the virtual rooms will not be exported."));
#endif
	
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVRoomsAndBuildings;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
		tosExport	<<textquote<<"Room"<<textquote<<fieldSeparator
				<<textquote<<"Room Capacity"<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				<<textquote<<"Building"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
				<<textquote<<"Building"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif
				
	QStringList checkBuildings;
	for(Room* r : std::as_const(gt.rules.roomsList)){
		tosExport	<<textquote<<protectCSV(r->name)<<textquote<<fieldSeparator
				<<CustomFETString::number(r->capacity)<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				<<textquote<<protectCSV(r->building)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(r->comments)<<textquote<<Qt::endl;
#else
				<<textquote<<protectCSV(r->building)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(r->comments)<<textquote<<endl;
#endif
		if(!checkBuildings.contains(r->building)&&!r->building.isEmpty())
			checkBuildings<<r->building;
	}
	
	lastWarnings+=Export::tr("%1 rooms (with buildings) exported.").arg(gt.rules.roomsList.size())+"\n";
	if(gt.rules.buildingsList.size()!=checkBuildings.size()){
		lastWarnings+=Export::tr("Warning! Only %1 of %2 building names are exported, because %3 buildings don't contain any room.").arg(checkBuildings.size()).arg(gt.rules.buildingsList.size()).arg(gt.rules.buildingsList.size()-checkBuildings.size())+"\n";
	}
	
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+QString("\n");
		return false;
	}
	fileExport.close();
	
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVSubjects(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVSubjects(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVSubjects;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<"Subject"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<"Subject"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif

	for(Subject* s : std::as_const(gt.rules.subjectsList)){
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<protectCSV(s->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(s->comments)<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<protectCSV(s->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(s->comments)<<textquote<<endl;
#endif
		if(!checkComponentSeparator(s->name, componentSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is a subject's name").arg(s->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 subjects exported.").arg(gt.rules.subjectsList.size())+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVTeachers(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVTeachers(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVTeachers;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<"Teacher"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<"Teacher"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif

	for(Teacher* t : std::as_const(gt.rules.teachersList)){
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<textquote<<protectCSV(t->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(t->comments)<<textquote<<Qt::endl;
#else
		tosExport<<textquote<<protectCSV(t->name)<<textquote<<fieldSeparator<<textquote<<protectCSVComments(t->comments)<<textquote<<endl;
#endif
		if(!checkComponentSeparator(t->name, componentSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is a teacher's name").arg(t->name)+"\n";
		if(!checkSetSeparator(t->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.", "%1 is a teacher's name").arg(t->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 teachers exported.").arg(gt.rules.teachersList.size())+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVStudents(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVStudents(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVStudents;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
		tosExport	<<textquote<<"Year"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Year"<<textquote<<fieldSeparator
				<<textquote<<"Group"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Group"<<textquote<<fieldSeparator
				<<textquote<<"Subgroup"<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				<<textquote<<"Number of Students per Subgroup"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
				<<textquote<<"Number of Students per Subgroup"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif

	int ig=0;
	int is=0;
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
		tosExport<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<textquote<<protectCSVComments(sty->comments)<<textquote<<Qt::endl;
#else
					<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<textquote<<protectCSVComments(sty->comments)<<textquote<<endl;
#endif
		if(!checkComponentSeparator(sty->name, componentSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is a students year's name").arg(sty->name)+"\n";
		if(!checkSetSeparator(sty->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.", "%1 is a students year's name").arg(sty->name)+"\n";
		for(StudentsGroup* stg : std::as_const(sty->groupsList)){
			ig++;
			tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
					<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator
					<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					<<CustomFETString::number(stg->numberOfStudents)<<fieldSeparator<<fieldSeparator<<fieldSeparator<<textquote<<protectCSVComments(stg->comments)<<textquote<<Qt::endl;
#else
					<<CustomFETString::number(stg->numberOfStudents)<<fieldSeparator<<fieldSeparator<<fieldSeparator<<textquote<<protectCSVComments(stg->comments)<<textquote<<endl;
#endif
			if(!checkComponentSeparator(stg->name, componentSeparator))
				lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is a students group's name").arg(stg->name)+"\n";
			if(!checkSetSeparator(stg->name, setSeparator))
				lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.", "%1 is a students group's name").arg(stg->name)+"\n";
			for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
				is++;
				tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
						<<CustomFETString::number(sty->numberOfStudents)<<fieldSeparator
						<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
						<<CustomFETString::number(stg->numberOfStudents)<<fieldSeparator
						<<textquote<<protectCSV(sts->name)<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
						<<CustomFETString::number(sts->numberOfStudents)<<fieldSeparator<<textquote<<protectCSVComments(sts->comments)<<textquote<<Qt::endl;
#else
						<<CustomFETString::number(sts->numberOfStudents)<<fieldSeparator<<textquote<<protectCSVComments(sts->comments)<<textquote<<endl;
#endif
				if(!checkComponentSeparator(sts->name, componentSeparator))
					lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes component separator |.", "%1 is a students subgroup's name").arg(sts->name)+"\n";
				if(!checkSetSeparator(sts->name, setSeparator))
					lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 includes set separator +.", "%1 is a students subgroup's name").arg(sts->name)+"\n";
			}
		}
	}

	lastWarnings+=Export::tr("%1 years exported.").arg(gt.rules.yearsList.size())+"\n";
	lastWarnings+=Export::tr("%1 groups exported.").arg(ig)+"\n";
	lastWarnings+=Export::tr("%1 subgroups exported.").arg(is)+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+QString("\n");
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivities(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVActivities(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVActivities;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
		tosExport	<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tags"<<textquote<<fieldSeparator
				<<textquote<<"Total Duration"<<textquote<<fieldSeparator
				<<textquote<<"Split Duration"<<textquote<<fieldSeparator
				<<textquote<<"Min Days"<<textquote<<fieldSeparator
				<<textquote<<"Weight"<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				<<textquote<<"Consecutive"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
				<<textquote<<"Consecutive"<<textquote<<fieldSeparator<<textquote<<"Comments"<<textquote<<endl;
#endif

	//code by Liviu Lalescu (begin)
	//better detection of min days constraint
	QHash<int, int> activitiesRepresentant;
	QHash<int, int> activitiesNumberOfSubactivities;
	//QHash<int, ConstraintMinDaysBetweenActivities*> activitiesConstraints;
	QHash<int, TimeConstraint*> activitiesConstraints; //can be ConstraintMinDaysBetweenActivities* or ConstraintMinHalfDaysBetweenActivities*
	
	activitiesRepresentant.clear();
	activitiesNumberOfSubactivities.clear();
	activitiesConstraints.clear();
	
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		assert(!activitiesRepresentant.contains(act->id));
		activitiesRepresentant.insert(act->id, act->activityGroupId); //act->id is key, act->agid is value
	
		if(act->activityGroupId>0){
			int n=activitiesNumberOfSubactivities.value(act->activityGroupId, 0); //0 here means default value
			n++;
			activitiesNumberOfSubactivities.insert(act->activityGroupId, n); //overwrites old value
		}
	}
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES && tc->active){
			ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) tc;
	
			QSet<int> aset;
			int repres=-1;
	
			for(int i=0; i<c->n_activities; i++){
				int aid=c->activitiesIds[i];
				aset.insert(aid);
	
				if(activitiesRepresentant.value(aid,0)==aid)
					repres=aid; //does not matter if there are more representatives in this constraint, the constraint will be skipped anyway in this case
			}
	
			bool oktmp=false;
	
			if(repres>0){
				if(aset.count()==activitiesNumberOfSubactivities.value(repres, 0)){
					oktmp=true;
					for(int aid : std::as_const(aset))
						if(activitiesRepresentant.value(aid, 0)!=repres){
							oktmp=false;
							break;
						}
				}
			}
	
			if(!oktmp){
				lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
				" it does not refer to a whole larger container activity").arg(c->getDescription(gt.rules))+"\n";
			}
	
			if(oktmp){
				TimeConstraint* generic_oldc=activitiesConstraints.value(repres, nullptr);
				if(generic_oldc!=nullptr){
					if(generic_oldc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
						ConstraintMinDaysBetweenActivities* oldc=(ConstraintMinDaysBetweenActivities*)generic_oldc;
						if(oldc->weightPercentage < c->weightPercentage){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with larger weight percentage, referring to the same activities")
								.arg(oldc->getDescription(gt.rules))+"\n";
						}
						else if(oldc->weightPercentage > c->weightPercentage){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with larger weight percentage, referring to the same activities")
								.arg(c->getDescription(gt.rules))+"\n";
						}
		
						//equal weights - choose the lowest number of min days
						else if(oldc->minDays > c->minDays){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and higher number of min days, referring to the same activities")
								.arg(c->getDescription(gt.rules))+"\n";
						}
						else if(oldc->minDays < c->minDays){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and higher number of min days, referring to the same activities")
								.arg(oldc->getDescription(gt.rules))+"\n";
						}
		
						//equal weights and min days - choose the one with consecutive is same day true
						else if(oldc->consecutiveIfSameDay==true){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and same number of min days and"
								" consecutive if on the same day true, referring to the same activities").arg(c->getDescription(gt.rules))+"\n";
						}
						else if(c->consecutiveIfSameDay==true){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and same number of min days and"
								" consecutive if on the same day true, referring to the same activities").arg(oldc->getDescription(gt.rules))+"\n";
						}
					}
					else if(generic_oldc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
							" there exists another constraint of type min days between activities referring to the same activities"
							" (a min days between activities constraint is considered stronger than a min half days between activities constraint)")
							.arg(generic_oldc->getDescription(gt.rules))+"\n";
					}
				}
				else{
					activitiesConstraints.insert(repres, c);
				}
			}
		}
		else if(tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES && tc->active){
			ConstraintMinHalfDaysBetweenActivities* c=(ConstraintMinHalfDaysBetweenActivities*) tc;
	
			QSet<int> aset;
			int repres=-1;
	
			for(int i=0; i<c->n_activities; i++){
				int aid=c->activitiesIds[i];
				aset.insert(aid);
	
				if(activitiesRepresentant.value(aid,0)==aid)
					repres=aid; //does not matter if there are more representatives in this constraint, the constraint will be skipped anyway in this case
			}
	
			bool oktmp=false;
	
			if(repres>0){
				if(aset.count()==activitiesNumberOfSubactivities.value(repres, 0)){
					oktmp=true;
					for(int aid : std::as_const(aset))
						if(activitiesRepresentant.value(aid, 0)!=repres){
							oktmp=false;
							break;
						}
				}
			}
	
			if(!oktmp){
				lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
				" it does not refer to a whole larger container activity").arg(c->getDescription(gt.rules))+"\n";
			}
	
			if(oktmp){
				TimeConstraint* generic_oldc=activitiesConstraints.value(repres, nullptr);
				if(generic_oldc!=nullptr){
					if(generic_oldc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
						ConstraintMinHalfDaysBetweenActivities* oldc=(ConstraintMinHalfDaysBetweenActivities*)generic_oldc;
						if(oldc->weightPercentage < c->weightPercentage){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with larger weight percentage, referring to the same activities")
								.arg(oldc->getDescription(gt.rules))+"\n";
						}
						else if(oldc->weightPercentage > c->weightPercentage){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with larger weight percentage, referring to the same activities")
								.arg(c->getDescription(gt.rules))+"\n";
						}
		
						//equal weights - choose the lowest number of min days
						else if(oldc->minDays > c->minDays){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and higher number of min days, referring to the same activities")
								.arg(c->getDescription(gt.rules))+"\n";
						}
						else if(oldc->minDays < c->minDays){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and higher number of min days, referring to the same activities")
								.arg(oldc->getDescription(gt.rules))+"\n";
						}
		
						//equal weights and min days - choose the one with consecutive is same day true
						else if(oldc->consecutiveIfSameDay==true){
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and same number of min days and"
								" consecutive if on the same day true, referring to the same activities").arg(c->getDescription(gt.rules))+"\n";
						}
						else if(c->consecutiveIfSameDay==true){
							activitiesConstraints.insert(repres, c); //overwrites old value
							lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
								" there exists another constraint of this type with the same weight percentage and same number of min days and"
								" consecutive if on the same day true, referring to the same activities").arg(oldc->getDescription(gt.rules))+"\n";
						}
					}
					else if(generic_oldc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
						lastWarnings+=Export::tr("Note: Constraint %1 was skipped, because"
							" there exists another constraint of type min days between activities referring to the same activities"
							" (a min days between activities constraint is considered stronger than a min half days between activities constraint)")
							.arg(c->getDescription(gt.rules))+"\n";
					}
				}
				else{
					activitiesConstraints.insert(repres, c);
				}
			}
		}
	}
	//code by Liviu Lalescu (end)
	
	bool manuallyEditedPart1=false;
	bool manuallyEditedPart2=false;

	Activity* acti;
	Activity* actiNext;
	int countExportedActivities=0;
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		acti=gt.rules.activitiesList[ai];
		QString tl;
		QString sl;
		QString atl;
		QString stl;
		//if(acti->active){
		if((acti->activityGroupId==acti->id)||(acti->activityGroupId==0)){
			bool diffTeachers, diffSubject, diffActivityTag, diffStudents, diffCompNStud, diffNStud, diffActive;
			if(isActivityNotManuallyEditedPart1(ai, diffTeachers, diffSubject, diffActivityTag, diffStudents, tl, sl, atl, stl)){
			}
			else{
				QStringList s;
				if(diffTeachers)
					s.append(tr("different teachers"));
				if(diffSubject)
					s.append(tr("different subject"));
				if(diffActivityTag)
					s.append(tr("different activity tags"));
				if(diffStudents)
					s.append(tr("different students"));
				
				manuallyEditedPart1=true;
				
				lastWarnings+=tr("The subactivities with the activity group id %1 are different between themselves (they were separately edited)."
					" The fields which are different are: %2. These fields will be separated by the %3 symbol, one group for each subactivity.")
					.arg(CustomFETString::number(acti->activityGroupId)).arg(s.join(translatedCommaSpace())).arg("|")+"\n";
			}
			if(isActivityNotManuallyEditedPart2(ai, diffCompNStud, diffNStud, diffActive)){
			}
			else if(!diffCompNStud && !diffActive){
			}
			else{
				QStringList s;
				if(diffCompNStud)
					s.append(tr("different Boolean variable 'must compute n total students'"));
				if(diffNStud && diffCompNStud)
					s.append(tr("different number of students"));
				if(diffActive)
					s.append(tr("different active flag"));
				
				manuallyEditedPart2=true;
				
				lastWarnings+=tr("The subactivities with the activity group id %1 are different between themselves (they were separately edited),"
					" so the export will not be very accurate. The fields which are different will be considered those of the representative subactivity."
					" The fields which are different are: %2").arg(CustomFETString::number(acti->activityGroupId)).arg(s.join(translatedCommaSpace()))+"\n";
			}
			if(!acti->active){
				if(acti->activityGroupId==0)
					lastWarnings+=tr("The activity with id %1 has disabled active flag but it is exported.").arg(CustomFETString::number(acti->id))+"\n";
				else
					lastWarnings+=tr("The subactivities with activity group id %1 have disabled active flag but they are exported.").arg(CustomFETString::number(acti->activityGroupId))+"\n";
			}
			
			countExportedActivities++;
			//students set
			tosExport<<textquote;
			/*
			for(int s=0; s<acti->studentsNames.size(); s++){
				if(s!=0)
					tosExport<<"+";
				tosExport<<protectCSV(acti->studentsNames[s]);
			}
			*/
			
			tosExport<<stl;
			
			tosExport<<textquote<<fieldSeparator<<textquote;
			//subject
			/*tosExport<<protectCSV(acti->subjectName);*/
			
			tosExport<<sl;
			
			tosExport<<textquote<<fieldSeparator<<textquote;
			//teachers
			/*
			for(int t=0; t<acti->teachersNames.size(); t++){
				if(t!=0)
					tosExport<<"+";
				tosExport<<protectCSV(acti->teachersNames[t]);
			}
			*/
			
			tosExport<<tl;
			
			tosExport<<textquote<<fieldSeparator<<textquote;
			//activity tags
			/*
			for(int s=0; s<acti->activityTagsNames.size(); s++){
				if(s!=0)
					tosExport<<"+";
				tosExport<<protectCSV(acti->activityTagsNames[s]);
			}
			*/
			
			tosExport<<atl;
			
			tosExport<<textquote<<fieldSeparator;
			//total duration
			tosExport<<CustomFETString::number(acti->totalDuration);
			tosExport<<fieldSeparator<<textquote;
			//split duration
			for(int aiNext=ai; aiNext<gt.rules.activitiesList.size(); aiNext++){
				actiNext=gt.rules.activitiesList[aiNext];
				if(acti->activityGroupId!=0&&actiNext->activityGroupId==acti->activityGroupId){
					if(aiNext!=ai)
						tosExport<<"+";
					tosExport<<actiNext->duration;
				} else {
					if(acti->activityGroupId==0&&actiNext->activityGroupId==acti->activityGroupId){
						assert(ai==aiNext);
						//assert(actiNext->duration==actiNext->totalDuration);
						if(actiNext->duration>1)
							tosExport<<actiNext->duration;
					}
					aiNext=gt.rules.activitiesList.size();
				}
			}
			tosExport<<textquote<<fieldSeparator;
			//min days
			//start new code, because of Liviu's detection
			bool careAboutMinDays=false;
			
			TimeConstraint* tc=activitiesConstraints.value(acti->id, nullptr);
			
			if(tc!=nullptr){
				if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
					ConstraintMinDaysBetweenActivities* tcmd=(ConstraintMinDaysBetweenActivities*)tc;
					if(acti->id==acti->activityGroupId)
						careAboutMinDays=true;
					//end new code
					if(careAboutMinDays)
						tosExport<<CustomFETString::number(tcmd->minDays);
					tosExport<<fieldSeparator;
					//min days weight
					if(careAboutMinDays)
						tosExport<<CustomFETString::number(tcmd->weightPercentage);
					tosExport<<fieldSeparator;
					//min days consecutive
					if(careAboutMinDays)
						tosExport<<tcmd->consecutiveIfSameDay;
					tosExport<<fieldSeparator<<textquote<<protectCSVComments(acti->comments)<<textquote;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					tosExport<<Qt::endl;
#else
					tosExport<<endl;
#endif
				}
				else{
					assert(tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES);
					ConstraintMinHalfDaysBetweenActivities* tcmd=(ConstraintMinHalfDaysBetweenActivities*)tc;
					if(acti->id==acti->activityGroupId)
						careAboutMinDays=true;
					//end new code
					if(careAboutMinDays)
						tosExport<<CustomFETString::number(tcmd->minDays)<<"h";
					tosExport<<fieldSeparator;
					//min days weight
					if(careAboutMinDays)
						tosExport<<CustomFETString::number(tcmd->weightPercentage);
					tosExport<<fieldSeparator;
					//min days consecutive
					if(careAboutMinDays)
						tosExport<<tcmd->consecutiveIfSameDay;
					tosExport<<fieldSeparator<<textquote<<protectCSVComments(acti->comments)<<textquote;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					tosExport<<Qt::endl;
#else
					tosExport<<endl;
#endif
				}
			}
			else{
				tosExport<<fieldSeparator<<fieldSeparator<<fieldSeparator<<textquote<<protectCSVComments(acti->comments)<<textquote;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				tosExport<<Qt::endl;
#else
				tosExport<<endl;
#endif
			}
		}
		//}
	}
	
#ifndef FET_COMMAND_LINE
	if(manuallyEditedPart1){
		QMessageBox::warning(parent, tr("FET warning"), tr("There are subactivities which were modified separately - so the "
		 "components had different values for subject, activity tags, teachers, or students. The export is done, "
		 "but for each field which is different there will be added n_subactivities fields, separated by the %1 symbol.").arg("|"));
	}
	if(manuallyEditedPart2){
		QMessageBox::warning(parent, tr("FET warning"), tr("There are subactivities which were modified separately - so the "
		 "components had different values for 'must compute n total students' or number of students from the representative subactivity. "
		 "The export is done, but it is not very accurate."));
	}
#else
	if(manuallyEditedPart1){
		cout<<qPrintable(tr("FET warning"))<<endl;
		cout<<qPrintable(tr("There are subactivities which were modified separately - so the "
		 "components had different values for subject, activity tags, teachers, or students. The export is done, "
		 "but for each field which is different there will be added n_subactivities fields, separated by the %1 symbol.").arg("|"))
		 <<endl;
	}
	if(manuallyEditedPart2){
		cout<<qPrintable(tr("FET warning"))<<endl;
		cout<<qPrintable(tr("There are subactivities which were modified separately - so the "
		 "components had different values for 'must compute n total students' or number of students from the representative subactivity. "
		 "The export is done, but it is not very accurate."))
		 <<endl;
	}
#endif

	lastWarnings+=Export::tr("%1 activities exported.").arg(countExportedActivities)+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVActivitiesStatistics(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVActivitiesStatistics(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVActivitiesStatistics;

#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	if(head)
		tosExport	<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Total Duration"<<textquote;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				tosExport<<Qt::endl;
#else
				tosExport<<endl;
#endif

	Activity* acti;
	int countExportedActivities=0;
	QMap<LocaleString, int> tmpIdentDuration;	//not QHash, because I want a nice order of the activities
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		acti=gt.rules.activitiesList[ai];
		if(acti->active){
			int tmpD=acti->duration;
			QString tmpIdent=textquote;
			if(acti->studentsNames.size()>0){
				for(QStringList::const_iterator it=acti->studentsNames.constBegin(); it!=acti->studentsNames.constEnd(); it++){
					tmpIdent+=protectCSV(*it);
					if(it!=acti->studentsNames.constEnd()-1)
						tmpIdent+="+";
				}
			}
			tmpIdent+=textquote+fieldSeparator+textquote+protectCSV(acti->subjectName)+textquote+fieldSeparator+textquote;
			if(acti->teachersNames.size()>0){
				for(QStringList::const_iterator it=acti->teachersNames.constBegin(); it!=acti->teachersNames.constEnd(); it++){
					tmpIdent+=protectCSV(*it);
					if(it!=acti->teachersNames.constEnd()-1)
						tmpIdent+="+";
				}
			}
			tmpIdent+=textquote+fieldSeparator;
			tmpD+=tmpIdentDuration.value(tmpIdent);
			tmpIdentDuration.insert(tmpIdent, tmpD);
		}
	}

	QMap<LocaleString, int>::const_iterator it=tmpIdentDuration.constBegin();
	while(it!=tmpIdentDuration.constEnd()){
		countExportedActivities++;
		tosExport<<it.key();
		tosExport<<textquote<<CustomFETString::number(it.value())<<textquote;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
		tosExport<<Qt::endl;
#else
		tosExport<<endl;
#endif
		it++;
	}

	lastWarnings+=Export::tr("%1 active activities statistics exported.").arg(countExportedActivities)+"\n";
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
bool Export::exportCSVTimetable(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton){
#else
bool Export::exportCSVTimetable(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head){
#endif
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);	//TODO: remove s2, because too long filenames!

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	QString UNDERSCORE="_";
	if(INPUT_FILENAME_XML=="")
		UNDERSCORE="";
	QString file=PREFIX_CSV+s2+UNDERSCORE+CSVTimetable;
	
#ifndef FET_COMMAND_LINE
	if(!Export::okToWrite(parent, file, msgBoxButton))
#else
	if(!Export::okToWrite(file))
#endif
		return false;
	
	QFile fileExport(file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!fileExport.open(QIODeviceBase::WriteOnly)){
#else
	if(!fileExport.open(QIODevice::WriteOnly)){
#endif
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
	}
	QTextStream tosExport(&fileExport);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tosExport.setEncoding(QStringConverter::Utf8);
#else
	tosExport.setCodec("UTF-8");
#endif
	tosExport.setGenerateByteOrderMark(true);
	
	//section "Activity Id" was added by Liviu Lalescu on 2010-01-26, as suggested on the forum
	if(head)
		tosExport
				<<textquote<<"Activity Id"<<textquote<<fieldSeparator
				<<textquote<<"Day"<<textquote<<fieldSeparator
				<<textquote<<"Hour"<<textquote<<fieldSeparator
				<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tags"<<textquote<<fieldSeparator
				<<textquote<<"Room"<<textquote<<fieldSeparator
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
				<<textquote<<"Comments"<<textquote<<Qt::endl;
#else
				<<textquote<<"Comments"<<textquote<<endl;
#endif

	if(gt.rules.initialized && gt.rules.internalStructureComputed
	 && students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready){
		QSet<QString> printableActivityTagsSet;
		for(int i=0; i<gt.rules.nInternalActivityTags; i++)
			if(gt.rules.internalActivityTagsList[i]->printable)
				printableActivityTagsSet.insert(gt.rules.internalActivityTagsList[i]->name);
		
		Activity *act;
		int exportedActivities=0;
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			if(best_solution.times[i]!=UNALLOCATED_TIME) {
				exportedActivities++;
				act=&gt.rules.internalActivitiesList[i];
				int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
				int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
				int r=best_solution.rooms[i];
				for(int dd=0; dd < act->duration; dd++){
					assert(hour+dd<gt.rules.nHoursPerDay);
					
					//Activity id - added by Liviu on 2010-01-26
					tosExport<<textquote<<CustomFETString::number(act->id)<<textquote<<fieldSeparator;
					
					//Day
					tosExport<<textquote<<protectCSV(gt.rules.daysOfTheWeek[day])<<textquote<<fieldSeparator;
					//Period
					tosExport<<textquote<<protectCSV(gt.rules.hoursOfTheDay[hour+dd])<<textquote<<fieldSeparator<<textquote;
					//Students Sets
					for(int s=0; s<act->studentsNames.size(); s++){
						if(s!=0)
							tosExport<<"+";
						tosExport<<protectCSV(act->studentsNames[s]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Subject
					tosExport<<protectCSV(act->subjectName);
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Teachers
					for(int t=0; t<act->teachersNames.size(); t++){
						if(t!=0)
							tosExport<<"+";
						tosExport<<protectCSV(act->teachersNames[t]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					
					//Activity Tags
					bool begin=true;
					for(int s=0; s<act->activityTagsNames.size(); s++){
						if(printableActivityTagsSet.contains(act->activityTagsNames[s])){
							if(!begin)
								tosExport<<"+";
							else
								begin=false;
							tosExport<<protectCSV(act->activityTagsNames[s]);
						}
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					
					//Room
					if(best_solution.rooms[i] != UNSPECIFIED_ROOM && best_solution.rooms[i] != UNALLOCATED_SPACE){
						assert(best_solution.rooms[i]>=0 && best_solution.rooms[i]<gt.rules.nInternalRooms);
						if(gt.rules.internalRoomsList[r]->isVirtual==false){
							tosExport<<protectCSV(gt.rules.internalRoomsList[r]->name);
						} else {
							QStringList rooms;
							for(int x : std::as_const(best_solution.realRoomsList[i])){
								rooms.append(gt.rules.internalRoomsList[x]->name);
							}
							if(SHOW_VIRTUAL_ROOMS_IN_TIMETABLES)
								tosExport<<protectCSV(gt.rules.internalRoomsList[r]->name)<<" ("<<protectCSV(rooms.join("+"))<<")";
							else
								tosExport<<protectCSV(rooms.join("+"));
						}
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//Comments
					QString tmpString=protectCSVComments(act->comments);
					//tmpString.replace("\n", " ");
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					tosExport<<tmpString<<textquote<<Qt::endl;
#else
					tosExport<<tmpString<<textquote<<endl;
#endif
				}
			}
		}
		lastWarnings+=Export::tr("%1 scheduled activities exported.").arg(exportedActivities)+"\n";
	} else {
		lastWarnings+=Export::tr("0 scheduled activities exported, because no timetable was generated.")+"\n";
	}
	if(fileExport.error()!=QFileDevice::NoError){
		lastWarnings+=Export::tr("FET critical. Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(file).arg(fileExport.errorString())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}

#ifndef FET_COMMAND_LINE
LastWarningsDialogE::LastWarningsDialogE(QWidget* parent, const QString& lastWarning): QDialog(parent)
{
	this->setWindowTitle(tr("FET - export comments", "The comments of the exporting operation"));
	QVBoxLayout* lastWarningsMainLayout=new QVBoxLayout(this);

	QTextEdit* lastWarningsText=new QTextEdit();
	lastWarningsText->setMinimumWidth(500);				//width
	lastWarningsText->setMinimumHeight(250);
	lastWarningsText->setReadOnly(true);
	lastWarningsText->setWordWrapMode(QTextOption::NoWrap);
	lastWarningsText->setPlainText(lastWarning);

	//Start Buttons
	QPushButton* pb1=new QPushButton(tr("&OK"));
	//pb1->setAutoDefault(true);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb1);

	//Start adding all into main layout
	lastWarningsMainLayout->addWidget(lastWarningsText);
	lastWarningsMainLayout->addLayout(hl);

	connect(pb1, &QPushButton::clicked, this, &LastWarningsDialogE::accept);
	
	//pb1->setDefault(true);

	pb1->setDefault(true);
	pb1->setFocus();
}

LastWarningsDialogE::~LastWarningsDialogE()
{
}
#endif
