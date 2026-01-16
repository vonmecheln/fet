/***************************************************************************
                          activitiesform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Liviu Lalescu
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

#include "activitiesform.h"
#include "addactivityform.h"
#include "modifyactivityform.h"

#include "activitiestagsform.h"

#include "activityplanningform.h"

#include "advancedfilterform.h"

#include "longtextmessagebox.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

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

ActivitiesForm::ActivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);
	
	currentActivityTextEdit->setReadOnly(true);

	modifyActivityPushButton->setDefault(true);

	activitiesListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QSettings settings(COMPANY, PROGRAM);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related-check-box-state"), "false").toBool());

	/*invertedTeacherCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-teacher-check-box-state"), "false").toBool());
	invertedStudentsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-students-check-box-state"), "false").toBool());
	invertedSubjectCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-subject-check-box-state"), "false").toBool());
	invertedActivityTagCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/inverted-activity-tag-check-box-state"), "false").toBool());*/
	
	connect(activitiesListWidget, &QListWidget::currentRowChanged, this, &ActivitiesForm::activityChanged);
	
	//selectionChanged();
	connect(activitiesListWidget, &QListWidget::itemSelectionChanged, this, &ActivitiesForm::selectionChanged);
	
	connect(addActivityPushButton, &QPushButton::clicked, this, &ActivitiesForm::addActivity);
	connect(removeActivitiesPushButton, &QPushButton::clicked, this, &ActivitiesForm::removeActivities);

	connect(closePushButton, &QPushButton::clicked, this, &ActivitiesForm::close);

	connect(filterCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filter);

	connect(modifyActivityPushButton, &QPushButton::clicked, this, &ActivitiesForm::modifyActivity);
	connect(activitiesListWidget, &QListWidget::itemDoubleClicked, this, &ActivitiesForm::modifyActivity);
	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::studentsFilterChanged);
	connect(helpPushButton, &QPushButton::clicked, this, &ActivitiesForm::help);

	connect(activatePushButton, &QPushButton::clicked, this, &ActivitiesForm::activateActivities);
	connect(deactivatePushButton, &QPushButton::clicked, this, &ActivitiesForm::deactivateActivities);

	//connect(activateAllPushButton, SIG NAL(clicked()), this, SL OT(activateAllActivities()));
	//connect(deactivateAllPushButton, SIG NAL(clicked()), this, SL OT(deactivateAllActivities()));

	connect(commentsPushButton, &QPushButton::clicked, this, &ActivitiesForm::activityComments);
	
	connect(activityTagsPushButton, &QPushButton::clicked, this, &ActivitiesForm::changeActivityTags);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addActivityPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addActivityPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyActivityPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyActivityPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeActivitiesPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeActivitiesPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activatePushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivatePushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivatePushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}

	//////////////////
	QString settingsName="ActivitiesAdvancedFilterForm";
	
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

	connect(invertedTeacherCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filterChanged);
	connect(invertedStudentsCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::studentsFilterChanged);
	connect(invertedSubjectCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filterChanged);
	connect(invertedActivityTagCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filterChanged);

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
	
		filterChanged();
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

			this->studentsFilterChanged();
		}
	}

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ActivitiesForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ActivitiesForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ActivitiesForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ActivitiesForm::filterChanged);
}

ActivitiesForm::~ActivitiesForm()
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

	QString settingsName="ActivitiesAdvancedFilterForm";
	
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

bool ActivitiesForm::filterOk(Activity* act)
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
	//filtering of the activities, see the progress.wasCanceled() message.
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

void ActivitiesForm::studentsFilterChanged()
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

void ActivitiesForm::filterChanged()
{
	disconnect(activitiesListWidget, &QListWidget::itemSelectionChanged, this, &ActivitiesForm::selectionChanged);

	int /*nacts=0,*/ nsubacts=0, nh=0;
	int ninact=0, ninacth=0;

	QString s;
	activitiesListWidget->clear();
	visibleActivitiesList.clear();
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Filtering the activities", "Title of a progress dialog"));
	progress.setLabelText(tr("Filtering the activities ... please wait"));
	progress.setRange(0, qMax(gt.rules.activitiesList.size(), 1));
	progress.setModal(true);
	
	int k=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		progress.setValue(i);
		if(progress.wasCanceled()){
			LongTextMessageBox::largeInformation(this, tr("FET warning"), tr("You canceled the filtering of the activities - the list of activities will be incomplete.")+QString(" ")+
			 tr("Note: if filtering of the activities takes too much, it might be because you are filtering on the detailed description with constraints of the activities,"
			 " which checks each activity against each time constraint, each space constraint, and each group activities in the initial order item, which might be too much."
			 " Please consider filtering on the description or detailed description of the activities, instead.")+QString("\n\n")+tr("Note: if you are using more filters,"
			 " like 'the activity should have a certain teacher', you might obtain much faster results if you filter on the description/detailed description of the activity"
			 " for the teacher and on the detailed description with constraints for other filters (meaning that you should prefer using the description/detailed description"
			 " instead of the detailed description with constraints, whenever it is possible).")+QString(" ")+tr("Also, the order of the detailed description with constraints"
			 " filters is important: you should put firstly the ones which filter out more activities (if you selected the 'All' radio button) or put firstly the ones which"
			 " accept immediately more activities (if you selected the 'Any' radio button)."));
			break;
		}
		
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleActivitiesList.append(act);
			activitiesListWidget->addItem(s);
			k++;
			
			if(!act->active){
				activitiesListWidget->item(k-1)->setBackground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
				activitiesListWidget->item(k-1)->setForeground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
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
	numberTextLabel->setText(tr("No: %1 / %2", "No means number, %1 is the number of active activities, %2 is the total number of activities."
		" Please leave spaces between fields, so that they are better visible").arg(NA).arg(NT));
	durationTextLabel->setText(tr("Dur: %1 / %2", "Dur means duration, %1 is the duration of active activities, %2 is the total duration of activities."
		" Please leave spaces between fields, so that they are better visible").arg(DA).arg(DT));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));
	
	if(activitiesListWidget->count()>0)
		activitiesListWidget->setCurrentRow(0);
	else
		currentActivityTextEdit->setPlainText(QString(""));
	
	selectionChanged();
	connect(activitiesListWidget, &QListWidget::itemSelectionChanged, this, &ActivitiesForm::selectionChanged);
}

void ActivitiesForm::addActivity()
{
	int nInitialActs=gt.rules.activitiesList.count();

	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	
	AddActivityForm addActivityForm(this, tn, stn, sn, atn);
	setParentAndOtherThings(&addActivityForm, this);
	addActivityForm.exec();

	if(gt.rules.activitiesList.count()!=nInitialActs){
		assert(gt.rules.activitiesList.count()>nInitialActs);

		//rebuild the activities list box
		filterChanged();
	
		int ind=activitiesListWidget->count()-1;
		if(ind>=0)
			activitiesListWidget->setCurrentRow(ind);
	}
	activitiesListWidget->setFocus();
}

void ActivitiesForm::modifyActivity()
{
	int ind=activitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity"));
		return;
	}
	
	assert(ind<visibleActivitiesList.count());
	
	int valv=activitiesListWidget->verticalScrollBar()->value();
	int valh=activitiesListWidget->horizontalScrollBar()->value();

	Activity* act=visibleActivitiesList[ind];
	assert(act!=nullptr);
	
	QStringList teachers=act->teachersNames;
	bool diffTeachers=false;
	
	QString subject=act->subjectName;
	bool diffSubject=false;
	
	QStringList activityTags=act->activityTagsNames;
	bool diffActivityTags=false;
	
	QStringList students=act->studentsNames;
	bool diffStudents=false;
	
	int nTotalStudents=act->nTotalStudents;
	bool diffNTotalStudents=false;
	
	bool computeNTotalStudents=act->computeNTotalStudents;
	bool diffComputeNTotalStudents=false;
	
	if(act->isSplit()){
		int nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act2=gt.rules.activitiesList[i];
			if(act2->activityGroupId==act->activityGroupId){
				nSplit++;
				
				if(teachers!=act2->teachersNames)
					diffTeachers=true;
				if(subject!=act2->subjectName)
					diffSubject=true;
				if(activityTags!=act2->activityTagsNames)
					diffActivityTags=true;
				if(students!=act2->studentsNames)
					diffStudents=true;
				if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ nTotalStudents!=act2->nTotalStudents )
					diffNTotalStudents=true;
				if(computeNTotalStudents!=act2->computeNTotalStudents)
					diffComputeNTotalStudents=true;
			}
			if(nSplit>MAX_SPLIT_OF_AN_ACTIVITY){
				QMessageBox::warning(this, tr("FET information"),
				 tr("Cannot modify this large activity, because it contains more than %1 activities.")
				 .arg(MAX_SPLIT_OF_AN_ACTIVITY));
				return;
			}
		}
		
		if(diffTeachers || diffSubject || diffActivityTags || diffStudents || diffNTotalStudents || diffComputeNTotalStudents){
			QStringList s;
			if(diffTeachers)
				s.append(tr("different teachers"));
			if(diffSubject)
				s.append(tr("different subject"));
			if(diffActivityTags)
				s.append(tr("different activity tags"));
			if(diffStudents)
				s.append(tr("different students"));
			if(diffComputeNTotalStudents)
				s.append(tr("different Boolean variable 'must compute n total students'"));
			if(diffNTotalStudents)
				s.append(tr("different number of students"));
				
			QString s2;
			s2+=tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
			 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
			 " from this larger split activity.");
			s2+="\n\n";
			s2+=tr("The fields which are different are: %1").arg(s.join(translatedCommaSpace()));
				
			/*int t=QMessageBox::warning(this, tr("FET warning"), s2, tr("Abort"), tr("Continue"), QString(), 1, 0);
			
			if(t==0)
				return;*/
			QMessageBox::StandardButton t=QMessageBox::warning(this, tr("FET warning"), s2, QMessageBox::Ok | QMessageBox::Cancel);
			
			if(t==QMessageBox::Cancel)
				return;
		}
	}
	
	ModifyActivityForm modifyActivityForm(this, act->id, act->activityGroupId);
	int t;
	setParentAndOtherThings(&modifyActivityForm, this);
	t=modifyActivityForm.exec();
	
	if(t==QDialog::Accepted){
		filterChanged();
	
		activitiesListWidget->verticalScrollBar()->setValue(valv);
		activitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(ind >= activitiesListWidget->count())
			ind = activitiesListWidget->count()-1;
		if(ind>=0)
			activitiesListWidget->setCurrentRow(ind);
	}
	else{
		assert(t==QDialog::Rejected);
	}
}

void ActivitiesForm::removeActivities()
{
	QList<int> tl;
	
	QString su;

	QString s=tr("Remove these selected activities?");
	s+=" ";
	s+=tr("(If you select subactivities from a larger split activity, all the subactivities with the same group id will be automatically removed, "
	 "even if they are not selected.)");
	s+="\n\n";
	for(int i=0; i<activitiesListWidget->count(); i++)
		if(activitiesListWidget->item(i)->isSelected()){
			assert(i<visibleActivitiesList.count());
			Activity* act=visibleActivitiesList.at(i);
			assert(act!=nullptr);

			tl.append(act->id);

			/*if(act->isSplit()){
				s+=tr("There will also be removed the related activities from the same larger split activity.");
				s+="\n";
			}*/
			
			QString tmps=act->getDetailedDescription(gt.rules);
			tmps+="\n";

			s+=tmps;
			
			su+=tmps;
		}

	int t=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
	 s, tr("Yes"), tr("No"), QString(), 0, 1 );
	if(t==1){
		activitiesListWidget->setFocus();
		return;
	}
	
	gt.rules.removeActivities(tl, true);
	PlanningChanged::increasePlanningCommunicationSpinBox();

	if(!tl.isEmpty())
		gt.rules.addUndoPoint(tr("Removed %1 activities (plus all the subactivities from the same larger split activity, even if they were not selected):").arg(tl.count())+QString("\n\n")+su);

	int valv=activitiesListWidget->verticalScrollBar()->value();
	int valh=activitiesListWidget->horizontalScrollBar()->value();

	int cr=activitiesListWidget->currentRow();

	filterChanged();
	
	if(cr>=0){
		if(cr<activitiesListWidget->count())
			activitiesListWidget->setCurrentRow(cr);
		else if(activitiesListWidget->count()>0)
			activitiesListWidget->setCurrentRow(activitiesListWidget->count()-1);
	}

	activitiesListWidget->verticalScrollBar()->setValue(valv);
	activitiesListWidget->horizontalScrollBar()->setValue(valh);

	activitiesListWidget->setFocus();

	/*case 0: // The user clicked the OK button or pressed Enter
		gt.rules.removeActivity(act->id, act->activityGroupId);
		PlanningChanged::increasePlanningCommunicationSpinBox();
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		return;
	}*/
}

void ActivitiesForm::activityChanged()
{
	int index=activitiesListWidget->currentRow();
	
	if(index<0){
		currentActivityTextEdit->setPlainText(QString(""));
		return;
	}
	if(index>=visibleActivitiesList.count()){
		currentActivityTextEdit->setPlainText(tr("Invalid activity"));
		return;
	}

	QString s;
	Activity* act=visibleActivitiesList[index];

	assert(act!=nullptr);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	currentActivityTextEdit->setPlainText(s);
}

void ActivitiesForm::help()
{
	QString s;
	
	s+=tr("Useful instructions/tips:");
	s+="\n\n";
	
	s+=tr("Above the activities list we have 2 labels, containing 4 numbers. The first label contains text: No: a / b. The first number a is the"
		" number of active activities (we number each individual subactivity as 1), while the second number b is the number of total activities."
		" The second label contains text: Dur: c / d. The third number c is the duration of active activities, in periods"
		" (or FET hours), while the fourth number d is the duration of total activities, in periods (or FET hours)."
		" So, No means number and Dur means duration.");
	s+="\n\n";
	s+=tr("Example: No: 100 / 102, Dur: 114 / 117. They represent: 100 - the number of active activities,"
		" then 102 - the number of total activities,"
		" 114 - the duration of active activities (in periods or FET hours) and 117 - the duration of total activities"
		" (in periods or FET hours). In this example we have 2 inactive activities with their combined duration being 3 periods.");
	
	s+="\n\n";
	s+=tr("Explanation of the short description of an activity: first comes the id."
		" If the activity is inactive, an X follows. Then the duration. Then, if the activity is split, a slash and the total duration."
		" Then teachers, subject, activity tag (if it is not void) and students. Then the number of students (if specified).");
	s+="\n\n";
	s+=tr("The activities which are inactive:", "This is the help for activities which are inactive, after this field there come explanations for how inactive activities are displayed.");
	s+="\n";
	s+=" -";
	s+=tr("have an X mark after the id.", "It refers to inactive activities, which have this mark after the id.");
	s+="\n";
	s+=" -";
	s+=tr("will appear with different background color.", "It refers to inactive activities");
	s+="\n\n";
	s+=tr("To modify an activity, you can also double click it.");
	s+="\n\n";
	s+=tr("Show related: if you select this, there will be listed activities for groups and subgroups contained also in the current set (if the current set"
		" is a year or a group) and also higher ranked year or group (if the current set is a group or a subgroup).");
	
	s+=tr("There are two available filters: one is simple, based on the teacher, students, subject, or activity tag, the other one is more advanced (select the 'Filter' check box)."
		" The resulted overall filter is the combined AND of these two filters, if both filters are active, or the active one, if only one is active. If no filters are active,"
		" all the activities will be shown.");
	
	s+="\n\n";
	s+=tr("Inverted: this will show all the activities which _don't_ respect the selected filter for the teacher, students, subject, or activity tag.");
	
	s+="\n\n";
	s+=tr("Note that it is possible to activate/deactivate an activity in two different ways: by modifying an activity and selecting/deselecting the 'Active'"
		" check box, or directly, by clicking the corresponding buttons in this dialog.");
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void ActivitiesForm::activityComments()
{
	int ind=activitiesListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected activity"));
		return;
	}
	
	assert(ind<visibleActivitiesList.count());

	Activity* act=visibleActivitiesList[ind];
	assert(act!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Activity comments"));
	
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
	
	const QString settingsName=QString("ActivityCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString ab=act->getDetailedDescription(gt.rules);
	
		act->comments=commentsPT->toPlainText();

		gt.rules.addUndoPoint(tr("Changed an activity's comments. Activity before:\n\n%1\nComments after:\n\n%2").arg(ab).arg(act->comments));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;

		activitiesListWidget->currentItem()->setText(act->getDescription(gt.rules));
		activityChanged();
	}

	activitiesListWidget->setFocus();
}

void ActivitiesForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
		
		activitiesListWidget->setFocus();
		
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for activities"), true, all, descrDetDescrDetDescrWithConstraints, contains, text, caseSensitive, "ActivitiesAdvancedFilterForm");

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

		activitiesListWidget->setFocus();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &ActivitiesForm::filter);
	}
	
	delete filterForm;
}

void ActivitiesForm::activateActivities()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected activities?");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			activitiesListWidget->setFocus();
			return;
		}
	}
	
	QString su;

	int cnt=0;
	for(int i=0; i<activitiesListWidget->count(); i++)
		if(activitiesListWidget->item(i)->isSelected()){
			assert(i<visibleActivitiesList.count());
			Activity* act=visibleActivitiesList.at(i);
			assert(act!=nullptr);
			if(!act->active){
				su+=act->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				act->active=true;
			}
		}
	if(cnt>0){
		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Activated %1 activities:", "%1 is the number of activated activities").arg(cnt)+QString("\n\n")+su);
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
		
		int valv=activitiesListWidget->verticalScrollBar()->value();
		int valh=activitiesListWidget->horizontalScrollBar()->value();

		int cr=activitiesListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<activitiesListWidget->count())
				activitiesListWidget->setCurrentRow(cr);
			else if(activitiesListWidget->count()>0)
				activitiesListWidget->setCurrentRow(activitiesListWidget->count()-1);
		}

		activitiesListWidget->verticalScrollBar()->setValue(valv);
		activitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Activated %1 activities", "%1 is the number of activated activities").arg(cnt));
	}
	
	activitiesListWidget->setFocus();
}

void ActivitiesForm::deactivateActivities()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected activities?");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(ret==QMessageBox::No){
			activitiesListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	for(int i=0; i<activitiesListWidget->count(); i++)
		if(activitiesListWidget->item(i)->isSelected()){
			assert(i<visibleActivitiesList.count());
			Activity* act=visibleActivitiesList.at(i);
			assert(act!=nullptr);
			if(act->active){
				su+=act->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				act->active=false;
			}
		}
	if(cnt>0){
		PlanningChanged::increasePlanningCommunicationSpinBox();

		gt.rules.addUndoPoint(tr("Deactivated %1 activities:", "%1 is the number of deactivated activities").arg(cnt)+QString("\n\n")+su);
		
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
		
		int valv=activitiesListWidget->verticalScrollBar()->value();
		int valh=activitiesListWidget->horizontalScrollBar()->value();

		int cr=activitiesListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<activitiesListWidget->count())
				activitiesListWidget->setCurrentRow(cr);
			else if(activitiesListWidget->count()>0)
				activitiesListWidget->setCurrentRow(activitiesListWidget->count()-1);
		}

		activitiesListWidget->verticalScrollBar()->setValue(valv);
		activitiesListWidget->horizontalScrollBar()->setValue(valh);

		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 activities", "%1 is the number of deactivated activities").arg(cnt));
	}

	activitiesListWidget->setFocus();
}

void ActivitiesForm::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(activitiesListWidget->count()==visibleActivitiesList.count());
	for(int i=0; i<activitiesListWidget->count(); i++)
		if(activitiesListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleActivitiesList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected activities, %1 is the number of active"
	 " selected activities, %2 is the total number of selected activities").arg(nActive).arg(nTotal));
}

void ActivitiesForm::changeActivityTags()
{
	QList<Activity*> tl;
	for(int i=0; i<activitiesListWidget->count(); i++)
		if(activitiesListWidget->item(i)->isSelected()){
			assert(i<visibleActivitiesList.count());
			Activity* act=visibleActivitiesList.at(i);
			assert(act!=nullptr);
			tl.append(act);
		}

	if(tl.isEmpty()){
		QMessageBox::warning(this, tr("FET warning"), tr("No activities selected."));
		
		return;
	}

	ActivitiesTagsForm activitiesTagsForm(this, tl);
	setParentAndOtherThings(&activitiesTagsForm, this);
	activitiesTagsForm.exec();

	//////
	int valv=activitiesListWidget->verticalScrollBar()->value();
	int valh=activitiesListWidget->horizontalScrollBar()->value();

	int cr=activitiesListWidget->currentRow();

	filterChanged();
	
	if(cr>=0){
		if(cr<activitiesListWidget->count())
			activitiesListWidget->setCurrentRow(cr);
		else if(activitiesListWidget->count()>0)
			activitiesListWidget->setCurrentRow(activitiesListWidget->count()-1);
	}

	activitiesListWidget->verticalScrollBar()->setValue(valv);
	activitiesListWidget->horizontalScrollBar()->setValue(valh);

	activitiesListWidget->setFocus();
}
