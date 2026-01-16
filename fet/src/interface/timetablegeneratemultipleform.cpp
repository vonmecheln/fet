/***************************************************************************
                          timetablegeneratemultipleform.cpp  -  description
                             -------------------
    begin                : Aug 20, 2007
    copyright            : (C) 2007 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "longtextmessagebox.h"

#include "generate.h"

#include "timetablegeneratemultipleform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include "rules.h"

#include <ctime>

#include <algorithm> //for std::max

#include <QMessageBox>

//#include <QMutex>

#include <QScrollBar>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QFrame>

#include <QDir>

#include <QApplication>
#include <QCoreApplication>
#include <QtGlobal>

#include <QSizePolicy>

#include <QProcess>
//#include <QTimer>

#include <QDate>
#include <QTime>
#include <QLocale>
#include <QString>

#include <QSemaphore>

#include <QSettings>

#include <QFileInfo>

#include <QFont>

#include <QThread> //only for QThread::idealThreadCount()

#include <mutex>
//#include <condition_variable>

//extern QMutex myMutex;

//Old comment below:
//The 'static' qualifiers were commented out below (in 8 places) to address possible crashes on some platforms.
//static QMutex fitnessMutexInThreads;
static std::mutex fitnessMutexInThreads;

extern const QString COMPANY;
extern const QString PROGRAM;

extern Generate gen;

extern QString initialOrderOfActivities;

//static Matrix1D<Worker> generateMultipleWorker;
//static Matrix1D<QThread> generateMultipleThread;

static int allNThreads;

static Matrix1D<QSemaphore> semaphoreTimetableStarted;
static Matrix1D<QSemaphore> semaphoreTimetableFinished;

//static QSemaphore semaphoreRestarted;

/*static Matrix1D<std::binary_semaphore> semaphoreTimetableStarted;
static Matrix1D<std::binary_semaphore> semaphoreTimetableFinished;*/
/*static Matrix1D<std::condition_variable> cvTimetableStarted;
static Matrix1D<std::condition_variable> cvTimetableFinished;*/

//static QSemaphore semaphoreTimetableFinished;

//static QSemaphore semaphoreTimetableStarted;

//Represents the current status of the generation - running or stopped.
extern bool generation_running_multi;

//extern QSemaphore semaphorePlacedActivity;

static Matrix1D<Generate> genMultiMatrix;
//Generate genMulti;

static int nTimetables;
static int timeLimit;

extern Solution best_solution;

extern QString conflictsStringTitle;
extern QString conflictsString;

static Matrix1D<time_t> process_start_time;

static Matrix1D<TimetablingThread> timetablingThreads;

const QString settingsNameMultiple=QString("TimetableGenerateMultipleInitialOrderForm");

void TimetablingThread::startGenerating()
{
	//time_t start_time;

	assert(genMultiMatrix[_nThread].abortOptimization==false);
	assert(genMultiMatrix[_nThread].restart==false);
	assert(genMultiMatrix[_nThread].paused==false);
	assert(genMultiMatrix[_nThread].pausedTime==0);
	
	//time(&start_time);
	time(&process_start_time[_nThread]);

	bool restarted;
	bool impossible;
	bool timeExceeded;

	//emit timetableStarted(_nThread/*, nOverallTimetable+1*/);
	//semaphoreTimetableStarted[_nThread].acquire();

	genMultiMatrix[_nThread].semaphorePlacedActivity.tryAcquire(); //this has effect after forcingly stopping the generation on this thread
	assert(genMultiMatrix[_nThread].semaphorePlacedActivity.available()==0);
	assert(genMultiMatrix[_nThread].semaphoreFinished.available()==0);
	genMultiMatrix[_nThread].generateWithSemaphore(timeLimit, restarted, impossible, timeExceeded, true); //true means threaded
	//genMultiMatrix[_nThread].myMutex.lock();
	if(genMultiMatrix[_nThread].abortOptimization){
		//genMultiMatrix[_nThread].myMutex.unlock();
		//cout<<"returning, because abortOptimization of _nThread="<<_nThread<<endl;
		return;
	}

	QString s;
	
	bool ok;

	if(allNThreads>=2)
		s=tr("(Thread %1)").arg(_nThread+1)+QString(" ");
	else
		s=QString("");

	if(restarted){
		s+=tr("Generation was stopped by user");

		////////2011-05-26
		int mact=genMultiMatrix[_nThread].maxActivitiesPlaced;
		int mseconds=genMultiMatrix[_nThread].timeToHighestStage;

		bool zero=false;
		if(mseconds==0)
			zero=true;
		int hh=mseconds/3600;
		mseconds%=3600;
		int mm=mseconds/60;
		mseconds%=60;
		int ss=mseconds;
		QString tim;
		if(hh>0){
			tim+=" ";
			tim+=tr("%1 h", "hours").arg(hh);
		}
		if(mm>0){
			tim+=" ";
			tim+=tr("%1 m", "minutes").arg(mm);
		}
		if(ss>0 || zero){
			tim+=" ";
			tim+=tr("%1 s", "seconds").arg(ss);
		}
		tim.remove(0, 1);
		s+=QString(". ");
		s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);
		///////

		s+=translatedDot();

		ok=false;
	}
	else if(impossible){
		s+=tr("Timetable impossible to generate");
		s+=translatedDot();
		ok=false;
	}
	else if(timeExceeded){
		s+=tr("Time exceeded for current timetable");

		////////2011-05-26
		int mact=genMultiMatrix[_nThread].maxActivitiesPlaced;
		int mseconds=genMultiMatrix[_nThread].timeToHighestStage;

		bool zero=false;
		if(mseconds==0)
			zero=true;
		int hh=mseconds/3600;
		mseconds%=3600;
		int mm=mseconds/60;
		mseconds%=60;
		int ss=mseconds;
		QString tim;
		if(hh>0){
			tim+=" ";
			tim+=tr("%1 h", "hours").arg(hh);
		}
		if(mm>0){
			tim+=" ";
			tim+=tr("%1 m", "minutes").arg(mm);
		}
		if(ss>0 || zero){
			tim+=" ";
			tim+=tr("%1 s", "seconds").arg(ss);
		}
		tim.remove(0, 1);
		s+=QString(". ");
		s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);
		///////

		s+=translatedDot();

		ok=false;
	}
	else{
		ok=true;
		
		time_t finish_time;
		time(&finish_time);
		int seconds=int(difftime(finish_time, process_start_time[_nThread]))-genMultiMatrix[_nThread].pausedTime;
		if(seconds<0)
			seconds=0;
		int hours=seconds/3600;
		seconds%=3600;
		int minutes=seconds/60;
		seconds%=60;
		
		FakeString tmp;
		fitnessMutexInThreads.lock();
		genMultiMatrix[_nThread].c.fitness(gt.rules, &tmp);
		fitnessMutexInThreads.unlock();
		
		s+=tr("Timetable breaks %1 soft constraints, has %2 soft conflicts total, and was generated in %3 hours, %4 minutes and %5 seconds.")
		 .arg(genMultiMatrix[_nThread].c.conflictsWeightList.count())
		 .arg(CustomFETString::numberPlusTwoDigitsPrecision(genMultiMatrix[_nThread].c.conflictsTotal))
		 .arg(hours)
		 .arg(minutes)
		 .arg(seconds);
	}
	//genMultiMatrix[_nThread].myMutex.unlock();
	//emit resultReady(_nThread, s, ok);

	Q_EMIT timetableGenerated(_nThread, nOverallTimetable+1, s, ok);

	//if(restarted)
	//	semaphoreRestarted.release();

	semaphoreTimetableFinished[_nThread].acquire();

	/*std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);
	cvTimetableFinished[_nThread].wait(lck);*/
}

/*Controller::Controller()
{
	Worker* worker=new Worker;
	worker->moveToThread(&workerThread);
	connect(&workerThread, SIG NAL(finished()), worker, SL OT(deleteLater()));
	connect(this, SIG NAL(operate(int)), worker, SL OT(doWork(int)));
	connect(worker, SIG NAL(resultReady(int, const QString&, bool)), this, SL OT(handleResults(int, const QString&, bool)));
	workerThread.start();
}

Controller::~Controller()
{
	workerThread.quit();
	workerThread.wait();
}

void Controller::handleResults(int nThread, const QString& s, bool ok)
{
	emit timetableGenerated(nThread, nOverallTimetable+1, s, ok);
	semaphoreTimetableFinished[_nThread].acquire();
}

void Controller::startOperate(int nThread)
{
	emit operate(nThread);
}*/

TimetableGenerateMultipleForm::TimetableGenerateMultipleForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	//Not necessary, this property is set in the ui file.
	//timetablesTabWidget->setUsesScrollButtons(true);

	pausedLabel->setVisible(false);
	
	QFont font(pausedLabel->font());
	font.setBold(true);
	pausedLabel->setFont(font);
	
	currentResultsTextEdit->setReadOnly(true);
	
	startPushButton->setDefault(true);

	connect(startPushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::start);
	connect(stopPushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::stop);
	connect(closePushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::closePressed);
	connect(helpPushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::help);
	connect(seeInitialOrderPushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::seeInitialOrder);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	generation_running_multi=false;
	
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	restartPushButton->setDisabled(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);
	threadsGroupBox->setEnabled(true);
	seeInitialOrderPushButton->setDisabled(true);

	labels.append(textLabel);
	pausedLabels.append(pausedLabel);

	pausePushButtons.append(pausePushButton);
	pausePushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(pausePushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::pauseCurrentThread);

	restartPushButtons.append(restartPushButton);
	restartPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//QPushButton* pb=new QPushButton(tr("Restart"), wd);
	//pb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(restartPushButton, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::restartCurrentThread);

	nThreadsSpinBox->setMinimum(1);
	nThreadsSpinBox->setMaximum(std::max(1, QThread::idealThreadCount()));
	nThreadsSpinBox->setValue(1); //this is necessary, before the connection to nThreadsChanged(int)

	connect(nThreadsSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &TimetableGenerateMultipleForm::nThreadsChanged);
	QSettings settings(COMPANY, PROGRAM);
	nThreadsSpinBox->setValue(settings.value(this->metaObject()->className()+QString("/number-of-threads"), "1").toInt()); //this is necessary, after the connection to nThreadsChanged(int)
	timetablesTabWidget->setCurrentIndex(0);

	minutesSpinBox->setValue(settings.value(this->metaObject()->className()+QString("/time-limit"), "600000").toInt());
	timetablesSpinBox->setValue(settings.value(this->metaObject()->className()+QString("/number-of-timetables"), "10").toInt());
}

TimetableGenerateMultipleForm::~TimetableGenerateMultipleForm()
{
	QSettings settings(COMPANY, PROGRAM);
	saveFETDialogGeometry(this);
	settings.setValue(this->metaObject()->className()+QString("/number-of-threads"), nThreadsSpinBox->value());

	settings.setValue(this->metaObject()->className()+QString("/time-limit"), minutesSpinBox->value());
	settings.setValue(this->metaObject()->className()+QString("/number-of-timetables"), timetablesSpinBox->value());

	if(generation_running_multi)
		this->stop();

	for(Solution* sol : std::as_const(highestStageSolutions))
		delete sol;
	highestStageSolutions.clear();
	nTimetableForHighestStageSolutions.clear();
	nThreadForHighest.clear();
	generationTimedOutForHighest.clear();
	timeForHighestStageSolutions.clear();
	
	//assert(controllersList.count()==0);
}

void TimetableGenerateMultipleForm::nThreadsChanged(int nt)
{
	int oldIndex=timetablesTabWidget->currentIndex();

	int oldN=timetablesTabWidget->count();
	if(oldN>nt){
		for(int i=oldN-1; i>=nt; i--){
			QWidget* wd=timetablesTabWidget->widget(i);
			timetablesTabWidget->removeTab(i);
			assert(labels.count()>0);
			labels.removeLast();

			assert(restartPushButtons.count()>0);
			restartPushButtons.removeLast();

			assert(pausePushButtons.count()>0);
			pausePushButtons.removeLast();

			assert(pausedLabels.count()>0);
			pausedLabels.removeLast();

			delete wd;
		}
		
		if(oldIndex<timetablesTabWidget->count())
			timetablesTabWidget->setCurrentIndex(oldIndex);
		else
			timetablesTabWidget->setCurrentIndex(timetablesTabWidget->count()-1);
	}
	else if(oldN<nt){
		for(int i=oldN; i<nt; i++){
			QWidget* wd=new QWidget(timetablesTabWidget);
			
			QLabel* plb=new QLabel(tr("[PAUSED]", "The generation is paused"), wd);
			plb->setVisible(false);
			///////
			QFont font(plb->font());
			font.setBold(true);
			plb->setFont(font);
			
			QLabel* lb=new QLabel(tr("Current timetable: 0 out of 0 activities placed, 0h 0m 0s\nMax placed activities: 0 (at 0 s)"), wd);
			
			QVBoxLayout* lbl=new QVBoxLayout;
			lbl->addWidget(plb);
			lbl->addWidget(lb);

			QPushButton* ppb=new QPushButton(tr("Pause", "Pause the generation"), wd);
			ppb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			ppb->setDisabled(true);
			connect(ppb, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::pauseCurrentThread);

			// https://stackoverflow.com/questions/10053839/how-does-designer-create-a-line-widget
			QFrame* line=new QFrame(wd);
			line->setFrameShape(QFrame::HLine);
			line->setFrameShadow(QFrame::Sunken);

			QPushButton* rpb=new QPushButton(tr("Restart", "Restart the generation (stop the current generation and begin a new different one). "
			 "Please make a clear distinction between 'Restart the generation' and 'Continue the generation'."), wd);
			rpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			rpb->setDisabled(true);
			connect(rpb, &QPushButton::clicked, this, &TimetableGenerateMultipleForm::restartCurrentThread);

			QVBoxLayout* vbl=new QVBoxLayout;
			vbl->addWidget(ppb);
			vbl->addWidget(line);
			vbl->addWidget(rpb);

			QHBoxLayout* hbl=new QHBoxLayout(wd);
			hbl->addLayout(lbl);
			hbl->addStretch();
			hbl->addLayout(vbl);
			
			timetablesTabWidget->addTab(wd, QString::number(i+1));
			
			labels.append(lb);
			pausePushButtons.append(ppb);
			restartPushButtons.append(rpb);
			pausedLabels.append(plb);
		}
		
		timetablesTabWidget->setCurrentIndex(timetablesTabWidget->count()-1);
	}
}

void TimetableGenerateMultipleForm::help()
{
	//2024-10-07
	QString destDir;
	
	if(!generation_running_multi){
		QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

		if(s2.right(4)==".fet")
			s2=s2.left(s2.length()-4);
		
		destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multiple";

		if(QFileInfo::exists(destDir)){
			int i=2;
			for(;;){
				QString CODN=destDir+"-"+QString::number(i);
				if(!QFileInfo::exists(CODN)){
					destDir=CODN;
					break;
				}
				i++;
			}
		}
	}
	else{
		destDir=MULTIPLE_OUTPUT_DIRECTORY;
	}
	////////////

	QString s=tr("You can see the generated timetables on the hard disk, in HTML and XML formats and the soft conflicts in text format,"
	 " or the latest timetable (or the latest highest-stage timetable, if no timetable was completed) in the Timetable/View menu.")
	 +"\n\n"
	 +tr("Note that, for large data, each timetable might occupy more megabytes of hard disk space, so make sure you have enough space"
	 " (you can check the dimension of a single timetable as a precaution). Each attempted timetable will correspond to a folder in %1"
	 " that contains information about the random seed that was used, but only completed timetables will contain the full set of timetable files.")
	 .arg(QDir::toNativeSeparators(destDir))
	 +"\n\n"
	 +tr("For finished timetables, there are also saved the timetables in .fet format (data + constraints to lock the timetable), so that you"
	 " can open each of them later.")
	 +"\n\n"
	 +tr("If you get an impossible timetable, please enter menu Generate (single) and see the initial order of evaluation of activities; this might help.")
	 +"\n\n"
	 +tr("You can limit the search time, by specifying the maximum number of minutes allowed to spend for each timetable (option %1).").arg("'"+tr("Limit each")+"'")
	 +" "+tr("The maximum and also the predefined value is %1 minutes, which means %2 hours, so virtually unlimited.").arg(600000).arg(10000)
	 +"\n\n"
	 +tr("Note that if you start the multiple generation with the same global seed, the timetables will be identical (if you let the generations finish).")
	 +" "+tr("The seed of the first thread will be the global seed multiplied in each component with the number of threads (modulo m1=%1 and respectively m2=%2),"
	 " for the second thread the seed will be the same as for the first thread, but +1 in each component (modulo m1 and respectively m2), for the third thread the"
	 " seed will be the same as for the first thread, but +2 in each component, and so on.")
	 .arg(gen.rng.m1).arg(gen.rng.m2)
	 +" "+tr("(If the component of a such computed seed will be all zeroes, we will add to the components of that seed the total number of threads instead"
	 " of the thread number starting from zero.)")
	 +" "+tr("This method was suggested by %1.", "%1 is a person").arg("Chichi Lalescu")
	 +" "+tr("After generating multiple, the global seed will become equal to the seed of the first thread.")
	 +"\n\n"
	 +tr("The number of threads is limited by your computer processor(s). If you have for example an 8 core/16 thread processor, the maximum allowed"
	 " number of threads is 16. In this case you can make a comparison of generation time with 8 threads or with 16 threads. If you generate on a single thread,"
	 " the speed of generation of a timetable will be in general a bit higher than that of obtaining a single timetable by generating on multiple threads,"
	 " because the processor slows down if you are using more threads, but you will obtain more timetables in a comparable time.")
	 +"\n\n"
	 +tr("WARNING: As you use more threads, the processor will be used to a greater extent and it might overheat. Also, the system might become"
	 " slow or nonreponsive.")
	 +"\n\n"
	 +tr("Note: If your file is very big, containing a very large number of teachers or total subgroups, generating on more threads might consume the"
	 " processor cache and the generation might become very slow compared to generating on a single thread.")
	 ;
	
	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void TimetableGenerateMultipleForm::start(){
	int nThreads=nThreadsSpinBox->value();
	assert(nThreads>=1);
	genMultiMatrix.resize(nThreads);
	timetablingThreads.resize(nThreads);
	process_start_time.resize(nThreads);
	//generateMultipleThread.resize(nThreads);
	//generateMultipleWorker.resize(nThreads);
	semaphoreTimetableStarted.resize(nThreads);
	semaphoreTimetableFinished.resize(nThreads);
	//cvTimetableStarted.resize(nThreads);
	//cvTimetableFinished.resize(nThreads);
	allNThreads=nThreads;

	for(Solution* sol : std::as_const(highestStageSolutions))
		delete sol;
	highestStageSolutions.clear();
	nTimetableForHighestStageSolutions.clear();
	nThreadForHighest.clear();
	generationTimedOutForHighest.clear();
	timeForHighestStageSolutions.clear();
	
	for(int t=0; t<nThreads; t++){
		/*genMultiMatrix[t].rng.s10=(gen.rng.s10*nThreads+t)%gen.rng.m1;
		genMultiMatrix[t].rng.s11=(gen.rng.s11*nThreads+t)%gen.rng.m1;
		genMultiMatrix[t].rng.s12=(gen.rng.s12*nThreads+t)%gen.rng.m1;
		if(genMultiMatrix[t].rng.s10==0 && genMultiMatrix[t].rng.s11==0 && genMultiMatrix[t].rng.s12==0){
			assert(t!=0);
			genMultiMatrix[t].rng.s10=(gen.rng.s10*nThreads+nThreads)%gen.rng.m1;
			genMultiMatrix[t].rng.s11=(gen.rng.s11*nThreads+nThreads)%gen.rng.m1;
			genMultiMatrix[t].rng.s12=(gen.rng.s12*nThreads+nThreads)%gen.rng.m1;
		}
		assert(genMultiMatrix[t].rng.s10!=0 || genMultiMatrix[t].rng.s11!=0 || genMultiMatrix[t].rng.s12!=0);

		genMultiMatrix[t].rng.s20=(gen.rng.s20*nThreads+t)%gen.rng.m2;
		genMultiMatrix[t].rng.s21=(gen.rng.s21*nThreads+t)%gen.rng.m2;
		genMultiMatrix[t].rng.s22=(gen.rng.s22*nThreads+t)%gen.rng.m2;
		if(genMultiMatrix[t].rng.s20==0 && genMultiMatrix[t].rng.s21==0 && genMultiMatrix[t].rng.s22==0){
			assert(t!=0);
			genMultiMatrix[t].rng.s20=(gen.rng.s20*nThreads+nThreads)%gen.rng.m2;
			genMultiMatrix[t].rng.s21=(gen.rng.s21*nThreads+nThreads)%gen.rng.m2;
			genMultiMatrix[t].rng.s22=(gen.rng.s22*nThreads+nThreads)%gen.rng.m2;
		}
		assert(genMultiMatrix[t].rng.s20!=0 || genMultiMatrix[t].rng.s21!=0 || genMultiMatrix[t].rng.s22!=0);*/

		//init method suggested by Chichi Lalescu
		qint64 s10=(gen.rng.s10*nThreads+t)%gen.rng.m1;
		qint64 s11=(gen.rng.s11*nThreads+t)%gen.rng.m1;
		qint64 s12=(gen.rng.s12*nThreads+t)%gen.rng.m1;
		if(s10==0 && s11==0 && s12==0){
			assert(t!=0);
			s10=(gen.rng.s10*nThreads+nThreads)%gen.rng.m1;
			s11=(gen.rng.s11*nThreads+nThreads)%gen.rng.m1;
			s12=(gen.rng.s12*nThreads+nThreads)%gen.rng.m1;
		}
		
		qint64 s20=(gen.rng.s20*nThreads+t)%gen.rng.m2;
		qint64 s21=(gen.rng.s21*nThreads+t)%gen.rng.m2;
		qint64 s22=(gen.rng.s22*nThreads+t)%gen.rng.m2;
		if(s20==0 && s21==0 && s22==0){
			assert(t!=0);
			s20=(gen.rng.s20*nThreads+nThreads)%gen.rng.m2;
			s21=(gen.rng.s21*nThreads+nThreads)%gen.rng.m2;
			s22=(gen.rng.s22*nThreads+nThreads)%gen.rng.m2;
		}
		
		genMultiMatrix[t].rng.initializeMRG32k3a(s10, s11, s12, s20, s21, s22);
	}
	
	nTimetables=timetablesSpinBox->value();
	assert(nTimetables>0);
	timeLimit=60*minutesSpinBox->value(); //seconds
	assert(timeLimit>0);

	//2024-10-07
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	MULTIPLE_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multiple";

	if(QFileInfo::exists(MULTIPLE_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=MULTIPLE_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				MULTIPLE_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}
	////////////

	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure(this)){
			QMessageBox::warning(this, TimetableGenerateMultipleForm::tr("FET warning"), TimetableGenerateMultipleForm::tr("Data is wrong. Please correct and try again"));
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
			TimetableGenerateMultipleForm::tr("You have entered the generation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	currentResultsTextEdit->setPlainText("");

	bool ok=genMultiMatrix[0].precompute(this);
	if(!ok){
		currentResultsTextEdit->setPlainText(TimetableGenerateMultipleForm::tr("Cannot optimize - please modify your data"));
		currentResultsTextEdit->update();

		QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Your data cannot be processed - please modify it as instructed."));

		return;
	}

	//assert(controllersList.count()==0);
	for(int t=0; t<nThreads; t++){
		pausePushButtons.at(t)->setEnabled(true);
		pausePushButtons.at(t)->setText(tr("Pause", "Pause the generation"));
		
		pausedLabels[t]->setVisible(false);
		
		labels[t]->setEnabled(true);

		restartPushButtons.at(t)->setEnabled(true);

		//generateMultipleWorker[t].disconnect(); //disconnect all connections for this QThread
		genMultiMatrix[t].disconnect(); //disconnect all connections for this Generate
		
		//Controller* controller=new Controller();
		timetablingThreads[t].disconnect();
		
		//connect(controller, SIG NAL(timetableStarted(int, int)), this, SL OT(timetableStarted(int, int)));
		//connect(controller, SIG NAL(timetableGenerated(int, int, const QString&, bool)), this, SL OT(timetableGenerated(int, int, const QString&, bool)));
		
		//controllersList.append(controller);
		
		timetablingThreads[t]._nThread=t;

		genMultiMatrix[t].nThread=t;
		//genMultiMatrix[t].isRunning=false;

		connect(&genMultiMatrix[t], &Generate::activityPlaced, this, &TimetableGenerateMultipleForm::activityPlaced);
		connect(&timetablingThreads[t], &TimetablingThread::timetableGenerated, this, &TimetableGenerateMultipleForm::timetableGenerated);
	}
	
	startPushButton->setDisabled(true);
	stopPushButton->setEnabled(true);
	minutesGroupBox->setDisabled(true);
	timetablesGroupBox->setDisabled(true);
	closePushButton->setDisabled(true);
	threadsGroupBox->setDisabled(true);
	seeInitialOrderPushButton->setEnabled(true);

	nGeneratedTimetables=0;
	nSuccessfullyGeneratedTimetables=0;
	highestPlacedActivities=0;
	
	generation_running_multi=true;

	numberOfGeneratedTimetablesLabel->setText(tr("Generated: %1").arg(0));
	numberOfSuccessfullyGeneratedTimetablesLabel->setText(tr("Successfully: %1").arg(0));

	time(&all_processes_start_time);
	
	//assert(controllersList.count()==nThreads);

	//for(int t=0; t<nThreads; t++)
	//	controllersList.at(t)->_nThread=t;
	
	for(int t=0; t<nThreads; t++){
		genMultiMatrix[t].c.makeUnallocated(gt.rules);

		genMultiMatrix[t].abortOptimization=false;
		genMultiMatrix[t].restart=false;
		genMultiMatrix[t].paused=false;
		genMultiMatrix[t].pausedTime=0;
		
		timetablingThreads[t].nOverallTimetable=t;

		//Controller* tc=controllersList.at(t);
		//tc->nOverallTimetable=t;

		timetableStarted(t, timetablingThreads[t].nOverallTimetable+1);

		semaphoreTimetableStarted[t].acquire();
		/*std::mutex mtx;
		std::unique_lock<std::mutex> lck(mtx);
		cvTimetableStarted[t].wait(lck);*/

		//timetablingThreads[t].startGenerating();

		/*timetablingThreads[t]._internalGeneratingThread=std::thread([=]{timetablingThreads[t].startGenerating();});
		timetablingThreads[t]._internalGeneratingThread.detach();*/

		//old comment below:
		//2023-09-27: Must put [=] here, [&] does not work, gives various crashes. I am not sure why, I think maybe because
		//the TimetablingThread must be reinstantiated in the detached thread. Probably t goes out of scope.
		//std::thread([=]{timetablingThreads[t].startGenerating();}).detach();

		std::thread([t]{timetablingThreads[t].startGenerating();}).detach();
	}
}

void TimetableGenerateMultipleForm::timetableStarted(int nThread, int timetable)
{
	TimetableExport::writeRandomSeed(this, genMultiMatrix[nThread].rng, timetable, true); //true represents the 'before' state
	
	semaphoreTimetableStarted[nThread].release();
	//cvTimetableStarted[nThread].notify_one();
}

void TimetableGenerateMultipleForm::timetableGenerated(int nThread, int timetable, const QString& description, bool ok)
{
	assert(!genMultiMatrix[nThread].isRunning);
	
	//QCoreApplication::sendPostedEvents();
	//genMultiMatrix[nThread].semaphorePlacedActivity.release();

	genMultiMatrix[nThread].semaphoreFinished.acquire();

	//genMultiMatrix[nThread].semaphorePlacedActivity.tryAcquire();

	nGeneratedTimetables++;
	numberOfGeneratedTimetablesLabel->setText(tr("Generated: %1").arg(nGeneratedTimetables));
	if(ok){
		nSuccessfullyGeneratedTimetables++;
		numberOfSuccessfullyGeneratedTimetablesLabel->setText(tr("Successfully: %1").arg(nSuccessfullyGeneratedTimetables));
		highestPlacedActivities=genMultiMatrix[nThread].maxActivitiesPlaced;
		assert(highestPlacedActivities==gt.rules.nInternalActivities);
	}
	else{
		if(highestPlacedActivities<genMultiMatrix[nThread].maxActivitiesPlaced){
			highestPlacedActivities=genMultiMatrix[nThread].maxActivitiesPlaced;
			for(Solution* sol : std::as_const(highestStageSolutions))
				delete sol;
			highestStageSolutions.clear();
			nTimetableForHighestStageSolutions.clear();
			nThreadForHighest.clear();
			generationTimedOutForHighest.clear();
			timeForHighestStageSolutions.clear();
			Solution* sol=new Solution();
			sol->copyForHighestStage(gt.rules, genMultiMatrix[nThread].highestStageSolution);
			highestStageSolutions.append(sol);
			nTimetableForHighestStageSolutions.append(timetable);
			nThreadForHighest.append(nThread);
			generationTimedOutForHighest.append(true);
			timeForHighestStageSolutions.append(genMultiMatrix[nThread].timeToHighestStage);
		}
		else if(highestPlacedActivities==genMultiMatrix[nThread].maxActivitiesPlaced
		 && !nTimetableForHighestStageSolutions.contains(timetable) /*not a very nice test this 'contains' :-( , but sometimes we could have ended up with duplicates in the list*/){
			Solution* sol=new Solution();
			sol->copyForHighestStage(gt.rules, genMultiMatrix[nThread].highestStageSolution);
			highestStageSolutions.append(sol);
			nTimetableForHighestStageSolutions.append(timetable);
			nThreadForHighest.append(nThread);
			generationTimedOutForHighest.append(true);
			timeForHighestStageSolutions.append(genMultiMatrix[nThread].timeToHighestStage);
		}
	}

	if(nGeneratedTimetables<=nTimetables){
		if(ok)
			TimetableExport::writeRandomSeed(this, genMultiMatrix[nThread].rng, timetable, false); //false represents the 'before' state

		QString s=QString("");
		s+=tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description").arg(timetable).arg(description);
		currentResultsTextEdit->append(s);
	
		bool begin;
		if(nGeneratedTimetables==1)
			begin=true;
		else
			begin=false;
		TimetableExport::writeReportForMultiple(this, s, begin);
	
		if(ok){
			//needed to get the conflicts string
			FakeString tmp;
			fitnessMutexInThreads.lock();
			genMultiMatrix[nThread].c.fitness(gt.rules, &tmp);
			fitnessMutexInThreads.unlock();
	
			/*TimetableExport::getStudentsTimetable(genMultiMatrix[nThread].c);
			TimetableExport::getTeachersTimetable(genMultiMatrix[nThread].c);
			TimetableExport::getRoomsTimetable(genMultiMatrix[nThread].c);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(genMultiMatrix[nThread].c);

			TimetableExport::writeGenerationResults(this, timetable);

			//update the string representing the conflicts
			conflictsStringTitle=tr("Soft conflicts", "Title of dialog");
			conflictsString = "";

			conflictsString+=tr("Number of broken soft constraints: %1").arg(best_solution.conflictsWeightList.count());

			conflictsString+="\n";

			conflictsString+=tr("Total soft conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(best_solution.conflictsTotal));

			conflictsString+="\n";
			conflictsString+=tr("Soft conflicts listing (in decreasing order):")+"\n";

			for(const QString& t : std::as_const(best_solution.conflictsDescriptionList))
				conflictsString+=t+"\n";
			
			updateAllTimetableViewDialogs();

			if(BEEP_AT_END_OF_GENERATION_EACH_TIMETABLE)
				QApplication::beep();

			if(ENABLE_COMMAND_AT_END_OF_GENERATION_EACH_TIMETABLE){
				QString s=commandAtEndOfGenerationEachTimetable.simplified();
				if(!s.isEmpty()){
					QStringList sl=s.split(" ");
					assert(sl.count()>=1);
					QString command=sl.at(0);
					QStringList arguments;
					for(int i=1; i<sl.count(); i++)
						arguments.append(sl.at(i));
					
					QProcess::startDetached(command, arguments);
				}
			}
		}
	}

	//semaphoreTimetableFinished.release();
	
	semaphoreTimetableFinished[nThread].release();
	//cvTimetableFinished[nThread].notify_one();
	//generateMultipleThread[nThread].quit();
	//generateMultipleThread[nThread].wait();
	//while(generateMultipleThread[nThread].isRunning()){
		//wait for the thread to finish.
	//}
	//assert(generateMultipleThread[nThread].isFinished());
	if(nGeneratedTimetables<nTimetables && generation_running_multi){
		//if(timetablingThreads[nThread]._internalGeneratingThread.joinable())
		//	timetablingThreads[nThread]._internalGeneratingThread.join();
	
		//Controller* oldController=controllersList.at(nThread);
		//controllersList.removeAt(nThread);
		//int toldn=oldController->nOverallTimetable;
		int toldn=timetablingThreads[nThread].nOverallTimetable;
		//delete oldController;
		//oldController->deleteLater();
	
		genMultiMatrix[nThread].c.makeUnallocated(gt.rules);

		genMultiMatrix[nThread].abortOptimization=false;
		genMultiMatrix[nThread].restart=false;
		genMultiMatrix[nThread].paused=false;
		genMultiMatrix[nThread].pausedTime=0;

		//Controller* controller=new Controller();
		//controllersList[nThread]=controller;
		
		//controller->_nThread=nThread;
		//controller->nOverallTimetable=toldn+allNThreads;
		timetablingThreads[nThread].nOverallTimetable=toldn+allNThreads;
		
		//connect(controller, SIG NAL(timetableStarted(int, int)), this, SL OT(timetableStarted(int, int)));
		
		//connect(controller, SIG NAL(timetableGenerated(int, int, const QString&, bool)), this, SL OT(timetableGenerated(int, int, const QString&, bool)));
		
		//timetableStarted(nThread, controller->nOverallTimetable+1/*+allNThreads*/);
		timetableStarted(nThread, timetablingThreads[nThread].nOverallTimetable+1);

		//QCoreApplication::sendPostedEvents();

		semaphoreTimetableStarted[nThread].acquire();
		/*std::mutex mtx;
		std::unique_lock<std::mutex> lck(mtx);
		cvTimetableStarted[nThread].wait(lck);*/

		//timetablingThreads[nThread]._internalGeneratingThread=std::thread(timetablingThreads[nThread].startGenerating);
		/*timetablingThreads[nThread]._internalGeneratingThread=std::thread([=]{timetablingThreads[nThread].startGenerating();});
		timetablingThreads[nThread]._internalGeneratingThread.detach();*/

		//old comment below:
		//2023-09-27: Must put [=] here, [&] does not work, gives various crashes. I am not sure why, I think maybe because
		//the TimetablingThread must be reinstantiated in the detached thread. Probably nThread goes out of scope.
		//std::thread([=]{timetablingThreads[nThread].startGenerating();}).detach();
		std::thread([nThread]{timetablingThreads[nThread].startGenerating();}).detach();

		//controller->startOperate(nThread);
	}
	else if(generation_running_multi){
		//assert(controllersList.count()==allNThreads);
		for(int t=0; t<allNThreads; t++){
			//This is always done (1 || stuff) because we need to disconnect the Generate.
			//if(1 || generateMultipleThread[t].isRunning()){

			/*genMultiMatrix[t].myMutex.lock();
			bool iR=genMultiMatrix[t].isRunning;
			genMultiMatrix[t].myMutex.unlock();*/

			if(t!=nThread){
				if(genMultiMatrix[t].isRunning){
					//genMultiMatrix[t].myMutex.lock();
					genMultiMatrix[t].abortOptimization=true;
					//genMultiMatrix[t].disconnect();
					//genMultiMatrix[t].myMutex.unlock();
	
					//QCoreApplication::sendPostedEvents();
					genMultiMatrix[t].semaphorePlacedActivity.release();
		
					genMultiMatrix[t].semaphoreFinished.acquire();

					//genMultiMatrix[t].semaphorePlacedActivity.tryAcquire();
				}
			}
			else{
				assert(!genMultiMatrix[t].isRunning);
			}
			
			//delete controllersList[t];
			//}
			
			//if(timetablingThreads[t]._internalGeneratingThread.joinable())
			//	timetablingThreads[t]._internalGeneratingThread.join();

			if(highestPlacedActivities<genMultiMatrix[t].maxActivitiesPlaced){
				highestPlacedActivities=genMultiMatrix[t].maxActivitiesPlaced;
				for(Solution* sol : std::as_const(highestStageSolutions))
					delete sol;
				highestStageSolutions.clear();
				nTimetableForHighestStageSolutions.clear();
				nThreadForHighest.clear();
				generationTimedOutForHighest.clear();
				timeForHighestStageSolutions.clear();
				Solution* sol=new Solution();
				sol->copyForHighestStage(gt.rules, genMultiMatrix[t].highestStageSolution);
				highestStageSolutions.append(sol);
				nTimetableForHighestStageSolutions.append(timetablingThreads[t].nOverallTimetable+1);
				nThreadForHighest.append(t);
				generationTimedOutForHighest.append(true);
				timeForHighestStageSolutions.append(genMultiMatrix[t].timeToHighestStage);
			}
			else if(highestPlacedActivities==genMultiMatrix[t].maxActivitiesPlaced
			 && !nTimetableForHighestStageSolutions.contains(timetablingThreads[t].nOverallTimetable+1) /*not a very nice test this 'contains' :-( , but sometimes we could have ended up with duplicates in the list*/){
				Solution* sol=new Solution();
				sol->copyForHighestStage(gt.rules, genMultiMatrix[t].highestStageSolution);
				highestStageSolutions.append(sol);
				nTimetableForHighestStageSolutions.append(timetablingThreads[t].nOverallTimetable+1);
				nThreadForHighest.append(t);
				generationTimedOutForHighest.append(true);
				timeForHighestStageSolutions.append(genMultiMatrix[t].timeToHighestStage);
			}
		}
		generationFinished();
	}
}

void TimetableGenerateMultipleForm::restartCurrentThread()
{
	if(!generation_running_multi){
		return;
	}
	
	int t=timetablesTabWidget->currentIndex();
	assert(t>=0);
	assert(t<nThreadsSpinBox->value());
	
	restartPushButtons.at(t)->setDisabled(true);
	pausePushButtons.at(t)->setDisabled(true);
	pausePushButtons.at(t)->setText(tr("Pause", "Pause the generation"));
	
	pausedLabels.at(t)->setVisible(false);
	
	labels.at(t)->setEnabled(true);
	
	if(genMultiMatrix[t].isRunning && !genMultiMatrix[t].restart){
		genMultiMatrix[t].restart=true;
		//semaphoreRestarted.acquire();
		//genMultiMatrix[t].semaphorePlacedActivity.release();
		//genMultiMatrix[t].semaphoreFinished.acquire();
	}

	restartPushButtons.at(t)->setEnabled(true);
	pausePushButtons.at(t)->setEnabled(true);
}

void TimetableGenerateMultipleForm::pauseCurrentThread()
{
	if(!generation_running_multi){
		return;
	}
	
	int t=timetablesTabWidget->currentIndex();
	assert(t>=0);
	assert(t<nThreadsSpinBox->value());
	
	if(genMultiMatrix[t].isRunning){
		if(!genMultiMatrix[t].paused){
			pausePushButtons.at(t)->setText(tr("Continue", "Continue the generation (it was previously paused, and now it will continue from where it was paused). "
			 "Please make a clear distinction between 'Continue the generation' and 'Restart the generation'."));
			genMultiMatrix[t].paused=true;

			labels[t]->setDisabled(true);

			pausedLabels[t]->setVisible(true);
			//QString s=tr("[PAUSED]", "Generation is paused")+QString("\n")+labels[t]->text();
			//labels[t]->setText(s);
		}
		else{
			pausedLabels[t]->setVisible(false);
			
			labels[t]->setEnabled(true);

			pausePushButtons.at(t)->setText(tr("Pause", "Pause the generation"));
			genMultiMatrix[t].paused=false;
		}
	}
}

void TimetableGenerateMultipleForm::stop()
{
	if(!generation_running_multi){
		return;
	}

	generation_running_multi=false;
	
	//assert(controllersList.count()==nThreadsSpinBox->value());

	for(int t=0; t<nThreadsSpinBox->value(); t++){
		pausedLabels[t]->setVisible(false);
		
		labels[t]->setEnabled(true);

		pausePushButtons.at(t)->setDisabled(true);
		pausePushButtons.at(t)->setText(tr("Pause", "Pause the generation"));
		restartPushButtons.at(t)->setDisabled(true);
		
		/*genMultiMatrix[t].myMutex.lock();
		bool iR=genMultiMatrix[t].isRunning;
		genMultiMatrix[t].myMutex.unlock();*/

		if(genMultiMatrix[t].isRunning){
			//cout<<"1. t="<<t<<endl;
			//genMultiMatrix[t].myMutex.lock();
			genMultiMatrix[t].abortOptimization=true;
			//genMultiMatrix[t].myMutex.unlock();

			//QCoreApplication::sendPostedEvents();
			genMultiMatrix[t].semaphorePlacedActivity.release();

			genMultiMatrix[t].semaphoreFinished.acquire();

			//genMultiMatrix[t].semaphorePlacedActivity.tryAcquire();
		}

		//cout<<"2. t="<<t<<endl;
		//if(timetablingThreads[t]._internalGeneratingThread.joinable())
		//	timetablingThreads[t]._internalGeneratingThread.join();
		//cout<<"3. t="<<t<<endl;
		//if(timetablingThreads[t].joinable())
		//	timetablingThreads[t].join();

		//if(controllersList.at(t)->workerThread.isRunning()){
		//	genMultiMatrix[t].myMutex.lock();

		//	genMultiMatrix[t].abortOptimization=true;
		if(nSuccessfullyGeneratedTimetables==0){
			if(highestPlacedActivities<genMultiMatrix[t].maxActivitiesPlaced){
				highestPlacedActivities=genMultiMatrix[t].maxActivitiesPlaced;
				for(Solution* sol : std::as_const(highestStageSolutions))
					delete sol;
				highestStageSolutions.clear();
				nTimetableForHighestStageSolutions.clear();
				nThreadForHighest.clear();
				generationTimedOutForHighest.clear();
				timeForHighestStageSolutions.clear();
				Solution* sol=new Solution();
				sol->copyForHighestStage(gt.rules, genMultiMatrix[t].highestStageSolution);
				highestStageSolutions.append(sol);
				//nTimetableForHighestStageSolutions.append(controllersList.at(t)->nOverallTimetable+1);
				nTimetableForHighestStageSolutions.append(timetablingThreads[t].nOverallTimetable+1);
				nThreadForHighest.append(t);
				generationTimedOutForHighest.append(false);
				timeForHighestStageSolutions.append(genMultiMatrix[t].timeToHighestStage);
			}
			else if(highestPlacedActivities==genMultiMatrix[t].maxActivitiesPlaced
			 && !nTimetableForHighestStageSolutions.contains(timetablingThreads[t].nOverallTimetable+1) /*not a very nice test this 'contains' :-( , but sometimes we could have ended up with duplicates in the list*/){
				Solution* sol=new Solution();
				sol->copyForHighestStage(gt.rules, genMultiMatrix[t].highestStageSolution);
				highestStageSolutions.append(sol);
				//nTimetableForHighestStageSolutions.append(controllersList.at(t)->nOverallTimetable+1);
				nTimetableForHighestStageSolutions.append(timetablingThreads[t].nOverallTimetable+1);
				nThreadForHighest.append(t);
				generationTimedOutForHighest.append(false);
				timeForHighestStageSolutions.append(genMultiMatrix[t].timeToHighestStage);
			}
		}

		//genMultiMatrix[t].myMutex.unlock();
	}

	QString s=TimetableGenerateMultipleForm::tr("Generation interrupted!");

	/*QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";*/

	time_t final_time;
	time(&final_time);
	int sec=int(difftime(final_time, all_processes_start_time));
	int h=sec/3600;
	sec%=3600;
	int m=sec/60;
	sec%=60;

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	QString ms2=QString("");
	
	QString stringForDisk=s;
	
	QString ms3=QString("");
	if(nSuccessfullyGeneratedTimetables>=1){
		s+=QString("\n\n");
		s+=TimetableGenerateMultipleForm::tr("From the interface you can access the last successfully generated timetable.");
	}
	else{
		s+=QString("\n\n");
		s+=TimetableGenerateMultipleForm::tr("From the interface you can access the last highest-stage timetable and on the disk"
		 " there were saved all the highest-stage timetables (the highest number of scheduled activities is %1, reached in %2 timetables).")
		 .arg(highestPlacedActivities).arg(highestStageSolutions.count());
		
		stringForDisk+=QString("\n\n");
		stringForDisk+=TimetableGenerateMultipleForm::tr("On the disk there were saved all the highest-stage timetables (the highest number"
		 " of scheduled activities is %1, reached in %2 timetables).").arg(highestPlacedActivities).arg(highestStageSolutions.count());

		for(int i=0; i<highestStageSolutions.count(); i++){
			Solution* sol=highestStageSolutions.at(i);
			int nTimetable=nTimetableForHighestStageSolutions.at(i);
			int nThread=nThreadForHighest.at(i);
			bool timedOut=generationTimedOutForHighest.at(i);
			
			int secondsToHighestStage=timeForHighestStageSolutions.at(i);
			bool zero=(secondsToHighestStage==0);
			int hours=secondsToHighestStage/3600;
			secondsToHighestStage%=3600;
			int minutes=secondsToHighestStage/60;
			secondsToHighestStage%=60;
			int seconds=secondsToHighestStage;
			
			QString description;
			if(allNThreads>=2)
				description=tr("(Thread %1)").arg(nThread+1)+QString(" ");
			else
				description=QString("");
			if(timedOut)
				description+=tr("Time exceeded.");
			else
				description+=tr("Generation stopped.");
			description+=QString(" ");

			//description+=tr("Maximum placed activities: %1.").arg(highestPlacedActivities);

			QString tim;
			if(hours>0){
				tim+=" ";
				tim+=tr("%1 h", "hours").arg(hours);
			}
			if(minutes>0){
				tim+=" ";
				tim+=tr("%1 m", "minutes").arg(minutes);
			}
			if(seconds>0 || zero){
				tim+=" ";
				tim+=tr("%1 s", "seconds").arg(seconds);
			}
			tim.remove(0, 1);
			description+=tr("Maximum placed activities: %1 (at %2).", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(highestPlacedActivities).arg(tim);
			//description+=translatedDot();
			
			if(i>=1 || currentResultsTextEdit->toPlainText().isEmpty())
				currentResultsTextEdit->append(tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
				 .arg(nTimetable).arg(description));
			else
				currentResultsTextEdit->append(QString("\n")+tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
				 .arg(nTimetable).arg(description));
			ms3+=tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
			 .arg(nTimetable).arg(description)+QString("\n");

			//needed to get the conflicts string
			FakeString tmp;
			fitnessMutexInThreads.lock();
			sol->fitness(gt.rules, &tmp);
			fitnessMutexInThreads.unlock();

			/*TimetableExport::getStudentsTimetable(*sol);
			TimetableExport::getTeachersTimetable(*sol);
			TimetableExport::getRoomsTimetable(*sol);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(*sol);

			TimetableExport::writeGenerationResults(this, nTimetable, true);

			//update the string representing the conflicts
			conflictsStringTitle=tr("Conflicts", "Title of dialog");
			conflictsString = "";

			conflictsString+=tr("Number of broken constraints: %1").arg(sol->conflictsWeightList.count());

			conflictsString+="\n";

			conflictsString+=tr("Total conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(sol->conflictsTotal));

			conflictsString+="\n";
			conflictsString+=tr("Conflicts listing (in decreasing order):")+"\n";

			for(const QString& t : std::as_const(sol->conflictsDescriptionList))
				conflictsString+=t+"\n";
		}
		updateAllTimetableViewDialogs();
	}

	if(!ms3.isEmpty())
		ms3+=QString("\n");

	s+="\n\n";
	s+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1.").arg(QDir::toNativeSeparators(MULTIPLE_OUTPUT_DIRECTORY));
	s+="\n\n";
	s+=tr("Total searching time was: %1h %2m %3s.").arg(h).arg(m).arg(sec);

	stringForDisk+="\n\n";
	stringForDisk+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1.").arg(QDir::toNativeSeparators(MULTIPLE_OUTPUT_DIRECTORY));
	stringForDisk+="\n\n";
	stringForDisk+=tr("Total searching time was: %1h %2m %3s.").arg(h).arg(m).arg(sec);

	ms2+=QString("\n\n");
	ms2+=TimetableGenerateMultipleForm::tr("This file was automatically generated by FET %1 on %2.",
	 "%1 is the FET version, %2 is the date and time when this file was generated.").arg(FET_VERSION).arg(sTime);

	if(nGeneratedTimetables>=1)
		TimetableExport::writeReportForMultiple(this, QString("\n")+ms3+stringForDisk+ms2, false);
	else
		TimetableExport::writeReportForMultiple(this, ms3+stringForDisk+ms2, true);

	QMessageBox::information(this, tr("FET information"), s);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);
	closePushButton->setEnabled(true);
	threadsGroupBox->setEnabled(true);
	
	/*gen.rng.s10=genMultiMatrix[0].rng.s10;
	gen.rng.s11=genMultiMatrix[0].rng.s11;
	gen.rng.s12=genMultiMatrix[0].rng.s12;
	gen.rng.s20=genMultiMatrix[0].rng.s20;
	gen.rng.s21=genMultiMatrix[0].rng.s21;
	gen.rng.s22=genMultiMatrix[0].rng.s22;
	assert(gen.rng.s10!=0 || gen.rng.s11!=0 || gen.rng.s12!=0);
	assert(gen.rng.s20!=0 || gen.rng.s21!=0 || gen.rng.s22!=0);*/
	gen.rng.initializeMRG32k3a(genMultiMatrix[0].rng.s10, genMultiMatrix[0].rng.s11, genMultiMatrix[0].rng.s12,
	 genMultiMatrix[0].rng.s20, genMultiMatrix[0].rng.s21, genMultiMatrix[0].rng.s22);

	/*for(Controller* tc : std::as_const(controllersList))
		//delete tc;
		tc->deleteLater();
	controllersList.clear();*/
}

void TimetableGenerateMultipleForm::generationFinished()
{
	if(!generation_running_multi){
		return;
	}

	generation_running_multi=false;

	/*QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";*/
	
	time_t final_time;
	time(&final_time);
	int s=int(difftime(final_time, all_processes_start_time));
	int h=s/3600;
	s%=3600;
	int m=s/60;
	s%=60;

	QString ms=QString("");
	ms+=TimetableGenerateMultipleForm::tr("Generation finished!");
	
	QString stringForDisk=ms;
	
	QString ms3=QString("");
	
	if(nSuccessfullyGeneratedTimetables>=1){
		ms+=QString("\n\n");
		ms+=TimetableGenerateMultipleForm::tr("From the interface you can access the last successfully generated timetable.");
	}
	else{
		ms+=QString("\n\n");
		ms+=TimetableGenerateMultipleForm::tr("From the interface you can access the last highest-stage timetable and on the disk"
		 " there were saved all the highest-stage timetables (the highest number of scheduled activities is %1, reached in %2 timetables).")
		 .arg(highestPlacedActivities).arg(highestStageSolutions.count());

		stringForDisk+=QString("\n\n");
		stringForDisk+=TimetableGenerateMultipleForm::tr("On the disk there were saved all the highest-stage timetables (the highest number"
		 " of scheduled activities is %1, reached in %2 timetables).").arg(highestPlacedActivities).arg(highestStageSolutions.count());

		for(int i=0; i<highestStageSolutions.count(); i++){
			Solution* sol=highestStageSolutions.at(i);
			int nTimetable=nTimetableForHighestStageSolutions.at(i);
			int nThread=nThreadForHighest.at(i);
			bool timedOut=generationTimedOutForHighest.at(i);

			int secondsToHighestStage=timeForHighestStageSolutions.at(i);
			bool zero=(secondsToHighestStage==0);
			int hours=secondsToHighestStage/3600;
			secondsToHighestStage%=3600;
			int minutes=secondsToHighestStage/60;
			secondsToHighestStage%=60;
			int seconds=secondsToHighestStage;

			QString description;
			if(allNThreads>=2)
				description=tr("(Thread %1)").arg(nThread+1)+QString(" ");
			else
				description=QString("");
			if(timedOut)
				description+=tr("Time exceeded.");
			else
				description+=tr("Generation stopped.");
			description+=QString(" ");

			//description+=tr("Maximum placed activities: %1.").arg(highestPlacedActivities);

			QString tim;
			if(hours>0){
				tim+=" ";
				tim+=tr("%1 h", "hours").arg(hours);
			}
			if(minutes>0){
				tim+=" ";
				tim+=tr("%1 m", "minutes").arg(minutes);
			}
			if(seconds>0 || zero){
				tim+=" ";
				tim+=tr("%1 s", "seconds").arg(seconds);
			}
			tim.remove(0, 1);
			description+=tr("Maximum placed activities: %1 (at %2).", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(highestPlacedActivities).arg(tim);
			//description+=translatedDot();

			if(i>=1 || currentResultsTextEdit->toPlainText().isEmpty())
				currentResultsTextEdit->append(tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
				 .arg(nTimetable).arg(description));
			else
				currentResultsTextEdit->append(QString("\n")+tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
				 .arg(nTimetable).arg(description));
			ms3+=tr("Timetable no: %1 => %2", "%1 is the number of this timetable when generating multiple timetables, %2 is its description")
			 .arg(nTimetable).arg(description)+QString("\n");

			//needed to get the conflicts string
			FakeString tmp;
			fitnessMutexInThreads.lock();
			sol->fitness(gt.rules, &tmp);
			fitnessMutexInThreads.unlock();

			/*TimetableExport::getStudentsTimetable(*sol);
			TimetableExport::getTeachersTimetable(*sol);
			TimetableExport::getRoomsTimetable(*sol);*/
			TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(*sol);

			TimetableExport::writeGenerationResults(this, nTimetable, true);

			//update the string representing the conflicts
			conflictsStringTitle=tr("Conflicts", "Title of dialog");
			conflictsString = "";

			conflictsString+=tr("Number of broken constraints: %1").arg(sol->conflictsWeightList.count());

			conflictsString+="\n";

			conflictsString+=tr("Total conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(sol->conflictsTotal));

			conflictsString+="\n";
			conflictsString+=tr("Conflicts listing (in decreasing order):")+"\n";

			for(const QString& t : std::as_const(sol->conflictsDescriptionList))
				conflictsString+=t+"\n";
		}
		updateAllTimetableViewDialogs();
	}

	if(!ms3.isEmpty())
		ms3+=QString("\n");

	ms+=QString("\n\n");
	ms+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1.").arg(QDir::toNativeSeparators(MULTIPLE_OUTPUT_DIRECTORY));
	ms+=QString("\n\n");
	ms+=TimetableGenerateMultipleForm::tr("Total searching time was %1h %2m %3s.").arg(h).arg(m).arg(s);
	
	stringForDisk+=QString("\n\n");
	stringForDisk+=TimetableGenerateMultipleForm::tr("The results were saved in the directory %1.").arg(QDir::toNativeSeparators(MULTIPLE_OUTPUT_DIRECTORY));
	stringForDisk+=QString("\n\n");
	stringForDisk+=TimetableGenerateMultipleForm::tr("Total searching time was %1h %2m %3s.").arg(h).arg(m).arg(s);

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	QString ms2=QString("\n\n");
	ms2+=TimetableGenerateMultipleForm::tr("This file was automatically generated by FET %1 on %2.",
	 "%1 is the FET version, %2 is the date and time when this file was generated.").arg(FET_VERSION).arg(sTime);
	
	assert(nGeneratedTimetables>=1);
	TimetableExport::writeReportForMultiple(this, QString("\n")+ms3+stringForDisk+ms2, false);

//Old comment below (2020-08-14).
//On Windows we do not beep for Qt >= 5.14.1, because the QMessageBox below beeps itself.
//It would be better to test at runtime, not at compile time, but it is easier/safer this way.
//(The alternative would be to develop a parser for the function qVersion(), but I am not sure it will always respect the exact format "vM.vm.vp".)
//We test the macro Q_OS_WIN32 because on the old Qt 4 it is the only available macro from these three below,
//Q_OS_WIN, Q_OS_WIN32, and Q_OS_WIN64 (which are available on Qt 5.14.1). Yes, the test is redundant (because if QT_VERSION < 5.14.1
//the condition is true and if QT_VERSION >= 5.14.1 then all these three macros are available), but this doesn't hurt.
//#if (!defined(Q_OS_WIN) && !defined(Q_OS_WIN32) && !defined(Q_OS_WIN64)) || (QT_VERSION < QT_VERSION_CHECK(5,14,1))
	if(BEEP_AT_END_OF_GENERATION)
		QApplication::beep();
//#endif

	if(ENABLE_COMMAND_AT_END_OF_GENERATION){
		QString s=commandAtEndOfGeneration.simplified();
		if(!s.isEmpty()){
			QStringList sl=s.split(" ");
			assert(sl.count()>=1);
			QString command=sl.at(0);
			QStringList arguments;
			for(int i=1; i<sl.count(); i++)
				arguments.append(sl.at(i));
			
			/*if(DETACHED_NOTIFICATION==false){
				QProcess* myProcess=new QProcess();
				if(terminateCommandAfterSeconds>0)
					QTimer::singleShot(terminateCommandAfterSeconds*1000, myProcess, SL OT(terminate()));
				if(killCommandAfterSeconds>0)
					QTimer::singleShot(killCommandAfterSeconds*1000, myProcess, SL OT(kill()));
				
				//https://www.qtcentre.org/threads/43083-Freeing-a-QProcess-after-it-has-finished-using-deleteLater()
				connect(myProcess, SIG NAL(finished(int)), myProcess, SL OT(deleteLater()));
				myProcess->start(command, arguments);
			}*/
			//else{
			QProcess::startDetached(command, arguments);
			//}
		}
	}
	
	//Trick so that the message box will be silent (the only sound is thus the beep above).
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(TimetableGenerateMultipleForm::tr("FET information"));
	msgBox.setText(ms);
	msgBox.exec();
	//QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"), ms);
	
	for(int t=0; t<nThreadsSpinBox->value(); t++){
		pausedLabels[t]->setVisible(false);
		
		labels[t]->setEnabled(true);

		pausePushButtons.at(t)->setDisabled(true);
		pausePushButtons.at(t)->setText(tr("Pause", "Pause the generation"));
		restartPushButtons.at(t)->setDisabled(true);
	}
	
	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	minutesGroupBox->setEnabled(true);
	timetablesGroupBox->setEnabled(true);
	closePushButton->setEnabled(true);
	threadsGroupBox->setEnabled(true);
	
	/*gen.rng.s10=genMultiMatrix[0].rng.s10;
	gen.rng.s11=genMultiMatrix[0].rng.s11;
	gen.rng.s12=genMultiMatrix[0].rng.s12;
	gen.rng.s20=genMultiMatrix[0].rng.s20;
	gen.rng.s21=genMultiMatrix[0].rng.s21;
	gen.rng.s22=genMultiMatrix[0].rng.s22;
	assert(gen.rng.s10!=0 || gen.rng.s11!=0 || gen.rng.s12!=0);
	assert(gen.rng.s20!=0 || gen.rng.s21!=0 || gen.rng.s22!=0);*/
	gen.rng.initializeMRG32k3a(genMultiMatrix[0].rng.s10, genMultiMatrix[0].rng.s11, genMultiMatrix[0].rng.s12,
	 genMultiMatrix[0].rng.s20, genMultiMatrix[0].rng.s21, genMultiMatrix[0].rng.s22);

	/*for(Controller* tc : std::as_const(controllersList))
		//delete tc;
		tc->deleteLater();
	controllersList.clear();*/
}

void TimetableGenerateMultipleForm::activityPlaced(int nThread, int na)
{
	assert(nThread>=0);
	assert(nThread<nThreadsSpinBox->value());
	
	//if(genMultiMatrix[nThread].paused) -> DON'T DO THIS!!! We need to release the semaphore below.
	//	return;

	genMultiMatrix[nThread].myMutex.lock();
	int seconds=genMultiMatrix[nThread].searchTime; //seconds
	int mact=genMultiMatrix[nThread].maxActivitiesPlaced;
	int mseconds=genMultiMatrix[nThread].timeToHighestStage;
	genMultiMatrix[nThread].myMutex.unlock();

	////////2011-05-26
	genMultiMatrix[nThread].semaphorePlacedActivity.release();
	//genMultiMatrix[nThread].cvForPlacedActivity.notify_one();

	//time_t finish_time;
	//time(&finish_time);
	//int seconds=int(difftime(finish_time, start_time));
	//int seconds=int(difftime(finish_time, process_start_time[nThread]));
	int hours=seconds/3600;
	seconds%=3600;
	int minutes=seconds/60;
	seconds%=60;

	QString s;

	bool zero=false;
	if(mseconds==0)
		zero=true;
	int hh=mseconds/3600;
	mseconds%=3600;
	int mm=mseconds/60;
	mseconds%=60;
	int ss=mseconds;

	QString tim;
	if(hh>0){
		tim+=" ";
		tim+=tr("%1 h", "hours").arg(hh);
	}
	if(mm>0){
		tim+=" ";
		tim+=tr("%1 m", "minutes").arg(mm);
	}
	if(ss>0 || zero){
		tim+=" ";
		tim+=tr("%1 s", "seconds").arg(ss);
	}
	tim.remove(0, 1);
	s+=QString("\n");
	s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);
	///////
	
	//QString prefixS;
	//if(genMultiMatrix[nThread].paused)
	//	prefixS=tr("[PAUSED]", "Generation is paused")+QString("\n");
	
	labels[nThread]->setText(/*prefixS+*/tr("Current timetable: %1 out of %2 activities placed, %3h %4m %5s")
	 .arg(na)
	 .arg(gt.rules.nInternalActivities)
	 .arg(hours)
	 .arg(minutes)
	 .arg(seconds)+s);
}

void TimetableGenerateMultipleForm::closePressed()
{
	if(!generation_running_multi)
		this->close();
}

void TimetableGenerateMultipleForm::seeInitialOrder()
{
	QString s=initialOrderOfActivities;

	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - information about initial order of evaluation of activities"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsNameMultiple);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsNameMultiple);
}
