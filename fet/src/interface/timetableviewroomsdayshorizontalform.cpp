/***************************************************************************
                          timetableviewroomsdayshorizontalform.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <Qt>

#include <QtGlobal>

#include "longtextmessagebox.h"

#include "fetmainform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "lockunlock.h"

#include "matrix.h"

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QListWidget>

#include <QList>

#include <QCoreApplication>
#include <QApplication>

#include <QString>
#include <QStringList>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

//begin by Marco Vassura
#include <QBrush>
#include <QColor>
//end by Marco Vassura

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern bool simulation_running;
extern bool simulation_running_multi;

extern Solution best_solution;

extern Matrix2D<double> notAllowedRoomTimePercentages;
extern Matrix2D<bool> breakDayHour;

extern QSet<int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet<int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

TimetableViewRoomsDaysHorizontalForm::TimetableViewRoomsDaysHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	//verticalSplitter->setStretchFactor(0, 1);	//unneeded, because both have the same value
	//verticalSplitter->setStretchFactor(1, 1);	//unneeded, because both have the same value
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);
	
	roomsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(roomsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(roomChanged(const QString&)));
	connect(roomsTimetableTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimeSpacePushButton, SIGNAL(clicked()), this, SLOT(lock()));
	connect(lockTimePushButton, SIGNAL(clicked()), this, SLOT(lockTime()));
	connect(lockSpacePushButton, SIGNAL(clicked()), this, SLOT(lockSpace()));

	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());

//////////TODO
/*    double time_start = get_time();
*/
//////////

/////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//these 2 lines are not really needed - we just keep them to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	//Commented on 2018-07-20
	//LockUnlock::increaseCommunicationSpinBox();

//////////TODO
/*
    double time_end = get_time();
    cout<<"This program has run for: "<<time_end-time_start<<" seconds."<<endl;
*/
//////////

	roomsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
	roomsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		roomsTimetableTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		roomsTimetableTable->setVerticalHeaderItem(i, item);
	}

	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			
			roomsTimetableTable->setItem(j, k, item);
			
		//	if(j==0 && k==0)
		//		roomsTimetableTable->setCurrentItem(item);
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
	roomsTimetableTable->horizontalHeader()->setMinimumSectionSize(roomsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	roomsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	roomsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	///////////////

	roomsListWidget->clear();
	
	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
	}
	else{
		for(int i=0; i<gt.rules.nInternalRooms; i++)
			roomsListWidget->addItem(gt.rules.internalRoomsList[i]->name);
	}
	
	if(roomsListWidget->count()>0)
		roomsListWidget->setCurrentRow(0);

	//added by Volker Dirr
	connect(&communicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateRoomsTimetableTable()));
}

void TimetableViewRoomsDaysHorizontalForm::newTimetableGenerated()
{
	/*setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	//verticalSplitter->setStretchFactor(0, 1);	//unneeded, because both have the same value
	//verticalSplitter->setStretchFactor(1, 1);	//unneeded, because both have the same value
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);
	
	roomsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(roomsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(roomChanged(const QString&)));
	connect(roomsTimetableTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimeSpacePushButton, SIGNAL(clicked()), this, SLOT(lock()));
	connect(lockTimePushButton, SIGNAL(clicked()), this, SLOT(lockTime()));
	connect(lockSpacePushButton, SIGNAL(clicked()), this, SLOT(lockSpace()));

	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());
*/

//////////TODO
/*    double time_start = get_time();
*/
//////////

/////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//these 2 lines are not really needed - we just keep them to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	//DON'T UNCOMMENT THIS CODE -> LEADS TO CRASH IF THERE ARE MORE VIEWS OPENED.
	//LockUnlock::increaseCommunicationSpinBox();

//////////TODO
/*
    double time_end = get_time();
    cout<<"This program has run for: "<<time_end-time_start<<" seconds."<<endl;
*/
//////////

	roomsTimetableTable->clear();
	roomsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
	roomsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		roomsTimetableTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		roomsTimetableTable->setVerticalHeaderItem(i, item);
	}

	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			
			roomsTimetableTable->setItem(j, k, item);
			
		//	if(j==0 && k==0)
		//		roomsTimetableTable->setCurrentItem(item);
		}
	}
	
/*
	//resize columns
	//if(!columnResizeModeInitialized){
	roomsTimetableTable->horizontalHeader()->setMinimumSectionSize(roomsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	roomsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	roomsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	///////////////
*/

	disconnect(roomsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(roomChanged(const QString&)));
	roomsListWidget->clear();
	connect(roomsListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(roomChanged(const QString&)));
	
	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		assert(0);
		//QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
	}
	else{
		for(int i=0; i<gt.rules.nInternalRooms; i++)
			roomsListWidget->addItem(gt.rules.internalRoomsList[i]->name);
	}
	
	if(roomsListWidget->count()>0)
		roomsListWidget->setCurrentRow(0);

	//added by Volker Dirr
	//connect(&communicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateRoomsTimetableTable()));
}

TimetableViewRoomsDaysHorizontalForm::~TimetableViewRoomsDaysHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-splitter-state"), verticalSplitter->saveState());

	//save horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());
}

void TimetableViewRoomsDaysHorizontalForm::resizeRowsAfterShow()
{
	roomsTimetableTable->resizeRowsToContents();
}

void TimetableViewRoomsDaysHorizontalForm::roomChanged(const QString& roomName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	if(roomName==QString())
		return;

	int roomIndex=gt.rules.searchRoom(roomName);
	if(roomIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms timetable dialog"));
		return;
	}

	updateRoomsTimetableTable();
}

void TimetableViewRoomsDaysHorizontalForm::updateRoomsTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable "
		"or close the timetable view rooms dialog"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable "
		"or close the timetable view rooms dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	QString roomName;

	if(roomsListWidget->currentRow()<0 || roomsListWidget->currentRow()>=roomsListWidget->count())
		return;

	roomName = roomsListWidget->currentItem()->text();
	s = roomName;
	roomNameTextLabel->setText(s);
	
	int roomIndex=gt.rules.searchRoom(roomName);

	if(roomIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("You have an old timetable view rooms dialog opened - please close it"));
		return;
	}

	assert(gt.rules.initialized);

	assert(roomIndex>=0);

	for(int j=0; j<gt.rules.nHoursPerDay && j<roomsTimetableTable->rowCount(); j++){
		for(int k=0; k<gt.rules.nDaysPerWeek && k<roomsTimetableTable->columnCount(); k++){
			QFont font(roomsTimetableTable->item(j, k)->font());
			font.setBold(false);
			font.setItalic(false);
			font.setUnderline(false);
			roomsTimetableTable->item(j, k)->setFont(font);

			//begin by Marco Vassura
			// add colors (start)
			//if(USE_GUI_COLORS) {
			roomsTimetableTable->item(j, k)->setBackground(roomsTimetableTable->palette().color(QPalette::Base));
			roomsTimetableTable->item(j, k)->setForeground(roomsTimetableTable->palette().color(QPalette::Text));
			//}
			// add colors (end)
			//end by Marco Vassura
			s = "";
			QString longString="";
			int ai=rooms_timetable_weekly[roomIndex][k][j]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=nullptr);
				
				if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
					QString ats=act->activityTagsNames.join(", ");
					s += act->subjectName + " " + ats;
				}
				else{
					s += act->subjectName;
				}
				
				if(act->teachersNames.count()>0){
					s+="\n";
					s+=act->teachersNames.join(", ");
				}
				
				if(act->studentsNames.count()>0){
					s+="\n";
					s+=act->studentsNames.join(", ");
				}
				
				//added by Volker Dirr (start)
				QString descr="";
				QString tt="";
				if(idsOfPermanentlyLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "PLT", "Abbreviation for permanently locked time. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				else if(idsOfLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "LT", "Abbreviation for locked time. There are 5 strings: permanently locked time, permanently locked space, "
					"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id)){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "PLS", "Abbreviation for permanently locked space. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				else if(idsOfLockedSpace.contains(act->id)){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "LS", "Abbreviation for locked space. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "PD", "Abbreviation for preferred day. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
				}
				if(descr!=""){
					descr.prepend("\n(");
					//descr.prepend(" ");
					descr.append(")");
				}

				longString=s+descr;
				//added by Volker Dirr (end)

				if(idsOfPermanentlyLockedTime.contains(act->id) || idsOfLockedTime.contains(act->id)){
					QFont font(roomsTimetableTable->item(j, k)->font());
					font.setBold(true);
					roomsTimetableTable->item(j, k)->setFont(font);
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id) || idsOfLockedSpace.contains(act->id)){
					QFont font(roomsTimetableTable->item(j, k)->font());
					font.setItalic(true);
					roomsTimetableTable->item(j, k)->setFont(font);
				}
				if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
					QFont font(roomsTimetableTable->item(j, k)->font());
					font.setUnderline(true);
					roomsTimetableTable->item(j, k)->setFont(font);
				}

				//begin by Marco Vassura
				// add colors (start)
				if(USE_GUI_COLORS /*&& act->studentsNames.count()>0*/){
					QBrush bg(stringToColor(act->subjectName+" "+act->studentsNames.join(", ")));
					roomsTimetableTable->item(j, k)->setBackground(bg);
					double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
					if (brightness<0.5)
						roomsTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::White));
					else
						roomsTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::Black));
#else
					if (brightness<0.5)
						roomsTimetableTable->item(j, k)->setForeground(QBrush(Qt::white));
					else
						roomsTimetableTable->item(j, k)->setForeground(QBrush(Qt::black));
#endif
				}
				// add colors (end)
				//end by Marco Vassura
			}
			else{
				if(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]>=0 && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					s+="-x-";
				else if(breakDayHour[k][j] && PRINT_BREAK_TIME_SLOTS)
					s+="-X-";
			}
			roomsTimetableTable->item(j, k)->setText(s);
			roomsTimetableTable->item(j, k)->setToolTip(longString);
		}
	}
	//for(int i=0; i<gt.rules.nHoursPerDay; i++)
	//	roomsTimetableTable->adjustRow(i); //added in version 3_12_20

	roomsTimetableTable->resizeRowsToContents();
	
	//cout<<"timetableviewroomsform updated form."<<endl;
	
	detailActivity(roomsTimetableTable->currentItem());
}

void TimetableViewRoomsDaysHorizontalForm::resizeEvent(QResizeEvent* event){
	QDialog::resizeEvent(event);

	roomsTimetableTable->resizeRowsToContents();
}

//begin by Marco Vassura
//slightly modified by Liviu Lalescu on 2021-03-01
QColor TimetableViewRoomsDaysHorizontalForm::stringToColor(const QString& s)
{
	// CRC-24 based on RFC 2440 Section 6.1
	unsigned long int crc = 0xB704CEUL;
	QByteArray ba=s.toUtf8();
	for(char c : qAsConst(ba)){
		unsigned char uc=(unsigned char)(c);
		crc ^= (uc & 0xFF) << 16;
		for (int i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000UL)
				crc ^= 0x1864CFBUL;
		}
	}
	return QColor::fromRgb(int((crc>>16) & 0xFF), int((crc>>8) & 0xFF), int(crc & 0xFF));
}
//end by Marco Vassura

void TimetableViewRoomsDaysHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewRoomsDaysHorizontalForm::detailActivity(QTableWidgetItem* item){
	if(item==nullptr){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}
	
	if(item->row()>=gt.rules.nHoursPerDay || item->column()>=gt.rules.nDaysPerWeek){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable "
		"or close the timetable view rooms dialog"));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	QString roomName;

	if(roomsListWidget->currentRow()<0 || roomsListWidget->currentRow()>=roomsListWidget->count())
		return;

	roomName = roomsListWidget->currentItem()->text();
	s = roomName;
	roomNameTextLabel->setText(s);

	int roomIndex=gt.rules.searchRoom(roomName);

	if(roomIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms dialog"));
		return;
	}

	assert(roomIndex>=0);
	int j=item->row();
	int k=item->column();
	s = "";
	if(j>=0 && k>=0){
		int ai=rooms_timetable_weekly[roomIndex][k][j]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=nullptr);
			s += act->getDetailedDescriptionWithConstraints(gt.rules);
			//s += act->getDetailedDescription(gt.rules);

			int r=best_solution.rooms[ai];
			if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
				s+="\n";
				s+=tr("Room: %1").arg(gt.rules.internalRoomsList[r]->name);
				
				if(gt.rules.internalRoomsList[r]->isVirtual==true){
					QStringList tsl;
					for(int i : qAsConst(best_solution.realRoomsList[ai]))
						tsl.append(gt.rules.internalRoomsList[i]->name);
					s+=QString(" (")+tsl.join(", ")+QString(")");
				}
				
				if(gt.rules.internalRoomsList[r]->building!=""){
					s+="\n";
					s+=tr("Building=%1").arg(gt.rules.internalRoomsList[r]->building);
				}
				s+="\n";
				s+=tr("Capacity=%1").arg(gt.rules.internalRoomsList[r]->capacity);
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
				t=", ";
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
				t=", ";
			}
			if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "preferred day", "refers to activity");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
		}
		else{
			if(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]>=0){
				s+=tr("Room is not available with weight %1%").arg(CustomFETString::number(notAllowedRoomTimePercentages[roomIndex][k+j*gt.rules.nDaysPerWeek]));
				s+="\n";
			}
			if(breakDayHour[k][j]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setPlainText(s);
}

void TimetableViewRoomsDaysHorizontalForm::lock()
{
	this->lock(true, true);
}

void TimetableViewRoomsDaysHorizontalForm::lockTime()
{
	this->lock(true, false);
}

void TimetableViewRoomsDaysHorizontalForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewRoomsDaysHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(simulation_running || simulation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	//find room index
	QString roomName;
	if(roomsListWidget->currentRow()<0 || roomsListWidget->currentRow()>=roomsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a room"));
		return;
	}
	roomName = roomsListWidget->currentItem()->text();
	int i=gt.rules.searchRoom(roomName);

	/*if(!(rooms_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view rooms timetable dialog - please generate a new timetable"));
		return;
	}
	assert(rooms_schedule_ready);*/
	
	if(i<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid room - please close this dialog and open a new view rooms timetable dialog"));
		return;
	}
	
	Solution* c=&best_solution;
	
	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	//lock selected activities
	QSet<int> careAboutIndex;		//added by Volker Dirr. Needed, because of activities with duration > 1
	careAboutIndex.clear();
	for(int j=0; j<gt.rules.nHoursPerDay && j<roomsTimetableTable->rowCount(); j++){
		for(int k=0; k<gt.rules.nDaysPerWeek && k<roomsTimetableTable->columnCount(); k++){
			if(roomsTimetableTable->item(j, k)->isSelected()){
				int ai=rooms_timetable_weekly[i][k][j];
				if(ai!=UNALLOCATED_ACTIVITY && !careAboutIndex.contains(ai)){	//modified, because of activities with duration > 1
					careAboutIndex.insert(ai);					//Needed, because of activities with duration > 1
					int a_tim=c->times[ai];
					int hour=a_tim/gt.rules.nDaysPerWeek;
					int day=a_tim%gt.rules.nDaysPerWeek;

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

							for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(act->id, QSet<ConstraintActivityPreferredStartingTime*>())){
								assert(c->activityId==act->id);
								if(c->activityId==act->id && c->weightPercentage==100.0 && c->active && c->day>=0 && c->hour>=0){
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
									else{
										if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											  );
										}
										else{
											tmptc.append((TimeConstraint*)c);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

							for(TimeConstraint* deltc : qAsConst(tmptc)){
								s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeTimeConstraint(deltc);
								idsOfLockedTime.remove(act->id);
								unlockedT++;
								//delete deltc; - done by rules.removeTimeConstraint(...)
							}
							tmptc.clear();
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							/*int k;
							k=QMessageBox::information(this, tr("FET information"), s,
							 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							
							if(k==0)
								report=false;*/
							QMessageBox::StandardButton k;
							k=QMessageBox::information(this, tr("FET information"), s, QMessageBox::YesToAll | QMessageBox::Ok);
							
							if(k==QMessageBox::YesToAll)
								report=false;
						}
					}
					
					int ri=c->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
						QStringList tl;
						if(gt.rules.internalRoomsList[ri]->isVirtual==false)
							assert(c->realRoomsList[ai].isEmpty());
						else
							for(int rr : qAsConst(c->realRoomsList[ai]))
								tl.append(gt.rules.internalRoomsList[rr]->name);
						
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100.0, act->id, (gt.rules.internalRoomsList[ri])->name, tl, false);
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

							for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(act->id, QSet<ConstraintActivityPreferredRoom*>())){
								assert(c->activityId==act->id);
								if(c->activityId==act->id && c->weightPercentage==100.0 && c->active
								 && (gt.rules.internalRoomsList[ri]->isVirtual==false
								 || (gt.rules.internalRoomsList[ri]->isVirtual==true && !c->preferredRealRoomsNames.isEmpty()))){
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
									else{
										if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											  );
										}
										else{
											tmpsc.append((SpaceConstraint*)c);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

							for(SpaceConstraint* delsc : qAsConst(tmpsc)){
								s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
								gt.rules.removeSpaceConstraint(delsc);
								idsOfLockedSpace.remove(act->id);
								unlockedS++;
								//delete delsc; - done by rules.removeSpaceConstraint(...)
							}
							tmpsc.clear();
						}  //modified by Volker Dirr, so you can also unlock (end)

						if(report){
							/*int k;
							k=QMessageBox::information(this, tr("FET information"), s,
						 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
								
		 					if(k==0)
								report=false;*/
							QMessageBox::StandardButton k;
							k=QMessageBox::information(this, tr("FET information"), s, QMessageBox::YesToAll | QMessageBox::Ok);
							
							if(k==QMessageBox::YesToAll)
								report=false;
						}
					}
				}
			}
		}
	}
	
	QStringList added;
	QStringList removed;
	if(FET_LANGUAGE=="en_US"){
		if(addedT>0){
			if(addedT==1)
				added << QString("Added 1 locking time constraint.");
			else
				added << QString("Added %1 locking time constraints.").arg(addedT);
		}
		if(addedS>0){
			if(addedS==1)
				added << QString("Added 1 locking space constraint.");
			else
				added << QString("Added %1 locking space constraints.").arg(addedS);
		}
		if(unlockedT>0){
			if(unlockedT==1)
				removed << QString("Removed 1 locking time constraint.");
			else
				removed << QString("Removed %1 locking time constraints.").arg(unlockedT);
		}
		if(unlockedS>0){
			if(unlockedS==1)
				removed << QString("Removed 1 locking space constraint.");
			else
				removed << QString("Removed %1 locking space constraints.").arg(unlockedS);
		}
	}
	else{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		if(addedT>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedS);
		}
#else
		if(addedT>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See http://doc.qt.io/qt-5/i18n-plural-rules.html for advice on how to correctly translate this field."
			 "Also, see http://doc.qt.io/qt-5/i18n-source-translation.html, section 'Handling Plurals'."
			 "You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 unlockedS);
		}
#endif
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

////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //not really needed, just to test
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
/////////

	LockUnlock::increaseCommunicationSpinBox(); //this is needed
}

void TimetableViewRoomsDaysHorizontalForm::help()
{
	QString s="";
	s+=QCoreApplication::translate("TimetableViewForm", "Lock/unlock: you can select one or more activities in the table and toggle lock/unlock in time, space or both.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "There will be added or removed locking constraints for the selected activities (they can be unlocked only if they are not permanently locked).");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Locking time constraints are constraints of type activity preferred starting time. Locking space constraints are constraints of type"
		" activity preferred room. You can see these constraints in the corresponding constraints dialogs. New locking constraints are added at the end of the list of constraints.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "A bold font cell means that the activity is locked in time, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An italic font cell means that the activity is locked in space, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An underlined font cell means that the activity has a preferred day constraint (with any weight).");

	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Note: In this dialog, the virtual rooms' timetable will be empty.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
