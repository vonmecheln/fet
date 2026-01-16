/***************************************************************************
                          timetableviewstudentstimehorizontalform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include <QGuiApplication>

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QAbstractItemView>

#include <QBrush>
#include <QColor>

#include <QPainter>

#include <QSettings>

#include <QMessageBox>

#include <QPair>

#include <algorithm>

#include "studentsnotavailabletimestimehorizontalform.h"
#include "timeconstraint.h"

#define YES		(QString("X"))
#define NO		(QString(" "))

extern const QString COMPANY;
extern const QString PROGRAM;

extern const int MINIMUM_WIDTH_SPIN_BOX_VALUE;
extern const int MINIMUM_HEIGHT_SPIN_BOX_VALUE;

void StudentsNotAvailableTimesTimeHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	//int day=index.column()/gt.rules.nHoursPerDay;
	//int hour=index.column()%gt.rules.nHoursPerDay;
	int hour=index.column()%nColumns;

	/*if(day>=0 && day<gt.rules.nDaysPerWeek-1 && hour==gt.rules.nHoursPerDay-1){
		QPen pen(painter->pen());
		pen.setWidth(2);
		painter->setPen(pen);
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
	}*/

	/*assert(table!=nullptr);
	QBrush bg(table->item(index.row(), index.column())->background());
	QPen pen(painter->pen());

	double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
	if (brightness<0.5)
		pen.setColor(Qt::white);
	else
		pen.setColor(Qt::black);

	painter->setPen(pen);*/

	if(QGuiApplication::isLeftToRight()){
		if(hour==0){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}
		if(hour==nColumns-1){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}

		if(index.row()==0){
			painter->drawLine(option.rect.topLeft(), option.rect.topRight());
			painter->drawLine(option.rect.topLeft().x(), option.rect.topLeft().y()+1, option.rect.topRight().x(), option.rect.topRight().y()+1);
		}
		if(index.row()==nRows-1){
			painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
			painter->drawLine(option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1, option.rect.bottomRight().x(), option.rect.bottomRight().y()-1);
		}
	}
	else if(QGuiApplication::isRightToLeft()){
		if(hour==0){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}
		if(hour==nColumns-1){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}

		if(index.row()==0){
			painter->drawLine(option.rect.topRight(), option.rect.topLeft());
			painter->drawLine(option.rect.topRight().x(), option.rect.topRight().y()+1, option.rect.topLeft().x(), option.rect.topLeft().y()+1);
		}
		if(index.row()==nRows-1){
			painter->drawLine(option.rect.bottomRight(), option.rect.bottomLeft());
			painter->drawLine(option.rect.bottomRight().x(), option.rect.bottomRight().y()-1, option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1);
		}
	}
	//I think we should not do an 'else {assert(0);}' here, because the layout might be unspecified, according to Qt documentation.
}

StudentsNotAvailableTimesTimeHorizontalForm::StudentsNotAvailableTimesTimeHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);

	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	teachersComboBox->addItem(QString(""));
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teachersComboBox->addItem(tch->name);

	teachersComboBox->setCurrentIndex(0);
	
	populateStudentsComboBox(studentsComboBox, QString(""), true);

	studentsComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem(QString(""));
	for(Subject* sbj : std::as_const(gt.rules.subjectsList))
		subjectsComboBox->addItem(sbj->name);

	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem(QString(""));
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);

	activityTagsComboBox->setCurrentIndex(0);

	QSettings settings(COMPANY, PROGRAM);

	if(settings.contains(this->metaObject()->className()+QString("/use-filter")))
		filterCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-filter")).toBool());
	else
		filterCheckBox->setChecked(false);

	if(settings.contains(this->metaObject()->className()+QString("/show-related")))
		showRelatedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-related")).toBool());
	else
		showRelatedCheckBox->setChecked(false);
	
	tableViewSetHighlightHeader(naTableWidget);

	naTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QSet<QString> ts;
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
		assert(!ts.contains(sty->name));
		ts.insert(sty->name);
		allStudentsNames.append(sty->name);
		allStudentsType.append(STUDENTS_YEAR);
		
		for(StudentsGroup* stg : std::as_const(sty->groupsList)){
			if(!ts.contains(stg->name)){
				ts.insert(stg->name);
				allStudentsNames.append(stg->name);
				allStudentsType.append(STUDENTS_GROUP);
			}
			
			for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
				if(!ts.contains(sts->name)){
					ts.insert(sts->name);
					allStudentsNames.append(sts->name);
					allStudentsType.append(STUDENTS_SUBGROUP);
				}
			}
		}
	}

	snaMatrix.resize(allStudentsNames.count(), gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	inactiveConstraint.resize(allStudentsNames.count());
	for(int s=0; s<allStudentsNames.count(); s++){
		inactiveConstraint[s]=false;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				snaMatrix[s][d][h]=false;

		QSet<ConstraintStudentsSetNotAvailableTimes*> stc=gt.rules.ssnatHash.value(allStudentsNames.at(s), QSet<ConstraintStudentsSetNotAvailableTimes*>());
		assert(stc.count()<=1);
		if(!stc.isEmpty()){
			ConstraintStudentsSetNotAvailableTimes* ctr=*stc.constBegin();
			for(int i=0; i<ctr->days.count(); i++){
				int d=ctr->days.at(i);
				int h=ctr->hours.at(i);
				snaMatrix[s][d][h]=true;
			}
			if(ctr->active==false)
				inactiveConstraint[s]=true;
		}
	}

	connect(buttonBox, &QDialogButtonBox::accepted, this, &StudentsNotAvailableTimesTimeHorizontalForm::ok);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &StudentsNotAvailableTimesTimeHorizontalForm::cancel);
	connect(naTableWidget, &QTableWidget::itemClicked, this, &StudentsNotAvailableTimesTimeHorizontalForm::itemClicked);
	connect(selectedPushButton, &QPushButton::clicked, this, &StudentsNotAvailableTimesTimeHorizontalForm::selectedClicked);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	naTableWidget->setRowCount(allStudentsNames.count());
	naTableWidget->setColumnCount(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	naTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	naTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
	naTableWidget->verticalHeader()->setResizeMode(QHeaderView::Interactive);
	naTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif

	oldItemDelegate=naTableWidget->itemDelegate();
	newItemDelegate=new StudentsNotAvailableTimesTimeHorizontalDelegate(nullptr, naTableWidget->rowCount(), gt.rules.nHoursPerDay);
	naTableWidget->setItemDelegate(newItemDelegate);

	bool min2letters=false;
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		if(gt.rules.daysOfTheWeek[d].size()>gt.rules.nHoursPerDay){
			min2letters=true;
			break;
		}
	}

	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		QString dayName=gt.rules.daysOfTheWeek[d];
		int t=dayName.size();
		int q=t/gt.rules.nHoursPerDay;
		int r=t%gt.rules.nHoursPerDay;
		QStringList list;
		
		if(q==0)
			q=1;
		
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			if(!min2letters){
				list.append(dayName.left(1));
				dayName.remove(0, 1);
			}
			else if(i<r || q<=1){
				assert(q>=1);
				list.append(dayName.left(q+1));
				dayName.remove(0, q+1);
			}
			else{
				list.append(dayName.left(q));
				dayName.remove(0, q);
			}
		}
	
		for(int h=0; h<gt.rules.nHoursPerDay; h++){
			QTableWidgetItem* item=new QTableWidgetItem(list.at(h)+"\n"+gt.rules.hoursOfTheDay[h]);
			item->setToolTip(gt.rules.daysOfTheWeek[d]+"\n"+gt.rules.hoursOfTheDay[h]);
			naTableWidget->setHorizontalHeaderItem(d*gt.rules.nHoursPerDay+h, item);
		}
	}
	for(int s=0; s<allStudentsNames.count(); s++){
		QTableWidgetItem* item=new QTableWidgetItem(allStudentsNames.at(s));
		item->setToolTip(allStudentsNames.at(s));
		naTableWidget->setVerticalHeaderItem(s, item);
	}

	if(settings.contains(this->metaObject()->className()+QString("/use-colors")))
		colorsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-colors")).toBool());
	else
		colorsCheckBox->setChecked(false);

	for(int s=0; s<allStudentsNames.count(); s++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				QTableWidgetItem* item;
				if(snaMatrix[s][d][h])
					item=new QTableWidgetItem(YES);
				else
					item=new QTableWidgetItem(NO);
				naTableWidget->setItem(s, d*gt.rules.nHoursPerDay+h, item); //before colorItem(item) below, so that we know the row
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
					item->setToolTip(allStudentsNames.at(s)+QString("\n")+gt.rules.daysOfTheWeek[d]+QString("\n")+gt.rules.hoursOfTheDay[h]);

				if(inactiveConstraint[s]){
					QFont font=item->font();
					font.setItalic(true);
					item->setFont(font);
				}

				colorItem(item);
			}
		}
	}

	initialRecommendedHeight=naTableWidget->verticalHeader()->sectionSizeHint(0);

	int h;
	int w;

	if(settings.contains(this->metaObject()->className()+QString("/vertical-header-size"))){
		h=settings.value(this->metaObject()->className()+QString("/vertical-header-size")).toInt();
		if(h==0)
			h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
	else{
		h=MINIMUM_HEIGHT_SPIN_BOX_VALUE;
	}
//	if(h==0)
//		h=initialRecommendedHeight;

	if(settings.contains(this->metaObject()->className()+QString("/horizontal-header-size"))){
		w=settings.value(this->metaObject()->className()+QString("/horizontal-header-size")).toInt();
		if(w==0)
			w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
	else{
		w=MINIMUM_WIDTH_SPIN_BOX_VALUE;
	}
//	if(w==0)
//		w=2*initialRecommendedHeight;

	widthSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	widthSpinBox->setMinimum(MINIMUM_WIDTH_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	widthSpinBox->setMaximum(naTableWidget->verticalHeader()->maximumSectionSize());
#else
	widthSpinBox->setMaximum(maxScreenWidth(this));
#endif
	widthSpinBox->setValue(w);
	widthSpinBox->setSpecialValueText(tr("Automatic", "Automatic mode of selection of the width"));

	heightSpinBox->setSuffix(QString(" ")+tr("px", "Abbreviation for pixels"));
	heightSpinBox->setMinimum(MINIMUM_HEIGHT_SPIN_BOX_VALUE);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	heightSpinBox->setMaximum(naTableWidget->verticalHeader()->maximumSectionSize());
#else
	heightSpinBox->setMaximum(maxScreenWidth(this));
#endif
	heightSpinBox->setValue(h);
	heightSpinBox->setSpecialValueText(tr("Automatic", "Automatic mode of selection of the height"));

	naTableWidget->horizontalHeader()->setMinimumSectionSize(MINIMUM_WIDTH_SPIN_BOX_VALUE);
	naTableWidget->verticalHeader()->setMinimumSectionSize(MINIMUM_HEIGHT_SPIN_BOX_VALUE);

	widthSpinBoxValueChanged();
	heightSpinBoxValueChanged();

	connect(widthSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::widthSpinBoxValueChanged);
	connect(heightSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::heightSpinBoxValueChanged);

	if(settings.contains(this->metaObject()->className()+QString("/years-check-box-state"))){
		bool ycb=settings.value(this->metaObject()->className()+QString("/years-check-box-state")).toBool();
		yearsCheckBox->setChecked(ycb);
	}
	else{
		yearsCheckBox->setChecked(true);
	}
	if(settings.contains(this->metaObject()->className()+QString("/groups-check-box-state"))){
		bool gcb=settings.value(this->metaObject()->className()+QString("/groups-check-box-state")).toBool();
		groupsCheckBox->setChecked(gcb);
	}
	else{
		groupsCheckBox->setChecked(true);
	}
	if(settings.contains(this->metaObject()->className()+QString("/subgroups-check-box-state"))){
		bool scb=settings.value(this->metaObject()->className()+QString("/subgroups-check-box-state")).toBool();
		subgroupsCheckBox->setChecked(scb);
	}
	else{
		subgroupsCheckBox->setChecked(false);
	}

	checkBoxesChanged();

	connect(yearsCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged);
	connect(groupsCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged);
	connect(subgroupsCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged);

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::checkBoxesChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::checkBoxesChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &StudentsNotAvailableTimesTimeHorizontalForm::checkBoxesChanged);

	connect(filterCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::filterCheckBoxToggled);

	connect(showRelatedCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged);

	connect(colorsCheckBox, &QCheckBox::toggled, this, &StudentsNotAvailableTimesTimeHorizontalForm::colorsCheckBoxToggled);

	filterCheckBoxToggled();
}

StudentsNotAvailableTimesTimeHorizontalForm::~StudentsNotAvailableTimesTimeHorizontalForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/use-filter"), filterCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/show-related"), showRelatedCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/use-colors"), colorsCheckBox->isChecked());

	if(heightSpinBox->value()<=MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/vertical-header-size"), heightSpinBox->value());

	if(widthSpinBox->value()<=MINIMUM_WIDTH_SPIN_BOX_VALUE)
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), 0);
	else
		settings.setValue(this->metaObject()->className()+QString("/horizontal-header-size"), widthSpinBox->value());

	settings.setValue(this->metaObject()->className()+QString("/years-check-box-state"), yearsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/groups-check-box-state"), groupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/subgroups-check-box-state"), subgroupsCheckBox->isChecked());

	naTableWidget->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
}

void StudentsNotAvailableTimesTimeHorizontalForm::studentsFilterChanged()
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
	
	checkBoxesChanged();
}

bool StudentsNotAvailableTimesTimeHorizontalForm::filterOk(const QString& stName)
{
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(act->studentsNames.contains(stName)){
			bool t=true;
			
			if(t && teachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(teachersComboBox->currentText()))
				t=false;
			if(t && subjectsComboBox->currentText()!=QString("") && subjectsComboBox->currentText()!=act->subjectName)
				t=false;
			if(t && activityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(activityTagsComboBox->currentText()))
				t=false;
			if(t && studentsComboBox->currentText()!=QString(""))
				if(!showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
					t=false;
			
			if(t)
				return true;
		}
	}
	
	return false;
}

void StudentsNotAvailableTimesTimeHorizontalForm::colorItem(QTableWidgetItem* item)
{
	assert(item->row()>=0 && item->row()<allStudentsNames.count());
	if(colorsCheckBox->isChecked()){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		if(!inactiveConstraint[item->row()]){
			if(item->text()==NO)
				item->setBackground(QBrush(QColorConstants::DarkGreen));
			else
				item->setBackground(QBrush(QColorConstants::DarkRed));
			item->setForeground(QBrush(QColorConstants::LightGray));
		}
		else{
			if(item->text()==NO)
				item->setBackground(QBrush(QColorConstants::LightGray));
			else
				item->setBackground(QBrush(QColorConstants::DarkGray));
			item->setForeground(QBrush(QColorConstants::Gray));
		}
#else
		if(!inactiveConstraint[item->row()]){
			if(item->text()==NO)
				item->setBackground(QBrush(Qt::darkGreen));
			else
				item->setBackground(QBrush(Qt::darkRed));
			item->setForeground(QBrush(Qt::lightGray));
		}
		else{
			if(item->text()==NO)
				item->setBackground(QBrush(Qt::lightGray));
			else
				item->setBackground(QBrush(Qt::darkGray));
			item->setForeground(QBrush(Qt::gray));
		}
#endif
	}
	else{
		if(!inactiveConstraint[item->row()]){
			item->setBackground(this->palette().base());
			item->setForeground(this->palette().text());
		}
		else{
			item->setBackground(this->palette().shadow());
			item->setForeground(this->palette().light());
		}
	}
}

void StudentsNotAvailableTimesTimeHorizontalForm::itemClicked(QTableWidgetItem* item)
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
}

void StudentsNotAvailableTimesTimeHorizontalForm::widthSpinBoxValueChanged()
{
	if(widthSpinBox->value()==MINIMUM_WIDTH_SPIN_BOX_VALUE)
		naTableWidget->horizontalHeader()->setDefaultSectionSize(2*initialRecommendedHeight);
	else
		naTableWidget->horizontalHeader()->setDefaultSectionSize(widthSpinBox->value());
}

void StudentsNotAvailableTimesTimeHorizontalForm::heightSpinBoxValueChanged()
{
	if(heightSpinBox->value()==MINIMUM_HEIGHT_SPIN_BOX_VALUE)
		naTableWidget->verticalHeader()->setDefaultSectionSize(initialRecommendedHeight);
	else
		naTableWidget->verticalHeader()->setDefaultSectionSize(heightSpinBox->value());
}

void StudentsNotAvailableTimesTimeHorizontalForm::checkBoxesChanged()
{
	assert(allStudentsNames.count()==allStudentsType.count());
	if(filterCheckBox->isChecked()==false){
		for(int s=0; s<allStudentsNames.count(); s++){
			if(allStudentsType.at(s)==STUDENTS_YEAR)
				naTableWidget->setRowHidden(s, !yearsCheckBox->isChecked());
			else if(allStudentsType.at(s)==STUDENTS_GROUP)
				naTableWidget->setRowHidden(s, !groupsCheckBox->isChecked());
			else if(allStudentsType.at(s)==STUDENTS_SUBGROUP)
				naTableWidget->setRowHidden(s, !subgroupsCheckBox->isChecked());
		}
	}
	else{
		for(int s=0; s<allStudentsNames.count(); s++){
			QString stName=allStudentsNames.at(s);
			int stType=allStudentsType.at(s);
			
			if(stType==STUDENTS_YEAR && yearsCheckBox->isChecked()){
				if(filterOk(stName))
					naTableWidget->setRowHidden(s, false);
				else
					naTableWidget->setRowHidden(s, true);
			}
			else if(stType==STUDENTS_GROUP && groupsCheckBox->isChecked()){
				if(filterOk(stName))
					naTableWidget->setRowHidden(s, false);
				else
					naTableWidget->setRowHidden(s, true);
			}
			else if(stType==STUDENTS_SUBGROUP && subgroupsCheckBox->isChecked()){
				if(filterOk(stName))
					naTableWidget->setRowHidden(s, false);
				else
					naTableWidget->setRowHidden(s, true);
			}
			else{
				naTableWidget->setRowHidden(s, true);
			}
		}
	}
}

void StudentsNotAvailableTimesTimeHorizontalForm::selectedClicked()
{
	bool firstFound=false;
	bool firstIsNO;
	for(int s=0; s<allStudentsNames.count(); s++){
		if(!naTableWidget->isRowHidden(s)){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					QTableWidgetItem* it=naTableWidget->item(s, d*gt.rules.nHoursPerDay+h);
					if(it->isSelected()){
						if(!firstFound){
							firstFound=true;
							firstIsNO=(it->text()==NO);
						}
						if(firstIsNO)
							it->setText(YES);
						else
							it->setText(NO);
						colorItem(it);
					}
				}
			}
		}
	}
}

void StudentsNotAvailableTimesTimeHorizontalForm::filterCheckBoxToggled()
{
	filterGroupBox->setVisible(filterCheckBox->isChecked());

	checkBoxesChanged();
}

void StudentsNotAvailableTimesTimeHorizontalForm::colorsCheckBoxToggled()
{
	for(int s=0; s<allStudentsNames.count(); s++)
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				colorItem(naTableWidget->item(s, d*gt.rules.nHoursPerDay+h));
}

void StudentsNotAvailableTimesTimeHorizontalForm::ok()
{
	int added=0;

	int emptyRemoved=0;
	int inactiveEmptyRemoved=0;

	int modified=0;
	int inactiveModified=0;
	
	int modifiedOrder=0;
	int inactiveModifiedOrder=0;

	for(int s=0; s<allStudentsNames.count(); s++)
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				snaMatrix[s][d][h]=(naTableWidget->item(s, d*gt.rules.nHoursPerDay+h)->text()==YES);

	QList<TimeConstraint*> tl;
	
	QStringList addedNames;
	QStringList emptyRemovedNames;
	QStringList modifiedNames;
	QStringList modifiedOrderNames;

	for(int s=0; s<allStudentsNames.count(); s++){
		QList<int> daysList;
		QList<int> hoursList;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(snaMatrix[s][d][h]){
					daysList.append(d);
					hoursList.append(h);
				}
			}
		}

		QSet<ConstraintStudentsSetNotAvailableTimes*> stc=gt.rules.ssnatHash.value(allStudentsNames.at(s), QSet<ConstraintStudentsSetNotAvailableTimes*>());
		assert(stc.count()<=1);
		if(!stc.isEmpty()){
			ConstraintStudentsSetNotAvailableTimes* ctr=*stc.constBegin();

			if(!daysList.isEmpty()){
				QList<QPair<int, int>> lold;
				for(int i=0; i<ctr->days.count(); i++)
					lold.append(QPair<int, int>(ctr->days.at(i), ctr->hours.at(i)));

				QList<QPair<int, int>> lnew;
				for(int i=0; i<daysList.count(); i++)
					lnew.append(QPair<int, int>(daysList.at(i), hoursList.at(i)));

				if(lold!=lnew){
					modifiedOrder++;
					if(!ctr->active)
						inactiveModifiedOrder++;
				}

				std::sort(lold.begin(), lold.end());
				std::sort(lnew.begin(), lnew.end());
				
				if(lold!=lnew){
					modified++;
					modifiedOrder--;
					assert(modifiedOrder>=0);

					if(!ctr->active){
						inactiveModified++;
						inactiveModifiedOrder--;
						assert(inactiveModifiedOrder>=0);
					}
					
					modifiedNames.append(allStudentsNames.at(s));
				}
				else{
					modifiedOrderNames.append(allStudentsNames.at(s));
				}

				ctr->days=daysList;
				ctr->hours=hoursList;
			}
			else{
				tl.append(ctr);
				
				emptyRemoved++;
				if(!ctr->active)
					inactiveEmptyRemoved++;

				emptyRemovedNames.append(allStudentsNames.at(s));
			}
		}
		else{
			if(!daysList.isEmpty()){
				ConstraintStudentsSetNotAvailableTimes* ctr=new ConstraintStudentsSetNotAvailableTimes(100, allStudentsNames.at(s), daysList, hoursList);
				bool tt=gt.rules.addTimeConstraint(ctr);
				assert(tt);
				
				added++;
				
				addedNames.append(allStudentsNames.at(s));
			}
		}
	}

	if(!tl.isEmpty())
		gt.rules.removeTimeConstraints(tl);

	if(added>0 || modified>0 || modifiedOrder>0 || emptyRemoved>0){
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		QString addedS;
		QString modifiedS;
		QString modifiedOrderS;
		QString emptyRemovedS;

		if(FET_LANGUAGE=="en_US"){
			if(added>0){
				if(added==1)
					addedS=QString("1 new constraint was added");
				else
					addedS=QString("%1 new constraints were added").arg(added);

				addedS+=QString(".\n");
			}

			if(modified>0){
				if(modified==1)
					modifiedS=QString("1 constraint was modified");
				else
					modifiedS=QString("%1 constraints were modified").arg(modified);

				if(inactiveModified>0){
					modifiedS+=QString(" (");
					if(inactiveModified==1){
						if(modified==1)
							modifiedS+=QString("it is inactive");
						else
							modifiedS+=QString("1 of them is inactive");
					}
					else if(inactiveModified<modified){
						assert(modified>=2);
						modifiedS+=QString("%1 of them are inactive").arg(inactiveModified);
					}
					else{
						assert(inactiveModified==modified);
						assert(modified>=2);
						modifiedS+=QString("they are inactive");
					}
					modifiedS+=QString(")");
				}

				modifiedS+=QString(".\n");
			}

			if(modifiedOrder>0){
				if(modifiedOrder==1)
					modifiedOrderS=QString("1 constraint was left with the same set of selected not available time slots, but the program reordered the list of these slots");
				else
					modifiedOrderS=QString("%1 constraints were left with the same set of selected not available time slots, but the program reordered the list of these slots").arg(modifiedOrder);

				if(inactiveModifiedOrder>0){
					modifiedOrderS+=QString(" (");
					if(inactiveModifiedOrder==1){
						if(modifiedOrder==1)
							modifiedOrderS+=QString("it is inactive");
						else
							modifiedOrderS+=QString("1 of them is inactive");
					}
					else if(inactiveModifiedOrder<modifiedOrder){
						assert(modifiedOrder>=2);
						modifiedOrderS+=QString("%1 of them are inactive").arg(inactiveModifiedOrder);
					}
					else{
						assert(inactiveModifiedOrder==modifiedOrder);
						assert(modifiedOrder>=2);
						modifiedOrderS+=QString("they are inactive");
					}
					modifiedOrderS+=QString(")");
				}

				modifiedOrderS+=QString(".\n");
			}

			if(emptyRemoved>0){
				if(emptyRemoved==1)
					emptyRemovedS=QString("1 constraint was removed, because it had zero selected not available time slots");
				else
					emptyRemovedS=QString("%1 constraints were removed, because they had zero selected not available time slots").arg(emptyRemoved);

				if(inactiveEmptyRemoved>0){
					emptyRemovedS+=QString(" (");
					if(inactiveEmptyRemoved==1){
						if(emptyRemoved==1)
							emptyRemovedS+=QString("it was inactive");
						else
							emptyRemovedS+=QString("1 of them was inactive");
					}
					else if(inactiveEmptyRemoved<emptyRemoved){
						assert(emptyRemoved>=2);
						emptyRemovedS+=QString("%1 of them were inactive").arg(inactiveEmptyRemoved);
					}
					else{
						assert(inactiveEmptyRemoved==emptyRemoved);
						assert(emptyRemoved>=2);
						emptyRemovedS+=QString("they were inactive").arg(inactiveEmptyRemoved);
					}
					emptyRemovedS+=QString(")");
				}

				emptyRemovedS+=QString(".\n");
			}
		}
		else{
			if(added>0){
				addedS=tr("%n new constraint(s) were added",
				  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
				  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
				  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
				  " (open these files with Qt Linguist and see the translation of this field).",
				  added);
				addedS+=QString(".\n");
			}

			if(modified>0){
				modifiedS=tr("%n constraint(s) were modified",
				  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
				  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
				  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
				  " (open these files with Qt Linguist and see the translation of this field).",
				  modified);

				if(inactiveModified>0){
					modifiedS+=QString(" (");
					if(inactiveModified==1 && modified==1){
						modifiedS+=tr("it is inactive", "It refers to a constraint.");
					}
					else if(inactiveModified<modified){
						assert(modified>=2);
						modifiedS+=tr("%n of them are inactive",
						  "It refers to constraints."
						  " "
						  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
						  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
						  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
						  " (open these files with Qt Linguist and see the translation of this field).",
						  inactiveModified);
					}
					else{
						assert(inactiveModified==modified);
						assert(modified>=2);
						modifiedS+=tr("they are inactive", "It refers to constraints.");
					}
					modifiedS+=QString(")");
				}

				modifiedS+=QString(".\n");
			}

			if(modifiedOrder>0){
				modifiedOrderS=tr("%n constraint(s) were left with the same set of selected not available time slots, but the program reordered the list of these slots",
				  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
				  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
				  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
				  " (open these files with Qt Linguist and see the translation of this field).",
				  modifiedOrder);

				if(inactiveModifiedOrder>0){
					modifiedOrderS+=QString(" (");
					if(inactiveModifiedOrder==1 && modifiedOrder==1){
						modifiedOrderS+=tr("it is inactive", "It refers to a constraint.");
					}
					else if(inactiveModifiedOrder<modifiedOrder){
						assert(modifiedOrder>=2);
						modifiedOrderS+=tr("%n of them are inactive",
						  "It refers to constraints."
						  " "
						  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
						  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
						  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
						  " (open these files with Qt Linguist and see the translation of this field).",
						  inactiveModifiedOrder);
					}
					else{
						assert(inactiveModifiedOrder==modifiedOrder);
						assert(modifiedOrder>=2);
						modifiedOrderS+=tr("they are inactive", "It refers to constraints.");
					}
					modifiedOrderS+=QString(")");
				}

				modifiedOrderS+=QString(".\n");
			}

			if(emptyRemoved>0){
				emptyRemovedS=tr("%n constraint(s) were removed, because they had zero selected not available time slots",
				  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
				  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
				  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
				  " (open these files with Qt Linguist and see the translation of this field).",
				  emptyRemoved);

				if(inactiveEmptyRemoved>0){
					emptyRemovedS+=QString(" (");
					if(inactiveEmptyRemoved==1 && emptyRemoved==1){
						emptyRemovedS+=tr("it was inactive", "It refers to a constraint.");
					}
					else if(inactiveEmptyRemoved<emptyRemoved){
						assert(emptyRemoved>=2);
						emptyRemovedS+=tr("%n of them were inactive",
						  "It refers to constraints."
						  " "
						  "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
						  " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
						  " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
						  " (open these files with Qt Linguist and see the translation of this field).",
						  inactiveEmptyRemoved);
					}
					else{
						assert(inactiveEmptyRemoved==emptyRemoved);
						assert(emptyRemoved>=2);
						emptyRemovedS+=tr("they were inactive", "It refers to constraints.");
					}
					emptyRemovedS+=QString(")");
				}

				emptyRemovedS+=QString(".\n");
			}
		}

		QString s=tr("Details about the operations which were done on the list of constraints of type 'students set not available time slots':");
		s+="\n\n";
		if(added>0)
			s+=addedS;
		if(modified>0)
			s+=modifiedS;
		if(modifiedOrder>0)
			s+=modifiedOrderS;
		if(emptyRemoved>0)
			s+=emptyRemovedS;
		s.chop(1);
		QMessageBox::information(this, tr("FET information"), s);
		
		QString s2;
		if(added>0){
			s2+=tr("Added constraints for these %1 students sets: %2.",
			 "%1 is the number of affected students sets, %2 is the list of their names").arg(added).arg(addedNames.join(translatedCommaSpace()));
			s2+=QString("\n");
		}
		if(modified>0){
			s2+=tr("Modified constraints for these %1 students sets: %2.",
			 "%1 is the number of affected students sets, %2 is the list of their names").arg(modified).arg(modifiedNames.join(translatedCommaSpace()));
			s2+=QString("\n");
		}
		if(modifiedOrder>0){
			s2+=tr("Modified order of the slots in the constraints for these %1 students sets: %2.",
			 "%1 is the number of affected students sets, %2 is the list of their names").arg(modifiedOrder).arg(modifiedOrderNames.join(translatedCommaSpace()));
			s2+=QString("\n");
		}
		if(emptyRemoved>0){
			s2+=tr("Removed constraints for these %1 students sets: %2.",
			 "%1 is the number of affected students sets, %2 is the list of their names").arg(emptyRemoved).arg(emptyRemovedNames.join(translatedCommaSpace()));
			s2+=QString("\n");
		}

		gt.rules.addUndoPoint(tr("Modified the students' not available times constraints in the time horizontal view.")+QString("\n\n")+s+QString("\n\n")+s2);
	}

	this->close();
}

void StudentsNotAvailableTimesTimeHorizontalForm::cancel()
{
	this->close();
}
