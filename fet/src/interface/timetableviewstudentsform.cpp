/***************************************************************************
                          timetableviewstudentsform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
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

#include "tablewidgetupdatebug.h"

#include "longtextmessagebox.h"

#include "fetmainform.h"
#include "timetableviewstudentsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "solution.h"

#include "lockunlock.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <q3textedit.h>
#include <qstring.h>

#include <QSplitter>
#include <QList>

#include <QDesktopWidget>

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QCoreApplication>

#include <QString>
#include <QStringList>

/*
#include <iostream>
#include <fstream>
using namespace std;*/

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;

extern bool simulation_running;

extern Solution best_solution;

extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern QSet <int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet <int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet <int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet <int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

TimetableViewStudentsForm::TimetableViewStudentsForm()
{
    setupUi(this);
    
	//columnResizeModeInitialized=false;

    QList<int> tmpList;
    tmpList<<10000<<2500;
    splitter->setSizes(tmpList);
    
    QList<int> tmpList2;
    tmpList2<<3000<<10000;
    splitter_2->setSizes(tmpList2);
    
    studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

	groupsListBox->clear();
	subgroupsListBox->clear();

    connect(yearsListBox, SIGNAL(highlighted(QString)), this /*TimetableViewStudentsForm_template*/, SLOT(yearChanged(QString)));
    connect(groupsListBox, SIGNAL(highlighted(QString)), this /*TimetableViewStudentsForm_template*/, SLOT(groupChanged(QString)));
    connect(subgroupsListBox, SIGNAL(highlighted(QString)), this /*TimetableViewStudentsForm_template*/, SLOT(subgroupChanged(QString)));
    connect(closePushButton, SIGNAL(clicked()), this /*TimetableViewStudentsForm_template*/, SLOT(close()));
    connect(studentsTimetableTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(detailActivity(QTableWidgetItem*)));
    connect(lockTimePushButton, SIGNAL(clicked()), this /*TimetableViewStudentsForm_template*/, SLOT(lockTime()));
    connect(lockSpacePushButton, SIGNAL(clicked()), this /*TimetableViewStudentsForm_template*/, SLOT(lockSpace()));
    connect(lockTimeSpacePushButton, SIGNAL(clicked()), this /*TimetableViewStudentsForm_template*/, SLOT(lockTimeSpace()));

    connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

//////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//these lines are not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	LockUnlock::increaseCommunicationSpinBox();


	studentsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
	studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		studentsTimetableTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		studentsTimetableTable->setVerticalHeaderItem(i, item);
	}
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			studentsTimetableTable->setItem(j, k, item);

			//if(j==0 && k==0)
			//	teachersTimetableTable->setCurrentItem(item);
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
	studentsTimetableTable->horizontalHeader()->setMinimumSectionSize(studentsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	//}
	///////////////
	
	yearsListBox->clear();
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		yearsListBox->insertItem(sty->name);
	}
	//yearChanged(yearsListBox->currentText());
	if(yearsListBox->count()>0){
		yearsListBox->setCurrentItem(0);
		yearsListBox->setSelected(0, true);
	}

	//added by Volker Dirr
	connect(&communicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateStudentsTimetableTable()));
	
	//studentsTimetableTable->setSelectionMode(Q3Table::NoSelection);
}

TimetableViewStudentsForm::~TimetableViewStudentsForm()
{
}

void TimetableViewStudentsForm::resizeRowsAfterShow()
{
	studentsTimetableTable->resizeRowsToContents();
//	tableWidgetUpdateBug(studentsTimetableTable);
}

void TimetableViewStudentsForm::yearChanged(const QString &yearName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		//groupsListBox->clear();
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(yearName==QString::null)
		return;
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;
	
		/*if(gt.rules.augmentedYearsList.size()>0)
			yearIndex=0;
		else
			return;*/
	}

	groupsListBox->clear();
	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListBox->insertItem(stg->name);
	}

	//groupChanged(groupsListBox->currentText());
	if(groupsListBox->count()>0){
		groupsListBox->setCurrentItem(0);
		groupsListBox->setSelected(0, true);
	}
}

void TimetableViewStudentsForm::groupChanged(const QString &groupName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(groupName==QString::null)
		return;

	QString yearName=yearsListBox->currentText();
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;

		/*if(gt.rules.augmentedYearsList.size()>0){
			yearIndex=0;
			yearName=gt.rules.augmentedYearsList.at(0)->name;
		}
		else
			return;*/
	}

	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	int groupIndex=gt.rules.searchAugmentedGroup(yearName, groupName);
	if(groupIndex<0){
		QMessageBox::warning(this, tr("FET warning"),
		 tr("Invalid group in the selected year, or the groups of the current year are not updated")+
		 "\n\n"+
		 tr("Solution: please try to select a different year and after that select the current year again, "
		 "to refresh the groups list, or close this dialog and open again the students view timetable dialog"));
		return;
		
		/*if(sty->groupsList.size()>0)
			groupIndex=0;
		else
			return;*/
	}

	subgroupsListBox->clear();
	
	StudentsGroup* stg=sty->groupsList.at(groupIndex);
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListBox->insertItem(sts->name);
	}

	if(subgroupsListBox->count()>0){
		subgroupsListBox->setCurrentItem(0);
		subgroupsListBox->setSelected(0, true);
	}
}

void TimetableViewStudentsForm::subgroupChanged(const QString &subgroupName)
{
	Q_UNUSED(subgroupName);
	//if(subgroupName!="")
	//	;	

	updateStudentsTimetableTable();
}

void TimetableViewStudentsForm::updateStudentsTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	
	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null)
		return;
	if(groupsListBox->currentText()==QString::null)
		return;
	if(subgroupsListBox->currentText()==QString::null)
		return;

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	if( ! ((StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname)) ){
		QMessageBox::information(this, tr("FET warning"), tr("You have an old timetable view students dialog opened - please close it"));
		return;
	}

	s="";
/*	s = tr("Students");
	s += ": ";*/
	//s += yearname;
	//s += ", ";
	//s += groupname;
	//s += ", ";
	s += subgroupname;

	classNameTextLabel->setText(s);

	assert(gt.rules.initialized);

/*	studentsTimetableTable->setNumRows(gt.rules.nHoursPerDay);
	studentsTimetableTable->setNumCols(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		studentsTimetableTable->horizontalHeader()->setLabel(j, gt.rules.daysOfTheWeek[j]);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		studentsTimetableTable->verticalHeader()->setLabel(i, gt.rules.hoursOfTheDay[i]);*/

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			s="";
			int ai=students_timetable_weekly[i][k][j]; //activity index
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=NULL);
				
				QString ats=act->activityTagsNames.join(", ");
				s+=act->subjectName +" "+ ats;
				
				if(act->teachersNames.count()>0){
					s+="\n";
					s+=act->teachersNames.join(", ");
				}
				
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					//s+=" ";
					//s+=tr("R:%1", "Room").arg(gt.rules.internalRoomsList[r]->name);
					s+="\n";
					s+=gt.rules.internalRoomsList[r]->name;
				}

				//added by Volker Dirr (start)
				QString descr="";
				QString t="";
				if(idsOfPermanentlyLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "PLT", "Abbreviation for permanently locked time. There are 4 string: permanently locked time, permanently locked space, "
						"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					t=", ";
				}
				else if(idsOfLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "LT", "Abbreviation for locked time. There are 4 string: permanently locked time, permanently locked space, "
						"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					t=", ";
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id)){
					descr+=t+QCoreApplication::translate("TimetableViewForm", "PLS", "Abbreviation for permanently locked space. There are 4 string: permanently locked time, permanently locked space, "
						"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
				}
				else if(idsOfLockedSpace.contains(act->id)){
					descr+=t+QCoreApplication::translate("TimetableViewForm", "LS", "Abbreviation for locked space. There are 4 string: permanently locked time, permanently locked space, "
						"locked time, locked space. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
				}
				if(descr!=""){
					descr.prepend("\n(");
					descr.append(")");
				}
				s+=descr;
				//added by Volker Dirr (end)
			}
			else{
				if((subgroupNotAvailableDayHour[i][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					s+="-x-";
			}
			studentsTimetableTable->item(j, k)->setText(s);
		}
	}
//	for(int i=0; i<gt.rules.nHoursPerDay; i++)
//		studentsTimetableTable->adjustRow(i); //added in version 3_9_16, on 16 Oct. 2004

	studentsTimetableTable->resizeRowsToContents();
	
	tableWidgetUpdateBug(studentsTimetableTable);
	
	//cout<<"timetableviewstudentsform updated form."<<endl;
	
	detailActivity(studentsTimetableTable->currentItem());
}

void TimetableViewStudentsForm::resizeEvent(QResizeEvent* event){
	QDialog::resizeEvent(event);

	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsForm::detailActivity(QTableWidgetItem* item)
{
	if(item==NULL){
		//detailsTextEdit->setText(tr("Invalid selected cell"));
		detailsTextEdit->setText(QString(""));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null)
		return;
	if(groupsListBox->currentText()==QString::null)
		return;
	if(subgroupsListBox->currentText()==QString::null)
		return;

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
/*	if(!(i<gt.rules.nInternalSubgroups)){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students dialog"));
		return;
	}*/
	assert(i<gt.rules.nInternalSubgroups);
	int j=item->row();
	int k=item->column();
	s="";
	if(j>=0 && k>=0){
		int ai=students_timetable_weekly[i][k][j]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=NULL);
			//s+=act->getDetailedDescriptionWithConstraints(gt.rules);
			s+=act->getDetailedDescription(gt.rules);

			//int r=rooms_timetable_weekly[i][k][j];
			int r=best_solution.rooms[ai];
			if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
				s+="\n";
				s+=tr("Room: %1").arg(gt.rules.internalRoomsList[r]->name);
			}
			//added by Volker Dirr (start)
			QString descr="";
			QString t="";
			if(idsOfPermanentlyLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "permanently locked time", "refers to activity");
				t=", ";
			}
			else if(idsOfLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "locked time", "refers to activity");
				t=", ";
			}
			if(idsOfPermanentlyLockedSpace.contains(act->id)){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "permanently locked space", "refers to activity");
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
		}
		else{
			if(subgroupNotAvailableDayHour[i][k][j]){
				s+=tr("Students subgroup is not available 100% in this slot");
				s+="\n";
			}
			if(breakDayHour[k][j]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setText(s);
}

void TimetableViewStudentsForm::lockTime()
{
	lock(true, false);
}

void TimetableViewStudentsForm::lockSpace()
{
	lock(false, true);
}

void TimetableViewStudentsForm::lockTimeSpace()
{
	lock(true, true);
}

void TimetableViewStudentsForm::lock(bool lockTime, bool lockSpace)
{
	//cout<<"students begin, isc="<<gt.rules.internalStructureComputed<<endl;

	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	//find subgroup index
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString yearname;
	QString groupname;
	QString subgroupname;

	if(yearsListBox->currentText()==QString::null){
		QMessageBox::information(this, tr("FET information"), tr("Please select a year"));
		return;
	}
	if(groupsListBox->currentText()==QString::null){
		QMessageBox::information(this, tr("FET information"), tr("Please select a group"));
		return;
	}
	if(subgroupsListBox->currentText()==QString::null){
		QMessageBox::information(this, tr("FET information"), tr("Please select a subgroup"));
		return;
	}

	yearname = yearsListBox->currentText();
	groupname = groupsListBox->currentText();
	subgroupname = subgroupsListBox->currentText();

	Solution* tc=&best_solution;

	StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}
	assert(sts);
	int i;
	for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i<gt.rules.nInternalSubgroups);

	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	//lock selected activities
	QSet <int> careAboutIndex;		//added by Volker Dirr. Needed, because of activities with duration > 1
	careAboutIndex.clear();
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(studentsTimetableTable->item(j, k)->isSelected()){
				int ai=students_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY && !careAboutIndex.contains(ai)){	//modified, because of activities with duration > 1
					careAboutIndex.insert(ai);					//Needed, because of activities with duration > 1
					int a_tim=tc->times[ai];
					int hour=a_tim/gt.rules.nDaysPerWeek;
					int day=a_tim%gt.rules.nDaysPerWeek;
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(lockTime){
						ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false);
						bool t=gt.rules.addTimeConstraint(ctr);
						QString s;
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							addedT++;
							idsOfLockedTime.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<TimeConstraint*> tmptc;
							tmptc.clear();
							int count=0;
							foreach(TimeConstraint* tc, gt.rules.timeConstraintsList){
								if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
									ConstraintActivityPreferredStartingTime* c=(ConstraintActivityPreferredStartingTime*) tc;
									if(c->activityId==act->id && tc->weightPercentage==100.0 && c->day>=0 && c->hour>=0){
										count++;
										if(c->permanentlyLocked){
											if(idsOfLockedTime.contains(c->activityId) || !idsOfPermanentlyLockedTime.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												);
											}
											else{
												s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
											}
										}
										else {
											if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												);
											}
											else{
												tmptc.append(tc);
											}
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expects to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

							foreach(TimeConstraint* deltc, tmptc){
								s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeTimeConstraint(deltc);
								//delete deltc; - this is done by rules.remove...
								idsOfLockedTime.remove(act->id);
								unlockedT++;
							}
							tmptc.clear();
							//gt.rules.internalStructureComputed=false;
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							int k;
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
		 					if(k==0)
								report=false;
						}
					}

					int ri=tc->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, false);
						bool t=gt.rules.addSpaceConstraint(ctr);

						QString s;
						
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							addedS++;
							idsOfLockedSpace.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<SpaceConstraint*> tmpsc;
							tmpsc.clear();
							int count=0;
							foreach(SpaceConstraint* sc, gt.rules.spaceConstraintsList){
								if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
									ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) sc;
									if(c->activityId==act->id && sc->weightPercentage==100.0){
										count++;
										if(c->permanentlyLocked){
											if(idsOfLockedSpace.contains(c->activityId) || !idsOfPermanentlyLockedSpace.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												);
											}
											else{
												s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
											}
										}
										else {
											if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
												QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
												 +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
												 +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
												 +"\n\n"+tr("Please report possible bug")
												);
											}
											else
												tmpsc.append(sc);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expects to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

							foreach(SpaceConstraint* delsc, tmpsc){
								s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeSpaceConstraint(delsc);
								idsOfLockedSpace.remove(act->id);
								unlockedS++;
								//delete delsc; done by rules.remove...
							}
							tmpsc.clear();
							//gt.rules.internalStructureComputed=false;
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							int k;
							if(t)
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							else
								k=QMessageBox::information(this, tr("FET information"), s,
							 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							
		 					if(k==0)
								report=false;
						}
					}
				}
			}
		}
	}
	
	QStringList added;
	QStringList removed;
	if(addedT>0){
		if(addedT==1)
			added << QCoreApplication::translate("TimetableViewForm", "Added 1 locking time constraint.", "constraint is singular (only 1 constraint)");
		else
			added << QCoreApplication::translate("TimetableViewForm", "Added %1 locking time constraints.", "%1 is >= 2, so constraints is plural").arg(addedT);
	}
	if(addedS>0){
		if(addedS==1)
			added << QCoreApplication::translate("TimetableViewForm", "Added 1 locking space constraint.", "constraint is singular (only 1 constraint)");
		else
			added << QCoreApplication::translate("TimetableViewForm", "Added %1 locking space constraints.", "%1 is >= 2, so constraints is plural").arg(addedS);
	}
	if(unlockedT>0){
		if(unlockedT==1)
			removed << QCoreApplication::translate("TimetableViewForm", "Removed 1 locking time constraint.", "constraint is singular (only 1 constraint)");
		else
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %1 locking time constraints.", "%1 is >= 2, so constraints is plural").arg(unlockedT);
	}
	if(unlockedS>0){
		if(unlockedS==1)
			removed << QCoreApplication::translate("TimetableViewForm", "Removed 1 locking space constraint.", "constraint is singular (only 1 constraint)");
		else
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %1 locking space constraints.", "%1 is >= 2, so constraints is plural").arg(unlockedS);
	}
	QString ad=added.join("\n");
	QString re=removed.join("\n");
	QStringList all;
	if(!ad.isEmpty())
		all<<ad;
	if(!re.isEmpty())
		all<<re;
	QString s=all.join("\n\n");
	if(s.isEmpty())
		s=QCoreApplication::translate("TimetableViewForm", "No locking constraints added or removed.");
	QMessageBox::information(this, tr("FET information"), s);

///////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //just to make sure, not really needed, but to test better
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	LockUnlock::increaseCommunicationSpinBox(); //this is needed
	
	//cout<<"students end, isc="<<gt.rules.internalStructureComputed<<endl;
	//cout<<endl;
}

void TimetableViewStudentsForm::help()
{
	QString s="";
	//s+=QCoreApplication::translate("TimetableViewForm", "You can drag sections to increase/decrease them.");
	//s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Lock/unlock: you can select one or more activities in the table and toggle lock/unlock in time, space or both.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "There will be added or removed locking constraints for the selected activities (they can be unlocked only if they are not permanently locked).");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Locking time constraints are constraints of type activity preferred starting time. Locking space constraints are constraints of type"
		" activity preferred room. You can see these constraints in the corresponding constraints dialogs. New locking constraints are added at the end of the list of constraints.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "If a cell is (permanently) locked in time or space, it contains abbreviations to show that: PLT (permanently locked time), LT (locked time), "
		"PLS (permanently locked space) or LS (locked space).", "Translate the abbreviations also. Make sure the abbreviations in your language are different between themselves "
		"and the user can differentiate easily between them. These abbreviations may appear also in other places, please use the same abbreviations.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
