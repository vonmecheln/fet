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

#include <QtGui>

#include <QMessageBox>

#include <QSettings>

#include "centerwidgetonscreen.h"

extern const QString COMPANY;
extern const QString PROGRAM;

SplitYearForm::SplitYearForm(QWidget* parent, const QString _year): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(categoriesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numberOfCategoriesChanged()));
	connect(category1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(category1Changed()));
	connect(pushButton3, SIGNAL(clicked()), this, SLOT(help()));
	connect(category2SpinBox, SIGNAL(valueChanged(int)), this, SLOT(category2Changed()));
	connect(category3SpinBox, SIGNAL(valueChanged(int)), this, SLOT(category3Changed()));
	connect(pushButton4, SIGNAL(clicked()), this, SLOT(reset()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);
	
	_sep=settings.value(this->metaObject()->className()+QString("/separator-string"), QString(" ")).toString();
	
	_nCategories=settings.value(this->metaObject()->className()+QString("/number-of-categories"), 1).toInt();

	_nDiv1=settings.value(this->metaObject()->className()+QString("/category/1/number-of-divisions"), 2).toInt();
	_nDiv2=settings.value(this->metaObject()->className()+QString("/category/2/number-of-divisions"), 2).toInt();
	_nDiv3=settings.value(this->metaObject()->className()+QString("/category/3/number-of-divisions"), 2).toInt();

	_cat1div1=settings.value(this->metaObject()->className()+QString("/category/1/division/1"), QString("")).toString();
	_cat1div2=settings.value(this->metaObject()->className()+QString("/category/1/division/2"), QString("")).toString();
	_cat1div3=settings.value(this->metaObject()->className()+QString("/category/1/division/3"), QString("")).toString();
	_cat1div4=settings.value(this->metaObject()->className()+QString("/category/1/division/4"), QString("")).toString();
	_cat1div5=settings.value(this->metaObject()->className()+QString("/category/1/division/5"), QString("")).toString();
	_cat1div6=settings.value(this->metaObject()->className()+QString("/category/1/division/6"), QString("")).toString();
	_cat1div7=settings.value(this->metaObject()->className()+QString("/category/1/division/7"), QString("")).toString();
	_cat1div8=settings.value(this->metaObject()->className()+QString("/category/1/division/8"), QString("")).toString();
	_cat1div9=settings.value(this->metaObject()->className()+QString("/category/1/division/9"), QString("")).toString();
	_cat1div10=settings.value(this->metaObject()->className()+QString("/category/1/division/10"), QString("")).toString();
	_cat1div11=settings.value(this->metaObject()->className()+QString("/category/1/division/11"), QString("")).toString();
	_cat1div12=settings.value(this->metaObject()->className()+QString("/category/1/division/12"), QString("")).toString();

	_cat2div1=settings.value(this->metaObject()->className()+QString("/category/2/division/1"), QString("")).toString();
	_cat2div2=settings.value(this->metaObject()->className()+QString("/category/2/division/2"), QString("")).toString();
	_cat2div3=settings.value(this->metaObject()->className()+QString("/category/2/division/3"), QString("")).toString();
	_cat2div4=settings.value(this->metaObject()->className()+QString("/category/2/division/4"), QString("")).toString();
	_cat2div5=settings.value(this->metaObject()->className()+QString("/category/2/division/5"), QString("")).toString();
	_cat2div6=settings.value(this->metaObject()->className()+QString("/category/2/division/6"), QString("")).toString();

	_cat3div1=settings.value(this->metaObject()->className()+QString("/category/3/division/1"), QString("")).toString();
	_cat3div2=settings.value(this->metaObject()->className()+QString("/category/3/division/2"), QString("")).toString();
	_cat3div3=settings.value(this->metaObject()->className()+QString("/category/3/division/3"), QString("")).toString();
	_cat3div4=settings.value(this->metaObject()->className()+QString("/category/3/division/4"), QString("")).toString();
	_cat3div5=settings.value(this->metaObject()->className()+QString("/category/3/division/5"), QString("")).toString();
	_cat3div6=settings.value(this->metaObject()->className()+QString("/category/3/division/6"), QString("")).toString();

	year=_year;

	QString s=tr("Splitting year: %1").arg(year);
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
	category1Division7LineEdit->setText(_cat1div7);
	category1Division8LineEdit->setText(_cat1div8);
	category1Division9LineEdit->setText(_cat1div9);
	category1Division10LineEdit->setText(_cat1div10);
	category1Division11LineEdit->setText(_cat1div11);
	category1Division12LineEdit->setText(_cat1div12);

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
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);
	
	settings.setValue(this->metaObject()->className()+QString("/separator-string"), _sep);
	
	settings.setValue(this->metaObject()->className()+QString("/number-of-categories"), _nCategories);

	settings.remove(this->metaObject()->className()+QString("/category"));

	if(_nCategories>=1)
		settings.setValue(this->metaObject()->className()+QString("/category/1/number-of-divisions"), _nDiv1);
	if(_nCategories>=2)
		settings.setValue(this->metaObject()->className()+QString("/category/2/number-of-divisions"), _nDiv2);
	if(_nCategories>=3)
		settings.setValue(this->metaObject()->className()+QString("/category/3/number-of-divisions"), _nDiv3);
	
	/////////////
	if(_nCategories>=1 && _nDiv1>=1)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/1"), _cat1div1);
	if(_nCategories>=1 && _nDiv1>=2)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/2"), _cat1div2);
	if(_nCategories>=1 && _nDiv1>=3)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/3"), _cat1div3);
	if(_nCategories>=1 && _nDiv1>=4)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/4"), _cat1div4);
	if(_nCategories>=1 && _nDiv1>=5)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/5"), _cat1div5);
	if(_nCategories>=1 && _nDiv1>=6)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/6"), _cat1div6);
	if(_nCategories>=1 && _nDiv1>=7)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/7"), _cat1div7);
	if(_nCategories>=1 && _nDiv1>=8)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/8"), _cat1div8);
	if(_nCategories>=1 && _nDiv1>=9)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/9"), _cat1div9);
	if(_nCategories>=1 && _nDiv1>=10)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/10"), _cat1div10);
	if(_nCategories>=1 && _nDiv1>=11)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/11"), _cat1div11);
	if(_nCategories>=1 && _nDiv1>=12)
		settings.setValue(this->metaObject()->className()+QString("/category/1/division/12"), _cat1div12);
	///////////
	if(_nCategories>=2 && _nDiv2>=1)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/1"), _cat2div1);
	if(_nCategories>=2 && _nDiv2>=2)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/2"), _cat2div2);
	if(_nCategories>=2 && _nDiv2>=3)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/3"), _cat2div3);
	if(_nCategories>=2 && _nDiv2>=4)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/4"), _cat2div4);
	if(_nCategories>=2 && _nDiv2>=5)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/5"), _cat2div5);
	if(_nCategories>=2 && _nDiv2>=6)
		settings.setValue(this->metaObject()->className()+QString("/category/2/division/6"), _cat2div6);
	///////////
			
	///////////
	if(_nCategories>=3 && _nDiv3>=1)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/1"), _cat3div1);
	if(_nCategories>=3 && _nDiv3>=2)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/2"), _cat3div2);
	if(_nCategories>=3 && _nDiv3>=3)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/3"), _cat3div3);
	if(_nCategories>=3 && _nDiv3>=4)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/4"), _cat3div4);
	if(_nCategories>=3 && _nDiv3>=5)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/5"), _cat3div5);
	if(_nCategories>=3 && _nDiv3>=6)
		settings.setValue(this->metaObject()->className()+QString("/category/3/division/6"), _cat3div6);
	///////////
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
		category1Division3LineEdit->setHidden(true);
	else
		category1Division3LineEdit->setHidden(false);

	if(category1SpinBox->value()<4)
		category1Division4LineEdit->setHidden(true);
	else
		category1Division4LineEdit->setHidden(false);

	if(category1SpinBox->value()<5)
		category1Division5LineEdit->setHidden(true);
	else
		category1Division5LineEdit->setHidden(false);

	if(category1SpinBox->value()<6)
		category1Division6LineEdit->setHidden(true);
	else
		category1Division6LineEdit->setHidden(false);

	if(category1SpinBox->value()<7)
		category1Division7LineEdit->setHidden(true);
	else
		category1Division7LineEdit->setHidden(false);

	if(category1SpinBox->value()<8)
		category1Division8LineEdit->setHidden(true);
	else
		category1Division8LineEdit->setHidden(false);

	if(category1SpinBox->value()<9)
		category1Division9LineEdit->setHidden(true);
	else
		category1Division9LineEdit->setHidden(false);

	if(category1SpinBox->value()<10)
		category1Division10LineEdit->setHidden(true);
	else
		category1Division10LineEdit->setHidden(false);

	if(category1SpinBox->value()<11)
		category1Division11LineEdit->setHidden(true);
	else
		category1Division11LineEdit->setHidden(false);

	if(category1SpinBox->value()<12)
		category1Division12LineEdit->setHidden(true);
	else
		category1Division12LineEdit->setHidden(false);
}

void SplitYearForm::category2Changed()
{
	if(category2SpinBox->value()<3)
		category2Division3LineEdit->setHidden(true);
	else
		category2Division3LineEdit->setHidden(false);

	if(category2SpinBox->value()<4)
		category2Division4LineEdit->setHidden(true);
	else
		category2Division4LineEdit->setHidden(false);

	if(category2SpinBox->value()<5)
		category2Division5LineEdit->setHidden(true);
	else
		category2Division5LineEdit->setHidden(false);

	if(category2SpinBox->value()<6)
		category2Division6LineEdit->setHidden(true);
	else
		category2Division6LineEdit->setHidden(false);
}

void SplitYearForm::category3Changed()
{
	if(category3SpinBox->value()<3)
		category3Division3LineEdit->setHidden(true);
	else
		category3Division3LineEdit->setHidden(false);

	if(category3SpinBox->value()<4)
		category3Division4LineEdit->setHidden(true);
	else
		category3Division4LineEdit->setHidden(false);

	if(category3SpinBox->value()<5)
		category3Division5LineEdit->setHidden(true);
	else
		category3Division5LineEdit->setHidden(false);

	if(category3SpinBox->value()<6)
		category3Division6LineEdit->setHidden(true);
	else
		category3Division6LineEdit->setHidden(false);
}

void SplitYearForm::ok()
{
	QString separator=separatorLineEdit->text();

	QString namesCategory1[12+1];
	QString namesCategory2[6+1];
	QString namesCategory3[6+1];
	
	//CATEGORY 1
	if(category1Division1LineEdit->text()==""){
		QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
		return;
	}
	namesCategory1[1]=category1Division1LineEdit->text();
	if(category1Division2LineEdit->text()==""){
		QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
		return;
	}
	namesCategory1[2]=category1Division2LineEdit->text();
	if(category1SpinBox->value()>=3){
		if(category1Division3LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[3]=category1Division3LineEdit->text();
	}
	else
		namesCategory1[3]="";

	if(category1SpinBox->value()>=4){
		if(category1Division4LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[4]=category1Division4LineEdit->text();
	}
	else
		namesCategory1[4]="";

	if(category1SpinBox->value()>=5){
		if(category1Division5LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[5]=category1Division5LineEdit->text();
	}
	else
		namesCategory1[5]="";

	if(category1SpinBox->value()>=6){
		if(category1Division6LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[6]=category1Division6LineEdit->text();
	}
	else
		namesCategory1[6]="";

	//////////////////////////////////////
	if(category1SpinBox->value()>=7){
		if(category1Division7LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[7]=category1Division7LineEdit->text();
	}
	else
		namesCategory1[7]="";

	if(category1SpinBox->value()>=8){
		if(category1Division8LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[8]=category1Division8LineEdit->text();
	}
	else
		namesCategory1[8]="";

	if(category1SpinBox->value()>=9){
		if(category1Division9LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[9]=category1Division9LineEdit->text();
	}
	else
		namesCategory1[9]="";

	if(category1SpinBox->value()>=10){
		if(category1Division10LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[10]=category1Division10LineEdit->text();
	}
	else
		namesCategory1[10]="";

	if(category1SpinBox->value()>=11){
		if(category1Division11LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[11]=category1Division11LineEdit->text();
	}
	else
		namesCategory1[11]="";

	if(category1SpinBox->value()>=12){
		if(category1Division11LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory1[12]=category1Division12LineEdit->text();
	}
	else
		namesCategory1[12]="";
	/////////////////////////////////
	//////////////

	//CATEGORY 2
	if(categoriesSpinBox->value()>=2){
		if(category2Division1LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory2[1]=category2Division1LineEdit->text();
		if(category2Division2LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory2[2]=category2Division2LineEdit->text();
		if(category2SpinBox->value()>=3){
			if(category2Division3LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory2[3]=category2Division3LineEdit->text();
		}
		else
			namesCategory2[3]="";

		if(category2SpinBox->value()>=4){
			if(category2Division4LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory2[4]=category2Division4LineEdit->text();
		}
		else
			namesCategory2[4]="";

		if(category2SpinBox->value()>=5){
			if(category2Division5LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory2[5]=category2Division5LineEdit->text();
		}
		else
			namesCategory2[5]="";

		if(category2SpinBox->value()>=6){
			if(category2Division6LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
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
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory3[1]=category3Division1LineEdit->text();
		if(category3Division2LineEdit->text()==""){
			QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
			return;
		}
		namesCategory3[2]=category3Division2LineEdit->text();
		if(category3SpinBox->value()>=3){
			if(category3Division3LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory3[3]=category3Division3LineEdit->text();
		}
		else
			namesCategory3[3]="";

		if(category3SpinBox->value()>=4){
			if(category3Division4LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory3[4]=category3Division4LineEdit->text();
		}
		else
			namesCategory3[4]="";

		if(category3SpinBox->value()>=5){
			if(category3Division5LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory3[5]=category3Division5LineEdit->text();
		}
		else
			namesCategory3[5]="";

		if(category3SpinBox->value()>=6){
			if(category3Division6LineEdit->text()==""){
				QMessageBox::information(this, tr("FET information"), tr("Empty names not allowed"));
				return;
			}
			namesCategory3[6]=category3Division6LineEdit->text();
		}
		else
			namesCategory3[6]="";
	}
	else
		for(int i=1; i<=6; i++)
			namesCategory3[i]="";
	/////////////////////
			
	StudentsYear* y=(StudentsYear*)gt.rules.searchStudentsSet(year);
	assert(y!=NULL);
	
	if(y->groupsList.count()>0){
		int t=QMessageBox::question(this, tr("FET question"), tr("Year %1 is not empty and it will be emptied before adding"
		" the divisions you selected. This means that all the activities and constraints for"
		" the groups and subgroups in this year will be removed. It is strongly recommended to save your file before continuing."
		" You might also want, as an alternative, to modify manually the groups/subgroups from the corresponding menu, so that"
		" you will not lose constraints and activities referring to them."
		" Do you really want to empty year?").arg(year),
		 QMessageBox::Yes, QMessageBox::Cancel);
		 
		if(t==QMessageBox::Cancel)
			return;

		t=QMessageBox::warning(this, tr("FET warning"), tr("Year %1 will be emptied."
		 " This means that all constraints and activities referring to groups/subgroups in year %1 will be removed."
		 " Are you absolutely sure?").arg(year),
		 QMessageBox::Yes, QMessageBox::Cancel);
		 
		if(t==QMessageBox::Cancel)
			return;
			
		while(y->groupsList.count()>0){
			QString group=y->groupsList.at(0)->name;
			gt.rules.removeGroup(year, group);
		}
	}
		
	QStringList tmp;
	for(int i=1; i<=12; i++){
		if(namesCategory1[i]!=""){
			if(tmp.contains(namesCategory1[i])){
				QMessageBox::information(this, tr("FET information"), tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory1[i]);
		}
	}
	for(int j=1; j<=6; j++){
		if(namesCategory2[j]!=""){
			if(tmp.contains(namesCategory2[j])){
				QMessageBox::information(this, tr("FET information"), tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory2[j]);
		}
	}
	for(int k=1; k<=6; k++){
		if(namesCategory3[k]!=""){
			if(tmp.contains(namesCategory3[k])){
				QMessageBox::information(this, tr("FET information"), tr("Duplicate names not allowed"));
				return;
			}
			tmp.append(namesCategory3[k]);
		}
	}
		
	if(namesCategory3[1]!=""){ //3 categories
		for(int i=1; i<=12; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!="")
						for(int k=1; k<=6; k++)
							if(namesCategory3[k]!=""){
								QString t=year+separator+namesCategory1[i];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory2[j];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
									 "Please choose another name or remove old group").arg(t));
									return;
								}
								t=year+separator+namesCategory1[i]+separator+namesCategory2[j]+separator+namesCategory3[k];
								if(gt.rules.searchStudentsSet(t)!=NULL){
									QMessageBox::information(this, tr("FET information"), tr("Cannot add subgroup %1, because a set with same name exists. "
									 "Please choose another name or remove old subgroup").arg(t));
									return;
								}
							}
							
		StudentsSubgroup* subgroups[12+1][6+1][6+1];
		
		for(int i=1; i<=12; i++)
			for(int j=1; j<=6; j++)
				for(int k=1; k<=6; k++)
					subgroups[i][j][k]=NULL;

		for(int i=1; i<=12; i++)
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
								
		for(int i=1; i<=12; i++)
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
				for(int i=1; i<=12; i++)
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
				for(int i=1; i<=12; i++)
					for(int j=1; j<=6; j++)
						if(subgroups[i][j][k]!=NULL)
							gt.rules.addSubgroup(year, t, subgroups[i][j][k]);
			}
	}
	else if(namesCategory2[1]!=""){ //two categories
		for(int i=1; i<=12; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!=""){
						QString t=year+separator+namesCategory1[i];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+separator+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
							 "Please choose another name or remove old group").arg(t));
							return;
						}
						t=year+separator+namesCategory1[i]+separator+namesCategory2[j];
						if(gt.rules.searchStudentsSet(t)!=NULL){
							QMessageBox::information(this, tr("FET information"), tr("Cannot add subgroup %1, because a set with same name exists. "
							 "Please choose another name or remove old subgroup").arg(t));
							return;
						}
					}
						
		StudentsSubgroup* subgroups[12+1][6+1];
		
		for(int i=1; i<=12; i++)
			for(int j=1; j<=6; j++)
				subgroups[i][j]=NULL;

		for(int i=1; i<=12; i++)
			if(namesCategory1[i]!="")
				for(int j=1; j<=6; j++)
					if(namesCategory2[j]!=""){
						QString t=year+separator+namesCategory1[i]+separator+namesCategory2[j];
						assert(gt.rules.searchStudentsSet(t)==NULL);
						subgroups[i][j]=new StudentsSubgroup;
						subgroups[i][j]->name=t;
					}
								
		for(int i=1; i<=12; i++)
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
				for(int i=1; i<=12; i++)
					if(subgroups[i][j]!=NULL)
						gt.rules.addSubgroup(year, t, subgroups[i][j]);
			}
	}
	else{ //one categories
		assert(namesCategory1[1]!="");
		for(int i=1; i<=12; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				if(gt.rules.searchStudentsSet(t)!=NULL){
					QMessageBox::information(this, tr("FET information"), tr("Cannot add group %1, because a set with same name exists. "
					 "Please choose another name or remove old group").arg(t));
					return;
				}
			}
						
		for(int i=1; i<=12; i++)
			if(namesCategory1[i]!=""){
				QString t=year+separator+namesCategory1[i];
				assert(gt.rules.searchStudentsSet(t)==NULL);
				StudentsGroup* gr=new StudentsGroup;
				gr->name=t;
				gt.rules.addGroup(year, gr);
			}
	}
		
	QMessageBox::information(this, tr("FET information"), tr("Split of year complete, please check the groups and subgroups"
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
	_cat1div7=category1Division7LineEdit->text();
	_cat1div8=category1Division8LineEdit->text();
	_cat1div9=category1Division9LineEdit->text();
	_cat1div10=category1Division10LineEdit->text();
	_cat1div11=category1Division11LineEdit->text();
	_cat1div12=category1Division12LineEdit->text();
	
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
	QString s;

	s+=tr("You might first want to consider if dividing a year is necessary and on what options. Please remember"
	 " that FET can handle activities with multiple teachers/students sets. If you have say students set 9a, which is split"
	 " into 2 parts: English (teacher TE) and French (teacher TF), and language activities must be simultaneous, then you might not want to divide"
	 " according to this category, but add more larger activities, with students set 9a and teachers TE+TF."
	 " The only drawback is that each activity can take place only in one room in FET, so you might need to find a way to overcome that.");
	
	s+="\n\n";
	
	s+=tr("Please choose a number of categories and in each category the number of divisions. You can choose for instance"
	 " 3 categories, 5 divisions for the first category: a, b, c, d and e, 2 divisions for the second category: boys and girls,"
	 " and 3 divisions for the third: English, German and French."
	 " You can select 1, 2 or 3 categories, first with 2 to 12 divisions and the second and third ones each with 2 to 6 divisions."
	 " If you need 4 categories, you may apply this trick: consider 9a a year, 9b another year, ..., and divide them by 3 categories (more details below)"
	 ". For more values (very unlikely case) you will have to manually"
	 " add the groups and subgroups");
	 
	s+="\n\n";
	
	s+=tr("If you need to make a division of say year 9 in 4 categories (category1: a, b,c, d, category2: language,"
	 " category3: religion, category4: boys/girls), you might want to use this trick: consider first category to"
	 " define years: year 9a, year 9b, year 9c, year 9d, and divide each year by 3 categories: language, religion and boys/girls."
	 " For activities with year 9 - language 1 for instance, you need to add to these activities the groups 9a_language1+9b_language1+"
	 "9c_language1+9d_language1. For activities with year 9a, just add year 9a to the corresponding activities.");
	
	s+="\n\n";
	
	s+=tr("Please input from the beginning the correct divisions. After you inputted activities and constraints"
	 " for this year's groups and subgroups, dividing it again will remove the activities and constraints referring"
	 " to these groups/subgroups. I know this is not elegant, I hope I'll solve that in the future."
	 " You might want to use the alternative of manually adding/editing/removing groups/subgroups"
	 " in the groups/subgroups menu, though removing a group/subgroup will also remove the activities");
	 
	s+="\n\n";

	s+=tr("Probably you don't need to worry about empty subgroups (no significant speed changes), although I didn't test "
		"enough such situations."
		" You just need to know that for the moment the maximum total number of subgroups is %1 (which can be changed"
		", but nobody needed larger values)").arg(MAX_TOTAL_SUBGROUPS);
	 
	s+="\n\n";

	s+=tr("Please note that the dialog here will keep the last configuration of the last "
		 "divided year, it will not remember the values for a specific year you need to modify.");
		 
	s+="\n\n";

	s+=tr("Separator character(s) is of your choice (default is space)");
	 
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on dividing a year"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QPlainTextEdit* te=new QPlainTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), &dialog, SLOT(close()));

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
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
	category1Division7LineEdit->setText("");
	category1Division8LineEdit->setText("");
	category1Division9LineEdit->setText("");
	category1Division10LineEdit->setText("");
	category1Division11LineEdit->setText("");
	category1Division12LineEdit->setText("");

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
	
	///////
	
	/*
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
	_cat1div7=category1Division7LineEdit->text();
	_cat1div8=category1Division8LineEdit->text();
	_cat1div9=category1Division9LineEdit->text();
	_cat1div10=category1Division10LineEdit->text();
	_cat1div11=category1Division11LineEdit->text();
	_cat1div12=category1Division12LineEdit->text();
	
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
	*/
}
