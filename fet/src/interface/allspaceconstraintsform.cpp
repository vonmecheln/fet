/***************************************************************************
                          allspaceconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "allspaceconstraintsform.h"

#include "modifyconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintroomnotavailabletimesform.h"
#include "modifyconstraintactivitypreferredroomform.h"
#include "modifyconstraintactivitypreferredroomsform.h"

#include "modifyconstraintstudentssethomeroomform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include "modifyconstraintteacherhomeroomform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include "modifyconstraintstudentssetmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperdayform.h"
#include "modifyconstraintstudentssetmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentsmaxbuildingchangesperweekform.h"
#include "modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentsmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintteachermaxbuildingchangesperdayform.h"
#include "modifyconstraintteachersmaxbuildingchangesperdayform.h"
#include "modifyconstraintteachermaxbuildingchangesperweekform.h"
#include "modifyconstraintteachersmaxbuildingchangesperweekform.h"
#include "modifyconstraintteachermingapsbetweenbuildingchangesform.h"
#include "modifyconstraintteachersmingapsbetweenbuildingchangesform.h"

#include "modifyconstraintsubjectpreferredroomform.h"
#include "modifyconstraintsubjectpreferredroomsform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomsform.h"

#include "modifyconstraintactivitytagpreferredroomform.h"
#include "modifyconstraintactivitytagpreferredroomsform.h"

#include "lockunlock.h"

#include "advancedfilterform.h"

#include <QRegExp>

#include <QTextEdit>
#include <QListWidget>
#include <QScrollBar>

#include <QAbstractItemView>

const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;


bool AllSpaceConstraintsForm::filterInitialized=false;
bool AllSpaceConstraintsForm::all=true;
QList<int> AllSpaceConstraintsForm::descrDetDescr;
QList<int> AllSpaceConstraintsForm::contains;
QStringList AllSpaceConstraintsForm::text;
bool AllSpaceConstraintsForm::caseSensitive=false;

AllSpaceConstraintsForm::AllSpaceConstraintsForm()
{
    setupUi(this);
    
    modifyConstraintPushButton->setDefault(true);
    
    constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this /*AllSpaceConstraintsForm_template*/, SLOT(constraintChanged()));
    connect(closePushButton, SIGNAL(clicked()), this /*AllSpaceConstraintsForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*AllSpaceConstraintsForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*AllSpaceConstraintsForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this /*AllSpaceConstraintsForm_template*/, SLOT(modifyConstraint()));

   connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));

	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		

	if(!this->filterInitialized){
		all=true;
		descrDetDescr.clear();
		descrDetDescr.append(DESCRIPTION);
		contains.clear();
		contains.append(CONTAINS);
		text.clear();
		text.append(QString(""));
		caseSensitive=false;
		
		this->filterInitialized=true;
	}

	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);
	
	filterChanged();
}

AllSpaceConstraintsForm::~AllSpaceConstraintsForm()
{
}


bool AllSpaceConstraintsForm::filterOk(SpaceConstraint* ctr)
{
	if(!useFilter)
		return true;

	assert(descrDetDescr.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<bool> okPartial;
	
	for(int i=0; i<descrDetDescr.count(); i++){
		QString s;
		if(descrDetDescr.at(i)==DESCRIPTION)
			s=ctr->getDescription(gt.rules);
		else
			s=ctr->getDetailedDescription(gt.rules);
			
		QString t=text.at(i);
		if(contains.at(i)==CONTAINS){
			okPartial.append(s.contains(t, csens));
		}
		else if(contains.at(i)==DOESNOTCONTAIN){
			okPartial.append(!(s.contains(t, csens)));
		}
		else if(contains.at(i)==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)>=0);
		}
		else if(contains.at(i)==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial.append(regExp.indexIn(s)<0);
		}
		else
			assert(0);
	}
	
	if(all){
		bool ok=true;
		foreach(bool b, okPartial)
			ok = ok && b;
			
		return ok;
	}
	else{ //any
		bool ok=false;
		foreach(bool b, okPartial)
			ok = ok || b;
			
		return ok;
	}
}

void AllSpaceConstraintsForm::filterChanged()
{
	visibleSpaceConstraintsList.clear();
	constraintsListWidget->clear();
	foreach(SpaceConstraint* ctr, gt.rules.spaceConstraintsList)
		if(filterOk(ctr)){
			visibleSpaceConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
		
	if(constraintsListWidget->count()<=0)
		currentConstraintTextEdit->setText("");
	else
		constraintsListWidget->setCurrentRow(0);
	
	constraintsTextLabel->setText(tr("%1 Space Constraints", "%1 represents the number of constraints").arg(visibleSpaceConstraintsList.count()));
}



void AllSpaceConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;

	assert(index<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(index);
	assert(ctr!=NULL);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void AllSpaceConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
	
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
		ModifyConstraintBasicCompulsorySpaceForm form((ConstraintBasicCompulsorySpace*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
		ModifyConstraintRoomNotAvailableTimesForm form((ConstraintRoomNotAvailableTimes*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		ModifyConstraintActivityPreferredRoomForm form((ConstraintActivityPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
		ModifyConstraintActivityPreferredRoomsForm form((ConstraintActivityPreferredRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
		ModifyConstraintStudentsSetHomeRoomForm form((ConstraintStudentsSetHomeRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ModifyConstraintStudentsSetHomeRoomsForm form((ConstraintStudentsSetHomeRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
		ModifyConstraintTeacherHomeRoomForm form((ConstraintTeacherHomeRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ModifyConstraintTeacherHomeRoomsForm form((ConstraintTeacherHomeRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
		ModifyConstraintSubjectPreferredRoomForm form((ConstraintSubjectPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
		ModifyConstraintSubjectPreferredRoomsForm form((ConstraintSubjectPreferredRooms*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintSubjectActivityTagPreferredRoomForm form((ConstraintSubjectActivityTagPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintSubjectActivityTagPreferredRoomsForm form((ConstraintSubjectActivityTagPreferredRooms*)ctr);
		form.exec();
	}
	///6 apr 2009
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
		ModifyConstraintActivityTagPreferredRoomForm form((ConstraintActivityTagPreferredRoom*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ModifyConstraintActivityTagPreferredRoomsForm form((ConstraintActivityTagPreferredRooms*)ctr);
		form.exec();
	}
	///
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm form((ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintStudentsMaxBuildingChangesPerDayForm form((ConstraintStudentsMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm form((ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintStudentsMaxBuildingChangesPerWeekForm form((ConstraintStudentsMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm form((ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintStudentsMinGapsBetweenBuildingChangesForm form((ConstraintStudentsMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}

	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeacherMaxBuildingChangesPerDayForm form((ConstraintTeacherMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
		ModifyConstraintTeachersMaxBuildingChangesPerDayForm form((ConstraintTeachersMaxBuildingChangesPerDay*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeacherMaxBuildingChangesPerWeekForm form((ConstraintTeacherMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
		ModifyConstraintTeachersMaxBuildingChangesPerWeekForm form((ConstraintTeachersMaxBuildingChangesPerWeek*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeacherMinGapsBetweenBuildingChangesForm form((ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else if(ctr->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm form((ConstraintTeachersMinGapsBetweenBuildingChanges*)ctr);
		form.exec();
	}
	else{
		QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
		 " is not correctly recognized in all space constraints dialog. FET will skip this error, so that you can continue work."
		 ". Probably the constraint can be modified from the specific constraint dialog."));
//		assert(0);
//		exit(1);
	}

	filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
}

void AllSpaceConstraintsForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	bool recompute, t;
	
	QListWidgetItem* item;
	
	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		// The user clicked the OK again button or pressed Enter
		
		QMessageBox::StandardButton wr=QMessageBox::Yes;
		
		if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){ //additional confirmation for this one
			QString s=tr("Do you really want to remove the basic compulsory space constraint?");
			s+=" ";
			s+=tr("You cannot generate a timetable without this constraint.");
			s+="\n\n";
			s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
				"Miscellaneous -> Basic compulsory space constraints.");
			
			wr=QMessageBox::warning(this, tr("FET warning"), s,
				QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		}
		
		if(wr==QMessageBox::Yes){
			if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				recompute=true;
			}
			else{
				recompute=false;
			}
	
			t=gt.rules.removeSpaceConstraint(ctr);
			assert(t);
			visibleSpaceConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			item=constraintsListWidget->takeItem(i);
			delete item;
			
			constraintsTextLabel->setText(tr("%1 Space Constraints", "%1 represents the number of constraints").arg(visibleSpaceConstraintsList.count()));
	
			if(recompute){
				LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
				LockUnlock::increaseCommunicationSpinBox();
			}
		}
	}
	//else if(lres==1){
		// The user clicked the Cancel or pressed Escape
	//}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		currentConstraintTextEdit->setText(QString(""));
}

void AllSpaceConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(all, descrDetDescr, contains, text, caseSensitive);

	//centerWidgetOnScreen(filterForm);

	int t=filterForm->exec();
	
	if(t==QDialog::Accepted){
		assert(useFilter==false);
		useFilter=true;
	
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
			
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
			
		descrDetDescr.clear();
		contains.clear();
		text.clear();
			
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescr.append(cb1->currentItem());
			contains.append(cb2->currentItem());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
		
		disconnect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, SIGNAL(toggled(bool)), this, SLOT(filter(bool)));
	}
	
	delete filterForm;
}
