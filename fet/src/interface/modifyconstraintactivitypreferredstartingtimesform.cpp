/***************************************************************************
                          modifyconstraintactivitypreferredstartingtimesform.cpp  -  description
                             ------------------
    begin                : Feb 15, 2005
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

#include "tablewidgetupdatebug.h"

#include "modifyconstraintactivitypreferredstartingtimesform.h"
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

//#define YES	(ModifyConstraintActivityPreferredStartingTimesForm::tr("Allowed", "Please keep translation short"))
//#define NO	(ModifyConstraintActivityPreferredStartingTimesForm::tr("Not allowed", "Please keep translation short"))
#define YES		(QString(" "))
#define NO		(QString("X"))

ModifyConstraintActivityPreferredStartingTimesForm::ModifyConstraintActivityPreferredStartingTimesForm(ConstraintActivityPreferredStartingTimes* ctr)
{
    setupUi(this);

    connect(preferredTimesTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(cancel()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(filterChanged()));
    connect(setAllAllowedPushButton, SIGNAL(clicked()), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(setAllSlotsAllowed()));
    connect(setAllNotAllowedPushButton, SIGNAL(clicked()), this /*ModifyConstraintActivityPreferredStartingTimesForm_template*/, SLOT(setAllSlotsNotAllowed()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);	*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	QSize tmp5=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	//compulsoryCheckBox->setChecked(ctr->compulsory);

	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

	studentsComboBox->insertItem("");
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}
	studentsComboBox->setCurrentItem(0);
	
	updateActivitiesComboBox();

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
	for(int k=0; k<ctr->nPreferredStartingTimes; k++){
		if(ctr->days[k]==-1 || ctr->hours[k]==-1)
			assert(0);
		int i=ctr->hours[k];
		int j=ctr->days[k];
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

	connect(preferredTimesTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(horizontalHeaderClicked(int)));
	connect(preferredTimesTable->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(verticalHeaderClicked(int)));

	preferredTimesTable->setSelectionMode(QAbstractItemView::NoSelection);

	tableWidgetUpdateBug(preferredTimesTable);
}

ModifyConstraintActivityPreferredStartingTimesForm::~ModifyConstraintActivityPreferredStartingTimesForm()
{
}

void ModifyConstraintActivityPreferredStartingTimesForm::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
		if(item->text()==YES)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
	}
}

void ModifyConstraintActivityPreferredStartingTimesForm::horizontalHeaderClicked(int col)
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

void ModifyConstraintActivityPreferredStartingTimesForm::verticalHeaderClicked(int row)
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

void ModifyConstraintActivityPreferredStartingTimesForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(YES);
			colorItem(preferredTimesTable->item(i,j));
		}
	tableWidgetUpdateBug(preferredTimesTable);
}

void ModifyConstraintActivityPreferredStartingTimesForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(NO);
			colorItem(preferredTimesTable->item(i,j));
		}
	tableWidgetUpdateBug(preferredTimesTable);
}

bool ModifyConstraintActivityPreferredStartingTimesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
//	if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void ModifyConstraintActivityPreferredStartingTimesForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraintActivityPreferredStartingTimesForm::itemClicked(QTableWidgetItem* item)
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

void ModifyConstraintActivityPreferredStartingTimesForm::updateActivitiesComboBox(){
	int i=0, j=-1;
	activitiesComboBox->clear();
	activitiesList.clear();
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			activitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->activitiesList.append(act->id);
			if(act->id==this->_ctr->activityId)
				j=i;
		
			i++;
		}
	}
	//assert(j>=0); only first time
	activitiesComboBox->setCurrentItem(j);
}

void ModifyConstraintActivityPreferredStartingTimesForm::cancel()
{
	this->close();
}

void ModifyConstraintActivityPreferredStartingTimesForm::ok()
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

	int i=activitiesComboBox->currentItem();
	assert(i<activitiesList.size());
	if(i<0 || activitiesComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	int id=activitiesList.at(i);
	
	/*foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
			ConstraintActivityPreferredStartingTimes* c=(ConstraintActivityPreferredStartingTimes*) tc;
			if(c->activityId==id && c!=this->_ctr){
				QMessageBox::warning(this, tr("FET information"),
				 tr("This activity id has other constraint of this type attached\n"
				 "Please remove the other constraints of type activity preferred starting times\n"
				 "referring to this activity before proceeding"));
				return;
			}
		}*/

	int days[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES];
	int hours[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES];
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->item(i, j)->text()==YES){
				if(n>=MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
					QString s=tr("Not enough slots (too many \"Yes\" values).");
					s+="\n";
					s+=tr("Please increase the variable MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES");
					s+="\n";
					s+=tr("Currently, it is %1").arg(MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES);
					QMessageBox::warning(this, tr("FET information"), s);
					
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
	this->_ctr->activityId=id;
	this->_ctr->nPreferredStartingTimes=n;
	for(int i=0; i<n; i++){
		this->_ctr->days[i]=days[i];
		this->_ctr->hours[i]=hours[i];
	}

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

#undef YES
#undef NO
