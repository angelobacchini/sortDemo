
#include "sortWidget.h"

sortWidget::sortWidget(int _numElements, QWidget* _parent) :
  QWidget(_parent), m_arraySize(_numElements), m_created(0), m_autoStep(0)
{
  m_bars = new QHBoxLayout();
  m_buttons = new QHBoxLayout();
  m_menu = new QHBoxLayout();

  populateArray();

  m_selectSort = new QComboBox();
  m_selectSort->setStyleSheet(COMBO_STYLE);
  m_selectSort->setFixedHeight(32);
  m_selectSort->setFixedWidth(200);
  m_selectSort->addItem(QString("selection sort"));
  m_selectSort->addItem(QString("insertion sort"));
  m_selectSort->addItem(QString("merge sort"));
  m_selectSort->addItem(QString("quick sort"));
  m_menu->addWidget(m_selectSort);

  m_selectSpeed = new QComboBox();
  m_selectSpeed->setStyleSheet(COMBO_STYLE);
  m_selectSpeed->setFixedHeight(32);
  m_selectSpeed->setFixedWidth(200);
  m_selectSpeed->addItem(QString("slow"));
  m_selectSpeed->addItem(QString("medium"));
  m_selectSpeed->addItem(QString("fast"));
  m_selectSpeed->setCurrentIndex(1);
  m_menu->addWidget(m_selectSpeed);

  m_sortInfo = new QLabel("sort info");
  m_sortInfo->setFixedHeight(32);
  m_sortInfo->setStyleSheet(INFO_STYLE);

  m_menu->addWidget(m_sortInfo);

  m_autoStepButton = new QPushButton();
  m_autoStepButton->setStyleSheet(BUTTON_STYLE);
  m_autoStepButton->setText("AUTO");
  m_buttons->addWidget(m_autoStepButton);

  m_stepButton = new QPushButton();
  m_stepButton->setStyleSheet(BUTTON_STYLE);
  m_stepButton->setText("STEP");
  m_buttons->addWidget(m_stepButton);

  m_reshuffleButton = new QPushButton();
  m_reshuffleButton->setStyleSheet(BUTTON_STYLE);
  m_reshuffleButton->setText("RESHUFFLE");
  m_buttons->addWidget(m_reshuffleButton);

  m_mainLayout = new QGridLayout(this);
  m_mainLayout->addLayout(m_menu, 0, 0);
  m_mainLayout->addLayout(m_bars, 1, 0);
  m_mainLayout->addLayout(m_buttons, 2, 0);
  this->setLayout(m_mainLayout);

  m_timer = new QTimer();
  m_timer->setSingleShot(true);

  m_processThread = new QThread(this);

  createSort();

  connect(m_stepButton, SIGNAL(pressed()), this, SLOT(step()));
  connect(m_reshuffleButton, SIGNAL(pressed()), this, SLOT(reshuffleArray()));
  connect(m_autoStepButton, SIGNAL(pressed()), this, SLOT(toggleAutoStep()));
  connect(m_selectSort, SIGNAL(currentTextChanged(QString)), this, SLOT(createSort()));
  connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
}

sortWidget::~sortWidget()
{
  QMetaObject::invokeMethod(m_sort, "stop", Qt::DirectConnection);
  m_resume.wakeAll();
  delete m_sort;
  delete m_timer;
  m_processThread->quit();
  m_processThread->wait();
}

void sortWidget::populateArray()
{
  QFrame* frame;
  int value;

  for(int i = 0; i < m_arraySize; i++)
  {
    value = qrand() % RANGE;
    m_array.append(value);

    frame = new QFrame();
    frame->setMinimumWidth(15);
    frame->setFixedHeight(value);
    frame->setStyleSheet(BAR_STYLE);
    m_bars->addWidget(frame);
    m_bars->setAlignment(frame, Qt::AlignBottom);
    m_barsArray.append(frame);
  }
}

void sortWidget::reshuffleArray()
{
  destroySort();
  int value;
  for(int i = 0; i < m_arraySize; i++)
  {
    value = qrand() % RANGE;
    m_array.replace(i, value);
    m_barsArray.at(i)->setFixedHeight(value);
    m_barsArray.at(i)->setStyleSheet(BAR_STYLE);
  }
  createSort();
}

void sortWidget::createSort()
{
  if(m_created)
    destroySort();

  if(m_selectSort->currentText() == QString("insertion sort"))
    m_sort = new insertionSort(&m_array, &m_mutex, &m_resume);
  else if (m_selectSort->currentText() == QString("merge sort"))
    m_sort = new mergeSort(&m_array, &m_mutex, &m_resume);
  else if (m_selectSort->currentText() == QString("quick sort"))
    m_sort = new quickSort(&m_array, &m_mutex, &m_resume);
  else
    m_sort = new selectionSort(&m_array, &m_mutex, &m_resume);

  m_sort->moveToThread(m_processThread);
  m_processThread->start();
  connect(m_sort, SIGNAL(updateUi(int,int)), this, SLOT(updateUi(int,int)));
  QMetaObject::invokeMethod(m_sort, "sort", Qt::QueuedConnection);
  m_created = 1;
}

void sortWidget::destroySort()
{
  if(m_created)
  {
    m_timer->stop();
    m_autoStep = 0;
    m_autoStepButton->setStyleSheet(BUTTON_STYLE);

    QMetaObject::invokeMethod(m_sort, "stop", Qt::DirectConnection);
    m_resume.wakeAll();

    QThread::msleep(100); // to avoid race condition on same systems
    m_mutex.lock();
    delete m_sort;
    m_processThread->quit();
    m_processThread->wait();
    m_created = 0;
    m_mutex.unlock();
  }
}

void sortWidget::updateUi(const int &_swaps, const int &_compares)
{
  m_mutex.lock();

  int sorted = 1;
  QString status("sorted");

  for(int i = 0; i < m_arraySize; i++)
  {
    if(m_barsArray.at(i)->height() != m_array.at(i))
      m_barsArray.at(i)->setStyleSheet(BAR_STYLE_HL);
    else
      m_barsArray.at(i)->setStyleSheet(BAR_STYLE);

    m_barsArray.at(i)->setFixedHeight(m_array.at(i));

    if(i > 0 && m_array.at(i) < m_array.at(i-1))
      sorted = 0;
  }

  if(!sorted)
    status.prepend("not ");

  m_sortInfo->setText(QString("<b>COMPARES:</b>" + QString::number(_compares) + "<b> | SWAPS:</b>" + QString::number(_swaps) + "<b> | STATUS:</b>" + status));

  m_mutex.unlock();
}

void sortWidget::step()
{
  m_resume.wakeAll();

  int stepPeriod;

  if(m_selectSpeed->currentIndex() == 0)
    stepPeriod = STEP_PERIOD_SLOW;
  else if(m_selectSpeed->currentIndex() == 2)
    stepPeriod = STEP_PERIOD_FAST;
  else
    stepPeriod = STEP_PERIOD_MEDIUM;

  if(m_autoStep)
    m_timer->start(stepPeriod);
}

void sortWidget::toggleAutoStep()
{
  if(m_autoStep)
  {
    m_autoStep = 0;
    m_autoStepButton->setStyleSheet(BUTTON_STYLE);
  }
  else
  {
    m_autoStep = 1;
    m_autoStepButton->setStyleSheet(BUTTON_STYLE_PRESSED);
    step();
  }
}
