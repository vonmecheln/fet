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

#include <QMessageBox>

#include <QLineEdit>

static int _nCategories=1;
static int _nDiv1=2;
static int _nDiv2=2;
static int _nDiv3=2;

static QString _cat1div1="";
static QString _cat1div2="";
static QString _cat1div3="";
static QString _cat1div4="";
static QString _cat1div5="";
static QString _cat1div6="";

static QString _cat2div1="";
static QString _cat2div2="";
static QString _cat2div3="";
static QString _cat2div4="";
static QString _cat2div5="";
static QString _cat2div6="";

static QString _cat3div1="";
static QString _cat3div2="";
static QString _cat3div3="";
static QString _cat3div4="";
static QString _cat3div5="";
static QString _cat3div6="";

static QString _sep=" ";

SplitYearForm::SplitYearForm(const QString _year)
{
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	year=_year;

	QString s=QObject::tr("Splitting year: %1").arg(year);
	splitYearTextLabel->setText(s);
	
	//restore last screen
	separatorLineEdit->setText(_sep);
	
	categoriesSpinBox->setValue(_nCategories);
	
	category1SpinBox->setValue(_nDiv1);
	category2SpinBox->setValue(_nDiv2);
	category3SpinBox->setValue(_nDiv3);
	
	category1Division1LineEdit->setText(_cat1div1);
	category1Division2LineEdit->setText(_cat1div2);
	category1Division3LineEdit->setText(_cat1div3);
	category1Division4LineEdit->setText(_cat1div4);
	category1Division5LineEdit->setText(_cat1div5);
	category1Division6LineEdit->setText(_cat1div6);

	category2Division1LineEdit->setText(_cat2div1);
	category2Division2LineEdit->setText(_cat2div2);
	category2Division3LineEdit->setText(_cat2div3);
	category2Division4LineEdit->setText(_cat2div4);
	category2Division5LineEdit->setText(_cat2div5);
	category2Division6LineEdit->setText(_cat2div6);

	category3Division1LineEdit->setText(_cat3div1);
	category3Division2LineEdit->setText(_cat3div2);
	category3Division3LineEdit->setText(_cat3div3);
	category3Division4LineEdit->setText(_cat3div4);
	category3Division5LineEdit->setText(_cat3div5);
	category3Division6LineEdit->setText(_cat3div6);
	/////////////////////

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
	if(categoriesSpinBox->value()<2)
		category2GroupBox->setDisabled(true);
	else
		category2GroupBox->setEnabled(true);

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

	if(category1SpinBox->value()<4)
		category1Division4LineEdit->setDisabled(true);
	else
		category1Division4LineEdit->setEnabled(true);

	if(category1SpinBox->value()<5)
		category1Division5LineEdit->setDisabled(true);
	else
		category1Division5LineEdit->setEnabled(true);

	if(category1SpinBox->value()<6)
		category1Division6LineEdit->setDisabled(true);
	else
		category1Division6LineEdit->setEnabled(true);
}

void SplitYearForm::category2Changed()
{
	if(category2SpinBox->value()<3)
		category2Division3LineEdit->setDisabled(true);
	else
		category2Division3LineEdit->setEnabled(true);

	if(category2SpinBox->value()<4)
		category2Division4LineEdit->setDisabled(true);
	else
		category2Division4LineEdit->setEnabled(true);

	if(category2SpinBox->value()<5)
		category2Division5LineEdit->setDisabled(true);
	else
		category2Division5LineEdit->setEnabled(true);

	if(category2SpinBox->value()<6)
		category2Division6LineEdit->setDisabled(true);
	else
		category2Division6LineEdit->setEnabled(true);
}

void SplitYearForm::category3Changed()
{
	if(category3SpinBox->value()<3)
		category3Division3LineEdit->setDisabled(true);
	else
		category3Division3LineEdit->setEnabled(true);

	if(category3SpinBox->value()<4)
		category3Division4LineEdit->setDisabled(true);
	else
		category3Division4LineEdit->setEnabled(true);

	if(category3SpinBox->value()<5)
		category3Division5LineEdit->setDisabled(true);
	else
		category3Division5LineEdit->setEnabled(true);

	if(category3SpinBox->value()<6)
		category3Division6LineEdit->setDisabled(true);
	else
		category3Division6LineEdit->setEnabled(true);
}

void SplitYearForm::ok()
{
	QString separator=separatorLineEdit->text();

	QString namesCategory1[6+1];
	QString namesCategory2[6+1];
	QString namesCategory3[6+1];
	
	//CATEGORY 1
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

	if(category1SpinBox->value()>=4){
		if(category1Division4LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory1[4]=category1Division4LineEdit->text();
	}
	else
		namesCategory1[4]="";

	if(category1SpinBox->value()>=5){
		if(category1Division5LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory1[5]=category1Division5LineEdit->text();
	}
	else
		namesCategory1[5]="";

	if(category1SpinBox->value()>=6){
		if(category1Division6LineEdit->text()==""){
			QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
			return;
		}
		namesCategory1[6]=category1Division6LineEdit->text();
	}
	else
		namesCategory1[6]="";
	//////////////

	//CATEGORY 2
	if(categoriesSpinBox->value()>=2){
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

		if(category2SpinBox->value()>=4){
			if(category2Division4LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory2[4]=category2Division4LineEdit->text();
		}
		else
			namesCategory2[4]="";

		if(category2SpinBox->value()>=5){
			if(category2Division5LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory2[5]=category2Division5LineEdit->text();
		}
		else
			namesCategory2[5]="";

		if(category2SpinBox->value()>=6){
			if(category2Division6LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory2[6]=category2Division6LineEdit->text();
		}
		else
			namesCategory2[6]="";
	}
	else
		for(int i=1; i<=6; i++)
			namesCategory2[i]="";
	/////////////////////
		
	//CATEGORY 3
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

		if(category3SpinBox->value()>=4){
			if(category3Division4LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory3[4]=category3Division4LineEdit->text();
		}
		else
			namesCategory3[4]="";

		if(category3SpinBox->value()>=5){
			if(category3Division5LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory3[5]=category3Division5LineEdit->text();
		}
		else
			namesCategory3[5]="";

		if(category3SpinBox->value()>=6){
			if(category3Division6LineEdit->text()==""){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Empty names not allowed"));
				return;
			}
			namesCategory3[6]=category3Division6LineEdit->text();
		}
		else
			namesCategory3[6]="";
	}
	else
		for(int i=1; i<=3; i++)
			namesCategory3[i]="";
	/////////////////////
			
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
	for(int i=1; i<=6; i++){
		if(namesCategory1[i]!=""){
			if(tmp.contains(namesCategory1[i])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory1[i]);
		}
	}
	for(int j=1; j<=6; j++){
		if(namesCategory2[j]!=""){
			if(tmp.contains(namesCategory2[j])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory2[j]);
		}
	}
	for(int k=1; k<=6; k++){
		if(namesCategory3[k]!=""){
			if(tmp.contains(namesCategory3[k])){
				QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory3[k]);
		}
	}
		
	if(namesCategory3[1]!=""){ //3 categories
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!="")
						for(int k=1; k<=6; k++)
							if(namesCategory3[k]!=""){
								QString t=year+separator+namesCategory1[i];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory2[j];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory1[i]+separator+namesCategory2[j]+separator+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add subgroup %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
							}
							
		StudentsSubgroup* subgroups[6+1][6+1][6+1];
		
		for(int i=1; i<=6; i++)
			for(int j=1; j<=6; j++)
				for(int k=1; k<=6; k++)
					subgroups[i][j][k]=NULL;

		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!="")
						for(int k=1; k<=6; k++)
							if(namesCategory3[k]!=""){
								QString t=year+separator+namesCategory1[i]+separator+namesCategory2[j]+separator+namesCategory3[k];
								assert(gt.rules.searchStudentsSet(t)==NULL);
								subgroups[i][j][k]=new StudentsSubgroup;
								subgroups[i][j][k]->name=t;
							}
								
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int j=1; j<=6; j++)
					for(int k=1; k<=6; k++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}

		for(int j=1; j<=6; j++)
			if(namesCategory2[j]!=""){
				QString t=year+separator+namesCategory2[j];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=6; i++)
					for(int k=1; k<=6; k++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}

		for(int k=1; k<=6; k++)
			if(namesCategory3[k]!=""){
				QString t=year+separator+namesCategory3[k];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=6; i++)
					for(int j=1; j<=6; j++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}
	}
	else if(namesCategory2[1]!=""){ //two categories
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!=""){
						QString t=year+separator+namesCategory1[i];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+separator+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+separator+namesCategory1[i]+separator+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add subgroup %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
					}
						
		StudentsSubgroup* subgroups[6+1][6+1];
		
		for(int i=1; i<=6; i++)
			for(int j=1; j<=6; j++)
				subgroups[i][j]=NULL;

		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!=""){
						QString t=year+separator+namesCategory1[i]+separator+namesCategory2[j];
						assert(gt.rules.searchStudentsSet(t)==NULL);
						subgroups[i][j]=new StudentsSubgroup;
						subgroups[i][j]->name=t;
					}
								
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int j=1; j<=6; j++)
					if(subgroups[i][j]!=NULL)
						gt.rules.addSubgroup(year, t, subgroups[i][j]);
			}

		for(int j=1; j<=6; j++)
			if(namesCategory2[j]!=""){
				QString t=year+separator+namesCategory2[j];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
				for(int i=1; i<=6; i++)
					if(subgroups[i][j]!=NULL)
						gt.rules.addSubgroup(year, t, subgroups[i][j]);
			}
	}
	else{ //one categories
		assert(namesCategory1[1]!="");
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				if(gt.rules.searchStudentsSet(t)!=NULL){
					QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Cannot add group %1, because a set with same name exists. "
					 "Please choose another name or remove old group").arg(t));
					return;
				}
			}
						
		for(int i=1; i<=6; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
			}
	}
		
	QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Split of year complete, please check the groups and subgroups"
	 " of year to make sure everything is OK"));
	 
	//saving page
	_sep=separatorLineEdit->text();
	
	_nCategories=categoriesSpinBox->value();
	
	_nDiv1=category1SpinBox->value();
	_nDiv2=category2SpinBox->value();
	_nDiv3=category3SpinBox->value();
	
	_cat1div1=category1Division1LineEdit->text();
	_cat1div2=category1Division2LineEdit->text();
	_cat1div3=category1Division3LineEdit->text();
	_cat1div4=category1Division4LineEdit->text();
	_cat1div5=category1Division5LineEdit->text();
	_cat1div6=category1Division6LineEdit->text();
	
	_cat2div1=category2Division1LineEdit->text();
	_cat2div2=category2Division2LineEdit->text();
	_cat2div3=category2Division3LineEdit->text();
	_cat2div4=category2Division4LineEdit->text();
	_cat2div5=category2Division5LineEdit->text();
	_cat2div6=category2Division6LineEdit->text();
	
	_cat3div1=category3Division1LineEdit->text();
	_cat3div2=category3Division2LineEdit->text();
	_cat3div3=category3Division3LineEdit->text();
	_cat3div4=category3Division4LineEdit->text();
	_cat3div5=category3Division5LineEdit->text();
	_cat3div6=category3Division6LineEdit->text();
	/////////////
	
	this->close();
}

void SplitYearForm::help()
{
	QString s=QObject::tr("\nPlease choose a number of categories and in each category the number of divisions. You can choose for instance"
	 " 2 categories, 2 divisions for the first category: boys and girls, and 3 divisions for the second: English, German and French."
	 " You can select 1, 2 or 3 categories, each with 2 to 6 divisions"
	 ". For more values (very unlikely case) you will have to manually"
	 " add the groups and subgroups");
	 
	s+=QObject::tr("\n\nPlease note that the dialog here will keep the last configuration of the last "
	 "divided year, it will not remember the values for a specific year you need to modify.");
	 
	s+=QObject::tr("\n\nSeparator character(s) is of your choice (default is space)");
	 
	QMessageBox::information(this, QObject::tr("FET help on dividing years"), s);
}

void SplitYearForm::reset() //reset to defaults
{
	separatorLineEdit->setText(" ");
	
	categoriesSpinBox->setValue(1);
	
	category1SpinBox->setValue(2);
	category2SpinBox->setValue(2);
	category3SpinBox->setValue(2);
	
	category1Division1LineEdit->setText("");
	category1Division2LineEdit->setText("");
	category1Division3LineEdit->setText("");
	category1Division4LineEdit->setText("");
	category1Division5LineEdit->setText("");
	category1Division6LineEdit->setText("");

	category2Division1LineEdit->setText("");
	category2Division2LineEdit->setText("");
	category2Division3LineEdit->setText("");
	category2Division4LineEdit->setText("");
	category2Division5LineEdit->setText("");
	category2Division6LineEdit->setText("");

	category3Division1LineEdit->setText("");
	category3Division2LineEdit->setText("");
	category3Division3LineEdit->setText("");
	category3Division4LineEdit->setText("");
	category3Division5LineEdit->setText("");
	category3Division6LineEdit->setText("");
	
	
	
	_sep=separatorLineEdit->text();
	
	_nCategories=categoriesSpinBox->value();
	
	_nDiv1=category1SpinBox->value();
	_nDiv2=category2SpinBox->value();
	_nDiv3=category3SpinBox->value();
	
	_cat1div1=category1Division1LineEdit->text();
	_cat1div2=category1Division2LineEdit->text();
	_cat1div3=category1Division3LineEdit->text();
	_cat1div4=category1Division4LineEdit->text();
	_cat1div5=category1Division5LineEdit->text();
	_cat1div6=category1Division6LineEdit->text();
	
	_cat2div1=category2Division1LineEdit->text();
	_cat2div2=category2Division2LineEdit->text();
	_cat2div3=category2Division3LineEdit->text();
	_cat2div4=category2Division4LineEdit->text();
	_cat2div5=category2Division5LineEdit->text();
	_cat2div6=category2Division6LineEdit->text();
	
	_cat3div1=category3Division1LineEdit->text();
	_cat3div2=category3Division2LineEdit->text();
	_cat3div3=category3Division3LineEdit->text();
	_cat3div4=category3Division4LineEdit->text();
	_cat3div5=category3Division5LineEdit->text();
	_cat3div6=category3Division6LineEdit->text();	
}
