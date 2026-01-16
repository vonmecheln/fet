/***************************************************************************
                          timetableviewstudentstimehorizontalform.cpp  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <Qt>

#include <QtGlobal>

#include "longtextmessagebox.h"

#include "fetmainform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "matrix.h"

#include "lockunlock.h"

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QListWidget>

#include <QList>

#include <QCoreApplication>
#include <QApplication>
#include <QGuiApplication>

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

#include <QHash>
#include <QSet>
#include <QPair>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;

extern Solution best_solution;

extern bool generation_running;
extern bool generation_running_multi;

//extern Matrix3D<bool> subgroupNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;
extern QHash<QString, QSet<QPair<int, int>>> studentsSetNotAvailableDayHour;

extern QSet<int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet<int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

extern const int MINIMUM_WIDTH_SPIN_BOX_VALUE; //trick found on the internet, so that these two constants are visible in timetableviewteacherstimehorizontalform.cpp
extern const int MINIMUM_HEIGHT_SPIN_BOX_VALUE;
const int MINIMUM_WIDTH_SPIN_BOX_VALUE=9;
const int MINIMUM_HEIGHT_SPIN_BOX_VALUE=9;

static bool REAL_VIEW=true;

void TimetableViewStudentsTimeHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	int hour=index.column()%nColumns;
	int hourPlusSpan=hour;
	if(tableView!=nullptr)
		hourPlusSpan+=tableView->columnSpan(index.row(), index.column())-1;

	if(QGuiApplication::isLeftToRight()){
		if(hour==0){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}
		if(hourPlusSpan==nColumns-1){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}

		if(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW){
			assert(nColumns>=2);
			int halfHour=hour%(nColumns/2);
			int halfHourPlusSpan=halfHour;
			if(tableView!=nullptr)
				halfHourPlusSpan+=tableView->columnSpan(index.row(), index.column())-1;
			if(halfHour==0)
				painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			if(halfHourPlusSpan==nColumns-1)
				painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
		}

		if(index.row()==0){
			painter->drawLine(option.rect.topLeft(), option.rect.topRight());
			painter->drawLine(option.rect.topLeft().x(), option.rect.topLeft().y()+1, option.rect.topRight().x(), option.rect.topRight().y()+1);
		}
		if(index.row()==nRows-1){
			painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
			painter->drawLine(option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1, option.rect.bottomRight().x(), option.rect.bottomRight().y()-1);
		}
	}
	else if(QGuiApplication::isRightToLeft()){
		if(hour==0){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}
		if(hourPlusSpan==nColumns-1){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}

		if(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW){
			assert(nColumns>=2);
			int halfHour=hour%(nColumns/2);
			int halfHourPlusSpan=halfHour;
			if(tableView!=nullptr)
				halfHourPlusSpan+=tableView->columnSpan(index.row(), index.column())-1;
			if(halfHour==0)
				painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			if(halfHourPlusSpan==nColumns-1)
				painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
		}

		if(index.row()==0){
			painter->drawLine(option.rect.topRight(), option.rect.topLeft());
			painter->drawLine(option.rect.topRight().x(), option.rect.topRight().y()+1, option.rect.topLeft().x(), option.rect.topLeft().y()+1);
		}
		if(index.row()==nRows-1){
			painter->drawLine(option.rect.bottomRight(), option.rect.bottomLeft());
			painter->drawLine(option.rect.bottomRight().x(), option.rect.bottomRight().y()-1, option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1);
		}
	}
	//I think we should not do an 'else {assert(0);}' here, because the layout might be unspecified, according to Qt documentation.
}

TimetableViewStudentsTimeHorizontalForm::TimetableViewStudentsTimeHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	verticalSplitter->setStretchFactor(0, 20);
	verticalSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 5);
	horizontalSplitter->setStretchFactor(1, 1);

	tableViewSetHighlightHeader(studentsTimetableTable);
	
	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	connect(closePushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::close);
	connect(studentsTimetableTable, &QTableWidget::currentItemChanged, this, &TimetableViewStudentsTimeHorizontalForm::currentItemChanged);
	connect(lockTimePushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::lockTime);
	connect(lockSpacePushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::lockSpace);
	connect(lockTimeSpacePushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::lockTimeSpace);

	connect(helpPushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::help);

	connect(lockToDaysPushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::lockDays);
	connect(unlockFromDaysPushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::unlockDays);
	connect(unlockAllFromDaysPushButton, &QPushButton::clicked, this, &TimetableViewStudentsTimeHorizontalForm::unlockAllDays);
	lockToDaysWeightLineEdit->setText(QString::number(100));
	
	lockRadioButton->setChecked(true);
	unlockRadioButton->setChecked(false);
	toggleRadioButton->setChecked(false);

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

	if(settings.contains(this->metaObject()->className()+QString("/lock-radio-button")))
		lockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/lock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/unlock-radio-button")))
		unlockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/unlock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/toggle-radio-button")))
		toggleRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/toggle-radio-button")).toBool());

///////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	//added by Volker Dirr
	//this line is not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	/*if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}*/

	//Commented on 2018-07-20
	//LockUnlock::increaseCommunicationSpinBox();
	
	usedStudentsSet.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(const QString& students : std::as_const(act->studentsNames)){
			if(!usedStudentsSet.contains(students))
				usedStudentsSet.insert(students);
		}
	}
	
	QHash<QString, QSet<QPair<int, int>>>::const_iterator it=studentsSetNotAvailableDayHour.constBegin();
	while(it!=studentsSetNotAvailableDayHour.constEnd()){
		if(!usedStudentsSet.contains(it.key()))
			usedStudentsSet.insert(it.key());
			
		it++;
	}
	
	QSet<QString> studentsSet2;
	usedStudentsList.clear();
	for(StudentsYear* year : std::as_const(gt.rules.augmentedYearsList)){
		if(usedStudentsSet.contains(year->name) && !studentsSet2.contains(year->name)){
			usedStudentsList.append(year->name);
			studentsSet2.insert(year->name);
		}
		for(StudentsGroup* group : std::as_const(year->groupsList)){
			if(usedStudentsSet.contains(group->name) && !studentsSet2.contains(group->name)){
				usedStudentsList.append(group->name);
				studentsSet2.insert(group->name);
			}
			for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
				if(usedStudentsSet.contains(subgroup->name) && !studentsSet2.contains(subgroup->name)){
					usedStudentsList.append(subgroup->name);
					studentsSet2.insert(subgroup->name);
				}
			}
		}
	}

	studentsTimetableTable->setRowCount(usedStudentsList.count());
	studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	oldItemDelegate=studentsTimetableTable->itemDelegate();
	newItemDelegate=new TimetableViewStudentsTimeHorizontalDelegate(nullptr, studentsTimetableTable, studentsTimetableTable->rowCount(), normalView?gt.rules.nHoursPerDay:2*gt.rules.nHoursPerDay);
	studentsTimetableTable->setItemDelegate(newItemDelegate);
	
	if(normalView){
		bool min2letters=false;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			if(gt.rules.daysOfTheWeek[d].size()>gt.rules.nHoursPerDay){
				min2letters=true;
				break;
			}
		}
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			QString dayName=gt.rules.daysOfTheWeek[d];
			int t=dayName.size();
			int q=t/gt.rules.nHoursPerDay;
			int r=t%gt.rules.nHoursPerDay;
			QStringList list;
			
			if(q==0)
				q=1;
			
			for(int i=0; i<gt.rules.nHoursPerDay; i++){
				if(!min2letters){
					list.append(dayName.left(1));
					dayName.remove(0, 1);
				}
				else if(i<r || q<=1){
					assert(q>=1);
					list.append(dayName.left(q+1));
					dayName.remove(0, q+1);
				}
				else{
					list.append(dayName.left(q));
					dayName.remove(0, q);
				}
			}
		
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.hoursOfTheDay[h]);
				item->setToolTip(gt.rules.daysOfTheWeek[d]+"\n"+gt.rules.hoursOfTheDay[h]);
				studentsTimetableTable->setHorizontalHeaderItem(d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}
	else{
		bool min2letters=false;
		for(int d=0; d<gt.rules.nRealDaysPerWeek; d++){
			if(gt.rules.realDaysOfTheWeek[d].size()>gt.rules.nRealHoursPerDay){
				min2letters=true;
				break;
			}
		}
		for(int d=0; d<gt.rules.nRealDaysPerWeek; d++){
			QString realDayName=gt.rules.realDaysOfTheWeek[d];
			int t=realDayName.size();
			int q=t/gt.rules.nRealHoursPerDay;
			int r=t%gt.rules.nRealHoursPerDay;
			QStringList list;
			
			if(q==0)
				q=1;
			
			for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
				if(!min2letters){
					list.append(realDayName.left(1));
					realDayName.remove(0, 1);
				}
				else if(i<r || q<=1){
					assert(q>=1);
					list.append(realDayName.left(q+1));
					realDayName.remove(0, q+1);
				}
				else{
					list.append(realDayName.left(q));
					realDayName.remove(0, q);
				}
			}
		
			for(int h=0; h<gt.rules.nRealHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.realHoursOfTheDay[h]);
				item->setToolTip(gt.rules.realDaysOfTheWeek[d]+"\n"+gt.rules.realHoursOfTheDay[h]);
				studentsTimetableTable->setHorizontalHeaderItem(d*gt.rules.nRealHoursPerDay+h, item);
			}
		}
	}
	
	for(int t=0; t<usedStudentsList.count(); t++){
		QTableWidgetItem* item=new QTableWidgetItem(usedStudentsList.at(t));
		item->setToolTip(usedStudentsList.at(t));
		studentsTimetableTable->setVerticalHeaderItem(t, item);
	}

	for(int t=0; t<usedStudentsList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(t, d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
//	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
	
	initialRecommendedHeight=studentsTimetableTable->verticalHeader()->sectionSizeHint(0);
	
	int h;
	int w;

	if(settings.contains(this->metaObject()->className()+QString("/vertical-header-size"))){
		h=settings.value(this->metaObject()->className()+QString("/vertical-header-size")).toInt();
		if(h==0)
			h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
	else{
		h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
//	if(h==0)
//		h=initialRecommendedHeight;

	if(settings.contains(this->metaObject()->className()+QString("/horizontal-header-size"))){
		w=settings.value(this->metaObject()->className()+QString("/horizontal-header-size")).toInt();
		if(w==0)
			w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
	else{
		w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
//	if(w==0)
//		w=2*initialRecommendedHeight;
		
	widthSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	widthSpinBox->setMinimum(MINIMUM_WIDTH_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	widthSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	widthSpinBox->setMaximum(maxScreenWidth(this));
#endif
	widthSpinBox->setValue(w);
	widthSpinBox->setSpecialValueText(tr("Automatic", "Automatic mode of selection of the width"));
	
	heightSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	heightSpinBox->setMinimum(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	heightSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	heightSpinBox->setMaximum(maxScreenWidth(this));
#endif
	heightSpinBox->setValue(h);
	heightSpinBox->setSpecialValueText(tr("Automatic", "Automatic mode of selection of the height"));
	
	studentsTimetableTable->horizontalHeader()->setMinimumSectionSize(MINIMUM_WIDTH_SPIN_BOX_VALUE);
	studentsTimetableTable->verticalHeader()->setMinimumSectionSize(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
	
	widthSpinBoxValueChanged();
	heightSpinBoxValueChanged();
	
	connect(widthSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &TimetableViewStudentsTimeHorizontalForm::widthSpinBoxValueChanged);
	connect(heightSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &TimetableViewStudentsTimeHorizontalForm::heightSpinBoxValueChanged);
	
//	teachersTimetableTable->verticalHeader()->setDefaultSectionSize(h);
//	teachersTimetableTable->horizontalHeader()->setDefaultSectionSize(w);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	studentsTimetableTable->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
	studentsTimetableTable->verticalHeader()->setResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif
	//}
	////////////////
	
	subjectsCheckBox->setChecked(true);
	teachersCheckBox->setChecked(false);
	
	if(settings.contains(this->metaObject()->className()+QString("/subjects-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/subjects-check-box-state")).toBool();
		subjectsCheckBox->setChecked(state);
	}
	if(settings.contains(this->metaObject()->className()+QString("/teachers-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/teachers-check-box-state")).toBool();
		teachersCheckBox->setChecked(state);
	}

	if(settings.contains(this->metaObject()->className()+QString("/use-colors")))
		colorsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-colors")).toBool());
	else
		colorsCheckBox->setChecked(false);

	if(settings.contains(this->metaObject()->className()+QString("/use-span")))
		spanCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-span")).toBool());
	else
		spanCheckBox->setChecked(true);

	connect(subjectsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable);
	connect(teachersCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable);

	connect(colorsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable);

	connect(spanCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable);
	
	//added by Volker Dirr
	connect(&communicationSpinBox, &CommunicationSpinBox::valueChanged, this, &TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable);
	
	updateStudentsTimetableTable();
}

void TimetableViewStudentsTimeHorizontalForm::newTimetableGenerated()
//Similar to the constructor
{
	//setupUi(this);
	
	//closePushButton->setDefault(true);
	
	//detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	/*verticalSplitter->setStretchFactor(0, 20);
	verticalSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 5);
	horizontalSplitter->setStretchFactor(1, 1);

	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	connect(closePushButton, SIG NAL(clicked()), this, SL OT(close()));
	connect(studentsTimetableTable, SIG NAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SL OT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIG NAL(clicked()), this, SL OT(lockTime()));
	connect(lockSpacePushButton, SIG NAL(clicked()), this, SL OT(lockSpace()));
	connect(lockTimeSpacePushButton, SIG NAL(clicked()), this, SL OT(lockTimeSpace()));

	connect(helpPushButton, SIG NAL(clicked()), this, SL OT(help()));*/
	
	/*lockRadioButton->setChecked(true);
	unlockRadioButton->setChecked(false);
	toggleRadioButton->setChecked(false);

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

	if(settings.contains(this->metaObject()->className()+QString("/lock-radio-button")))
		lockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/lock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/unlock-radio-button")))
		unlockRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/unlock-radio-button")).toBool());
	if(settings.contains(this->metaObject()->className()+QString("/toggle-radio-button")))
		toggleRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/toggle-radio-button")).toBool());*/

///////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	//added by Volker Dirr
	//this line is not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	/*if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}*/

	int r=studentsTimetableTable->currentRow();
	int c=studentsTimetableTable->currentColumn();

	//DON'T UNCOMMENT THIS CODE -> LEADS TO CRASH IF THERE ARE MORE VIEWS OPENED.
	//LockUnlock::increaseCommunicationSpinBox();
	
	usedStudentsSet.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(const QString& students : std::as_const(act->studentsNames)){
			if(!usedStudentsSet.contains(students))
				usedStudentsSet.insert(students);
		}
	}
	
	QHash<QString, QSet<QPair<int, int>>>::const_iterator it=studentsSetNotAvailableDayHour.constBegin();
	while(it!=studentsSetNotAvailableDayHour.constEnd()){
		if(!usedStudentsSet.contains(it.key()))
			usedStudentsSet.insert(it.key());
			
		it++;
	}
	
	QSet<QString> studentsSet2;
	usedStudentsList.clear();
	for(StudentsYear* year : std::as_const(gt.rules.augmentedYearsList)){
		if(usedStudentsSet.contains(year->name) && !studentsSet2.contains(year->name)){
			usedStudentsList.append(year->name);
			studentsSet2.insert(year->name);
		}
		for(StudentsGroup* group : std::as_const(year->groupsList)){
			if(usedStudentsSet.contains(group->name) && !studentsSet2.contains(group->name)){
				usedStudentsList.append(group->name);
				studentsSet2.insert(group->name);
			}
			for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
				if(usedStudentsSet.contains(subgroup->name) && !studentsSet2.contains(subgroup->name)){
					usedStudentsList.append(subgroup->name);
					studentsSet2.insert(subgroup->name);
				}
			}
		}
	}

	studentsTimetableTable->clear();
	studentsTimetableTable->setRowCount(usedStudentsList.count());
	studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	
	newItemDelegate->nRows=studentsTimetableTable->rowCount();
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	if(!realView)
		newItemDelegate->nColumns=gt.rules.nHoursPerDay;
	else
		newItemDelegate->nColumns=2*gt.rules.nHoursPerDay;
	/*studentsTimetableTable->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
	//oldItemDelegate=studentsTimetableTable->itemDelegate();
	newItemDelegate=new TimetableViewStudentsTimeHorizontalDelegate(nullptr, studentsTimetableTable->rowCount(), gt.rules.nHoursPerDay);
	studentsTimetableTable->setItemDelegate(newItemDelegate);*/
	
	bool normalView=!realView;
	if(normalView){
		bool min2letters=false;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			if(gt.rules.daysOfTheWeek[d].size()>gt.rules.nHoursPerDay){
				min2letters=true;
				break;
			}
		}
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			QString dayName=gt.rules.daysOfTheWeek[d];
			int t=dayName.size();
			int q=t/gt.rules.nHoursPerDay;
			int r=t%gt.rules.nHoursPerDay;
			QStringList list;
			
			if(q==0)
				q=1;
			
			for(int i=0; i<gt.rules.nHoursPerDay; i++){
				if(!min2letters){
					list.append(dayName.left(1));
					dayName.remove(0, 1);
				}
				else if(i<r || q<=1){
					assert(q>=1);
					list.append(dayName.left(q+1));
					dayName.remove(0, q+1);
				}
				else{
					list.append(dayName.left(q));
					dayName.remove(0, q);
				}
			}
		
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.hoursOfTheDay[h]);
				item->setToolTip(gt.rules.daysOfTheWeek[d]+"\n"+gt.rules.hoursOfTheDay[h]);
				studentsTimetableTable->setHorizontalHeaderItem(d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}
	else{
		bool min2letters=false;
		for(int d=0; d<gt.rules.nRealDaysPerWeek; d++){
			if(gt.rules.realDaysOfTheWeek[d].size()>gt.rules.nRealHoursPerDay){
				min2letters=true;
				break;
			}
		}
		for(int d=0; d<gt.rules.nRealDaysPerWeek; d++){
			QString realDayName=gt.rules.realDaysOfTheWeek[d];
			int t=realDayName.size();
			int q=t/gt.rules.nRealHoursPerDay;
			int r=t%gt.rules.nRealHoursPerDay;
			QStringList list;
			
			if(q==0)
				q=1;
			
			for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
				if(!min2letters){
					list.append(realDayName.left(1));
					realDayName.remove(0, 1);
				}
				else if(i<r || q<=1){
					assert(q>=1);
					list.append(realDayName.left(q+1));
					realDayName.remove(0, q+1);
				}
				else{
					list.append(realDayName.left(q));
					realDayName.remove(0, q);
				}
			}
		
			for(int h=0; h<gt.rules.nRealHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.realHoursOfTheDay[h]);
				item->setToolTip(gt.rules.realDaysOfTheWeek[d]+"\n"+gt.rules.realHoursOfTheDay[h]);
				studentsTimetableTable->setHorizontalHeaderItem(d*gt.rules.nRealHoursPerDay+h, item);
			}
		}
	}

	for(int t=0; t<usedStudentsList.count(); t++){
		QTableWidgetItem* item=new QTableWidgetItem(usedStudentsList.at(t));
		item->setToolTip(usedStudentsList.at(t));
		studentsTimetableTable->setVerticalHeaderItem(t, item);
	}

	for(int t=0; t<usedStudentsList.count(); t++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(t, d*gt.rules.nHoursPerDay+h, item);
			}
		}
	}
	
	//resize columns
	//if(!columnResizeModeInitialized){
//	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
	
	/*initialRecommendedHeight=studentsTimetableTable->verticalHeader()->sectionSizeHint(0);
	
	int h;
	int w;

	if(settings.contains(this->metaObject()->className()+QString("/vertical-header-size"))){
		h=settings.value(this->metaObject()->className()+QString("/vertical-header-size")).toInt();
		if(h==0)
			h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
	else{
		h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
//	if(h==0)
//		h=initialRecommendedHeight;

	if(settings.contains(this->metaObject()->className()+QString("/horizontal-header-size"))){
		w=settings.value(this->metaObject()->className()+QString("/horizontal-header-size")).toInt();
		if(w==0)
			w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
	else{
		w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
//	if(w==0)
//		w=2*initialRecommendedHeight;
		
	widthSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	widthSpinBox->setMinimum(MINIMUM_WIDTH_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	widthSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	widthSpinBox->setMaximum(maxScreenWidth(this));
#endif
	widthSpinBox->setValue(w);
	widthSpinBox->setSpecialValueText(tr("Automatic"));
	
	heightSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	heightSpinBox->setMinimum(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	heightSpinBox->setMaximum(studentsTimetableTable->verticalHeader()->maximumSectionSize());
#else
	heightSpinBox->setMaximum(maxScreenWidth(this));
#endif
	heightSpinBox->setValue(h);
	heightSpinBox->setSpecialValueText(tr("Automatic"));
	
	widthSpinBoxValueChanged();
	heightSpinBoxValueChanged();
	
	connect(widthSpinBox, SIG NAL(valueChanged(int)), this, SL OT(widthSpinBoxValueChanged()));
	connect(heightSpinBox, SIG NAL(valueChanged(int)), this, SL OT(heightSpinBoxValueChanged()));
	
//	teachersTimetableTable->verticalHeader()->setDefaultSectionSize(h);
//	teachersTimetableTable->horizontalHeader()->setDefaultSectionSize(w);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	studentsTimetableTable->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
	studentsTimetableTable->verticalHeader()->setResizeMode(QHeaderView::Interactive);
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif
	//}
	////////////////
	
	subjectsCheckBox->setChecked(true);
	teachersCheckBox->setChecked(false);
	
	if(settings.contains(this->metaObject()->className()+QString("/subjects-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/subjects-check-box-state")).toBool();
		subjectsCheckBox->setChecked(state);
	}
	if(settings.contains(this->metaObject()->className()+QString("/teachers-check-box-state"))){
		bool state=settings.value(this->metaObject()->className()+QString("/teachers-check-box-state")).toBool();
		teachersCheckBox->setChecked(state);
	}
	
	connect(subjectsCheckBox, SIG NAL(toggled(bool)), this, SL OT(updateStudentsTimetableTable()));
	connect(teachersCheckBox, SIG NAL(toggled(bool)), this, SL OT(updateStudentsTimetableTable()));*/
	
	//added by Volker Dirr
	//connect(&communicationSpinBox, SIG NAL(valueChanged(int)), this, SL OT(updateStudentsTimetableTable()));
	
	updateStudentsTimetableTable();

	if(r>=0 && r<studentsTimetableTable->rowCount() && c>=0 && c<studentsTimetableTable->columnCount())
		studentsTimetableTable->setCurrentCell(r, c);
}

TimetableViewStudentsTimeHorizontalForm::~TimetableViewStudentsTimeHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-splitter-state"), verticalSplitter->saveState());

	//save horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/lock-radio-button"), lockRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/unlock-radio-button"), unlockRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/toggle-radio-button"), toggleRadioButton->isChecked());

	if(heightSpinBox->value()<=MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), heightSpinBox->value());
	
	if(widthSpinBox->value()<=MINIMUM_WIDTH_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), widthSpinBox->value());
	
	settings.setValue(this->metaObject()->className()+QString("/subjects-check-box-state"), subjectsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/teachers-check-box-state"), teachersCheckBox->isChecked());
	
	settings.setValue(this->metaObject()->className()+QString("/use-colors"), colorsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/use-span"), spanCheckBox->isChecked());

	studentsTimetableTable->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
	
	usedStudentsList.clear();
	usedStudentsSet.clear();
	//activitiesForStudentsSet.clear();
	
	//notAvailableHash.clear();
}

void TimetableViewStudentsTimeHorizontalForm::resizeRowsAfterShow()
{
//	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsTimeHorizontalForm::updateStudentsTimetableTable(){
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

	assert(gt.rules.initialized);
	
	studentsTimetableTable->clearSpans();

	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());
		
		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), nullptr);
		assert(ss!=nullptr);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
		
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);
		
		QSet<QPair<int, int>> notAvailableDayHour=studentsSetNotAvailableDayHour.value(usedStudentsList.at(t), QSet<QPair<int, int>>());
		
		/*ConstraintStudentsSetNotAvailableTimes* ctr=notAvailableHash.value(usedStudentsList.at(t), nullptr);
		if(ctr!=nullptr){
			for(int i=0; i<ctr->days.count(); i++){
				int d=ctr->days.at(i);
				int h=ctr->hours.at(i);
				notAvailableDayHour.insert(QPair<int,int>(d,h));
			}
		}*/
		
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());

				//begin by Marco Vassura
				// add colors (start)
				//if(USE_GUI_COLORS) {
				studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setBackground(studentsTimetableTable->palette().base());
				studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(studentsTimetableTable->palette().text());
				//}
				// add colors (end)
				//end by Marco Vassura

				QString s = "";
				
				int ai=students_timetable_weekly[sbg][d][h]; //activity index
				
				bool ok=true;
				if(ai==UNALLOCATED_ACTIVITY){
					ok=false;
				}
				else{
					if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
						ok=false;
					}
				}
				
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ok){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					assert(act!=nullptr);

					if(best_solution.times[ai]/gt.rules.nDaysPerWeek==h){
						assert(best_solution.times[ai]%gt.rules.nDaysPerWeek==d);

						if(spanCheckBox->isChecked() && act->duration>=2)
							studentsTimetableTable->setSpan(t, d*gt.rules.nHoursPerDay+h, 1, act->duration);
					}
					
					if(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS){
						QString ats=act->activityTagsNames.join(translatedCommaSpace());
						s += act->subjectName+" "+ats;
					}
					else{
						s += act->subjectName;
					}
					
					//teachers
					if(act->teachersNames.count()>0){
						//s+=" ";
						s+="\n";
						s+=act->teachersNames.join(translatedCommaSpace());
					}
					
					if(act->studentsNames.count()==1){
						//Comment taken from the teachers view timetable time horizontal
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.at(0)==teachername);
					}
					else{
						assert(act->studentsNames.count()>=2);
						//Comment taken from the teachers view timetable time horizontal
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.contains(teachername));
						//s+=" ";
						s+="\n";
						s+=act->studentsNames.join(translatedCommaSpace());
					}
					
					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						//s+=" ";
						//s+=tr("R:%1", "Room").arg(gt.rules.internalRoomsList[r]->name);
						//s+=" ";
						s+="\n";
						s+=gt.rules.internalRoomsList[r]->name;
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

					if(idsOfPermanentlyLockedTime.contains(act->id) || idsOfLockedTime.contains(act->id)){
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setBold(true);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					else{
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setBold(false);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					
					if(idsOfPermanentlyLockedSpace.contains(act->id) || idsOfLockedSpace.contains(act->id)){
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setItalic(true);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					else{
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setItalic(false);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}

					if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setUnderline(true);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}
					else{
						QFont font(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->font());
						font.setUnderline(false);
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setFont(font);
					}

					s+=descr;
					//added by Volker Dirr (end)
				
					//begin by Marco Vassura
					// add colors (start)
					if(colorsCheckBox->isChecked() /*USE_GUI_COLORS*/ /*&& act->studentsNames.count()>0*/){
						QBrush bg(stringToColor(act->subjectName));
						studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setBackground(bg);
						double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						if (brightness<0.5)
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(QColorConstants::White));
						else
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(QColorConstants::Black));
#else
						if (brightness<0.5)
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(Qt::white));
						else
							studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setForeground(QBrush(Qt::black));
#endif
					}
					// add colors (end)
					//end by Marco Vassura
				}
				else{
//					if(subgroupNotAvailableDayHour[sbg][d][h] && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					if(notAvailableDayHour.contains(QPair<int,int>(d,h)) && PRINT_NOT_AVAILABLE_TIME_SLOTS)
						s+="-x-";
					else if(breakDayHour[d][h] && PRINT_BREAK_TIME_SLOTS)
						s+="-X-";
				}
				if(ok){
					QString s2;
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(teachersCheckBox->isChecked() && !act->teachersNames.isEmpty()){
						s2+=act->teachersNames.join(translatedCommaSpace());
					}
					if(subjectsCheckBox->isChecked()){
						if(!s2.isEmpty()){
							s2+=" ";
							//s2+="\n";
						}
						s2+=act->subjectName;
					}
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setText(s2);
				}
				else
					studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setText(s);
				studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->setToolTip(s);
			}
		}
	}
	//	for(int i=0; i<gt.rules.nHoursPerDay; i++) //added in version 3_9_16, on 16 Oct. 2004
	//		teachersTimetableTable->adjustRow(i);

//	teachersTimetableTable->resizeRowsToContents();
	
	detailActivity(studentsTimetableTable->currentItem());
}

/*void TimetableViewTeachersTimeHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

//	teachersTimetableTable->resizeRowsToContents();
}*/

//begin by Marco Vassura
//slightly modified by Liviu Lalescu on 2021-03-01
QColor TimetableViewStudentsTimeHorizontalForm::stringToColor(const QString& s)
{
	// CRC-24 based on RFC 2440 Section 6.1
	unsigned long int crc = 0xB704CEUL;
	QByteArray ba=s.toUtf8();
	for(char c : std::as_const(ba)){
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

void TimetableViewStudentsTimeHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewStudentsTimeHorizontalForm::detailActivity(QTableWidgetItem* item){
	if(item==nullptr){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(item->row()>=usedStudentsList.count() || item->column()>=gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		"or close the timetable view students dialog"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	
	assert(item->row()>=0);
	assert(item->column()>=0);

	int t=item->row();
	
	/*if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}*/

	int d=item->column()/gt.rules.nHoursPerDay;
	int h=item->column()%gt.rules.nHoursPerDay;

	int sbg=-1;
	
	if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
		QMessageBox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
		return;
	}
	assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
	StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), nullptr);
	assert(ss!=nullptr);
	if(ss->type==STUDENTS_YEAR){
		StudentsYear* year=(StudentsYear*)ss;
		sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* group=(StudentsGroup*)ss;
		sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
	}
	else if(ss->type==STUDENTS_SUBGROUP){
		StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
		sbg=subgroup->indexInInternalSubgroupsList;
	}
	else{
		assert(0);
	}
	
	assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);
	
	QSet<QPair<int, int>> notAvailableDayHour=studentsSetNotAvailableDayHour.value(usedStudentsList.at(t), QSet<QPair<int, int>>());

	/*QSet<QPair<int, int>> notAvailableDayHour;
	QSet<ConstraintStudentsSetNotAvailableTimes*> cs=gt.rules.ssnatHash.value(usedStudentsList.at(t), QSet<ConstraintStudentsSetNotAvailableTimes*>());
	if(!cs.isEmpty()){
		assert(cs.count()==1);
		ConstraintStudentsSetNotAvailableTimes* ctr=*(cs.begin());
		
		for(int i=0; i<ctr->days.count(); i++){
			int d=ctr->days.at(i);
			int h=ctr->hours.at(i);
			notAvailableDayHour.insert(QPair<int,int>(d,h));
		}
	}*/

	/*int teacher=gt.rules.searchTeacher(gt.rules.internalTeachersList[t]->name);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("The teacher is invalid - please close this dialog and open a new view teachers timetable"));
		return;
	}*/
	QString s = "";
	if(d>=0 && d<gt.rules.nDaysPerWeek && h>=0 && h<gt.rules.nHoursPerDay){
		int ai=students_timetable_weekly[sbg][d][h]; //activity index

		bool ok=true;
		if(ai==UNALLOCATED_ACTIVITY){
			ok=false;
		}
		else{
			if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
				ok=false;
			}
		}
		
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ok){
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
					for(int i : std::as_const(best_solution.realRoomsList[ai]))
						tsl.append(gt.rules.internalRoomsList[i]->name);
					s+=QString(" (")+tsl.join(translatedCommaSpace())+QString(")");
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
			QString tt="";
			if(idsOfPermanentlyLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "permanently locked time", "refers to activity");
				tt=", ";
			}
			else if(idsOfLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "locked time", "refers to activity");
				tt=", ";
			}
			if(idsOfPermanentlyLockedSpace.contains(act->id)){
				descr+=tt+QCoreApplication::translate("TimetableViewForm", "permanently locked space", "refers to activity");
				tt=", ";
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=tt+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
				tt=", ";
			}
			if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
				descr+=tt+QCoreApplication::translate("TimetableViewForm", "preferred day", "refers to activity");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
		}
		else{
			//if(subgroupNotAvailableDayHour[sbg][d][h]){
			if(notAvailableDayHour.contains(QPair<int,int>(d,h))){
				s+=tr("Students set is not available 100% in this slot");
				s+="\n";
			}
			if(breakDayHour[d][h]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setPlainText(s);
}

void TimetableViewStudentsTimeHorizontalForm::lockTime()
{
	this->lock(true, false);
}

void TimetableViewStudentsTimeHorizontalForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewStudentsTimeHorizontalForm::lockTimeSpace()
{
	this->lock(true, true);
}

void TimetableViewStudentsTimeHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
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

	Solution* tc=&best_solution;
	
	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;
	
	QSet<int> dummyActivitiesColumn; //Dummy activities (no students) column to be considered, because the whole column is selected.
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			assert(d*gt.rules.nHoursPerDay+h < studentsTimetableTable->columnCount());
			bool wholeColumn=true;
			for(int t=0; t<usedStudentsList.count(); t++){
				assert(t<studentsTimetableTable->rowCount());

				if(!studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){
					wholeColumn=false;
					break;
				}
			}
			if(wholeColumn)
				dummyActivitiesColumn.insert(d+h*gt.rules.nDaysPerWeek);
		}
	}
	
	QSet<int> dummyActivities;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		if(gt.rules.internalActivitiesList[ai].iSubgroupsList.count()==0){
			if(tc->times[ai]!=UNALLOCATED_TIME){
				int da=tc->times[ai]%gt.rules.nDaysPerWeek;
				int ha=tc->times[ai]/gt.rules.nDaysPerWeek;
				for(int ha2=ha; ha2<ha+gt.rules.internalActivitiesList[ai].duration; ha2++)
					if(dummyActivitiesColumn.contains(da+ha2*gt.rules.nDaysPerWeek))
						if(!dummyActivities.contains(ai))
							dummyActivities.insert(ai);
			}
		}
	}

	QSet<int> realActivities;
	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());

		/*if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
			QMessagebox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
			return;
		}*/

		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), nullptr);
		assert(ss!=nullptr);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
	
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());
				if(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){

					int ai=students_timetable_weekly[sbg][d][h];
					
					bool ok=true;
					if(ai==UNALLOCATED_ACTIVITY){
						ok=false;
					}
					else{
						if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
							ok=false;
						}
					}
		
					//Activity* act=gt.rules.activitiesList.at(ai);
					if(ok)
						if(!realActivities.contains(ai))
							realActivities.insert(ai);
				}
			}
		}
	}

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert( ! (realActivities.contains(ai) && dummyActivities.contains(ai)) );
		if(realActivities.contains(ai) || dummyActivities.contains(ai)){
			assert(tc->times[ai]!=UNALLOCATED_TIME);
			int day=tc->times[ai]%gt.rules.nDaysPerWeek;
			int hour=tc->times[ai]/gt.rules.nDaysPerWeek;

			Activity* act=&gt.rules.internalActivitiesList[ai];
			
			if(lockTime){
				QString s;
			
				QList<TimeConstraint*> tmptc;
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
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
								}
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
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									tmptc.append((TimeConstraint*)c);
								}
							}
						}
					}
				}
				if(count==0 && (lockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false);
					bool t=gt.rules.addTimeConstraint(ctr);
					QString s;
					if(t){
						addedT++;
						idsOfLockedTime.insert(act->id);
						s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
					}
					else{
						delete ctr;
						
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to add 1 constraint, but this is not possible. "
						 "Please report possible bug"));
					}
				}
				else if(count>=1 && (unlockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					if(count>=2)
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

					for(TimeConstraint* deltc : std::as_const(tmptc)){
						s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
						//gt.rules.removeTimeConstraint(deltc);
						idsOfLockedTime.remove(act->id);
						unlockedT++;
						//delete deltc; - done by rules.removeTimeConstraint(...)
					}
					gt.rules.removeTimeConstraints(tmptc);
				}
				tmptc.clear();

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
			
			int ri=tc->rooms[ai];
			if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
				QString s;
				
				QList<SpaceConstraint*> tmpsc;
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
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
								}
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
								if(unlockRadioButton->isChecked() || toggleRadioButton->isChecked()){
									tmpsc.append((SpaceConstraint*)c);
								}
							}
						}
					}
				}
				if(count==0 && (lockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					QStringList tl;
					if(gt.rules.internalRoomsList[ri]->isVirtual==false)
						assert(tc->realRoomsList[ai].isEmpty());
					else
						for(int rr : std::as_const(tc->realRoomsList[ai]))
							tl.append(gt.rules.internalRoomsList[rr]->name);
					
					ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, tl, false);
					bool t=gt.rules.addSpaceConstraint(ctr);
	
					QString s;
					
					if(t){
						addedS++;
						idsOfLockedSpace.insert(act->id);
						s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
					}
					else{
						delete ctr;
						
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to add 1 constraint, but this is not possible. "
						 "Please report possible bug"));
					}
				}
				else if(count>=1 && (unlockRadioButton->isChecked() || toggleRadioButton->isChecked())){
					if(count>=2)
						QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

					for(SpaceConstraint* delsc : std::as_const(tmpsc)){
						s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
						//gt.rules.removeSpaceConstraint(delsc);
						idsOfLockedSpace.remove(act->id);
						unlockedS++;
						//delete delsc; done by rules.removeSpaceConstraint(...)
					}
					gt.rules.removeSpaceConstraints(tmpsc);
				}
				tmpsc.clear();
			
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
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedS);
		}
#else
		if(addedT>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
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

	if(addedT>0 || addedS>0 || unlockedT>0 || unlockedS>0)
		gt.rules.addUndoPoint(tr("Locked/unlocked/toggled a selection of activities in the"
		  " timetable view students time horizontal dialog. The summary of the added/removed locking constraints is:\n\n%1").arg(s)+QString("\n"));

	////////// just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //not needed, just for testing
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewStudentsTimeHorizontalForm::unlockDays()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
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

	Solution* tc=&best_solution;
	
	QSet<int> dummyActivitiesColumn; //Dummy activities (no students) column to be considered, because the whole column is selected.
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			assert(d*gt.rules.nHoursPerDay+h < studentsTimetableTable->columnCount());
			bool wholeColumn=true;
			for(int t=0; t<usedStudentsList.count(); t++){
				assert(t<studentsTimetableTable->rowCount());

				if(!studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){
					wholeColumn=false;
					break;
				}
			}
			if(wholeColumn)
				dummyActivitiesColumn.insert(d+h*gt.rules.nDaysPerWeek);
		}
	}
	
	QSet<int> dummyActivities;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		if(gt.rules.internalActivitiesList[ai].iSubgroupsList.count()==0){
			if(tc->times[ai]!=UNALLOCATED_TIME){
				int da=tc->times[ai]%gt.rules.nDaysPerWeek;
				int ha=tc->times[ai]/gt.rules.nDaysPerWeek;
				for(int ha2=ha; ha2<ha+gt.rules.internalActivitiesList[ai].duration; ha2++)
					if(dummyActivitiesColumn.contains(da+ha2*gt.rules.nDaysPerWeek))
						if(!dummyActivities.contains(ai))
							dummyActivities.insert(ai);
			}
		}
	}

	QSet<int> realActivities;
	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());

		/*if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
			QMessagebox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
			return;
		}*/

		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), nullptr);
		assert(ss!=nullptr);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
	
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());
				if(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){

					int ai=students_timetable_weekly[sbg][d][h];
					
					bool ok=true;
					if(ai==UNALLOCATED_ACTIVITY){
						ok=false;
					}
					else{
						if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
							ok=false;
						}
					}
		
					//Activity* act=gt.rules.activitiesList.at(ai);
					if(ok)
						if(!realActivities.contains(ai))
							realActivities.insert(ai);
				}
			}
		}
	}

	QList<TimeConstraint*> tmptc;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert( ! (realActivities.contains(ai) && dummyActivities.contains(ai)) );
		if(realActivities.contains(ai) || dummyActivities.contains(ai)){
			assert(tc->times[ai]!=UNALLOCATED_TIME);
			//int day=tc->times[ai]%gt.rules.nDaysPerWeek;
			//int hour=tc->times[ai]/gt.rules.nDaysPerWeek;

			Activity* act=&gt.rules.internalActivitiesList[ai];

			assert(gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).count()<=1);
			for(ConstraintActivityPreferredDay* c : gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>())){
				tmptc.append(c);
			}
		}
	}
	int nUnlocked=tmptc.count();
	gt.rules.removeTimeConstraints(tmptc);
	tmptc.clear();

	QMessageBox::information(this, tr("FET information"), tr("There were removed %1 constraints").arg(nUnlocked));

	if(nUnlocked>0)
		gt.rules.addUndoPoint(tr("Unlocked from days a selection of activities in the"
		 " timetable view students time horizontal dialog. There were removed %1 locking constraints.").arg(nUnlocked)+QString("\n"));

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewStudentsTimeHorizontalForm::unlockAllDays()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
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

	QList<TimeConstraint*> tmptc;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];

		assert(gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).count()<=1);
		for(ConstraintActivityPreferredDay* c : gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>())){
			tmptc.append(c);
		}
	}
	int nUnlocked=tmptc.count();
	gt.rules.removeTimeConstraints(tmptc);
	tmptc.clear();

	QMessageBox::information(this, tr("FET information"), tr("There were removed %1 constraints").arg(nUnlocked));

	if(nUnlocked>0)
		gt.rules.addUndoPoint(tr("Unlocked from days all the activities in the"
		  " timetable view students time horizontal dialog. There were removed %1 locking constraints.").arg(nUnlocked)+QString("\n"));

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewStudentsTimeHorizontalForm::lockDays()
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
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

	double weight;
	QString tmp=lockToDaysWeightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	Solution* tc=&best_solution;
	
	QSet<int> dummyActivitiesColumn; //Dummy activities (no students) column to be considered, because the whole column is selected.
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			assert(d*gt.rules.nHoursPerDay+h < studentsTimetableTable->columnCount());
			bool wholeColumn=true;
			for(int t=0; t<usedStudentsList.count(); t++){
				assert(t<studentsTimetableTable->rowCount());

				if(!studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){
					wholeColumn=false;
					break;
				}
			}
			if(wholeColumn)
				dummyActivitiesColumn.insert(d+h*gt.rules.nDaysPerWeek);
		}
	}
	
	QSet<int> dummyActivities;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		if(gt.rules.internalActivitiesList[ai].iSubgroupsList.count()==0){
			if(tc->times[ai]!=UNALLOCATED_TIME){
				int da=tc->times[ai]%gt.rules.nDaysPerWeek;
				int ha=tc->times[ai]/gt.rules.nDaysPerWeek;
				for(int ha2=ha; ha2<ha+gt.rules.internalActivitiesList[ai].duration; ha2++)
					if(dummyActivitiesColumn.contains(da+ha2*gt.rules.nDaysPerWeek))
						if(!dummyActivities.contains(ai))
							dummyActivities.insert(ai);
			}
		}
	}

	QSet<int> realActivities;
	for(int t=0; t<usedStudentsList.count(); t++){
		assert(t<studentsTimetableTable->rowCount());

		/*if(!gt.rules.studentsHash.contains(usedStudentsList.at(t))){
			QMessagebox::warning(this, tr("FET warning"), tr("The students set is invalid - please close this dialog and open a new view students timetable"));
			return;
		}*/

		if(!gt.rules.studentsHash.contains(usedStudentsList.at(t)))
			continue;
		assert(gt.rules.studentsHash.contains(usedStudentsList.at(t)));
		StudentsSet* ss=gt.rules.studentsHash.value(usedStudentsList.at(t), nullptr);
		assert(ss!=nullptr);
		int sbg=-1;
		if(ss->type==STUDENTS_YEAR){
			StudentsYear* year=(StudentsYear*)ss;
			sbg=year->groupsList.at(0)->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* group=(StudentsGroup*)ss;
			sbg=group->subgroupsList.at(0)->indexInInternalSubgroupsList;
		}
		else if(ss->type==STUDENTS_SUBGROUP){
			StudentsSubgroup* subgroup=(StudentsSubgroup*)ss;
			sbg=subgroup->indexInInternalSubgroupsList;
		}
		else{
			assert(0);
		}
	
		assert(sbg>=0 && sbg<gt.rules.nInternalSubgroups);

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				assert(d*gt.rules.nHoursPerDay+h<studentsTimetableTable->columnCount());
				if(studentsTimetableTable->item(t, d*gt.rules.nHoursPerDay+h)->isSelected()){

					int ai=students_timetable_weekly[sbg][d][h];
					
					bool ok=true;
					if(ai==UNALLOCATED_ACTIVITY){
						ok=false;
					}
					else{
						if(!gt.rules.internalActivitiesList[ai].studentsNames.contains(usedStudentsList.at(t))){
							ok=false;
						}
					}
		
					//Activity* act=gt.rules.activitiesList.at(ai);
					if(ok)
						if(!realActivities.contains(ai))
							realActivities.insert(ai);
				}
			}
		}
	}

	//QList<int> lockedNotPermanentlyTimeActivities;
	QList<int> lockedDaysActivitiesList;
	QSet<int> lockedDaysActivitiesSet;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert( ! (realActivities.contains(ai) && dummyActivities.contains(ai)) );
		if(realActivities.contains(ai) || dummyActivities.contains(ai)){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			/*if(!gt.rules.apstHash.value(act->id, QSet<ConstraintActivityPreferredStartingTime*>()).isEmpty()){
				QSet<ConstraintActivityPreferredStartingTime*> lc=gt.rules.apstHash.value(act->id, QSet<ConstraintActivityPreferredStartingTime*>());
				for(const ConstraintActivityPreferredStartingTime* ctr : std::as_const(lc)){
					if(!ctr->permanentlyLocked){
						lockedNotPermanentlyTimeActivities.append(act->id);
						break;
					}
				}
			}*/
			if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
				lockedDaysActivitiesList.append(act->id);
				lockedDaysActivitiesSet.insert(act->id);
			}
		}
	}
	
	/*if(!lockedNotPermanentlyTimeActivities.isEmpty() || !lockedDaysActivities.isEmpty()){
		std::stable_sort(lockedNotPermanentlyTimeActivities.begin(), lockedNotPermanentlyTimeActivities.end());
		std::stable_sort(lockedDaysActivities.begin(), lockedDaysActivities.end());
	
		QString st;
		for(int aid : lockedNotPermanentlyTimeActivities)
			st+=QString::number(aid)+QString(", ");
		st.chop(2);

		QString sd;
		for(int aid : lockedDaysActivities)
			sd+=QString::number(aid)+QString(", ");
		sd.chop(2);
	
		QString lt=tr("These activities ids are already locked not permanently in time: %1.").arg(st);
		QString ld=tr("These activities ids are already locked to days: %1.").arg(sd);
		if(!lockedNotPermanentlyTimeActivities.isEmpty() && !lockedDaysActivities.isEmpty())
			QMessageBox::warning(this, tr("FET warning"), lt+"\n\n"+ld);
		else if(!lockedNotPermanentlyTimeActivities.isEmpty())
			QMessageBox::warning(this, tr("FET warning"), lt);
		else if(!lockedDaysActivities.isEmpty())
			QMessageBox::warning(this, tr("FET warning"), ld);
		else
			assert(0);
		return;
	}*/

	int nLocked=0;
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert( ! (realActivities.contains(ai) && dummyActivities.contains(ai)) );
		if(realActivities.contains(ai) || dummyActivities.contains(ai)){
			if(!lockedDaysActivitiesSet.contains(gt.rules.internalActivitiesList[ai].id)){
				assert(tc->times[ai]!=UNALLOCATED_TIME);
				int day=tc->times[ai]%gt.rules.nDaysPerWeek;
				//int hour=tc->times[ai]/gt.rules.nDaysPerWeek;
	
				Activity* act=&gt.rules.internalActivitiesList[ai];
	
				ConstraintActivityPreferredDay* c=new ConstraintActivityPreferredDay(weight, act->id, day);
				gt.rules.addTimeConstraint(c);
				nLocked++;
			}
		}
	}

	QMessageBox::information(this, tr("FET information"), tr("There were added %1 constraints").arg(nLocked));

	if(nLocked>0)
		gt.rules.addUndoPoint(tr("Locked to days a selection of activities in the"
		  " timetable view students time horizontal dialog. There were added %1 locking constraints.").arg(nLocked)+QString("\n"));

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewStudentsTimeHorizontalForm::widthSpinBoxValueChanged()
{
	if(widthSpinBox->value()==MINIMUM_WIDTH_SPIN_BOX_VALUE)
		studentsTimetableTable->horizontalHeader()->setDefaultSectionSize(2*initialRecommendedHeight);
	else
		studentsTimetableTable->horizontalHeader()->setDefaultSectionSize(widthSpinBox->value());
}

void TimetableViewStudentsTimeHorizontalForm::heightSpinBoxValueChanged()
{
	if(heightSpinBox->value()==MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		studentsTimetableTable->verticalHeader()->setDefaultSectionSize(initialRecommendedHeight);
	else
		studentsTimetableTable->verticalHeader()->setDefaultSectionSize(heightSpinBox->value());
}

void TimetableViewStudentsTimeHorizontalForm::help()
{
	QString s="";
	s+=QCoreApplication::translate("TimetableViewForm", "Lock/unlock: you can select one or more activities in the table and toggle lock/unlock in time, space or both.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "There will be added or removed locking constraints for the selected activities (they can be unlocked only if they are not permanently locked).");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Locking time constraints are constraints of type activity preferred starting time. Locking space constraints are constraints of type"
		" activity preferred room. You can see these constraints in the corresponding constraints dialogs. New locking constraints are added at the end of the list of constraints.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "If the activity displayed in a cell is (permanently) locked in time or space or has a preferred day, it contains abbreviations to show that: PLT (permanently locked time),"
		" LT (locked time), PLS (permanently locked space), LS (locked space), or PD (preferred day), as a tooltip.", "Translate the abbreviations also. Make sure the abbreviations in your language are different between"
		" themselves and the user can differentiate easily between them. These abbreviations may appear also in other places, please use the same abbreviations.");

	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "If a whole column (day+hour) is selected, there will be locked/unlocked also the dummy activities (activities with no students sets) from that column.");
	
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "A bold font cell means that the activity is locked in time, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An italic font cell means that the activity is locked in space, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An underlined font cell means that the activity has a preferred day constraint (with any weight).");
	
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "The button 'Lock to days' will add a constraint of type 'Activity preferred day' to each selected activity, with the selected weight, to the day"
		" in which that activity is scheduled. The button 'Unlock from days' does the opposite, and the button 'All' on its right will unlock the day"
		" from all the activities, even if they are present or not in this timetable view dialog (they may be dummy activities - without any student set, or"
		" not scheduled yet).");
	s+=" ";
	//s+=QCoreApplication::translate("TimetableViewForm", "This feature was suggested by %1 and %2 and might be useful in countries or regions in which"
	//	" the teachers structure changes over the first months of the teaching period. Teachers may come and go and a regeneration of the timetable"
	//	" is needed, but in such a way that most activities retain their scheduled day, to keep for instance exams/tests on the same day as in the old timetable.")
	//	.arg("Vangelis Karafillidis").arg("Alexey Chernous");
	s+=QCoreApplication::translate("TimetableViewForm", "This feature was suggested by %1 and %2 and may be useful in countries or regions where "
		"teachers are not all hired from the beginning of the school year, but are hired gradually, as well as teacher turnover "
		"(changes in their placement in schools by the competent authority) during the first months of the school year. In cases where a new "
		"teacher is hired, or another teacher's placement changes, the timetable needs to be updated, but in such a way that as many activities "
		"as possible remain on the same day on which they were scheduled (placed), so that - for example - the scheduled tests do not need to change "
		"their initially scheduled day.", "%1 and %2 are two users").arg("Vangelis Karafillidis").arg("Alexey Chernous");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
