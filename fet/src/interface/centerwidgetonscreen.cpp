/*
File centerwidgetonscreen.cpp
*/

/***************************************************************************
                          centerwidgetonscreen.cpp  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Liviu Lalescu
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

#include "rules.h"
#include "timetable.h"
#include "studentsset.h"

#ifndef FET_COMMAND_LINE
#include "fetmainform.h"

#include "timetableshowconflictsform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
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
#endif

#include <QObject>
#include <QMetaObject>

#include <QString>

#ifndef FET_COMMAND_LINE
#include <QHeaderView>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>

#include <QComboBox>

#include <QIcon>

#include <QSet>

extern const QString COMPANY;
extern const QString PROGRAM;

extern FetMainForm* pFetMainForm;

//extern QApplication* pqapplication;

extern Timetable gt;

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

void setStretchAvailabilityTableNicely(QTableWidget* tableWidget)
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
	for(int i=0; i<tableWidget->rowCount(); i++){
		for(int j=0; j<tableWidget->columnCount(); j++){
			QFont font=tableWidget->item(i,j)->font();
			font.setBold(true);
			tableWidget->item(i,j)->setFont(font);
		}
	}
	tableWidget->setCornerButtonEnabled(false);
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
		if(1 /*wi->isVisible()*/){
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdf!=nullptr){
				vsdf->newTimetableGenerated();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vstf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vstf!=nullptr){
				vstf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdf!=nullptr){
				vtchdf->newTimetableGenerated();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchtf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchtf!=nullptr){
				vtchtf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdf!=nullptr){
				vrdf->newTimetableGenerated();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrtf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrtf!=nullptr){
				vrtf->newTimetableGenerated();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=nullptr){
				scf->newTimetableGenerated();
				continue;
			}
		}
}

void highlightOnHorizontalHeaderClicked(QTableWidget* tableWidget, int col)
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

	if(col>=0 && col<gt.rules.nDaysPerWeek){
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
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
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
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

void highlightOnVerticalHeaderClicked(QTableWidget* tableWidget, int row)
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

	if(row>=0 && row<gt.rules.nHoursPerDay){
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
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
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
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

void highlightOnCellEntered(QTableWidget* tableWidget, int row, int col)
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

	if(row>=0 && row<gt.rules.nHoursPerDay && col>=0 && col<gt.rules.nDaysPerWeek){
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
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

		for(int i=0; i<gt.rules.nHoursPerDay; i++)
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

#else

void centerWidgetOnScreen(QWidget* widget)
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
}

void setRulesModifiedAndOtherThings(Rules* rules)
{
	Q_UNUSED(rules);
}

void setRulesUnmodifiedAndOtherThings(Rules* rules)
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
}

/*void closeAllTimetableViewDialogs()
{
}*/

void updateAllTimetableViewDialogs()
{
}

#endif
