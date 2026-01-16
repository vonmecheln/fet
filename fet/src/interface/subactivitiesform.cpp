/***************************************************************************
                          subactivitiesform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include "studentsset.h"

#include "subactivitiesform.h"
#include "modifysubactivityform.h"

#include "activityplanningform.h"

#include "advancedfilterform.h"

#include "longtextmessagebox.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include <QString>
#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>

#include <QAbstractItemView>

#include <QBrush>
#include <QPalette>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QProgressDialog>
#include <QtGlobal>

#include <algorithm>

extern bool teachers_schedule_ready;
extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;

extern const QString COMPANY;
extern const QString PROGRAM;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION < DETDESCRIPTIONWITHCONSTRAINTS, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;
const int DETDESCRIPTIONWITHCONSTRAINTS=2;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

SubactivitiesForm::SubactivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName): QDialog(parent)
{
	setupUi(this);
	
	filterCheckBox->setChecked(false);
	
	currentSubactivityTextEdit->setReadOnly(true);
	
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

	connect(subactivitiesListWidget, &QListWidget::currentRowChanged, this, &SubactivitiesForm::subactivityChanged);
	connect(closePushButton, &QPushButton::clicked, this, &SubactivitiesForm::close);

	connect(filterCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filter);

	connect(modifySubactivityPushButton, &QPushButton::clicked, this, &SubactivitiesForm::modifySubactivity);
	connect(subactivitiesListWidget, &QListWidget::itemDoubleClicked, this, &SubactivitiesForm::modifySubactivity);
	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::studentsFilterChanged);
	connect(helpPushButton, &QPushButton::clicked, this, &SubactivitiesForm::help);

	connect(activatePushButton, &QPushButton::clicked, this, &SubactivitiesForm::activateSubactivity);
	connect(deactivatePushButton, &QPushButton::clicked, this, &SubactivitiesForm::deactivateSubactivity);

	connect(activateAllPushButton, &QPushButton::clicked, this, &SubactivitiesForm::activateAllSubactivities);
	connect(deactivateAllPushButton, &QPushButton::clicked, this, &SubactivitiesForm::deactivateAllSubactivities);

	connect(commentsPushButton, &QPushButton::clicked, this, &SubactivitiesForm::subactivityComments);

	//////////////////
	QString settingsName="SubactivitiesAdvancedFilterForm";
	
	all=settings.value(settingsName+"/all-conditions", "true").toBool();
	
	descrDetDescrDetDescrWithConstraints.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescrDetDescrWithConstraints.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());
	
	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());
	
	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());

	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();
	
	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);
	//////////////////

	invertedTeacherCheckBox->setChecked(false);
	invertedStudentsCheckBox->setChecked(false);
	invertedSubjectCheckBox->setChecked(false);
	invertedActivityTagCheckBox->setChecked(false);

	connect(invertedTeacherCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filterChanged);
	connect(invertedStudentsCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::studentsFilterChanged);
	connect(invertedSubjectCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filterChanged);
	connect(invertedActivityTagCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filterChanged);

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
	
	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &SubactivitiesForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &SubactivitiesForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &SubactivitiesForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &SubactivitiesForm::filterChanged);
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

	QString settingsName="SubactivitiesAdvancedFilterForm";
	
	settings.setValue(settingsName+"/all-conditions", all);
	
	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescrDetDescrWithConstraints.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescrDetDescrWithConstraints.at(i));
	
	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));
	
	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));
	
	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool SubactivitiesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	bool ok=true;

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
	
	if(!useFilter || !ok)
		return ok;

	/////////////////////advanced filter
	assert(descrDetDescrDetDescrWithConstraints.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<int> perm;
	for(int i=0; i<descrDetDescrDetDescrWithConstraints.count(); i++)
		perm.append(i);
	//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries. This is written in the help when interrupting the
	//filtering of the subactivities, see the progress.wasCanceled() message.
	std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescrDetDescrWithConstraints.at(a)<descrDetDescrDetDescrWithConstraints.at(b);});
	for(int i=0; i<perm.count()-1; i++)
		assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))<=descrDetDescrDetDescrWithConstraints.at(perm.at(i+1)));

	int firstPosWithDescr=-1;
	int firstPosWithDetDescr=-1;
	int firstPosWithConstraints=-1;
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
			firstPosWithDescr=i;
		}
		else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
			firstPosWithDetDescr=i;
		}
		else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS && firstPosWithConstraints==-1){
			firstPosWithConstraints=i;
		}
	}
	
	QString s=QString("");
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DESCRIPTION){
			assert(firstPosWithDescr>=0);
			
			if(i==firstPosWithDescr)
				s=act->getDescription(gt.rules);
		}
		else if(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTION){
			assert(firstPosWithDetDescr>=0);
			
			if(i==firstPosWithDetDescr)
				s=act->getDetailedDescription(gt.rules);
		}
		else{
			assert(descrDetDescrDetDescrWithConstraints.at(perm.at(i))==DETDESCRIPTIONWITHCONSTRAINTS);
			
			assert(firstPosWithConstraints>=0);
			
			if(i==firstPosWithConstraints)
				s=act->getDetailedDescriptionWithConstraints(gt.rules); //warning: time consuming operation, goes through all the constraints and group activities in.initial order items.
		}
		
		bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.
	
		QString t=text.at(perm.at(i));
		if(contains.at(perm.at(i))==CONTAINS){
			okPartial=s.contains(t, csens);
		}
		else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
			okPartial=!(s.contains(t, csens));
		}
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		else if(contains.at(perm.at(i))==REGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=(regExp.match(s)).hasMatch();
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=!(regExp.match(s)).hasMatch();
		}
#else
		else if(contains.at(perm.at(i))==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)>=0);
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)<0);
		}
#endif
		else
			assert(0);
			
		if(all && !okPartial)
			return false;
		else if(!all && okPartial)
			return true;
	}
	
	return all;
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
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
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
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
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
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Filtering the subactivities", "Title of a progress dialog"));
	progress.setLabelText(tr("Filtering the subactivities ... please wait"));
	progress.setRange(0, qMax(gt.rules.activitiesList.size(), 1));
	progress.setModal(true);
	
	int k=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		progress.setValue(i);
		if(progress.wasCanceled()){
			LongTextMessageBox::largeInformation(this, tr("FET warning"), tr("You canceled the filtering of the subactivities - the list of subactivities will be incomplete.")
			 +QString(" ")+tr("Note: if filtering of the subactivities takes too much, it might be because you are filtering on the detailed description with constraints"
			 " of the subactivities, which checks each subactivity against each time constraint, each space constraint, and each group activities in the initial order item,"
			 " which might be too much. Please consider filtering on the description or detailed description of the subactivities, instead.")+QString("\n\n")
			 +tr("Note: if you are using more filters, like 'the subactivity should have a certain teacher', you might obtain much faster results if you filter on the"
			 " description/detailed description of the subactivity for the teacher and on the detailed description with constraints for other filters (meaning that you should"
			 " prefer using the description/detailed description instead of the detailed description with constraints, whenever it is possible).")+QString(" ")
			 +tr("Also, the order of the detailed description with constraints filters is important: you should put firstly the ones which filter out more subactivities"
			 " (if you selected the 'All' radio button) or put firstly the ones which accept immediately more subactivities (if you selected the 'Any' radio button)."));
			break;
		}
		
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleSubactivitiesList.append(act);
			subactivitiesListWidget->addItem(s);
			k++;
			
			if(!act->active){
				subactivitiesListWidget->item(k-1)->setBackground(subactivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
				subactivitiesListWidget->item(k-1)->setForeground(subactivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
			
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
	
	progress.setValue(qMax(gt.rules.activitiesList.size(), 1));
	
	assert(nsubacts-ninact>=0);
	NA=nsubacts-ninact;
	NT=nsubacts;
	assert(nh-ninacth>=0);
	DA=nh-ninacth;
	DT=nh;
	numberTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active subactivities, %2 is the total number of subactivities."
		" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
	durationTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active subactivities, %2 is the total duration of subactivities."
		" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
	
	if(subactivitiesListWidget->count()>0)
		subactivitiesListWidget->setCurrentRow(0);
	else
		currentSubactivityTextEdit->setPlainText(QString(""));
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
		currentSubactivityTextEdit->setPlainText(QString(""));
		return;
	}
	if(index>=visibleSubactivitiesList.count()){
		currentSubactivityTextEdit->setPlainText(QString("Invalid subactivity"));
		return;
	}

	QString s;
	Activity* act=visibleSubactivitiesList[index];

	assert(act!=nullptr);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	currentSubactivityTextEdit->setPlainText(s);
}

void SubactivitiesForm::help()
{
	QString s;
	
	s+=tr("Useful instructions/tips:");
	s+="\n\n";

	s+=tr("Above the (sub)activities list we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active (sub)activities (we number each individual subactivity as 1), while the second number b is the number of total (sub)activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active (sub)activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total (sub)activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active (sub)activities,"
		" then 102 - the number of total (sub)activities,"
		" 114 - the duration of active (sub)activities (in periods or FET hours) and 117 - the duration of total (sub)activities"
		" (in periods or FET hours). In this example we have 2 inactive (sub)activities with their combined duration being 3 periods.");

	s+="\n\n";
	s+=tr("Explanation of the short description of a (sub)activity: first comes the id."
		" If the (sub)activity is inactive, an X follows. Then the duration. Then, if the activity is split, a slash and the total duration."
		" Then teachers, subject, activity tag (if it is not void) and students. Then the number of students (if specified).");
	s+="\n\n";
	s+=tr("The (sub)activities which are inactive:");
	s+="\n";
	s+=" -";
	s+=tr("have an X mark after the id.");
	s+="\n";
	s+=" -";
	s+=tr("will appear with different background color.");
	s+="\n\n";
	s+=tr("To modify a subactivity, you can also double click it.");
	s+="\n\n";
	s+=tr("Show related: if you select this, there will be listed subactivities for groups and subgroups contained also in the current set (if the current set"
		" is a year or a group) and also higher ranked year or group (if the current set is a group or a subgroup).");
	
	s+=tr("There are two available filters: one is simple, based on the teacher, students, subject, or activity tag, the other one is more advanced (select the 'Filter' check box)."
		" The resulted overall filter is the combined AND of these two filters, if both filters are active, or the active one, if only one is active. If no filters are active,"
		" all the subactivities will be shown.");
	
	s+="\n\n";
	s+=tr("Inverted: this will show all the subactivities which _don't_ respect the selected filter for the teacher, students, subject, or activity tag.");
	
	s+="\n\n";
	s+=tr("Note that it is possible to activate/deactivate a subactivity in two different ways: by modifying a subactivity and selecting/deselecting the 'Active'"
		" check box, or directly, by clicking the corresponding buttons in this dialog.");
	
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
	
	connect(okPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QTextEdit* commentsPT=new QTextEdit();
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
		QString sb=act->getDetailedDescription(gt.rules);

		act->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed a subactivity's comments. Subactivity before:\n\n%1\nComments after:\n\n%2").arg(sb).arg(act->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;

		subactivitiesListWidget->currentItem()->setText(act->getDescription(gt.rules));
		subactivityChanged();
	}
}

void SubactivitiesForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for subactivities"), true, all, descrDetDescrDetDescrWithConstraints, contains, text, caseSensitive, "SubactivitiesAdvancedFilterForm");

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
		
		descrDetDescrDetDescrWithConstraints.clear();
		contains.clear();
		text.clear();
		
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescrDetDescrWithConstraints.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &SubactivitiesForm::filter);
	}
	
	delete filterForm;
}

void SubactivitiesForm::activateSubactivity()
{
	int i=subactivitiesListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subactivity"));
	
		subactivitiesListWidget->setFocus();

		return;
	}
	
	assert(i<visibleSubactivitiesList.count());
	Activity* act=visibleSubactivitiesList.at(i);
	
	if(!act->active){
		QString sb=act->getDetailedDescription(gt.rules);

		act->active=true;

		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Activated a subactivity:\n\n%1").arg(sb));
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;

		if(!filterOk(act)){ //Maybe the subactivity is no longer visible in the list widget, because of the filter.
			visibleSubactivitiesList.removeAt(i);
			subactivitiesListWidget->setCurrentRow(-1);
			QListWidgetItem* item=subactivitiesListWidget->takeItem(i);
			delete item;

			if(i>=subactivitiesListWidget->count())
				i=subactivitiesListWidget->count()-1;
			if(i>=0)
				subactivitiesListWidget->setCurrentRow(i);
			else
				currentSubactivityTextEdit->setPlainText(QString(""));
		}
		else{
			subactivitiesListWidget->currentItem()->setText(act->getDescription(gt.rules));
			subactivitiesListWidget->currentItem()->setBackground(QBrush());
			subactivitiesListWidget->currentItem()->setForeground(QBrush());
			subactivityChanged();
		}

		///////
		NA++;
		DA+=act->duration;
		numberTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active subactivities, %2 is the total number of subactivities."
			" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
		durationTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active subactivities, %2 is the total duration of subactivities."
			" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
	}
	
	subactivitiesListWidget->setFocus();
}

void SubactivitiesForm::deactivateSubactivity()
{
	int i=subactivitiesListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subactivity"));
	
		subactivitiesListWidget->setFocus();

		return;
	}
	
	assert(i<visibleSubactivitiesList.count());
	Activity* act=visibleSubactivitiesList.at(i);

	if(act->active){
		QString sb=act->getDetailedDescription(gt.rules);

		act->active=false;

		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Deactivated a subactivity:\n\n%1").arg(sb));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;

		if(!filterOk(act)){ //Maybe the subactivity is no longer visible in the list widget, because of the filter.
			visibleSubactivitiesList.removeAt(i);
			subactivitiesListWidget->setCurrentRow(-1);
			QListWidgetItem* item=subactivitiesListWidget->takeItem(i);
			delete item;

			if(i>=subactivitiesListWidget->count())
				i=subactivitiesListWidget->count()-1;
			if(i>=0)
				subactivitiesListWidget->setCurrentRow(i);
			else
				currentSubactivityTextEdit->setPlainText(QString(""));
		}
		else{
			subactivitiesListWidget->currentItem()->setText(act->getDescription(gt.rules));
			subactivitiesListWidget->currentItem()->setBackground(subactivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			subactivitiesListWidget->currentItem()->setForeground(subactivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
			subactivityChanged();
		}
		
		///////
		NA--;
		assert(NA>=0);
		DA-=act->duration;
		assert(DA>=0);
		numberTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active subactivities, %2 is the total number of subactivities."
			" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
		durationTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active subactivities, %2 is the total duration of subactivities."
			" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
	}
	
	subactivitiesListWidget->setFocus();
}

void SubactivitiesForm::activateAllSubactivities()
{
	QMessageBox::StandardButton ret=QMessageBox::No;
	QString s=tr("Are you sure you want to activate all the listed subactivities?");
	ret=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if(ret==QMessageBox::No){
		subactivitiesListWidget->setFocus();
		return;
	}

	QString su;
	int cnt=0;
	for(Activity* act : std::as_const(visibleSubactivitiesList)){
		if(!act->active){
			su+=tr("Subactivity:\n\n%1").arg(act->getDetailedDescription(gt.rules))+QString("\n");

			cnt++;
			act->active=true;
		}
	}
	if(cnt>0){
		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Activated all the filtered subactivities:\n\n%1").arg(su));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
		
		filterChanged();
		
		QMessageBox::information(this, tr("FET information"), tr("Activated %1 subactivities").arg(cnt));
	}
	
	subactivitiesListWidget->setFocus();
}

void SubactivitiesForm::deactivateAllSubactivities()
{
	QMessageBox::StandardButton ret=QMessageBox::No;
	QString s=tr("Are you sure you want to deactivate all the listed subactivities?");
	ret=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if(ret==QMessageBox::No){
		subactivitiesListWidget->setFocus();
		return;
	}

	QString su;
	int cnt=0;
	for(Activity* act : std::as_const(visibleSubactivitiesList)){
		if(act->active){
			su+=tr("Subactivity:\n\n%1").arg(act->getDetailedDescription(gt.rules))+QString("\n");

			cnt++;
			act->active=false;
		}
	}
	if(cnt>0){
		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Deactivated all the filtered subactivities:\n\n%1").arg(su));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
		
		filterChanged();
		
		QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 subactivities").arg(cnt));
	}

	subactivitiesListWidget->setFocus();
}
