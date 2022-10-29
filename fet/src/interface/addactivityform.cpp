/***************************************************************************
                          addactivityform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include <QtGlobal>

#include "longtextmessagebox.h"

#include "addactivityform.h"
#include "teacher.h"
#include "subject.h"
#include "studentsset.h"

#include "activityplanningform.h"

#include <QMessageBox>

#include <QDialog>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QScrollBar>

#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

QSpinBox* AddActivityForm::dur(int i)
{
	assert(i>=0 && i<durList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return durList.at(i);
}

QCheckBox* AddActivityForm::activ(int i)
{
	assert(i>=0 && i<activList.count());
	assert(i<MAX_SPLIT_OF_AN_ACTIVITY);
	return activList.at(i);
}

AddActivityForm::AddActivityForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName): QDialog(parent)
{
	setupUi(this);

	studentsSeparatelyCheckBox->setChecked(false);

	for(Teacher* tch : qAsConst(gt.rules.teachersList))
		teacherNamesSet.insert(tch->name);
	for(Subject* sbj : qAsConst(gt.rules.subjectsList))
		subjectNamesSet.insert(sbj->name);
	for(ActivityTag* at : qAsConst(gt.rules.activityTagsList))
		activityTagNamesSet.insert(at->name);
	
	allTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedTeachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedStudentsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	allActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	splitSpinBox->setMinimum(1);
	splitSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
	splitSpinBox->setValue(1);
	
	durList.clear();
	activList.clear();

	populateSubactivitiesTabWidget(splitSpinBox->value());

	QSettings settings(COMPANY, PROGRAM);

	subgroupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-subgroups-check-box-state"), "false").toBool());
	groupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-groups-check-box-state"), "true").toBool());
	yearsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-years-check-box-state"), "true").toBool());

	allTeachersRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/all-teachers-radio-button-state"), "true").toBool());
	qualifiedTeachersRadioButton->setChecked(settings.value(this->metaObject()->className()+QString("/qualified-teachers-radio-button-state"), "false").toBool());

	connect(subgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsChanged()));
	connect(groupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showGroupsChanged()));
	connect(yearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(showYearsChanged()));

	connect(splitSpinBox, SIGNAL(valueChanged(int)), this, SLOT(splitChanged()));

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(addActivityPushButton, SIGNAL(clicked()), this, SLOT(addActivity()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));

	connect(allTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addTeacher()));
	connect(selectedTeachersListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeTeacher()));
	connect(allStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addStudents()));
	connect(selectedStudentsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeStudents()));
	connect(allActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addActivityTag()));
	connect(selectedActivityTagsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeActivityTag()));

	connect(clearActivityTagPushButton, SIGNAL(clicked()), this, SLOT(clearActivityTags()));
	connect(clearStudentsPushButton, SIGNAL(clicked()), this, SLOT(clearStudents()));
	connect(clearTeacherPushButton, SIGNAL(clicked()), this, SLOT(clearTeachers()));

	connect(minDayDistanceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(minDaysChanged()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	selectedStudentsListWidget->clear();
	updateStudentsListWidget();

	updateSubjectsComboBox();
	if(subjectName!=""){
		int pos=-1;
		for(int i=0; i<subjectsComboBox->count(); i++){
			if(subjectsComboBox->itemText(i)==subjectName){
				pos=i;
				break;
			}
		}
		assert(pos>=0);
		subjectsComboBox->setCurrentIndex(pos);
	}
	else{
		//begin trick to pass a Qt 4.6.0 bug: the first entry is not highlighted with mouse until you move to second entry and then back up
		//also, this trick makes the combo box behave nicer under Windows: the first subject is shown with an ugly edge if not using this trick.
		if(subjectsComboBox->view()){
			subjectsComboBox->view()->setCurrentIndex(QModelIndex());
		}
		//end trick
		subjectsComboBox->setCurrentIndex(-1);
	}

	updateActivityTagsListWidget();

	//after updateSubjectsComboBox
	connect(subjectsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAllTeachersListWidget()));
	connect(allTeachersRadioButton, SIGNAL(toggled(bool)), this, SLOT(allTeachersRadioButtonToggled(bool)));
	connect(qualifiedTeachersRadioButton, SIGNAL(toggled(bool)), this, SLOT(qualifiedTeachersRadioButtonToggled(bool)));
	updateAllTeachersListWidget();
	selectedTeachersListWidget->clear();

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		minDayDistanceSpinBox->setMaximum(gt.rules.nDaysPerWeek-1);
	else
		minDayDistanceSpinBox->setMaximum(gt.rules.nDaysPerWeek/2-1);
	minDayDistanceSpinBox->setMinimum(0);
	minDayDistanceSpinBox->setValue(1);
	
	int nSplit=splitSpinBox->value();
	
	minDayDistanceTextLabel->setEnabled(nSplit>=2);
	minDayDistanceSpinBox->setEnabled(nSplit>=2);
	percentageTextLabel->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	
	nStudentsSpinBox->setMinimum(-1);
	nStudentsSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	nStudentsSpinBox->setValue(-1);

	addActivityPushButton->setDefault(true);
	addActivityPushButton->setFocus();
	
	if(teacherName!="")
		selectedTeachersListWidget->addItem(teacherName);
	if(studentsSetName!="")
		selectedStudentsListWidget->addItem(studentsSetName);
	if(activityTagName!="")
		selectedActivityTagsListWidget->addItem(activityTagName);
}

AddActivityForm::~AddActivityForm()
{
	saveFETDialogGeometry(this);
	
	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/show-subgroups-check-box-state"), subgroupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-groups-check-box-state"), groupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-years-check-box-state"), yearsCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/qualified-teachers-radio-button-state"), qualifiedTeachersRadioButton->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/all-teachers-radio-button-state"), allTeachersRadioButton->isChecked());
}

void AddActivityForm::populateSubactivitiesTabWidget(int n)
{
	int oldIndex=subactivitiesTabWidget->currentIndex();

	int oldN=subactivitiesTabWidget->count();
	if(oldN>n){
		for(int i=oldN-1; i>=n; i--){
			QWidget* wd=subactivitiesTabWidget->widget(i);
			subactivitiesTabWidget->removeTab(i);
			assert(durList.count()>0);
			durList.removeLast();
			assert(activList.count()>0);
			activList.removeLast();
			delete wd;
		}

		if(oldIndex<subactivitiesTabWidget->count())
			subactivitiesTabWidget->setCurrentIndex(oldIndex);
		else
			subactivitiesTabWidget->setCurrentIndex(subactivitiesTabWidget->count()-1);
	}
	else if(oldN<n){
		for(int i=oldN; i<n; i++){
			QWidget* wd=new QWidget(subactivitiesTabWidget);
			
			QCheckBox* cb=new QCheckBox(tr("Active"), wd);
			cb->setChecked(true);
			QSpinBox* sb=new QSpinBox(wd);
			sb->setMinimum(1);
			sb->setMaximum(MAX_HOURS_PER_DAY);
			sb->setValue(1);
			QLabel* ld=new QLabel(tr("Duration"), wd);
			
			QHBoxLayout* hld=new QHBoxLayout();
			hld->addWidget(ld);
			hld->addWidget(sb);
			
			QHBoxLayout* hla=new QHBoxLayout();
			hla->addStretch();
			hla->addWidget(cb);
			
			QVBoxLayout* vl=new QVBoxLayout(wd);
			vl->addLayout(hld);
			vl->addLayout(hla);
			
			subactivitiesTabWidget->addTab(wd, QString::number(i+1));
			
			durList.append(sb);
			activList.append(cb);
		}

		subactivitiesTabWidget->setCurrentIndex(subactivitiesTabWidget->count()-1);
	}
}

void AddActivityForm::allTeachersRadioButtonToggled(bool toggled)
{
	if(toggled)
		updateAllTeachersListWidget();
}

void AddActivityForm::qualifiedTeachersRadioButtonToggled(bool toggled)
{
	if(toggled)
		updateAllTeachersListWidget();
}

void AddActivityForm::updateAllTeachersListWidget()
{
	allTeachersListWidget->clear();
	
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		if(allTeachersRadioButton->isChecked() || subjectsComboBox->currentIndex()==-1){
			allTeachersListWidget->addItem(tch->name);
		}
		else{
			assert(qualifiedTeachersRadioButton->isChecked());
			assert(subjectsComboBox->currentText()!="");
			assert(subjectNamesSet.contains(subjectsComboBox->currentText()));
			if(tch->qualifiedSubjectsHash.contains(subjectsComboBox->currentText())){
				allTeachersListWidget->addItem(tch->name);
			}
		}
	}
}

void AddActivityForm::addTeacher()
{
	if(allTeachersListWidget->currentRow()<0 || allTeachersListWidget->currentRow()>=allTeachersListWidget->count())
		return;
	
	for(int i=0; i<selectedTeachersListWidget->count(); i++)
		if(selectedTeachersListWidget->item(i)->text()==allTeachersListWidget->currentItem()->text())
			return;
			
	selectedTeachersListWidget->addItem(allTeachersListWidget->currentItem()->text());
	selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void AddActivityForm::removeTeacher()
{
	if(selectedTeachersListWidget->count()<=0 || selectedTeachersListWidget->currentRow()<0 ||
	 selectedTeachersListWidget->currentRow()>=selectedTeachersListWidget->count())
		return;
	
	int i=selectedTeachersListWidget->currentRow();
	selectedTeachersListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedTeachersListWidget->takeItem(i);
	delete item;
	if(i<selectedTeachersListWidget->count())
		selectedTeachersListWidget->setCurrentRow(i);
	else
		selectedTeachersListWidget->setCurrentRow(selectedTeachersListWidget->count()-1);
}

void AddActivityForm::addStudents()
{
	if(allStudentsListWidget->currentRow()<0 || allStudentsListWidget->currentRow()>=allStudentsListWidget->count())
		return;
	
	assert(canonicalStudentsSetsNames.count()==allStudentsListWidget->count());
	QString sn=canonicalStudentsSetsNames.at(allStudentsListWidget->currentRow());

	for(int i=0; i<selectedStudentsListWidget->count(); i++)
		if(selectedStudentsListWidget->item(i)->text()==sn)
			return;
	
	selectedStudentsListWidget->addItem(sn);
	selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void AddActivityForm::removeStudents()
{
	if(selectedStudentsListWidget->count()<=0 || selectedStudentsListWidget->currentRow()<0 ||
	 selectedStudentsListWidget->currentRow()>=selectedStudentsListWidget->count())
		return;
	
	int i=selectedStudentsListWidget->currentRow();
	selectedStudentsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedStudentsListWidget->takeItem(i);
	delete item;
	if(i<selectedStudentsListWidget->count())
		selectedStudentsListWidget->setCurrentRow(i);
	else
		selectedStudentsListWidget->setCurrentRow(selectedStudentsListWidget->count()-1);
}

void AddActivityForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;
	
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;
	
	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddActivityForm::removeActivityTag()
{
	if(selectedActivityTagsListWidget->count()<=0 || selectedActivityTagsListWidget->currentRow()<0 ||
	 selectedActivityTagsListWidget->currentRow()>=selectedActivityTagsListWidget->count())
		return;
	
	int i=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(i);
	delete item;
	if(i<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(i);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddActivityForm::updateSubjectsComboBox()
{
	subjectsComboBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sbj=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sbj->name);
	}
}

void AddActivityForm::updateActivityTagsListWidget()
{
	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
	
	selectedActivityTagsListWidget->clear();
}

void AddActivityForm::showYearsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::showGroupsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::showSubgroupsChanged()
{
	updateStudentsListWidget();
}

void AddActivityForm::updateStudentsListWidget()
{
	const int INDENT=2;

	bool showYears=yearsCheckBox->isChecked();
	bool showGroups=groupsCheckBox->isChecked();
	bool showSubgroups=subgroupsCheckBox->isChecked();

	allStudentsListWidget->clear();
	canonicalStudentsSetsNames.clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		if(showYears){
			allStudentsListWidget->addItem(sty->name);
			canonicalStudentsSetsNames.append(sty->name);
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(showGroups){
				QString begin=QString("");
				QString end=QString("");
				begin=QString(INDENT, ' ');
				allStudentsListWidget->addItem(begin+stg->name+end);
				canonicalStudentsSetsNames.append(stg->name);
			}
			if(showSubgroups) for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];

				QString begin=QString("");
				QString end=QString("");
				begin=QString(2*INDENT, ' ');
				allStudentsListWidget->addItem(begin+sts->name+end);
				canonicalStudentsSetsNames.append(sts->name);
			}
		}
	}
	
	int q=allStudentsListWidget->verticalScrollBar()->minimum();
	allStudentsListWidget->verticalScrollBar()->setValue(q);
}

void AddActivityForm::splitChanged()
{
	int nSplit=splitSpinBox->value();
	
	minDayDistanceTextLabel->setEnabled(nSplit>=2);
	minDayDistanceSpinBox->setEnabled(nSplit>=2);
	percentageTextLabel->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(nSplit>=2 && minDayDistanceSpinBox->value()>0);

	populateSubactivitiesTabWidget(nSplit);
}

SecondMinDaysDialog::SecondMinDaysDialog(QWidget* p, int minD, double w) :QDialog(p)
{
	weight=-1;

	QString l=tr
	 ("You selected min days between activities %1 (above 1) and weight %2 (under 100.0). "
	  "Would you like to add also a second constraint to ensure that almost certainly the "
	  "distance between activities is at least %3 (%1-1) days? If yes, please select weight (recommended "
	  "95.0%-100.0%) and click Yes. If no, please click No (only one constraint will be added)").arg(CustomFETString::number(minD)).arg(w).arg(minD-1);
	l+="\n\n";
	l+=tr("(Yes means to add an additional constraint min %1 days between activities, weight 0.0%-100.0%. "
	  "If you say Yes, you will have 2 constraints min days added for current activities. "
	  "Adding the second constraint might lead to impossible timetables if the condition is "
	  "too tight, but you can remove the second constraint at any time).").arg(minD-1);
	l+="\n\n";
	l+=tr("Note: 95% is usually enough for min days constraints referring to same activities. "
	  "The weights are accumulated if referring to the same activities. If you have 2 constraints with say 95%"
	  " (say min n days and min n-1 days), "
	  "the min n days constraint is skipped with probability 5%, then min n-1 days constraint is skipped with "
	  "probability 0.25%=5%*5%, so you'll get in 99.75% cases the min n-1 days constraint respected.");
	l+="\n\n";
	l+=tr("Recommended answer is Yes, 95% (or higher).");

	setWindowTitle(tr("Add a second constraint or not?"));
				
	QVBoxLayout* vl=new QVBoxLayout(this);
	
	QPlainTextEdit* la=new QPlainTextEdit();
	la->setPlainText(l);
	la->setReadOnly(true);

	vl->addWidget(la);
				
	QPushButton* yes=new QPushButton(tr("Yes"));
	yes->setDefault(true);
	
	QPushButton* no=new QPushButton(tr("No"));

	QLabel* percLabel=new QLabel(this);
	percLabel->setText("Percentage");
	percText=new QLineEdit(this);
	percText->setText("95.0");
	
	//QHBoxLayout* hl2=new QHBoxLayout(vl);
	QHBoxLayout* hl2=new QHBoxLayout();
	vl->addLayout(hl2);
	
	//////
	QLabel* minDaysLabel=new QLabel(this);
	minDaysLabel->setText("Min days");
	QSpinBox* minDaysSpinBox=new QSpinBox(this);
	minDaysSpinBox->setMinimum(minD-1);
	minDaysSpinBox->setMaximum(minD-1);
	minDaysSpinBox->setValue(minD-1);
	minDaysSpinBox->setEnabled(false);
	//////

	//////
	hl2->addStretch(1);
	hl2->addWidget(minDaysLabel);
	hl2->addWidget(minDaysSpinBox);
	//////
	
	hl2->addStretch(1);
	hl2->addWidget(percLabel);
	hl2->addWidget(percText);
	
	//QHBoxLayout* hl=new QHBoxLayout(vl);
	QHBoxLayout* hl=new QHBoxLayout();
	vl->addLayout(hl);
	
	hl->addStretch(1);
	hl->addWidget(yes);
	hl->addWidget(no);
	
	connect(yes, SIGNAL(clicked()), this, SLOT(yesPressed()));
	connect(no, SIGNAL(clicked()), this, SLOT(reject()));
	
	int ww=this->sizeHint().width();
	if(ww>1000)
		ww=1000;
	if(ww<590)
		ww=590;

	int hh=this->sizeHint().height();
	if(hh>650)
		hh=650;
	if(hh<380)
		hh=380;
	
	this->resize(ww, hh);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

SecondMinDaysDialog::~SecondMinDaysDialog()
{
	saveFETDialogGeometry(this);
}

void SecondMinDaysDialog::yesPressed()
{
	double wt;
	QString tmp=percText->text();
	weight_sscanf(tmp, "%lf", &wt);
	if(wt<0.0 || wt>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be >=0 and <=100.0"));
		return;
	}
	weight=wt;
	accept();
}

void AddActivityForm::addActivity()
{
	if(studentsSeparatelyCheckBox->isChecked()){
		addMultipleActivities();
		return;
	}

	double weight;
	QString tmp=percentageLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(percentageLineEdit->isEnabled() && (weight<0.0 || weight>100.0)){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) for added constraint min days between activities"));
		return;
	}

	//subject
	QString subject_name=subjectsComboBox->currentText();
	bool found=subjectNamesSet.contains(subject_name);
	/*int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){*/
	if(!found){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid subject"));
		return;
	}

	QStringList activity_tags_names;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++){
		//assert(gt.rules.searchActivityTag(selectedActivityTagsListWidget->item(i)->text())>=0);
		assert(activityTagNamesSet.contains(selectedActivityTagsListWidget->item(i)->text()));
		activity_tags_names.append(selectedActivityTagsListWidget->item(i)->text());
	}

	//teachers
	QStringList teachers_names;
	if(selectedTeachersListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to add an activity without teacher(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedTeachersListWidget->count(); i++){
			//assert(gt.rules.searchTeacher(selectedTeachersListWidget->item(i)->text())>=0);
			assert(teacherNamesSet.contains(selectedTeachersListWidget->item(i)->text()));
			teachers_names.append(selectedTeachersListWidget->item(i)->text());
		}
	}

	//students
	int numberOfStudents=0;
	QStringList students_names;
	if(selectedStudentsListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to add an activity without student set(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;

		//Crash bug fixed when adding an activity without students but with a specified number of students >= 1
		//Reported by Zsolt Udvari
		//Reported and fixed on 2017-07-25
		if(nStudentsSpinBox->value()>=0)
			numberOfStudents=nStudentsSpinBox->value();
	}
	else{
		for(int i=0; i<selectedStudentsListWidget->count(); i++){
			//assert(gt.rules.searchStudentsSet(selectedStudentsListWidget->item(i)->text())!=nullptr);
			/*assert(numberOfStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=numberOfStudentsHash.value(selectedStudentsListWidget->item(i)->text());*/
			assert(gt.rules.permanentStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			numberOfStudents+=gt.rules.permanentStudentsHash.value(selectedStudentsListWidget->item(i)->text())->numberOfStudents;
			students_names.append(selectedStudentsListWidget->item(i)->text());
		}

		if(nStudentsSpinBox->value()>=0)
			numberOfStudents=nStudentsSpinBox->value();
	}

	if(splitSpinBox->value()==1){ //indivisible activity
		int duration=dur(0)->value();
		if(duration<0){
			QMessageBox::warning(this, tr("FET information"),
				tr("Invalid duration"));
			return;
		}

		bool active=false;
		if(activ(0)->isChecked())
			active=true;

		int activityid=0; //We set the id of this newly added activity = (the largest existing id + 1)
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > activityid)
				activityid = act->id;
		}
		activityid++;
		Activity a(gt.rules, activityid, 0, teachers_names, subject_name, activity_tags_names, students_names,
			duration, duration, /*parity,*/ active, (nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);

		bool already_existing=false;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if((*act)==a)
				already_existing=true;
		}

		if(already_existing){
			/*int t=QMessageBox::question(this, tr("FET question"),
				tr("A similar activity already exists. Do you want to insert current activity?"),
				tr("Yes"),tr("No"));
			assert(t==0 || t==1 || t==-1);
			if(t==1) //no pressed
				return;
			if(t==-1) //Esc pressed
				return;*/
			QMessageBox::StandardButton t=QMessageBox::question(this, tr("FET question"),
				tr("A similar activity already exists. Do you want to insert current activity?"),
				QMessageBox::Yes | QMessageBox::No);
			if(t==QMessageBox::No)
				return;
		}

		bool tmp=gt.rules.addSimpleActivityFast(this, activityid, 0, teachers_names, subject_name, activity_tags_names,
			students_names, duration, duration, active,
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);
		if(tmp)
			QMessageBox::information(this, tr("FET information"), tr("Activity added"));
		else
			QMessageBox::critical(this, tr("FET information"), tr("Activity NOT added - please report error"));
	}
	else{ //split activity
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			if(minDayDistanceSpinBox->value()>0 && splitSpinBox->value()>gt.rules.nDaysPerWeek){
				int t=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
				 tr("Possible incorrect setting. Are you sure you want to add current activity? See details below:")+"\n\n"+
				 tr("You want to add a container activity split into more than the number of days per week and also add a constraint min days between activities."
				  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
				 "\n\n"+
				 tr("The best way to add the activities would be:")+
				 "\n\n"+
				 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
				  ". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
				  " (possibly raising the weight of added constraint min days between activities up to 100%)")+
				  "\n\n"+
				 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
				  " activities equal with the number of days per week and the remaining components into other larger split activity."
				  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
				  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
				  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+
			  	 "\n\n"+
				 tr("Do you want to add current activities as they are now (not recommended) or cancel and edit them as instructed?")
				  ,
				 tr("Yes"), tr("No"), QString(), 0, 1);

				if(t==1)
					return;
			}
		}
		else{
			if(minDayDistanceSpinBox->value()>0 && splitSpinBox->value()>gt.rules.nDaysPerWeek/2){
				int t=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
				 tr("Possible incorrect setting. Are you sure you want to add current activity? See details below:")+"\n\n"+
				 tr("You want to add a container activity split into more than the number of real days per week and also add a constraint min days between activities."
				  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
				 "\n\n"+
				 tr("The best way to add the activities would be:")+
				 "\n\n"+
				 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of real days per week"
				  ". Example: 7 activities with duration 1 in a 5 real days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
				  " (possibly raising the weight of added constraint min days between activities up to 100%)")+
				  "\n\n"+
				 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
				  " activities equal with the number of real days per week and the remaining components into other larger split activity."
				  " For example, suppose you need to add 7 activities with duration 1 in a 5 real days week. Add 2 larger container activities,"
				  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
				  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+
			  	 "\n\n"+
				 tr("Do you want to add current activities as they are now (not recommended) or cancel and edit them as instructed?")
				  ,
				 tr("Yes"), tr("No"), QString(), 0, 1);

				if(t==1)
					return;
			}
		}

		int totalduration;
		QList<int> durations;
		QList<bool> active;
		int nsplit=splitSpinBox->value();

		totalduration=0;
		for(int i=0; i<nsplit; i++){
			durations.append(dur(i)->value());
			active.append(false);
			if(activ(i)->isChecked())
				active[i]=true;

			totalduration+=durations[i];
		}

		//the group id of this split activity and the id of the first partial activity
		//it is the maximum already existing id + 1
		int firstactivityid=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > firstactivityid)
				firstactivityid = act->id;
		}
		firstactivityid++;

		int minD=minDayDistanceSpinBox->value();
		bool tmp=gt.rules.addSplitActivityFast(this, firstactivityid, firstactivityid,
			teachers_names, subject_name, activity_tags_names, students_names,
			nsplit, totalduration, durations,
			active, minD, weight, forceConsecutiveCheckBox->isChecked(),
			(nStudentsSpinBox->value()==-1), nStudentsSpinBox->value(), numberOfStudents);
		if(tmp){
			if(minD>1 && weight<100.0){
				SecondMinDaysDialog second(this, minD, weight);
				setParentAndOtherThings(&second, this);
				int code=second.exec();

				if(code==QDialog::Accepted){
					assert(second.weight>=0 && second.weight<=100.0);
					QList<int> acts;
					for(int i=0; i<nsplit; i++){
						acts.append(firstactivityid+i);
					}
					TimeConstraint* c=new ConstraintMinDaysBetweenActivities(second.weight, forceConsecutiveCheckBox->isChecked(), nsplit, acts, minD-1);
					bool tmp=gt.rules.addTimeConstraint(c);
					assert(tmp);
				}
			}
		
			QMessageBox::information(this, tr("FET information"), tr("Split activity added."
			 " Please note that FET currently cannot check for duplicates when adding split activities"
			 ". It is advisable to check the statistics after adding all the activities"));
		}
		else
			QMessageBox::critical(this, tr("FET information"), tr("Split activity NOT added - error???"));
	}

	PlanningChanged::increasePlanningCommunicationSpinBox();
}

void AddActivityForm::addMultipleActivities()
{
	assert(studentsSeparatelyCheckBox->isChecked());

	double weight;
	QString tmp=percentageLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(percentageLineEdit->isEnabled() && (weight<0.0 || weight>100.0)){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) for added constraint min days between activities"));
		return;
	}

	//subject
	QString subject_name=subjectsComboBox->currentText();
	bool found=subjectNamesSet.contains(subject_name);
	/*int subject_index=gt.rules.searchSubject(subject_name);
	if(subject_index<0){*/
	if(!found){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid subject"));
		return;
	}

	QStringList activity_tags_names;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++){
		//assert(gt.rules.searchActivityTag(selectedActivityTagsListWidget->item(i)->text())>=0);
		assert(activityTagNamesSet.contains(selectedActivityTagsListWidget->item(i)->text()));
		activity_tags_names.append(selectedActivityTagsListWidget->item(i)->text());
	}

	//teachers
	QStringList teachers_names;
	if(selectedTeachersListWidget->count()<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Do you really want to add an activity without teacher(s)?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(t==QMessageBox::No)
			return;
	}
	else{
		for(int i=0; i<selectedTeachersListWidget->count(); i++){
			//assert(gt.rules.searchTeacher(selectedTeachersListWidget->item(i)->text())>=0);
			assert(teacherNamesSet.contains(selectedTeachersListWidget->item(i)->text()));
			teachers_names.append(selectedTeachersListWidget->item(i)->text());
		}
	}

	//students
	//int numberOfStudents=0;
	QStringList students_names;
	if(selectedStudentsListWidget->count()<=1){
		QMessageBox::warning(this, tr("FET question"),
		 tr("If you intend to add multiple activities for students (option 'Separately' is checked), please select at least 2 students sets."));

		return;
	}
	else{
		for(int i=0; i<selectedStudentsListWidget->count(); i++){
			assert(gt.rules.permanentStudentsHash.contains(selectedStudentsListWidget->item(i)->text()));
			students_names.append(selectedStudentsListWidget->item(i)->text());
		}
	}

	if(splitSpinBox->value()==1){ //indivisible activity
		int duration=dur(0)->value();
		if(duration<0){
			QMessageBox::warning(this, tr("FET information"),
				tr("Invalid duration"));
			return;
		}

		bool active=false;
		if(activ(0)->isChecked())
			active=true;

		int activityid=0; //We set the id of this newly added activity = (the largest existing id + 1)
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > activityid)
				activityid = act->id;
		}
		
		int cnt_act=0;
		activityid++;

		for(int st=0; st<students_names.count(); st++){
			int n1=nStudentsSpinBox->value();
			int n2;
			if(n1==-1)
				n2=gt.rules.permanentStudentsHash.value(students_names.at(st))->numberOfStudents;
			else
				n2=nStudentsSpinBox->value();
			
			Activity a(gt.rules, activityid, 0, teachers_names, subject_name, activity_tags_names, QStringList(students_names.at(st)),
				duration, duration, /*parity,*/ active, (nStudentsSpinBox->value()==-1), n1, n2);

			bool already_existing=false;
			for(int i=0; i<gt.rules.activitiesList.size(); i++){
				Activity* act=gt.rules.activitiesList[i];
				if((*act)==a)
					already_existing=true;
			}
	
			if(already_existing){
				/*int t=QMessageBox::question(this, tr("FET question"),
					tr("A similar activity for students set %1 already exists. Do you want to insert current activity?").arg(students_names.at(st)),
					tr("Yes"),tr("No"));
				assert(t==0 || t==1 ||t==-1);
				if(t==1){ //no pressed
					continue;
				}
				if(t==-1){ //Esc pressed
					continue;
				}*/
				
				QMessageBox::StandardButton t=QMessageBox::question(this, tr("FET question"),
					tr("A similar activity for students set %1 already exists. Do you want to insert current activity?").arg(students_names.at(st)),
					QMessageBox::Yes | QMessageBox::No);
				if(t==QMessageBox::No)
					return;
			}

			bool tmp=gt.rules.addSimpleActivityFast(this, activityid, 0, teachers_names, subject_name, activity_tags_names,
				QStringList(students_names.at(st)), duration, duration, active,
				(nStudentsSpinBox->value()==-1), n1, n2);
			if(tmp){
				cnt_act++;
				activityid++;
			}
		}
		QMessageBox::information(this, tr("FET information"), tr("%1 activities added").arg(cnt_act));
	}
	else{ //split activity
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			if(minDayDistanceSpinBox->value()>0 && splitSpinBox->value()>gt.rules.nDaysPerWeek){
				int t=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
				 tr("Possible incorrect setting. Are you sure you want to add current activity? See details below:")+"\n\n"+
				 tr("You want to add a container activity split into more than the number of days per week and also add a constraint min days between activities."
				  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
				 "\n\n"+
				 tr("The best way to add the activities would be:")+
				 "\n\n"+
				 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
				  ". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
				  " (possibly raising the weight of added constraint min days between activities up to 100%)")+
				  "\n\n"+
				 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
				  " activities equal with the number of days per week and the remaining components into other larger split activity."
				  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
				  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
				  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+
			  	 "\n\n"+
				 tr("Do you want to add current activities as they are now (not recommended) or cancel and edit them as instructed?")
				  ,
				 tr("Yes"), tr("No"), QString(), 0, 1);

				if(t==1)
					return;
			}
		}
		else{
			if(minDayDistanceSpinBox->value()>0 && splitSpinBox->value()>gt.rules.nDaysPerWeek/2){
				int t=LongTextMessageBox::largeConfirmation(this, tr("FET confirmation"),
				 tr("Possible incorrect setting. Are you sure you want to add current activity? See details below:")+"\n\n"+
				 tr("You want to add a container activity split into more than the number of real days per week and also add a constraint min days between activities."
				  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
				 "\n\n"+
				 tr("The best way to add the activities would be:")+
				 "\n\n"+
				 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
				  ". Example: 7 activities with duration 1 in a 5 real days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
				  " (possibly raising the weight of added constraint min days between activities up to 100%)")+
				  "\n\n"+
				 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
				  " activities equal with the number of days per week and the remaining components into other larger split activity."
				  " For example, suppose you need to add 7 activities with duration 1 in a 5 real days week. Add 2 larger container activities,"
				  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
				  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+
			  	 "\n\n"+
				 tr("Do you want to add current activities as they are now (not recommended) or cancel and edit them as instructed?")
				  ,
				 tr("Yes"), tr("No"), QString(), 0, 1);

				if(t==1)
					return;
			}
		}

		int totalduration;
		QList<int> durations;
		QList<bool> active;
		int nsplit=splitSpinBox->value();

		totalduration=0;
		for(int i=0; i<nsplit; i++){
			durations.append(dur(i)->value());
			active.append(false);
			if(activ(i)->isChecked())
				active[i]=true;

			totalduration+=durations[i];
		}

		//the group id of this split activity and the id of the first partial activity
		//it is the maximum already existing id + 1
		int firstactivityid=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->id > firstactivityid)
				firstactivityid = act->id;
		}
		firstactivityid++;

		int cnt_act=0;
		
		bool begin=true;
		bool addSecondConstraint=false;
		double secondWeight=-1.0;
		
		for(int st=0; st<students_names.count(); st++){
			int n1=nStudentsSpinBox->value();
			int n2;
			if(n1==-1)
				n2=gt.rules.permanentStudentsHash.value(students_names.at(st))->numberOfStudents;
			else
				n2=nStudentsSpinBox->value();

			int minD=minDayDistanceSpinBox->value();
			bool tmp=gt.rules.addSplitActivityFast(this, firstactivityid, firstactivityid,
				teachers_names, subject_name, activity_tags_names, QStringList(students_names.at(st)),
				nsplit, totalduration, durations,
				active, minD, weight, forceConsecutiveCheckBox->isChecked(),
				(nStudentsSpinBox->value()==-1), n1, n2);
			if(tmp){
				firstactivityid+=nsplit;
				cnt_act++;
			}
			
			if(tmp && begin){
				begin=false;
				if(minD>1 && weight<100.0){
					SecondMinDaysDialog second(this, minD, weight);
					setParentAndOtherThings(&second, this);
					int code=second.exec();
	
					if(code==QDialog::Accepted){
						addSecondConstraint=true;
						assert(second.weight>=0 && second.weight<=100.0);
						secondWeight=second.weight;
						QList<int> acts;
						for(int i=0; i<nsplit; i++){
							acts.append(firstactivityid+i-nsplit);
						}
						TimeConstraint* c=new ConstraintMinDaysBetweenActivities(secondWeight, forceConsecutiveCheckBox->isChecked(), nsplit, acts, minD-1);
						bool tmp=gt.rules.addTimeConstraint(c);
						assert(tmp);
					}
					else
						addSecondConstraint=false;
				}
			}
			else if(tmp){
				if(addSecondConstraint){
					assert(secondWeight>=0 && secondWeight<=100.0);
					QList<int> acts;
					for(int i=0; i<nsplit; i++){
						acts.append(firstactivityid+i-nsplit);
					}
					TimeConstraint* c=new ConstraintMinDaysBetweenActivities(secondWeight, forceConsecutiveCheckBox->isChecked(), nsplit, acts, minD-1);
					bool tmp=gt.rules.addTimeConstraint(c);
					assert(tmp);
				}
			}
		}
		QMessageBox::information(this, tr("FET information"), tr("%1 split activities added").arg(cnt_act));
	}

	PlanningChanged::increasePlanningCommunicationSpinBox();
}

void AddActivityForm::clearTeachers()
{
	selectedTeachersListWidget->clear();
}

void AddActivityForm::clearStudents()
{
	selectedStudentsListWidget->clear();
}

void AddActivityForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void AddActivityForm::help()
{
	QString s;
	
	s+=tr("Abbreviations in this dialog:");
	s+="\n\n";
	s+=tr("'Students' (the text near the spin box), means 'Number of students (-1 for automatic)'");
	s+="\n";
	s+=tr("'Split' means 'Split into ... activities per week'");
	s+="\n";
	s+=tr("'Min days' means 'The minimum required distance in days between each pair of activities'");
	s+="\n";
	s+=tr("'Weight %' means 'Percentage of added constraint (min days between activities constraint). Recommended: 95.0%-100.0%'");
	s+="\n";
	s+=tr("'Consecutive' means 'If activities on same day, force consecutive?'");
	s+="\n";
	s+=tr("The 'Duration' spin box and the 'Active' check box refer to each component of current activity, you can change "
	 "them for each component, separately, by selecting the corresponding tab in the tab widget.");
	s+="\n";
	s+=tr("'Qualified' means that only the teachers who are qualified to teach the selected subject will be shown in the 'Teachers' list.",
	 "Qualified refers to teachers");
	s+="\n";
	s+=tr("'Separately' means that multiple activities will be added, one for each selected students set, separately.");
	s+="\n\n";
	
	s+=tr("You can select a teacher from all the teachers with the mouse or with the keyboard tab/up/down, then "
	 "double click it to add it to the selected teachers for current activity. "
	 "You can then choose to remove a teacher from the selected teachers. You can highlight it "
	 "with the mouse or with the keyboard, then double click it to remove this teacher from the selected teachers.");
	
	s+="\n\n";
	
	s+=tr("The same procedure (double click) applies to students sets and activity tags.");
	
	s+="\n\n";
	
	s+=tr("You can check/uncheck show years, show groups or show subgroups.");
	s+="\n\n";
	
	 s+=tr("If you split a larger activity into more activities per week, you have a multitude of choices:\n"
	 "You can choose the minimum distance in days between each pair of activities."
	 " Please note that a minimum distance of 1 means that the activities must not be in the same day, "
	 "a minimum distance of 2 means that the activities must be separated by one day (distance from Monday"
	 " to Wednesday for instance is 2 days), etc.");

	s+="\n\n";
	
	 s+=tr("If you have for instance an activity with 2 lessons per week and you want to spread them to at "
	 "least 2 days distance, you can add a constraint min days with min days = 2 and weight 95% "
	 "(or higher). If you want also to ensure that activities will "
	 "be separated by at least one day, you can use this feature: "
	 "add a constraint min days with minimum days 2 and weight 95% or lower, and after that you'll get "
	 "the possibility to add another constraint with min 1 days and weight 95% or higher. "
	 "It works if you first select in the dialog the min days >= 2 and click Add activities. Or you can add manually the constraints "
	 "(difficult this way). "
	 "Important: it is best practice to consider both constraints to have 95% weight. The combination assures that "
	 "the resultant is 99.75% weight");

	s+="\n\n";
	
	s+=tr("Please note that the min days distance is a time constraint and you can only see/modify it in the "
	 "time constraints dialogs, not in the modify activity dialog. Additionally, you can see the constraints "
	 "for each activity in the details text box of each activity");

	s+="\n\n";
	
	 s+=tr("If you choose a value greater or equal with 1 for min days, a time constraint min days between activities will be added automatically "
	 "(you can see this constraint in the time constraints list or you can see this constraint in the "
	 "detailed description of the activity). You can select a weight percentage for this constraint. "
	 "If you select 100%, the constraint must be respected all the time. If you select 95%, there is a small chance "
	 "that the timetable will not respect this constraint. Recommended values are 95.0%-100.0% (maybe you could try "
	 "with 95%, then 99.75%, or even 100.0%, but the generation time might be larger). Generally, 99.75% might be a good value. "
	 "Note: if you put a value less than 100% and the constraint is too tough, FET is able to find that this constraint "
	 "is impossible and will break it. 99.75% might be better than 95% but possibly slower. The percentage is subjective "
	 "(if you put 95% you may get 6 soft conflicts and if you put 99.75% you may get 3 soft conflicts). "
	 "Starting with FET-5.3.6, it is possible to change this value for all constraints in one click, in constraint min days"
	 " between activities dialog.");

	s+="\n\n";

	s+=tr("There is another option, if the activities are in the same day, force consecutive activities. You can select "
	 "this option for instance if you have 5 lessons of math in 5 days, and there is no timetable which respects "
	 "fully the days separation. Then, you can set the weight percent of the min days constraint to 95% and "
	 "add consecutive if same day. You will have as results say 3 lessons with duration 1 and a 2 hours lesson in another day. "
	 "Please be careful: if the activities are on the same day, even if the constraint has 0% weight, then the activities are forced to be "
	 "consecutive.");

	s+="\n\n";

	s+=tr("The current algorithm will not schedule 3 or more activities on the same day if you add a min days between activities constraint.");

	s+="\n\n";
	
	s+=tr("Note: You cannot add 'consecutive if same day' with min days=0. If you want this, you have to add "
	 "min days at least 1 (and any weight percentage).");

	s+="\n\n";
	
	s+=tr("Starting with version 5.0.0, it is possible to add activities with no students or no teachers");

	s+="\n\n";
	
	s+=tr("If you select a number of min days above 1 (say this number is n), you will get the possibility "
	 "to add a second constraint min days between activities, with min days = n-1 and a percentage of your choice. Just click "
	 "Add activities");
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - help on adding activity(ies)"));

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

void AddActivityForm::minDaysChanged()
{
	percentageTextLabel->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
	percentageLineEdit->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
	forceConsecutiveCheckBox->setEnabled(splitSpinBox->value()>=2 && minDayDistanceSpinBox->value()>0);
}
