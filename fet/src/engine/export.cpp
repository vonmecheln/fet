/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          export.cpp  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   NULL program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Code contributed by Volker Dirr ( http://www.timetabling.de/ )

//TODO: convert all export funtions like this: tosExport<<qPrintable(QString::number(actiNext->duration))
//TODO: protect export strings. textquote must be dubbled
//TODO: count skipped min n day constraints?
//TODO: add cancel button

//#include "centerwidgetonscreen.h"
class QWidget;
void centerWidgetOnScreen(QWidget* widget);

#include "export.h"
#include "solution.h"
#include <QtGui>
#include <QHash>
#include <QSet>

extern Timetable gt;
extern Solution best_solution;
extern bool teachers_schedule_ready;
extern bool students_schedule_ready;
extern bool rooms_schedule_ready;

Export::Export()
{
}

Export::~Export()
{
}

void Export::exportCSV(){
	QString fieldSeparator=",";
	QString textquote="\"";
	QString setSeparator="+";
	bool head=true;
	bool ok=true;

	ok=selectSeparatorAndTextquote(textquote, fieldSeparator, head);
	QString lastWarnings;
	if(!ok){
		lastWarnings.insert(0,Export::tr("Export aborted")+"\n");
	} else {
		if(ok)
			ok=exportCSVActivityTags(lastWarnings, textquote, head);
		if(ok)
			ok=exportCSVRoomsAndBuildings(lastWarnings, textquote, fieldSeparator, head);
		if(ok)
			ok=exportCSVSubjects(lastWarnings, textquote, head);
		if(ok)
			ok=exportCSVTeachers(lastWarnings, textquote, head, setSeparator);
		if(ok)
			ok=exportCSVStudents(lastWarnings, textquote, fieldSeparator, head, setSeparator);
		if(ok)
			ok=exportCSVActivities(lastWarnings, textquote, fieldSeparator, head);
		if(ok)
			ok=exportCSVTimetable(lastWarnings, textquote, fieldSeparator, head);
		lastWarnings.insert(0,Export::tr("CSV files were exported to directory %1.").arg(OUTPUT_DIR)+"\n");
		if(ok)
			lastWarnings.insert(0,Export::tr("Exported complete")+"\n");
		else
			lastWarnings.insert(0,Export::tr("Export incomplete")+"\n");
	}

	lastWarningsDialogE lwd(lastWarnings);
	lwd.setWindowFlags(lwd.windowFlags() | Qt::WindowMinMaxButtonsHint);
	//lwd.show();
	int w=lwd.sizeHint().width();
        int h=lwd.sizeHint().height();
	lwd.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&lwd);

	ok=lwd.exec();
}

QString Export::protectCSV(const QString& str)
{
	QString p=str;
	p.replace("\"", "\"\"");
	return p;
}

bool Export::checkSetSeparator(const QString& str, const QString setSeparator)
{
	if(str.contains(setSeparator))
		return false;
	return true;
}


bool Export::selectSeparatorAndTextquote(QString& textquote, QString& fieldSeparator, bool& head){
	assert(gt.rules.initialized);

	QStringList separators;
	QStringList textquotes;
	separators<<","<<";"<<"|";
	textquotes<<"\""<<"'"<<Export::tr("no textquote");
	assert(textquotes[2].size()>1);

	QDialog separatorsDialog(NULL);
	separatorsDialog.setWindowTitle(QObject::tr("FET question"));
	QVBoxLayout* separatorsMainLayout=new QVBoxLayout(&separatorsDialog);

	QHBoxLayout* top=new QHBoxLayout();
	QLabel* topText=new QLabel();
	topText->setText(Export::tr("Please keep the default settings.\nImport of data will be easier with these settings."));
	top->addWidget(topText);

	QGroupBox* separatorsGroupBox = new QGroupBox(Export::tr("Please specify the separator between fields:"));
	QHBoxLayout* separatorBoxChoose=new QHBoxLayout();
	QComboBox* separatorsCB=new QComboBox();
	if(separators.size()>1){
		QLabel* separatorTextChoose=new QLabel();
		separatorTextChoose->setText(Export::tr("Use field separator:"));
		separatorsCB->insertItems(0,separators);
		separatorBoxChoose->addWidget(separatorTextChoose);
		separatorBoxChoose->addWidget(separatorsCB);
		separatorsGroupBox->setLayout(separatorBoxChoose);
	}

	QGroupBox* textquoteGroupBox = new QGroupBox(Export::tr("Please specify the text quote of text fields:"));
	QHBoxLayout* textquoteBoxChoose=new QHBoxLayout();
	QComboBox* textquoteCB=new QComboBox();
	if(separators.size()>1){
		QLabel* textquoteTextChoose=new QLabel();
		textquoteTextChoose->setText(Export::tr("Use textquote:"));
		textquoteCB->insertItems(0,textquotes);
		textquoteBoxChoose->addWidget(textquoteTextChoose);
		textquoteBoxChoose->addWidget(textquoteCB);
		textquoteGroupBox->setLayout(textquoteBoxChoose);
	}

	QGroupBox* firstLineGroupBox = new QGroupBox(Export::tr("Please specify the content of the first line:"));
	QVBoxLayout* firstLineChooseBox=new QVBoxLayout();
	QRadioButton* firstLineRadio1 = new QRadioButton(Export::tr("The first line is the heading."));
	QRadioButton* firstLineRadio2 = new QRadioButton(Export::tr("The first line contains data. Don't export heading."));
	firstLineRadio1->setChecked(true);
	firstLineChooseBox->addWidget(firstLineRadio1);
	firstLineChooseBox->addWidget(firstLineRadio2);
	firstLineGroupBox->setLayout(firstLineChooseBox);

	QPushButton* pb=new QPushButton(QObject::tr("OK"));
	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb);
	
	separatorsMainLayout->addLayout(top);
	separatorsMainLayout->addWidget(separatorsGroupBox);
	separatorsMainLayout->addWidget(textquoteGroupBox);

	separatorsMainLayout->addWidget(firstLineGroupBox);
	separatorsMainLayout->addLayout(hl);
	QObject::connect(pb, SIGNAL(clicked()), &separatorsDialog, SLOT(accept()));
		
	separatorsDialog.setWindowFlags(separatorsDialog.windowFlags() | Qt::WindowMinMaxButtonsHint);

	//separatorsDialog.show();
	int w=separatorsDialog.sizeHint().width();
        int h=separatorsDialog.sizeHint().height();
	separatorsDialog.setGeometry(0,0,w,h);
	centerWidgetOnScreen(&separatorsDialog);

	int ok=separatorsDialog.exec();
	if(!ok) return false;

	fieldSeparator=separatorsCB->currentText();
	textquote=textquoteCB->currentText();

	if(textquote.size()!=1)
		textquote="";

	if(firstLineRadio1->isChecked())
		head=true;
	else head=false;
	return true;
}

lastWarningsDialogE::lastWarningsDialogE(QString lastWarning, QWidget *parent): QDialog(parent)
{
	this->setWindowTitle(tr("FET - export comment"));
	QVBoxLayout* lastWarningsMainLayout=new QVBoxLayout(this);

	QTextEdit* lastWarningsText=new QTextEdit();
	lastWarningsText->setMinimumWidth(500);				//width
	lastWarningsText->setMinimumHeight(250);
	lastWarningsText->setReadOnly(true);
	lastWarningsText->setWordWrapMode(QTextOption::NoWrap);
	lastWarningsText->setText(lastWarning);

	//Start Buttons
	QPushButton* pb1=new QPushButton(tr("&Ok"));
	//pb1->setAutoDefault(true);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(pb1);

	//Start adding all into main layout
	lastWarningsMainLayout->addWidget(lastWarningsText);
	lastWarningsMainLayout->addLayout(hl);

	QObject::connect(pb1, SIGNAL(clicked()), this, SLOT(accept()));
	
	//pb1->setDefault(true);

	pb1->setDefault(true);
	pb1->setFocus();
	
	this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
}

bool Export::exportCSVActivityTags(QString& lastWarnings, const QString textquote, const bool head){

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Activity_Tags.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files

	if(head)
		tosExport<<textquote<<"Activity Tag"<<textquote<<endl;

	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* a=gt.rules.activityTagsList[i];
		tosExport<<textquote<<protectCSV(a->name)<<textquote<<endl;
	}

	lastWarnings+=Export::tr("%1 activity tags exported.").arg(gt.rules.activityTagsList.size())+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error()+"\n");
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVRoomsAndBuildings(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Rooms_and_Buildings.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport	<<textquote<<"Room"<<textquote<<fieldSeparator
				<<textquote<<"Room Capacity"<<textquote<<fieldSeparator
				<<textquote<<"Building"<<textquote<<endl;

	QStringList checkBuildings;
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* r=gt.rules.roomsList[i];
		tosExport	<<textquote<<protectCSV(r->name)<<textquote<<fieldSeparator
				<<qPrintable(QString::number(r->capacity))<<fieldSeparator
				<<textquote<<protectCSV(r->building)<<textquote<<endl;
		if(!checkBuildings.contains(r->building)&&!r->building.isEmpty())
			checkBuildings<<r->building;
	}

	lastWarnings+=Export::tr("%1 rooms (with buildings) exported.").arg(gt.rules.roomsList.size())+"\n";
	if(gt.rules.buildingsList.size()!=checkBuildings.size()){
		lastWarnings+=Export::tr("Warning! Only %1 of %2 building names are exported, because %3 buildings don't contain any room.").arg(checkBuildings.size()).arg(gt.rules.buildingsList.size()).arg(gt.rules.buildingsList.size()-checkBuildings.size())+"\n";
	}
	
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error()+"\n");
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVSubjects(QString& lastWarnings, const QString textquote, const bool head){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Subjects.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport<<textquote<<"Subject"<<textquote<<endl;

	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		tosExport<<textquote<<protectCSV(s->name)<<textquote<<endl;
	}

	lastWarnings+=Export::tr("%1 subjects exported.").arg(gt.rules.subjectsList.size())+"\n";	
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error()+"\n");
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVTeachers(QString& lastWarnings, const QString textquote, const bool head, const QString setSeparator){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Teachers.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport<<textquote<<"Teacher"<<textquote<<endl;

	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* t=gt.rules.teachersList[i];
		tosExport<<textquote<<protectCSV(t->name)<<textquote<<endl;
		if(!checkSetSeparator(t->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 include set separator +.").arg(t->name)+"\n";
	}

	lastWarnings+=Export::tr("%1 teachers exported.").arg(gt.rules.teachersList.size())+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error()+"\n");
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVStudents(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, const QString setSeparator){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Years_Groups_and_Subgroups.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport	<<textquote<<"Year"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Year"<<textquote<<fieldSeparator
				<<textquote<<"Group"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Group"<<textquote<<fieldSeparator
				<<textquote<<"Subgroup"<<textquote<<fieldSeparator
				<<textquote<<"Number of Students per Subgroup"<<textquote<<endl;

	int ig=0;
	int is=0;
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		tosExport<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
					<<qPrintable(QString::number(sty->numberOfStudents))<<fieldSeparator<<fieldSeparator<<fieldSeparator<<fieldSeparator<<endl;
		if(!checkSetSeparator(sty->name, setSeparator))
			lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 include set separator +.").arg(sty->name)+"\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			ig++;
			StudentsGroup* stg=sty->groupsList[j];
			tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
					<<qPrintable(QString::number(sty->numberOfStudents))<<fieldSeparator
					<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
					<<qPrintable(QString::number(stg->numberOfStudents))<<fieldSeparator<<fieldSeparator<<endl;
			if(!checkSetSeparator(stg->name, setSeparator))
				lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 include set separator +.").arg(stg->name)+"\n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				is++;
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tosExport	<<textquote<<protectCSV(sty->name)<<textquote<<fieldSeparator
						<<qPrintable(QString::number(sty->numberOfStudents))<<fieldSeparator
						<<textquote<<protectCSV(stg->name)<<textquote<<fieldSeparator
						<<qPrintable(QString::number(stg->numberOfStudents))<<fieldSeparator
						<<textquote<<protectCSV(sts->name)<<textquote<<fieldSeparator
						<<qPrintable(QString::number(sts->numberOfStudents))<<endl;
				if(!checkSetSeparator(sts->name, setSeparator))
					lastWarnings+=Export::tr("Warning! Import of activities will fail, because %1 include set separator +.").arg(sts->name)+"\n";
			}
		}
	}

	lastWarnings+=Export::tr("%1 years exported.").arg(gt.rules.yearsList.size())+"\n";
	lastWarnings+=Export::tr("%1 groups exported.").arg(ig)+"\n";
	lastWarnings+=Export::tr("%1 subgroups exported.").arg(is)+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error()+"\n");
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVActivities(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Activities.csv";

	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport	<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tag"<<textquote<<fieldSeparator
				<<textquote<<"Total Duration"<<textquote<<fieldSeparator
				<<textquote<<"Split Duration"<<textquote<<fieldSeparator
				<<textquote<<"Min N Days"<<textquote<<fieldSeparator
				<<textquote<<"Weight"<<textquote<<fieldSeparator
				<<textquote<<"Consecutive"<<textquote<<endl;

	//code by Liviu Lalescu (begin)
	//better detection of min n day constraint
	QHash<int, int> activitiesRepresentant;
	QHash<int, int> activitiesNumberOfSubactivities;
	QHash<int, ConstraintMinNDaysBetweenActivities*>activitiesConstraints;
	
	activitiesRepresentant.clear();
	activitiesNumberOfSubactivities.clear();
	activitiesConstraints.clear();
	
	foreach(Activity* act, gt.rules.activitiesList){
		assert(!activitiesRepresentant.contains(act->id));
		activitiesRepresentant.insert(act->id, act->activityGroupId); //act->id is key, act->agid is value
	
		if(act->activityGroupId>0){
			int n=activitiesNumberOfSubactivities.value(act->activityGroupId, 0); //0 here means default value
			n++;
			activitiesNumberOfSubactivities.insert(act->activityGroupId, n); //overwrites old value
		}
	}
	foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
		if(tc->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinNDaysBetweenActivities* c=(ConstraintMinNDaysBetweenActivities*) tc;
	
			QSet<int> aset;
			int repres=-1;
	
			for(int i=0; i<c->n_activities; i++){
				int aid=c->activitiesId[i];
				aset.insert(aid);
	
				if(activitiesRepresentant.value(aid,0)==aid)
					repres=aid; //does not matter if there are more representants in this constraint, the constraint will be skipped anyway in this case
			}
	
			bool oktmp=false;
	
			if(repres>0){
				if(aset.count()==activitiesNumberOfSubactivities.value(repres, 0)){
					oktmp=true;
					foreach(int aid, aset)
						if(activitiesRepresentant.value(aid, 0)!=repres){
							oktmp=false;
							break;
						}
				}
			}
	
			if(!oktmp){
				lastWarnings+=Export::tr("Warning! Constraint")+" "+c->getDescription(gt.rules)+tr("was skipped, because"
				" it refers not to a whole larger container activity")+"\n";
			}
	
			if(oktmp){
				ConstraintMinNDaysBetweenActivities* oldc=activitiesConstraints.value(repres, NULL);
				if(oldc!=NULL){
					if(oldc->weightPercentage < c->weightPercentage){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Warning! Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with larger weight percentage, referring to the same activities")+"\n";
					}
					else if(oldc->weightPercentage > c->weightPercentage){
						lastWarnings+=Export::tr("Warning! Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with larger weight percentage, referring to the same activities")+"\n";
					}
	
					//equal weights - choose the lowest number of min n days
					else if(oldc->minDays > c->minDays){
						lastWarnings+=Export::tr("Warning! Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and higher number of min days, referring to the same activities")+"\n";
					}
					else if(oldc->minDays < c->minDays){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Warning! Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and higher number of min days, referring to the same activities")+"\n";
					}
	
					//equal weights and min days - choose the one with consecutive is same day true
					else if(oldc->consecutiveIfSameDay==true){
						lastWarnings+=Export::tr("Warning! Constraint")+" "+c->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and same number of min days and"
							" consecutive if same day true, referring to the same activities")+"\n";
					}
					else if(c->consecutiveIfSameDay==true){
						activitiesConstraints.insert(repres, c); //overwrites old value
						lastWarnings+=Export::tr("Warning! Constraint")+" "+oldc->getDescription(gt.rules)+" "+tr("was skipped, because"
							" there exists another constraint of this type with same weight percentage and same number of min days and"
							" consecutive if same day true, referring to the same activities")+"\n";
					}
	
				}
				else
					activitiesConstraints.insert(repres, c);
			}
		}
	}
	//code by Liviu Lalescu (end)

	Activity* acti;
	Activity* actiNext;
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		acti=gt.rules.activitiesList[ai];
		if(acti->active){
			if((acti->activityGroupId==acti->id)||(acti->activityGroupId==0)){
				//if((acti->studentsNames.size()>0) && (acti->teachersNames.size()>0) ){
					//students set
					tosExport<<textquote;
					for(int s=0; s<acti->studentsNames.size(); s++){
						if(s!=0)
							tosExport<<"+";
						tosExport<<protectCSV(acti->studentsNames[s]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//subject
					tosExport<<protectCSV(acti->subjectName);
					tosExport<<textquote<<fieldSeparator<<textquote;
					//teachers
					for(int t=0; t<acti->teachersNames.size(); t++){
						if(t!=0)
							tosExport<<"+";
						tosExport<<protectCSV(acti->teachersNames[t]);
					}
					tosExport<<textquote<<fieldSeparator<<textquote;
					//activity tag
					tosExport<<protectCSV(acti->activityTagName);
					tosExport<<textquote<<fieldSeparator;
					//total duration
					tosExport<<qPrintable(QString::number(acti->totalDuration));
					tosExport<<fieldSeparator<<textquote;
					//split duration
					//start old code by Volker Dirr. useless now, because of Livius min n day detection
					/*bool careAboutMinDay=false;
					QList<int> affected_tcIDs;
					TimeConstraint* tc;
					tc=NULL;
					for(int tcID=0; tcID<gt.rules.timeConstraintsList.size(); tcID++){
						tc=gt.rules.timeConstraintsList[tcID];
						if(tc->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES)
							if(tc->isRelatedToActivity(acti))
								affected_tcIDs<<tcID;
					}
					if(affected_tcIDs.size()>1){
						QMessageBox::critical(NULL, QObject::tr("FET critical"),
						Export::tr("Please check the min n day constraint of the export file of activity %1.").arg(ai));
					}
					if(affected_tcIDs.size()==1){
						careAboutMinDay=true;
						tc=gt.rules.timeConstraintsList[affected_tcIDs[0]];
					}
					ConstraintMinNDaysBetweenActivities* tcmd;
					tcmd=(ConstraintMinNDaysBetweenActivities*)tc;
					for(int aiNext=ai; aiNext<gt.rules.activitiesList.size(); aiNext++){
						actiNext=gt.rules.activitiesList[aiNext];
						if(acti->activityGroupId!=0&&actiNext->activityGroupId==acti->activityGroupId){
							if(aiNext!=ai)
								tosExport<<"+";
							tosExport<<actiNext->duration;
							if(affected_tcIDs.size()==1){
								if(!tcmd->isRelatedToActivity(actiNext))
									careAboutMinDay=false;
							}
						} else {
							if(acti->activityGroupId==0&&actiNext->activityGroupId==acti->activityGroupId){
								assert(ai==aiNext);
								assert(actiNext->duration==actiNext->totalDuration);
								if(actiNext->duration>1)
									tosExport<<actiNext->duration;
							}	
							aiNext=gt.rules.activitiesList.size();
						}	
					}*/
					//end old code by Volker Dirr
					for(int aiNext=ai; aiNext<gt.rules.activitiesList.size(); aiNext++){
						actiNext=gt.rules.activitiesList[aiNext];
						if(acti->activityGroupId!=0&&actiNext->activityGroupId==acti->activityGroupId){
							if(aiNext!=ai)
								tosExport<<"+";
							tosExport<<actiNext->duration;
						} else {
							if(acti->activityGroupId==0&&actiNext->activityGroupId==acti->activityGroupId){
								assert(ai==aiNext);
								assert(actiNext->duration==actiNext->totalDuration);
								if(actiNext->duration>1)
									tosExport<<actiNext->duration;
							}	
							aiNext=gt.rules.activitiesList.size();
						}	
					}
					tosExport<<textquote<<fieldSeparator;
					//min n days
					//start new code, because of Livius detection
					bool careAboutMinDay=false;
					ConstraintMinNDaysBetweenActivities* tcmd=activitiesConstraints.value(acti->id, NULL);
					if(acti->id==acti->activityGroupId){
							if(tcmd!=NULL){
								careAboutMinDay=true;
							}
					}
					//end new code
					if(careAboutMinDay){
						assert(tcmd->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES);
						tosExport<<qPrintable(QString::number(tcmd->minDays));
					}
					tosExport<<fieldSeparator;
					//min n days weight
					if(careAboutMinDay)
						tosExport<<qPrintable(QString::number(tcmd->weightPercentage));
					tosExport<<fieldSeparator;
					//min n days consecutive
					if(careAboutMinDay)
						tosExport<<tcmd->consecutiveIfSameDay;
					tosExport<<endl;
				//}
			}
		}
	}

	lastWarnings+=Export::tr("%1 activities exported.").arg(gt.rules.activitiesList.size())+"\n";
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}


bool Export::exportCSVTimetable(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head){
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString file=OUTPUT_DIR+FILE_SEP+s2+"_Timetable.csv";
	
	QFile fileExport(file);
	if(!fileExport.open(QIODevice::WriteOnly)){
		lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(file)+"\n";
		return false;
		assert(0);
	}
	QTextStream tosExport(&fileExport);
	tosExport.setCodec("UTF-8");
	tosExport.setGenerateByteOrderMark(true);	//default is "true", but openOffice have problems to open those files
	
	if(head)
		tosExport	<<textquote<<"Day"<<textquote<<fieldSeparator
				<<textquote<<"Period"<<textquote<<fieldSeparator
				<<textquote<<"Students Sets"<<textquote<<fieldSeparator
				<<textquote<<"Subject"<<textquote<<fieldSeparator
				<<textquote<<"Teachers"<<textquote<<fieldSeparator
				<<textquote<<"Activity Tag"<<textquote<<fieldSeparator
				<<textquote<<"Room"<<textquote<<endl;

	if(gt.rules.initialized && gt.rules.internalStructureComputed
	 && students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready){
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
					//Activity Tag
					tosExport<<protectCSV(act->activityTagName)<<textquote<<fieldSeparator;
					//Room
					if(best_solution.rooms[i] != UNSPECIFIED_ROOM && best_solution.rooms[i] != UNALLOCATED_SPACE){
						assert(best_solution.rooms[i]>=0 && best_solution.rooms[i]<gt.rules.nInternalRooms);
						tosExport<<textquote<<protectCSV(gt.rules.internalRoomsList[r]->name)<<textquote;
					}
					tosExport<<endl;
				}
			}
		}	
		lastWarnings+=Export::tr("%1 scheduled activities exported.").arg(exportedActivities)+"\n";
	} else {
		lastWarnings+=Export::tr("0 scheduled activities exported, because no timetable was generated.")+"\n";
	}
	if(fileExport.error()>0){
		lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(file).arg(fileExport.error())+"\n";
		return false;
	}
	fileExport.close();
	return true;
}



bool Export::exportSchILD(QString& lastWarnings){
	//assert(gt.rules.initialized && gt.rules.internalStructureComputed); //i think i don't need that, or?

	//TODO:
	QString directoryname=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	int ret=QMessageBox::question(NULL, "Export to SchILD?", "Do you want to export dat-files for SchILD?\nSchiILD is the school administration software of NRW (Germany).\nhttp://www.svws.nrw.de/\nThis will create following files:\n- Exportprotokoll.log\n- Kurse.dat\n- KurseLehrkraefte.dat\n- LehrkraefteSonderzeiten.dat\n- LehrkraefteSonderzeiten.dat", QMessageBox::Yes | QMessageBox::No);

	if(ret==QMessageBox::Yes){
		bool ok;
		//TODO: write just ONE form with both questions
		int jahr=QInputDialog::getInteger(NULL, "FET question", "Please specify the year of the current data set:",QDate::currentDate().year(),2000,3000,1,&ok);
		int abschnitt=1;
		if(ok) abschnitt=QInputDialog::getInteger(NULL, "FET question", "Please specify the semester of the current data set:",1,1,4,1,&ok);
		if(ok){
			int not_active=0;
			bool kurs=false;
			QString kursbezeichnung;
			QString tmp;
			bool nurEinJahrgang;
			bool added=false;
			QFile fileK(directoryname+"Kurse.dat");
			if(!fileK.open(QIODevice::WriteOnly)){
				lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(directoryname)+"\n";
				return false;
				assert(0);
			}
			QTextStream tosK(&fileK);
			//tosK.setCodec("UTF-8");
			//tosK.setGenerateByteOrderMark(true);
		
			QFile fileKL(directoryname+"KurseLehrkraefte.dat");
			if(!fileKL.open(QIODevice::WriteOnly)){
				lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(directoryname+"\n");
				return false;
				assert(0);
			}
			QTextStream tosKL(&fileKL);
			//tosKL.setCodec("UTF-8");
			//tosKL.setGenerateByteOrderMark(true);
		
			QFile fileLS(directoryname+"LehrkraefteSonderzeiten.dat");
			if(!fileLS.open(QIODevice::WriteOnly)){
				lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(directoryname)+"\n";
				return false;
				assert(0);
			}
			QTextStream tosLS(&fileLS);
			//tosLS.setCodec("UTF-8");
			//tosLS.setGenerateByteOrderMark(true);
		
			QFile fileE(directoryname+"Exportprotokoll.log");
			if(!fileE.open(QIODevice::WriteOnly)){
				lastWarnings+=Export::tr("FET critical. Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(directoryname)+"\n";
				return false;
				assert(0);
			}
			QTextStream tosE(&fileE);
			//tosE.setCodec("UTF-8");
			//tosE.setGenerateByteOrderMark(true);
	
			tosE<<"Hinweis: Kursbezeichnungen wurden evt. verändert."<<endl;
			tosE<<"         Kurse.dat und KurseLehrkraefte.dat wird zum import der UV benötigt."<<endl;
			tosE<<"         LehrkraefteSonderzeiten.dat kann zum Import der Soderzeiten für die Statistik benutzt werden."<<endl;
			tosE<<"          Dazu muss die Kennzeichnung des Unterrichts (Aktivität) die 3 stellige ASD Schlüsselnummer sein."<<endl<<endl;
			tosE<<"Warnung: LehrkraefteSonderzeiten.dat enthält keine Jahrgangs und Abschnittsdaten!"<<endl;
			tosE<<"         Die Sonderzeiten werden immer dem in SchILD eingestelltem aktuellen Jahrgang & Abschnitt zugeordnet!"<<endl<<endl;
		
			tosK<<"KursBez|Jahr|Abschnitt|Jahrgang|Fach|Kursart|Wochenstd.|Wochenstd. KL|Kursleiter"<<endl;
			tosKL<<"KursBez|Jahr|Abschnitt|Lehrkraft|Wochenstd"<<endl;
			tosLS<<"Lehrkraft|Zeitart|Grund|Anzahl Stunden"<<endl;
			Activity* acti;
			for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
				acti=gt.rules.activitiesList[ai];
				if(acti->active){
					if((acti->activityGroupId==acti->id)||(acti->activityGroupId==0)){
						added=false;
						if((acti->studentsNames.size()>0) && (acti->teachersNames.size()>0) ){
							added=true;
							kurs=false;
							nurEinJahrgang=true;
							kursbezeichnung.clear();
							for(int s=0; s<acti->studentsNames.size(); s++){
								tmp.clear();
								tmp=acti->studentsNames[s];
								tmp.remove(' ');
								tmp=tmp.toUpper();
								assert(tmp.size()>0);	// I am not sure. is that possible?
								if((tmp.at(0)<'0')||(tmp.at(0)>'9'))
									tosE<<"Warnung: Bei Aktivität "<<acti->id<<" beginnt der Jahrgang nicht mit einer Zahl!"<<endl;
								if(tmp.size()>1){
									if((tmp.at(1)<'0')||(tmp.at(1)>'9')){
										tmp="0"+tmp;
										if(acti->studentsNames[s].size()>2)
											kurs=true;
									} else {
										if(acti->studentsNames[s].size()>3||acti->studentsNames[s].size()==2)
											kurs=true;
									}
								} else {
									tmp="0"+tmp;
									kurs=true;
								}
								if(s==0)
									kursbezeichnung=tmp;
								else
									kursbezeichnung+="_"+tmp;
								if(tmp.at(0)!=kursbezeichnung.at(0)||tmp.at(1)!=kursbezeichnung.at(1)){
									kurs=true;
									nurEinJahrgang=false;
								}
							}
							if(acti->studentsNames.size()>1)
								kurs=true;
							if(kurs)
								kursbezeichnung+="_"+acti->subjectName;		//TODO: dann sind auch AGs WP?! ***
							if(kursbezeichnung.size()>20){
								tosE<<"Warnung: Bei Aktivität "<<acti->id<<" ist die Kurzbezeichnung zu groß."<<endl;
								tosK<<qPrintable(kursbezeichnung.left(20));
							}
							else tosK<<qPrintable(kursbezeichnung);
							tosK<<"|";
							tosK<<jahr;
							tosK<<"|";
							tosK<<abschnitt;
							tosK<<"|";
							if(nurEinJahrgang)
								tosK<<qPrintable(kursbezeichnung.left(2));
							tosK<<"|";
							if(acti->subjectName.size()>20){
								tosE<<"Warnung: Bei Aktivität "<<acti->id<<" ist das Unterrichtsfach zu groß."<<endl;
								tosK<<qPrintable(acti->subjectName.left(20));
							}
							else tosK<<qPrintable(acti->subjectName);
							tosK<<"|";
							if(kurs){
								if(kursbezeichnung.contains(acti->subjectName)) //TODO: dann sind auch AGs WP?! ***
									tosK<<"WP";
								else tosK<<"PUT";
							}
							tosK<<"|";
							tosK<<acti->totalDuration;
							tosK<<"|";
							// Wochenstunden des Kursleiters
							tosK<<"|";
							if(acti->teachersNames[0].size()>10){
								tosE<<"Warnung: Bei Aktivität "<<acti->id<<" ist der Lehrername zu groß."<<endl;
								tosK<<qPrintable(acti->teachersNames[0].left(10));
							}
							else tosK<<qPrintable(acti->teachersNames[0]);
							for(int t=1; t<acti->teachersNames.size(); t++){ // This is correct, not the first teacher!
								if(kurs){
									if(kursbezeichnung.size()>20){
										tosKL<<qPrintable(kursbezeichnung.left(20));
									}
									else tosKL<<qPrintable(kursbezeichnung);
									tosKL<<"|";
									tosKL<<jahr;
									tosKL<<"|";
									tosKL<<abschnitt;
									tosKL<<"|";
									if(acti->teachersNames[t].size()>10){
										tosE<<"Warnung: Bei Aktivität "<<acti->id<<" ist ein Lehrername zu groß."<<endl;
										tosKL<<qPrintable(acti->teachersNames[t].left(10));
									} else tosKL<<qPrintable(acti->teachersNames[t]);
									tosKL<<"|";
									tosKL<<acti->totalDuration;
									tosKL<<endl;
								} else tosE<<"Warnung: Aktivität "<<acti->id<<" wurde nicht hinzugefügt."<<" Klassenteamteaching!"<<endl;
							}
							tosK<<"|";
							if(kurs)
								tosK<<"N";
							else tosK<<"J";
							tosK<<endl;
						}
						if(acti->activityTagName.size()==3 && (acti->teachersNames.size()>0)){
							if((acti->activityTagName.at(0)>='0')&&(acti->activityTagName.at(0)<='9')
							&&(acti->activityTagName.at(1)>='0')&&(acti->activityTagName.at(1)<='9')
							&&(acti->activityTagName.at(2)>='0')&&(acti->activityTagName.at(2)<='9')){
								added=true;
								for(int t=0; t<acti->teachersNames.size(); t++){ 
									if(acti->teachersNames[t].size()>10){
										tosE<<"Warnung: Bei Aktivität "<<acti->id<<" ist ein Lehrername zu groß."<<endl;
										tosLS<<qPrintable(acti->teachersNames[t].left(10));
									} else tosLS<<qPrintable(acti->teachersNames[t]);
									tosLS<<"|";
									if(acti->studentsNames.size()>0)	//validiere MEHRLEISTUNG ANRECHNUNG oder MINDERLEISTUNG anhand des ASD-Schlüssels
										tosLS<<"MEHRLEISTUNG";
									else tosLS<<"MINDERLEISTUNG";
									tosLS<<"|";
									tosLS<<qPrintable(acti->activityTagName);	//ASD-Schlüssel
									tosLS<<"|";
									tosLS<<acti->totalDuration;
									tosLS<<endl;
								}
							}
						}
						if((acti->studentsNames.size()>0) && (acti->teachersNames.size()==0) ){	//TODO: care about that activities!
							//added=true;
							tosE<<"Warnung: Aktivität "<<acti->id<<" wurde nicht hinzugefügt."<<" Schülerkopplung?"<<endl;
						}
						if((acti->studentsNames.size()==0) && (acti->teachersNames.size()==0) ){ //TODO: care about that activities!
							//added=true;
							tosE<<"Warnung: Aktivität "<<acti->id<<" wurde nicht hinzugefügt."<<" Raumkopplung?"<<endl;
						}
						if((acti->studentsNames.size()==0) && (acti->teachersNames.size()>0) ){	//TODO: care about that activities!
							if(added==false){
								//added=true;
								tosE<<"Warnung: Aktivität "<<acti->id<<" wurde nicht hinzugefügt."<<" Lehrerkopplung?"<<endl;
							} else tosE<<"Hinweis: Aktivität "<<acti->id<<" wurde nur in LehrkraefteSonderzeiten.dat hinzugefügt."<<" Zusätzlich Lehrerkopplung?"<<endl;
						}
					}
				} else not_active++;
				
			}
			if(not_active!=0)
				tosE<<endl<<"Hinweis: "<<not_active<<" Aktivitäten sind nicht aktiv."<<endl;
			tosE<<endl<<"Ende der Datei."<<endl<<endl;
		
			//TODO: If i output 2 or more same activities like this:
			// 05a|2008|1|05|MA||4||Tanja|
			// 05a|2008|1|05|MA||1||Tanja|
			// then i need to care about that this is just output as a single activity like this:
			// 05a|2008|1|05|MA||5||Tanja|
			// I don't need to care about that, if activities are enterd with an matrix!
			//TODO: check same starting time
		
		
			if(fileE.error()>0){
				lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(directoryname).arg(fileE.error())+"\n";
				return false;
			}
			fileE.close();
		
			if(fileLS.error()>0){
				lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(directoryname).arg(fileLS.error())+"\n";
				return false;
			}
			fileLS.close();
		
			if(fileKL.error()>0){
				lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(directoryname).arg(fileKL.error())+"\n";
				return false;
			}
			fileKL.close();
		
			if(fileK.error()>0){
				lastWarnings+=Export::tr("FET critical. Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(directoryname).arg(fileK.error())+"\n";
				return false;
			}
			fileK.close();
		}
		if(ok) QMessageBox::information(NULL, QObject::tr("FET information"), Export::tr("TODO: write content of file \"Exportprotokoll.log\" here."));
		else QMessageBox::warning(NULL, QObject::tr("FET warning"), Export::tr("User abort export"));
	}
	return true;
}
