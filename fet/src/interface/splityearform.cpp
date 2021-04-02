/***************************************************************************
                          splityearform.cpp  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "splityearform.h"

#include <QDesktopWidget>

#include <QtGui>

#include <QLineEdit>

SplitYearForm::SplitYearForm(const QString _year)
{
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	year=_year;
	
	splitYearTextLabel->setText(QObject::tr("Splitting year: %1").arg(year));

	numberOfCategoriesChanged();
	category1Changed();
	category2Changed();
	category3Changed();
}

SplitYearForm::~SplitYearForm()
{
}

void SplitYearForm::numberOfCategoriesChanged()
{
	if(categoriesSpinBox->value()<3)
		category3GroupBox->setDisabled(true);
	else
		category3GroupBox->setEnabled(true);
}

void SplitYearForm::category1Changed()
{
	if(category1SpinBox->value()<3)
		category1Division3LineEdit->setDisabled(true);
	else
		category1Division3LineEdit->setEnabled(true);
}

void SplitYearForm::category2Changed()
{
	if(category2SpinBox->value()<3)
		category2Division3LineEdit->setDisabled(true);
	else
		category2Division3LineEdit->setEnabled(true);
}

void SplitYearForm::category3Changed()
{
	if(category3SpinBox->value()<3)
		category3Division3LineEdit->setDisabled(true);
	else
		category3Division3LineEdit->setEnabled(true);
}

void SplitYearForm::ok()
{
	QString namesCategory1[4];
	QString namesCategory2[4];
	QString namesCategory3[4];
	
	if(category1Division1LineEdit->text()==""){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
		return;
	}
	namesCategory1[1]=category1Division1LineEdit->text();
	if(category1Division2LineEdit->text()==""){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
		return;
	}
	namesCategory1[2]=category1Division2LineEdit->text();
	if(category1SpinBox->value()>=3){
		if(category1Division3LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory1[3]=category1Division3LineEdit->text();
	}
	else
		namesCategory1[3]="";

	if(category2Division1LineEdit->text()==""){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
		return;
	}
	namesCategory2[1]=category2Division1LineEdit->text();
	if(category2Division2LineEdit->text()==""){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
		return;
	}
	namesCategory2[2]=category2Division2LineEdit->text();
	if(category2SpinBox->value()>=3){
		if(category2Division3LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory2[3]=category2Division3LineEdit->text();
	}
	else
		namesCategory2[3]="";
		
	if(categoriesSpinBox->value()>=3){
		if(category3Division1LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory3[1]=category3Division1LineEdit->text();
		if(category3Division2LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory3[2]=category3Division2LineEdit->text();
		if(category3SpinBox->value()>=3){
			if(category3Division3LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory3[3]=category3Division3LineEdit->text();
		}
		else
			namesCategory3[3]="";
	}
	else
		for(int i=1; i<=3; i++)
			namesCategory3[i]="";
			
	StudentsYear* y=(StudentsYear*)gt.rules.searchStudentsSet(year);
	assert(y!=NULL);
	
	if(y->groupsList.count()>0){
		int t=QMessageBox::question(this, QObject::tr("FET question"), QObject::tr("Year %1 is not empty and it will be emptied before adding"
		" the divisions you selected. Do you agree?").arg(year),
		 QMessageBox::Yes, QMessageBox::Cancel);
		 
		if(t==QMessageBox::Cancel)
			return;
			
		while(y->groupsList.count()>0){
			QString group=y->groupsList.at(0)->name;
			gt.rules.removeGroup(year, group);
		}
	}
		
	QStringList tmp;
	for(int i=1; i<=3; i++){
		if(namesCategory1[i]!=""){
			if(tmp.contains(namesCategory1[i])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory1[i]);
		}
	}
	for(int j=1; j<=3; j++){
		if(namesCategory2[j]!=""){
			if(tmp.contains(namesCategory2[j])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory2[j]);
		}
	}
	for(int k=1; k<=3; k++){
		if(namesCategory3[k]!=""){
			if(tmp.contains(namesCategory3[k])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory3[k]);
		}
	}
		
	if(namesCategory3[1]!=""){ //3 categories
		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=3; j++)
					if(namesCategory2[j]!="")
						for(int k=1; k<=3; k++)
							if(namesCategory3[k]!=""){
								QString t=year+" "+namesCategory1[i];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+" "+namesCategory2[j];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+" "+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+" "+namesCategory1[i]+" "+namesCategory2[j]+" "+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add subgroup %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
							}
							
		StudentsSubgroup* subgroups[4][4][4];
		
		for(int i=1; i<=3; i++)
			for(int j=1; j<=3; j++)
				for(int k=1; k<=3; k++)
					subgroups[i][j][k]=NULL;

		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=3; j++)
					if(namesCategory2[j]!="")
						for(int k=1; k<=3; k++)
							if(namesCategory3[k]!=""){
								QString t=year+" "+namesCategory1[i]+" "+namesCategory2[j]+" "+namesCategory3[k];
								assert(gt.rules.searchStudentsSet(t)==NULL);
								subgroups[i][j][k]=new StudentsSubgroup;
								subgroups[i][j][k]->name=t;
							}
								
		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!=""){
				QString t=year+" "+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int j=1; j<=3; j++)
					for(int k=1; k<=3; k++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}

		for(int j=1; j<=3; j++)
			if(namesCategory2[j]!=""){
				QString t=year+" "+namesCategory2[j];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=3; i++)
					for(int k=1; k<=3; k++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}

		for(int k=1; k<=3; k++)
			if(namesCategory3[k]!=""){
				QString t=year+" "+namesCategory3[k];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=3; i++)
					for(int j=1; j<=3; j++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}
	}
	else{ //two categories
		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=3; j++)
					if(namesCategory2[j]!=""){
						QString t=year+" "+namesCategory1[i];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+" "+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+" "+namesCategory1[i]+" "+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add subgroup %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
					}
						
		StudentsSubgroup* subgroups[4][4];
		
		for(int i=1; i<=3; i++)
			for(int j=1; j<=3; j++)
				subgroups[i][j]=NULL;

		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=3; j++)
					if(namesCategory2[j]!=""){
						QString t=year+" "+namesCategory1[i]+" "+namesCategory2[j];
						assert(gt.rules.searchStudentsSet(t)==NULL);
						subgroups[i][j]=new StudentsSubgroup;
						subgroups[i][j]->name=t;
					}
								
		for(int i=1; i<=3; i++)
			if(namesCategory1[i]!=""){
				QString t=year+" "+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int j=1; j<=3; j++)
					if(subgroups[i][j]!=NULL)
						gt.rules.addSubgroup(year, t, subgroups[i][j]);
			}

		for(int j=1; j<=3; j++)
			if(namesCategory2[j]!=""){
				QString t=year+" "+namesCategory2[j];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=3; i++)
					if(subgroups[i][j]!=NULL)
						gt.rules.addSubgroup(year, t, subgroups[i][j]);
			}
	}
		
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Split of year complete, please check the groups and subgroups"
	 " of year to make sure everything is OK"));
	
	this->close();
}
