/***************************************************************************
                          modifyconstraintsubactivitiespreferredtimeslotsform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "modifyconstraintsubactivitiespreferredtimeslotsform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QBrush>
#include <QColor>

//#define YES	(ModifyConstraintSubactivitiesPreferredTimeSlotsForm::tr("Allowed", "Please keep translation short"))
//#define NO	(ModifyConstraintSubactivitiesPreferredTimeSlotsForm::tr("Not allowed", "Please keep translation short"))
#define YES		(QString(" "))
#define NO		(QString("X"))

ModifyConstraintSubactivitiesPreferredTimeSlotsForm::ModifyConstraintSubactivitiesPreferredTimeSlotsForm(ConstraintSubactivitiesPreferredTimeSlots* ctr)
{
    setupUi(this);

    connect(preferredTimesTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubactivitiesPreferredTimeSlotsForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubactivitiesPreferredTimeSlotsForm_template*/, SLOT(ok()));
    connect(setAllAllowedPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubactivitiesPreferredTimeSlotsForm_template*/, SLOT(setAllSlotsAllowed()));
    connect(setAllNotAllowedPushButton, SIGNAL(clicked()), this /*ModifyConstraintSubactivitiesPreferredTimeSlotsForm_template*/, SLOT(setAllSlotsNotAllowed()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;

	updateTeachersComboBox();
	updateStudentsComboBox();
	updateSubjectsComboBox();
	updateActivityTagsComboBox();
	
	componentNumberSpinBox->setMinValue(1);
	componentNumberSpinBox->setMaxValue(99);
	componentNumberSpinBox->setValue(this->_ctr->componentNumber);
	
	preferredTimesTable->setRowCount(gt.rules.nHoursPerDay);
	preferredTimesTable->setColumnCount(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		preferredTimesTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		preferredTimesTable->setVerticalHeaderItem(i, item);
	}

	bool currentMatrix[MAX_HOURS_PER_DAY][MAX_DAYS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	for(int k=0; k<ctr->p_nPreferredTimeSlots; k++){
		if(ctr->p_hours[k]==-1 || ctr->p_days[k]==-1)
			assert(0);
		int i=ctr->p_hours[k];
		int j=ctr->p_days[k];
		currentMatrix[i][j]=true;
	}

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			preferredTimesTable->setItem(i, j, item);

			if(!currentMatrix[i][j])
				item->setText(NO);
			else
				item->setText(YES);
				
			colorItem(item);
		}
		
	preferredTimesTable->resizeRowsToContents();
				
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	connect(preferredTimesTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(horizontalHeaderClicked(int)));
	connect(preferredTimesTable->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(verticalHeaderClicked(int)));

	preferredTimesTable->setSelectionMode(QAbstractItemView::NoSelection);
	
	tableWidgetUpdateBug(preferredTimesTable);
}

ModifyConstraintSubactivitiesPreferredTimeSlotsForm::~ModifyConstraintSubactivitiesPreferredTimeSlotsForm()
{
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
		if(item->text()==YES)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
	}
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::horizontalHeaderClicked(int col)
{
	if(col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=preferredTimesTable->item(0, col)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int row=0; row<gt.rules.nHoursPerDay; row++){
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			preferredTimesTable->item(row, col)->setText(s);
			colorItem(preferredTimesTable->item(row,col));
		}
		tableWidgetUpdateBug(preferredTimesTable);
	}
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::verticalHeaderClicked(int row)
{
	if(row>=0 && row<gt.rules.nHoursPerDay){
		QString s=preferredTimesTable->item(row, 0)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
	
		for(int col=0; col<gt.rules.nDaysPerWeek; col++){
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			preferredTimesTable->item(row, col)->setText(s);
			colorItem(preferredTimesTable->item(row,col));
		}
		tableWidgetUpdateBug(preferredTimesTable);
	}
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(YES);
			colorItem(preferredTimesTable->item(i,j));
		}
	tableWidgetUpdateBug(preferredTimesTable);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(NO);
			colorItem(preferredTimesTable->item(i,j));
		}
	tableWidgetUpdateBug(preferredTimesTable);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::itemClicked(QTableWidgetItem* item)
{
	QString s=item->text();
	if(s==YES)
		s=NO;
	else{
		assert(s==NO);
		s=YES;
	}
	item->setText(s);
	colorItem(item);

	tableWidgetUpdateBug(preferredTimesTable);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateTeachersComboBox(){
	int i=0, j=-1;
	teachersComboBox->clear();
	teachersComboBox->insertItem("");
	if(this->_ctr->p_teacherName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.teachersList.size(); k++){
		Teacher* t=gt.rules.teachersList[k];
		teachersComboBox->insertItem(t->name);
		if(t->name==this->_ctr->p_teacherName)
			j=i;
		i++;
	}
	assert(j>=0);
	teachersComboBox->setCurrentItem(j);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateStudentsComboBox(){
	int i=0, j=-1;
	studentsComboBox->clear();
	studentsComboBox->insertItem("");
	if(this->_ctr->p_studentsName=="")
		j=i;
	i++;
	for(int m=0; m<gt.rules.yearsList.size(); m++){
		StudentsYear* sty=gt.rules.yearsList[m];
		studentsComboBox->insertItem(sty->name);
		if(sty->name==this->_ctr->p_studentsName)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->insertItem(stg->name);
			if(stg->name==this->_ctr->p_studentsName)
				j=i;
			i++;
			for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->insertItem(sts->name);
				if(sts->name==this->_ctr->p_studentsName)
					j=i;
				i++;
			}
		}
	}
	assert(j>=0);
	studentsComboBox->setCurrentItem(j);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateSubjectsComboBox(){
	int i=0, j=-1;
	subjectsComboBox->clear();
	subjectsComboBox->insertItem("");
	if(this->_ctr->p_subjectName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* s=gt.rules.subjectsList[k];
		subjectsComboBox->insertItem(s->name);
		if(s->name==this->_ctr->p_subjectName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentItem(j);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::updateActivityTagsComboBox(){
	int i=0, j=-1;
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	if(this->_ctr->p_activityTagName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.activityTagsList.size(); k++){
		ActivityTag* s=gt.rules.activityTagsList[k];
		activityTagsComboBox->insertItem(s->name);
		if(s->name==this->_ctr->p_activityTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentItem(j);
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	QString teacher=teachersComboBox->currentText();
	if(teacher!="")
		assert(gt.rules.searchTeacher(teacher)>=0);

	QString students=studentsComboBox->currentText();
	if(students!="")
		assert(gt.rules.searchStudentsSet(students)!=NULL);

	QString subject=subjectsComboBox->currentText();
	if(subject!="")
		assert(gt.rules.searchSubject(subject)>=0);
		
	QString activityTag=activityTagsComboBox->currentText();
	if(activityTag!="")
		assert(gt.rules.searchActivityTag(activityTag)>=0);
		
	/*
	if(teacher=="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified all the sub-activities with split index %1 (no teacher, students set, subject or activity tag specified)"
		 ". Gaps in teachers' and students' timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	if(teacher!="" && students=="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("You specified only the teacher. Gaps in this teacher's timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}
	if(teacher=="" && students!="" && subject=="" && activityTag==""){
		int t=QMessageBox::question(this, tr("FET question"),
		  tr("You specified only the students set. Gaps in this students set's timetable will be counted if using this constraint. Are you sure?")
		  +"\n\n"+tr("Do you want to add current constraint?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}*/

	int days[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS];
	int hours[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->item(i, j)->text()==YES){
				if(n>=MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
					QString s=tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=tr("Please increase the variable MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS");
					s+="\n";
					s+=tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS);
					QMessageBox::critical(this, tr("FET information"), s);
					
					return;
				}
				
				days[n]=j;
				hours[n]=i;
				n++;
			}

	if(n<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Warning: 0 slots selected. Are you sure?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
						 
		if(t==QMessageBox::Cancel)
				return;
	}

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	
	this->_ctr->componentNumber=componentNumberSpinBox->value();
	
	this->_ctr->p_teacherName=teacher;
	this->_ctr->p_studentsName=students;
	this->_ctr->p_subjectName=subject;
	this->_ctr->p_activityTagName=activityTag;
	this->_ctr->p_nPreferredTimeSlots=n;
	for(int i=0; i<n; i++){
		this->_ctr->p_days[i]=days[i];
		this->_ctr->p_hours[i]=hours[i];
	}

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintSubactivitiesPreferredTimeSlotsForm::cancel()
{
	this->close();
}

#undef YES
#undef NO
