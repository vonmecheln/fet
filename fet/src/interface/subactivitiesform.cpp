/***************************************************************************
                          subactivitiesform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include "studentsset.h"

#include "subactivitiesform.h"
#include "modifysubactivityform.h"

#include <QString>
#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>

#include <QAbstractItemView>

#include "longtextmessagebox.h"

#include <QBrush>
#include <QPalette>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

SubactivitiesForm::SubactivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName): QDialog(parent)
{
	setupUi(this);
	
	subactivityTextEdit->setReadOnly(true);
	
	modifySubactivityPushButton->setDefault(true);
	
	subactivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related-check-box-state"), "false").toBool());

	/*invertedTeacherCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-teacher-check-box-state"), "false").toBool());
	invertedStudentsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-students-check-box-state"), "false").toBool());
	invertedSubjectCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-subject-check-box-state"), "false").toBool());
	invertedActivityTagCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-activity-tag-check-box-state"), "false").toBool());*/

	connect(subactivitiesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(subactivityChanged()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	connect(modifySubactivityPushButton, SIGNAL(clicked()), this, SLOT(modifySubactivity()));
	connect(subactivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifySubactivity()));
	connect(showRelatedCheckBox, SIGNAL(toggled(bool)), this, SLOT(studentsFilterChanged()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(subactivityComments()));

	invertedTeacherCheckBox->setChecked(false);
	invertedStudentsCheckBox->setChecked(false);
	invertedSubjectCheckBox->setChecked(false);
	invertedActivityTagCheckBox->setChecked(false);

	connect(invertedTeacherCheckBox, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
	connect(invertedStudentsCheckBox, SIGNAL(toggled(bool)), this, SLOT(studentsFilterChanged()));
	connect(invertedSubjectCheckBox, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
	connect(invertedActivityTagCheckBox, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	teachersComboBox->addItem("");
	int cit=0;
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
		if(tch->name==teacherName)
			cit=i+1;
	}
	teachersComboBox->setCurrentIndex(cit);

	subjectsComboBox->addItem("");
	int cisu=0;
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
		if(sb->name==subjectName)
			cisu=i+1;
	}
	subjectsComboBox->setCurrentIndex(cisu);

	activityTagsComboBox->addItem("");
	int ciat=0;
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
		if(st->name==activityTagName)
			ciat=i+1;
	}
	activityTagsComboBox->setCurrentIndex(ciat);

	int cist=populateStudentsComboBox(studentsComboBox, studentsSetName, true);
	if(studentsSetName==""){
		assert(cist==0);
		studentsComboBox->setCurrentIndex(0);

		showedStudents.clear();
		showedStudents.insert("");

		this->filterChanged();
	}
	else{
		assert(cist!=0);
		
		if(cist==-1){
			studentsComboBox->setCurrentIndex(0);

			showWarningForInvisibleSubgroupActivity(parent, studentsSetName);

			showedStudents.clear();
			showedStudents.insert("");

			filterChanged();
		}
		else{
			studentsComboBox->setCurrentIndex(cist);

			studentsFilterChanged();
		}
	}
	
	connect(teachersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(studentsFilterChanged()));
	connect(subjectsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
}

SubactivitiesForm::~SubactivitiesForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/show-related-check-box-state"), showRelatedCheckBox->isChecked());

	/*settings.setValue(this->metaObject()->className()+QString("/inverted-teacher-check-box-state"), invertedTeacherCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/inverted-students-check-box-state"), invertedStudentsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/inverted-subject-check-box-state"), invertedSubjectCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/inverted-activity-tag-check-box-state"), invertedActivityTagCheckBox->isChecked());*/
}

bool SubactivitiesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		
		if(invertedTeacherCheckBox->isChecked())
			ok2=!ok2;
		
		if(!ok2)
			ok=false;
	}
	else{
		if(invertedTeacherCheckBox->isChecked() && act->teachersNames.count()>0)
			ok=false;
	}

	//subject
	if(!invertedSubjectCheckBox->isChecked()){
		if(sbn!="" && sbn!=act->subjectName)
			ok=false;
	}
	else{
		if(sbn=="")
			ok=false;
		else if(sbn==act->subjectName)
			ok=false;
	}
	
	//activity tag
	if(!invertedActivityTagCheckBox->isChecked()){
		if(atn!="" && !act->activityTagsNames.contains(atn))
			ok=false;
	}
	else{
		if(atn!=""){
			if(act->activityTagsNames.contains(atn))
				ok=false;
		}
		else{
			if(act->activityTagsNames.count()>0)
				ok=false;
		}
	}
	
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
			//if(*it == stn){
			if(showedStudents.contains(*it)){
				ok2=true;
				break;
			}

		if(invertedStudentsCheckBox->isChecked())
			ok2=!ok2;

		if(!ok2)
			ok=false;
	}
	else{
		assert(showedStudents.count()==1);
		assert(showedStudents.contains(""));

		if(invertedStudentsCheckBox->isChecked() && act->studentsNames.count()>0)
			ok=false;
	}
	
	return ok;
}

void SubactivitiesForm::studentsFilterChanged()
{
	bool showRelated=showRelatedCheckBox->isChecked();
	
	showedStudents.clear();
	
	if(!showRelated){
		showedStudents.insert(studentsComboBox->currentText());
	}
	else{
		if(studentsComboBox->currentText()=="")
			showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(studentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				showedStudents.insert(year->name);
				for(StudentsGroup* group : qAsConst(year->groupsList)){
					showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
						showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList))
					showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				showedStudents.insert(subgroup->name);
			}
			else
				assert(0);

			//up
			QString crt=studentsComboBox->currentText();
			for(StudentsYear* year : qAsConst(gt.rules.yearsList)){
				for(StudentsGroup* group : qAsConst(year->groupsList)){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : qAsConst(group->subgroupsList)){
						if(subgroup->name==crt){
							showedStudents.insert(year->name);
							showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}
	
	filterChanged();
}

void SubactivitiesForm::filterChanged()
{
	int /*nacts=0,*/ nsubacts=0, nh=0;
	int ninact=0, ninacth=0;

	QString s;
	subactivitiesListWidget->clear();
	visibleSubactivitiesList.clear();
	
	int k=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleSubactivitiesList.append(act);
			subactivitiesListWidget->addItem(s);
			k++;
			
			if(USE_GUI_COLORS && !act->active)
				subactivitiesListWidget->item(k-1)->setBackground(subactivitiesListWidget->palette().alternateBase());
			
			//if(act->id==act->activityGroupId || act->activityGroupId==0)
			//	nacts++;
			nsubacts++;
			
			nh+=act->duration;
			
			if(!act->active){
				ninact++;
				ninacth+=act->duration;
			}
		}
	}
	
	assert(nsubacts-ninact>=0);
	assert(nh-ninacth>=0);
	activeTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active subactivities, %2 is the total number of subactivities."
		"Please leave space between fields, so that they are better visible").arg(nsubacts-ninact).arg(nsubacts));
	totalTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active subactivities, %2 is the total duration of subactivities."
		"Please leave space between fields, so that they are better visible").arg(nh-ninacth).arg(nh));
	
	if(subactivitiesListWidget->count()>0)
		subactivitiesListWidget->setCurrentRow(0);
	else
		subactivityTextEdit->setPlainText(QString(""));
}

void SubactivitiesForm::modifySubactivity()
{
	int ind=subactivitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subactivity"));
		return;
	}
	
	assert(ind<visibleSubactivitiesList.count());
	
	int valv=subactivitiesListWidget->verticalScrollBar()->value();
	int valh=subactivitiesListWidget->horizontalScrollBar()->value();
	
	Activity* act=visibleSubactivitiesList[ind];
	assert(act!=nullptr);
	
	ModifySubactivityForm modifySubactivityForm(this, act->id, act->activityGroupId);
	int t;
	setParentAndOtherThings(&modifySubactivityForm, this);
	t=modifySubactivityForm.exec();

	if(t==QDialog::Accepted){
		//cout<<"Acc"<<endl;
		filterChanged();
		
		subactivitiesListWidget->verticalScrollBar()->setValue(valv);
		subactivitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(ind>=subactivitiesListWidget->count())
			ind=subactivitiesListWidget->count()-1;
		if(ind>=0)
			subactivitiesListWidget->setCurrentRow(ind);
	}
	else{
		//cout<<"Rej"<<endl;
		assert(t==QDialog::Rejected);
	}
}

void SubactivitiesForm::subactivityChanged()
{
	int index=subactivitiesListWidget->currentRow();

	if(index<0){
		subactivityTextEdit->setPlainText(QString(""));
		return;
	}
	if(index>=visibleSubactivitiesList.count()){
		subactivityTextEdit->setPlainText(QString("Invalid subactivity"));
		return;
	}

	QString s;
	Activity* act=visibleSubactivitiesList[index];

	assert(act!=nullptr);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	subactivityTextEdit->setPlainText(s);
}

void SubactivitiesForm::help()
{
	QString s;
	
	s+=tr("Useful instructions/tips:");
	s+="\n\n";

	s+=tr("Above the (sub)activities list, we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active (sub)activities (we number each individual subactivity as 1), while the second number b is the number of total (sub)activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active (sub)activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total (sub)activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active (sub)activities,"
		" then 102 - the number of total (sub)activities,"
		" 114 - the duration of active activities (in periods or FET hours) and 117 - the duration of total activities"
		" (in periods or FET hours). In this example we have 2 inactive activities with their combined duration being 3 periods.");

	s+="\n\n";
	s+=tr("Explanation of the short description of an activity: first comes the id."
		" If the activity is inactive, an X follows. Then the duration. Then, if the activity is split, a slash and the total duration."
		" Then teachers, subject, activity tag (if it is not void) and students. Then the number of students (if specified).");
	s+="\n\n";
	s+=tr("The activities which are inactive:");
	s+="\n";
	s+=" -";
	s+=tr("have an X mark after the id.");
	s+="\n";
/*	s+=" -";
	s+=tr("are shown with lowercase letters.");
	s+="\n";*/
	s+=" -";
	s+=tr("if you use colors in interface (see Settings/Interface menu), they will appear with different background color.");
	s+="\n\n";
	s+=tr("To modify a subactivity, you can also double click it.");
	s+="\n\n";
	s+=tr("Show related: if you select this, there will be listed subactivities for groups and subgroups contained also in the current set (if the current set"
		" is a year or a group) and also higher ranked year or group (if the current set is a group or a subgroup).");
	
	s+="\n\n";
	s+=tr("Inverted: this will show all the subactivities which _don't_ respect the selected filter.");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void SubactivitiesForm::subactivityComments()
{
	int ind=subactivitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subactivity"));
		return;
	}
	
	assert(ind<visibleSubactivitiesList.count());

	Activity* act=visibleSubactivitiesList[ind];
	assert(act!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Subactivity comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(accept()));
	connect(cancelPB, SIGNAL(clicked()), &getCommentsDialog, SLOT(reject()));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QPlainTextEdit* commentsPT=new QPlainTextEdit();
	commentsPT->setPlainText(act->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("SubactivityCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		act->comments=commentsPT->toPlainText();
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subactivitiesListWidget->currentItem()->setText(act->getDescription(gt.rules));
		subactivityChanged();
	}
}
