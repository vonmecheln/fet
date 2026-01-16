/*
File utilities.cpp
*/

/***************************************************************************
                          utilities.cpp  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "utilities.h"

#include <QtGlobal>

#ifndef FET_COMMAND_LINE

#include <Qt>

#include "rules.h"
#include "timetable.h"
#include "studentsset.h"

#include "fetmainform.h"

#include "timetableshowconflictsform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentsdaysverticalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteachersdaysverticalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomsdaysverticalform.h"
#include "timetableviewroomstimehorizontalform.h"

#include <QWidget>
#include <QApplication>
#include <QWidgetList>

#include <QStyle>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QGuiApplication>
#include <QScreen>
#else
#include <QDesktopWidget>
#endif

#include <QRect>
#include <QSize>
#include <QPoint>

#include <QCoreApplication>
#include <QMessageBox>

#include <QSettings>

#include <QObject>
#include <QMetaObject>

#include <QString>

#include <QHeaderView>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>

#include <QAbstractButton>

#include <QComboBox>

#include <QIcon>

#include <QSet>

#include <QBrush>

#define YES		(QString("X"))
#define YESV	(QString("✓"))
#define NO		(QString(" "))

extern const QString COMPANY;
extern const QString PROGRAM;

extern FetMainForm* pFetMainForm;

//extern QApplication* pqapplication;

extern Timetable gt;

CornerEnabledTableWidget::CornerEnabledTableWidget(bool _useColors, bool _V): QTableWidget()
{
	useColors=_useColors;
	V=_V;
}

void CornerEnabledTableWidget::selectAll()
{
	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	QString newText;
	if(item(0, 0)->text()==NO)
		newText=(V?YESV:YES);
	else
		newText=NO;
	for(int i=0; i<nH; i++)
		for(int j=0; j<nD; j++){
			item(i, j)->setText(newText);
			colorItemTimesTable(this, item(i,j));
		}
}

CornerEnabledTableWidgetOfSpinBoxes::CornerEnabledTableWidgetOfSpinBoxes(): QTableWidget()
{
}

void CornerEnabledTableWidgetOfSpinBoxes::selectAll()
{
	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	for(int i=0; i<nH; i++)
		for(int j=0; j<nD; j++)
			((QSpinBox*)cellWidget(i, j))->setValue(0);
}

void centerWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);

	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
/*	QRect frect=widget->frameGeometry();
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
	widget->move(frect.topLeft());*/
}

void forceCenterWidgetOnScreen(QWidget* widget)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	QRect frect=widget->frameGeometry();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	frect.moveCenter(widget->screen()->availableGeometry().center());
#else
	frect.moveCenter(QApplication::desktop()->availableGeometry(widget).center());
#endif
	widget->move(frect.topLeft());
}

/*void centerWidgetOnParent(QWidget* widget, QWidget* parent)
{
	//widget->setWindowFlags(widget->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	
	assert(parent!=nullptr);
	
	QRect frect=widget->geometry();
	frect.moveCenter(parent->geometry().center());
	widget->move(frect.topLeft());
}*/

int maxScreenWidth(QWidget* widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	QRect rect = widget->screen()->availableGeometry();
#else
	QRect rect = QApplication::desktop()->availableGeometry(widget);
#endif

	return rect.width();
}

int maxRecommendedWidth(QWidget* widget)
{
	int d=maxScreenWidth(widget);
	
	if(d<800)
		d=800;
		
	//if(d>1000)
	//	d=1000;
	
	return d;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name;
	if(alternativeName.isEmpty())
		name=QString(widget->metaObject()->className());
	else
		name=alternativeName;
	
	QRect rect=widget->geometry();
	settings.setValue(name+QString("/geometry"), rect);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	QSettings settings(COMPANY, PROGRAM);
	QString name;
	if(alternativeName.isEmpty())
		name=QString(widget->metaObject()->className());
	else
		name=alternativeName;
	
	if(settings.contains(name+QString("/geometry"))){
		QRect rect=settings.value(name+QString("/geometry")).toRect();
		if(rect.isValid()){
			bool ok=false;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
			for(QScreen* screen : QGuiApplication::screens()){
				if(screen->availableGeometry().intersects(rect)){
#else
			for(int i=0; i<QApplication::desktop()->screenCount(); i++){
				if(QApplication::desktop()->availableGeometry(i).intersects(rect)){
#endif
					ok=true;
					break;
				}
			}
		
			if(ok){
				widget->setGeometry(rect);
			}
		}
	}
}

void setParentAndOtherThings(QWidget* widget, QWidget* parent)
{
	Q_UNUSED(widget);
	Q_UNUSED(parent);

/*	if(!widget->parentWidget()){
		widget->setParent(parent, Qt::Dialog);
	
		QRect rect=widget->geometry();
		rect.translate(widget->geometry().x() - widget->frameGeometry().x(), widget->geometry().y() - widget->frameGeometry().y());
		widget->setGeometry(rect);
	}*/
}

void setStretchAvailabilityTableNicely(CornerEnabledTableWidget* tableWidget)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	int q=tableWidget->horizontalHeader()->defaultSectionSize();
	tableWidget->horizontalHeader()->setMinimumSectionSize(q);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	q=-1;
	for(int i=0; i<tableWidget->verticalHeader()->count(); i++)
		if(q<tableWidget->verticalHeader()->sectionSizeHint(i))
			q=tableWidget->verticalHeader()->sectionSizeHint(i);
	tableWidget->verticalHeader()->setMinimumSectionSize(q);
	
	//2011-09-23
	/*for(int i=0; i<tableWidget->rowCount(); i++){
		for(int j=0; j<tableWidget->columnCount(); j++){
			QFont font=tableWidget->item(i,j)->font();
			font.setBold(true);
			tableWidget->item(i,j)->setFont(font);
		}
	}*/
	tableWidget->setCornerButtonEnabled(true);

	//As in the second answer on https://stackoverflow.com/questions/22635867/is-it-possible-to-set-the-text-of-the-qtableview-corner-button (2025-02-23)
	QAbstractButton* button=tableWidget->findChild<QAbstractButton*>();
	if(button!=nullptr){
		QVBoxLayout* layout=new QVBoxLayout(button);
		layout->setContentsMargins(0, 0, 0, 0);
		QLabel* label=new QLabel(QString("∀"));
		//label->setStyleSheet("QLabel {font-face: ArialMT; font-size: 10px; color: #FFFFFF; font-weight: bold; }""QToolTip { color: #ffffff; background-color: #000000; border: 1px #000000; }");
		label->setAlignment(Qt::AlignCenter);
		//label->setToolTip("Text");
		label->setContentsMargins(2, 2, 2, 2);
		layout->addWidget(label);
	}
}

void setStretchAvailabilityTableNicely(CornerEnabledTableWidgetOfSpinBoxes* tableWidget)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	int q=tableWidget->horizontalHeader()->defaultSectionSize();
	tableWidget->horizontalHeader()->setMinimumSectionSize(q);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

	q=-1;
	for(int i=0; i<tableWidget->verticalHeader()->count(); i++)
		if(q<tableWidget->verticalHeader()->sectionSizeHint(i))
			q=tableWidget->verticalHeader()->sectionSizeHint(i);
	tableWidget->verticalHeader()->setMinimumSectionSize(q);
	
	//2011-09-23
	/*for(int i=0; i<tableWidget->rowCount(); i++){
		for(int j=0; j<tableWidget->columnCount(); j++){
			QFont font=tableWidget->item(i,j)->font();
			font.setBold(true);
			tableWidget->item(i,j)->setFont(font);
		}
	}*/
	tableWidget->setCornerButtonEnabled(true);

	//As in the second answer on https://stackoverflow.com/questions/22635867/is-it-possible-to-set-the-text-of-the-qtableview-corner-button (2025-02-23)
	QAbstractButton* button=tableWidget->findChild<QAbstractButton*>();
	if(button!=nullptr){
		QVBoxLayout* layout=new QVBoxLayout(button);
		layout->setContentsMargins(0, 0, 0, 0);
		QLabel* label=new QLabel(QString("∀"));
		//label->setStyleSheet("QLabel {font-face: ArialMT; font-size: 10px; color: #FFFFFF; font-weight: bold; }""QToolTip { color: #ffffff; background-color: #000000; border: 1px #000000; }");
		label->setAlignment(Qt::AlignCenter);
		//label->setToolTip("Text");
		label->setContentsMargins(2, 2, 2, 2);
		layout->addWidget(label);
	}
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	if(!rules->modified){
		rules->modified=true;
	
		if(rules==&gt.rules && pFetMainForm!=nullptr)
			if(!pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(true);
	}
}

void setRulesUnmodifiedAndOtherThings(Rules* rules)
{
	if(rules->modified){
		rules->modified=false;
	
		if(rules==&gt.rules && pFetMainForm!=nullptr)
			if(pFetMainForm->isWindowModified())
				pFetMainForm->setWindowModified(false);
	}
}

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current constraint is referring to subgroup %1, so the constraint is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");

	QMessageBox::warning(parent, title, message);
}

void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " You are trying to modify a constraint, leaving it to refer to the original subgroup %1, which is invisible in the combo box - but this is impossible.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");
	
	QMessageBox::warning(parent, title, message);
}

void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName)
{
	QString title=QCoreApplication::translate("VariousGlobalWarningMessages", "FET warning");

	QString message=QCoreApplication::translate("VariousGlobalWarningMessages", "Subgroups are invisible in combo boxes, by a global setting which you activated."
	 " The current activity is referring to subgroup %1, so the activity is displayed incorrectly with a void subgroup here.")
	 .arg(initialSubgroupName);
	message+="\n\n";
	message+=QCoreApplication::translate("VariousGlobalWarningMessages", "If you want, you can revert to the initial setting to show subgroups in combo boxes.");

	QMessageBox::warning(parent, title, message);
}

int populateStudentsComboBox(QComboBox* studentsComboBox, const QString& selectedStudentsSet, bool addEmptyAtBeginning)
{
	studentsComboBox->clear();
	
	int currentIndex=0;
	int selectedIndex=-1;
	
	if(addEmptyAtBeginning){
		studentsComboBox->addItem(QString(""));
		if(selectedStudentsSet==QString(""))
			selectedIndex=currentIndex;
		currentIndex++;
		if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED && gt.rules.yearsList.count()>0){
#if QT_VERSION >= QT_VERSION_CHECK(4,4,0)
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
		}
	}

	if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_SIMPLE){
		for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			for(StudentsGroup* stg : std::as_const(sty->groupsList)){
				studentsComboBox->addItem(stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
						studentsComboBox->addItem(sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_ICONS){
		for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			for(StudentsGroup* stg : std::as_const(sty->groupsList)){
				studentsComboBox->addItem(QIcon(":/images/group.png"), stg->name);
				if(stg->name==selectedStudentsSet)
					selectedIndex=currentIndex;
				currentIndex++;
				if(SHOW_SUBGROUPS_IN_COMBO_BOXES){
					for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
						studentsComboBox->addItem(QIcon(":/images/subgroup.png"), sts->name);
						if(sts->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
					}
				}
			}
		}
	}
	else if(STUDENTS_COMBO_BOXES_STYLE==STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED){
		QSet<QString> years;
		
		bool haveGroups=false;
	
		for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
			assert(!years.contains(sty->name));
			years.insert(sty->name);
			studentsComboBox->addItem(sty->name);
			if(sty->name==selectedStudentsSet)
				selectedIndex=currentIndex;
			currentIndex++;
			
			if(!haveGroups && sty->groupsList.count()>0)
				haveGroups=true;
		}
		
		if(haveGroups){
#if QT_VERSION >= QT_VERSION_CHECK(4,4,0)
			studentsComboBox->insertSeparator(studentsComboBox->count());
			currentIndex++;
#endif
			QSet<QString> groups;
		
			bool haveSubgroups=false;
	
			for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* stg : std::as_const(sty->groupsList)){
					if(!groups.contains(stg->name)){
						groups.insert(stg->name);
						studentsComboBox->addItem(stg->name);
						if(stg->name==selectedStudentsSet)
							selectedIndex=currentIndex;
						currentIndex++;
						
						if(!haveSubgroups && stg->subgroupsList.count()>0)
							haveSubgroups=true;
					}
				}
			}

			if(SHOW_SUBGROUPS_IN_COMBO_BOXES && haveSubgroups){
#if QT_VERSION >= QT_VERSION_CHECK(4,4,0)
				studentsComboBox->insertSeparator(studentsComboBox->count());
				currentIndex++;
#endif

				QSet<QString> subgroups;

				for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
					for(StudentsGroup* stg : std::as_const(sty->groupsList)){
						for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
							if(!subgroups.contains(sts->name)){
								subgroups.insert(sts->name);
								studentsComboBox->addItem(sts->name);
								if(sts->name==selectedStudentsSet)
									selectedIndex=currentIndex;
								currentIndex++;
							}
						}
					}
				}
			}
		}
	}
	else{
		assert(0);
	}
	
	return selectedIndex;
}

/*void closeAllTimetableViewDialogs()
{
	//QList<QWidget*> tlwl=pqapplication->topLevelWidgets();
	QWidgetList tlwl=QApplication::topLevelWidgets();

	for(QWidget* wi : std::as_const(tlwl))
		//if(wi->isVisible()){
		if(1){
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdf!=nullptr){
				vsdf->close();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vstf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vstf!=nullptr){
				vstf->close();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdf!=nullptr){
				vtchdf->close();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchtf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchtf!=nullptr){
				vtchtf->close();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdf!=nullptr){
				vrdf->close();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrtf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrtf!=nullptr){
				vrtf->close();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=nullptr){
				scf->close();
				continue;
			}
		}
}*/

void updateAllTimetableViewDialogs()
{
	//QList<QWidget*> tlwl=pqapplication->topLevelWidgets();
	QWidgetList tlwl=QApplication::topLevelWidgets();

	for(QWidget* wi : std::as_const(tlwl))
		if(true /*wi->isVisible()*/){
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdhf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdhf!=nullptr){
				vsdhf->newTimetableGenerated();
				continue;
			}

			TimetableViewStudentsDaysVerticalForm* vsdvf=qobject_cast<TimetableViewStudentsDaysVerticalForm*>(wi);
			if(vsdvf!=nullptr){
				vsdvf->newTimetableGenerated();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vsthf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vsthf!=nullptr){
				vsthf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdhf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdhf!=nullptr){
				vtchdhf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersDaysVerticalForm* vtchdvf=qobject_cast<TimetableViewTeachersDaysVerticalForm*>(wi);
			if(vtchdvf!=nullptr){
				vtchdvf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchthf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchthf!=nullptr){
				vtchthf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdhf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdhf!=nullptr){
				vrdhf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsDaysVerticalForm* vrdvf=qobject_cast<TimetableViewRoomsDaysVerticalForm*>(wi);
			if(vrdvf!=nullptr){
				vrdvf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrthf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrthf!=nullptr){
				vrthf->newTimetableGenerated();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=nullptr){
				scf->newTimetableGenerated();
				continue;
			}
		}
}

void highlightOnHorizontalHeaderClicked(CornerEnabledTableWidget* tableWidget, int col)
{
	bool respectsHeaderBackgroundColor=false;
#ifndef Q_OS_WIN
	respectsHeaderBackgroundColor=true;
#else
#if QT_VERSION >= QT_VERSION_CHECK(6,1,0)
	QString styleName=qApp->style()->name();
	if(QString::compare(styleName, QString("windowsvista"), Qt::CaseInsensitive)!=0
	 && QString::compare(styleName, QString("windows11"), Qt::CaseInsensitive)!=0)
		respectsHeaderBackgroundColor=true;
#endif
#endif

	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	if(col>=0 && col<nD){
		for(int j=0; j<nD; j++){
			if(j!=col){
				QTableWidgetItem* hhi=tableWidget->horizontalHeaderItem(j);
				if(hhi!=nullptr){
					if(respectsHeaderBackgroundColor){
						hhi->setBackground(tableWidget->palette().base());
						hhi->setForeground(tableWidget->palette().text());
					}
					else{
						QFont font=tableWidget->font();
						font.setBold(false);
						font.setItalic(false);
						hhi->setFont(font);
					}
				}
			}
		}
		for(int i=0; i<nH; i++){
			QTableWidgetItem* vhi=tableWidget->verticalHeaderItem(i);
			if(vhi!=nullptr){
				if(respectsHeaderBackgroundColor){
					vhi->setBackground(tableWidget->palette().base());
					vhi->setForeground(tableWidget->palette().text());
				}
				else{
					QFont font=tableWidget->font();
					font.setBold(false);
					font.setItalic(false);
					vhi->setFont(font);
				}
			}
		}

		QTableWidgetItem* hhi=tableWidget->horizontalHeaderItem(col);
		if(hhi!=nullptr){
			if(respectsHeaderBackgroundColor){
				hhi->setBackground(tableWidget->palette().highlight());
				hhi->setForeground(tableWidget->palette().highlightedText());
			}
			else{
				QFont font=tableWidget->font();
				font.setBold(true);
				font.setItalic(true);
				hhi->setFont(font);
			}
		}
	}
}

void highlightOnVerticalHeaderClicked(CornerEnabledTableWidget* tableWidget, int row)
{
	bool respectsHeaderBackgroundColor=false;
#ifndef Q_OS_WIN
	respectsHeaderBackgroundColor=true;
#else
#if QT_VERSION >= QT_VERSION_CHECK(6,1,0)
	QString styleName=qApp->style()->name();
	if(QString::compare(styleName, QString("windowsvista"), Qt::CaseInsensitive)!=0
	 && QString::compare(styleName, QString("windows11"), Qt::CaseInsensitive)!=0)
		respectsHeaderBackgroundColor=true;
#endif
#endif

	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	if(row>=0 && row<nH){
		for(int j=0; j<nD; j++){
			QTableWidgetItem* hhi=tableWidget->horizontalHeaderItem(j);
			if(hhi!=nullptr){
				if(respectsHeaderBackgroundColor){
					hhi->setBackground(tableWidget->palette().base());
					hhi->setForeground(tableWidget->palette().text());
				}
				else{
					QFont font=tableWidget->font();
					font.setBold(false);
					font.setItalic(false);
					hhi->setFont(font);
				}
			}
		}
		for(int i=0; i<nH; i++){
			if(i!=row){
				QTableWidgetItem* vhi=tableWidget->verticalHeaderItem(i);
				if(vhi!=nullptr){
					if(respectsHeaderBackgroundColor){
						vhi->setBackground(tableWidget->palette().base());
						vhi->setForeground(tableWidget->palette().text());
					}
					else{
						QFont font=tableWidget->font();
						font.setBold(false);
						font.setItalic(false);
						vhi->setFont(font);
					}
				}
			}
		}

		QTableWidgetItem* vhi=tableWidget->verticalHeaderItem(row);
		if(vhi!=nullptr){
			if(respectsHeaderBackgroundColor){
				vhi->setBackground(tableWidget->palette().highlight());
				vhi->setForeground(tableWidget->palette().highlightedText());
			}
			else{
				QFont font=tableWidget->font();
				font.setBold(true);
				font.setItalic(true);
				vhi->setFont(font);
			}
		}
	}
}

void highlightOnCellEntered(CornerEnabledTableWidget* tableWidget, int row, int col)
{
	bool respectsHeaderBackgroundColor=false;
#ifndef Q_OS_WIN
	respectsHeaderBackgroundColor=true;
#else
#if QT_VERSION >= QT_VERSION_CHECK(6,1,0)
	QString styleName=qApp->style()->name();
	if(QString::compare(styleName, QString("windowsvista"), Qt::CaseInsensitive)!=0
	 && QString::compare(styleName, QString("windows11"), Qt::CaseInsensitive)!=0)
		respectsHeaderBackgroundColor=true;
#endif
#endif

	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	if(row>=0 && row<nH && col>=0 && col<nD){
		for(int j=0; j<nD; j++){
			if(j!=col){
				QTableWidgetItem* hhi=tableWidget->horizontalHeaderItem(j);
				if(hhi!=nullptr){
					if(respectsHeaderBackgroundColor){
						hhi->setBackground(tableWidget->palette().base());
						hhi->setForeground(tableWidget->palette().text());
					}
					else{
						QFont font=tableWidget->font();
						font.setBold(false);
						font.setItalic(false);
						hhi->setFont(font);
					}
				}
			}
		}

		for(int i=0; i<nH; i++){
			if(i!=row){
				QTableWidgetItem* vhi=tableWidget->verticalHeaderItem(i);
				if(vhi!=nullptr){
					if(respectsHeaderBackgroundColor){
						vhi->setBackground(tableWidget->palette().base());
						vhi->setForeground(tableWidget->palette().text());
					}
					else{
						QFont font=tableWidget->font();
						font.setBold(false);
						font.setItalic(false);
						vhi->setFont(font);
					}
				}
			}
		}

		QTableWidgetItem* hhi=tableWidget->horizontalHeaderItem(col);
		if(hhi!=nullptr){
			if(respectsHeaderBackgroundColor){
				hhi->setBackground(tableWidget->palette().highlight());
				hhi->setForeground(tableWidget->palette().highlightedText());
			}
			else{
				QFont font=tableWidget->font();
				font.setBold(true);
				font.setItalic(true);
				hhi->setFont(font);
			}
		}

		QTableWidgetItem* vhi=tableWidget->verticalHeaderItem(row);
		if(vhi!=nullptr){
			if(respectsHeaderBackgroundColor){
				vhi->setBackground(tableWidget->palette().highlight());
				vhi->setForeground(tableWidget->palette().highlightedText());
			}
			else{
				QFont font=tableWidget->font();
				font.setBold(true);
				font.setItalic(true);
				vhi->setFont(font);
			}
		}
	}
}

void tableViewSetHighlightHeader(QTableView* tableWidget)
{
	//To better highlight the selected row/column, for instance if the user chose a bold interface font,
	//the selected header section looked identical with the others (unselected).
	//Sources of inspiration, as found on the internet,
	//here: https://www.qtcentre.org/threads/57210-how-to-change-background-color-of-individual-QHeaderView-section ,
	//here: https://www.qtcentre.org/threads/46841-Can-t-style-QHeaderView-section-selected-in-QSS-stylesheet ,
	//here: https://stackoverflow.com/questions/15519749/how-to-get-widget-background-qcolor ,
	//and in the Qt documentation.
	QString bc=qApp->palette().highlight().color().name();
	QString fc=qApp->palette().highlightedText().color().name();
	tableWidget->setStyleSheet(QString("QHeaderView::section:checked { background-color: ")+bc+QString("; color: ")+fc+QString(" }"));
	//this->setStyleSheet(QString("QHeaderView::section:hover { background-color: ")+bc+QString("; color: ")+fc+QString(" }"));
}

void colorItemTimesTable(CornerEnabledTableWidget* timesTable, QTableWidgetItem* item)
{
	if(timesTable->useColors){
		if(timesTable->V){
			if(item->text()==NO)
				item->setBackground(QBrush(QColorConstants::Svg::darkgoldenrod));
			else
				item->setBackground(QBrush(QColorConstants::Svg::darkcyan));
			item->setForeground(QBrush(QColorConstants::LightGray));
		}
		else{
			if(item->text()==NO)
				item->setBackground(QBrush(QColorConstants::DarkGreen));
			else
				item->setBackground(QBrush(QColorConstants::DarkRed));
			item->setForeground(QBrush(QColorConstants::LightGray));
		}
	}
	else{
		item->setBackground(timesTable->palette().base());
		item->setForeground(timesTable->palette().text());
	}
}

void initTimesTable(CornerEnabledTableWidget* timesTable)
{
	timesTable->clear();

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		timesTable->setRowCount(gt.rules.nHoursPerDay);
		timesTable->setColumnCount(gt.rules.nDaysPerWeek);

		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
			timesTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
			timesTable->setVerticalHeaderItem(i, item);
		}

		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++){
				QTableWidgetItem* item=new QTableWidgetItem(NO);

				QFont font=item->font();
				font.setBold(!timesTable->V);
				item->setFont(font);

				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				colorItemTimesTable(timesTable, item);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(gt.rules.daysOfTheWeek[j]+QString("\n")+gt.rules.hoursOfTheDay[i]);
				timesTable->setItem(i, j, item);
			}
	}
	else{
		timesTable->setRowCount(2*gt.rules.nHoursPerDay);
		timesTable->setColumnCount(gt.rules.nDaysPerWeek/2);

		for(int j=0; j<gt.rules.nDaysPerWeek/2; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]);
			timesTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<2*gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]);
			timesTable->setVerticalHeaderItem(i, item);
		}

		for(int i=0; i<2*gt.rules.nHoursPerDay; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek/2; j++){
				QTableWidgetItem* item=new QTableWidgetItem(NO);

				QFont font=item->font();
				font.setBold(!timesTable->V);
				item->setFont(font);

				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				colorItemTimesTable(timesTable, item);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(gt.rules.realDaysOfTheWeek[j]+QString("\n")+gt.rules.realHoursOfTheDay[i]);
				timesTable->setItem(i, j, item);
			}
	}
}

void fillTimesTable(CornerEnabledTableWidget* timesTable, const QList<int>& days, const QList<int>& hours, bool direct)
{
	timesTable->clearContents();

	Matrix2D<bool> currentMatrix;
	currentMatrix.resize(gt.rules.nHoursPerDay, gt.rules.nDaysPerWeek);

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int i=hours.at(k);
		int j=days.at(k);
		if(i>=0 && i<gt.rules.nHoursPerDay && j>=0 && j<gt.rules.nDaysPerWeek)
			currentMatrix[i][j]=true;
	}

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++){
				QTableWidgetItem* item=new QTableWidgetItem;

				QFont font=item->font();
				font.setBold(!timesTable->V);
				item->setFont(font);

				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(gt.rules.daysOfTheWeek[j]+QString("\n")+gt.rules.hoursOfTheDay[i]);
				timesTable->setItem(i, j, item);

				if(direct){
					if(currentMatrix[i][j])
						item->setText(timesTable->V?YESV:YES);
					else
						item->setText(NO);
				}
				else{
					if(currentMatrix[i][j])
						item->setText(NO);
					else
						item->setText(timesTable->V?YESV:YES);
				}

				colorItemTimesTable(timesTable, item);
			}
	}
	else{
		for(int i=0; i<2*gt.rules.nHoursPerDay; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek/2; j++){
				QTableWidgetItem* item=new QTableWidgetItem;

				QFont font=item->font();
				font.setBold(!timesTable->V);
				item->setFont(font);

				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(gt.rules.realDaysOfTheWeek[j]+QString("\n")+gt.rules.realHoursOfTheDay[i]);
				timesTable->setItem(i, j, item);

				if(direct){
					if(currentMatrix[i%gt.rules.nHoursPerDay][2*j+i/gt.rules.nHoursPerDay])
						item->setText(timesTable->V?YESV:YES);
					else
						item->setText(NO);
				}
				else{
					if(currentMatrix[i%gt.rules.nHoursPerDay][2*j+i/gt.rules.nHoursPerDay])
						item->setText(NO);
					else
						item->setText(timesTable->V?YESV:YES);
				}

				colorItemTimesTable(timesTable, item);
			}
	}
}

void getTimesTable(CornerEnabledTableWidget* timesTable, QList<int>& days, QList<int>& hours, bool direct)
{
	days.clear();
	hours.clear();
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				if(direct){
					if(timesTable->item(i, j)->text()==(timesTable->V?YESV:YES)){
						days.append(j);
						hours.append(i);
					}
				}
				else{
					if(timesTable->item(i, j)->text()==NO){
						days.append(j);
						hours.append(i);
					}
				}
	}
	else{
		for(int j=0; j<gt.rules.nDaysPerWeek/2; j++)
			for(int i=0; i<2*gt.rules.nHoursPerDay; i++)
				if(direct){
					if(timesTable->item(i, j)->text()==(timesTable->V?YESV:YES)){
						days.append(2*j+i/gt.rules.nHoursPerDay);
						hours.append(i%gt.rules.nHoursPerDay);
					}
				}
				else{
					if(timesTable->item(i, j)->text()==NO){
						days.append(2*j+i/gt.rules.nHoursPerDay);
						hours.append(i%gt.rules.nHoursPerDay);
					}
				}
	}
}

void horizontalHeaderClickedTimesTable(CornerEnabledTableWidget* timesTable, int col)
{
	highlightOnHorizontalHeaderClicked(timesTable, col);

	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	if(col>=0 && col<nD){
		QString s=timesTable->item(0, col)->text();
		if(s==(timesTable->V?YESV:YES))
			s=NO;
		else{
			assert(s==NO);
			s=(timesTable->V?YESV:YES);
		}

		for(int row=0; row<nH; row++){
			timesTable->item(row, col)->setText(s);
			colorItemTimesTable(timesTable, timesTable->item(row,col));
		}
	}
}

void verticalHeaderClickedTimesTable(CornerEnabledTableWidget* timesTable, int row)
{
	highlightOnVerticalHeaderClicked(timesTable, row);

	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	if(row>=0 && row<nH){
		QString s=timesTable->item(row, 0)->text();
		if(s==(timesTable->V?YESV:YES))
			s=NO;
		else{
			assert(s==NO);
			s=(timesTable->V?YESV:YES);
		}

		for(int col=0; col<nD; col++){
			timesTable->item(row, col)->setText(s);
			colorItemTimesTable(timesTable, timesTable->item(row,col));
		}
	}
}

void cellEnteredTimesTable(CornerEnabledTableWidget* timesTable, int row, int col)
{
	highlightOnCellEntered(timesTable, row, col);
}

void colorsCheckBoxToggledTimesTable(CornerEnabledTableWidget* timesTable)
{
	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	for(int i=0; i<nH; i++)
		for(int j=0; j<nD; j++)
			colorItemTimesTable(timesTable, timesTable->item(i,j));
}

void toggleAllClickedTimesTable(CornerEnabledTableWidget* timesTable)
{
	int nD, nH;
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		nD=gt.rules.nDaysPerWeek;
		nH=gt.rules.nHoursPerDay;
	}
	else{
		nD=gt.rules.nDaysPerWeek/2;
		nH=2*gt.rules.nHoursPerDay;
	}

	for(int i=0; i<nH; i++)
		for(int j=0; j<nD; j++){
			QString newText;
			if(timesTable->item(i, j)->text()==NO)
				newText=(timesTable->V?YESV:YES);
			else
				newText=NO;

			timesTable->item(i, j)->setText(newText);
			colorItemTimesTable(timesTable, timesTable->item(i,j));
		}
}

void itemClickedTimesTable(CornerEnabledTableWidget* timesTable, QTableWidgetItem* item)
{
	QString s=item->text();
	if(s==(timesTable->V?YESV:YES))
		s=NO;
	else{
		assert(s==NO);
		s=(timesTable->V?YESV:YES);
	}
	item->setText(s);
	colorItemTimesTable(timesTable, item);
}

#else

/*void centerWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);
}

void forceCenterWidgetOnScreen(QWidget* widget)
{
	Q_UNUSED(widget);
}

int maxScreenWidth(QWidget* widget)
{
	Q_UNUSED(widget);
	
	return 0;
}

int maxRecommendedWidth(QWidget* widget)
{
	Q_UNUSED(widget);

	return 0;
}

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	Q_UNUSED(widget);
	Q_UNUSED(alternativeName);
}

void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName)
{
	Q_UNUSED(widget);
	Q_UNUSED(alternativeName);
}

void setParentAndOtherThings(QWidget* widget, QWidget* parent)
{
	Q_UNUSED(widget);
	Q_UNUSED(parent);
}

void setStretchAvailabilityTableNicely(QTableWidget* tableWidget)
{
	Q_UNUSED(tableWidget);
}*/

void setRulesModifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

/*void setRulesUnmodifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName)
{
	Q_UNUSED(parent);
	Q_UNUSED(initialSubgroupName);
}

void populateStudentsComboBox(QComboBox* studentsComboBox)
{
	Q_UNUSED(studentsComboBox);
}*/

/*void closeAllTimetableViewDialogs()
{
}*/

/*void updateAllTimetableViewDialogs()
{
}*/

#endif
