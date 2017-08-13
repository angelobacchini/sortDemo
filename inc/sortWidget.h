
#ifndef SORTWIDGET_H
#define SORTWIDGET_H

#include <QtWidgets>
#include "global.h"
#include "sort.h"

class sortWidget : public QWidget
{
  Q_OBJECT

public:
  explicit sortWidget(int _numElements, QWidget* _parent = 0);
  virtual ~sortWidget();

private slots:
  void step();
  void createSort();
  void destroySort();
  void updateUi(const int& _swaps, const int& _compares);
  void reshuffleArray();
  void toggleAutoStep();

private:
  void populateArray();

  int m_arraySize;
  QVector<int> m_array;
  QVector<QFrame*> m_barsArray;

  QComboBox* m_selectSort;
  QComboBox* m_selectSpeed;
  QLabel* m_sortInfo;

  QPushButton* m_stepButton;
  QPushButton* m_reshuffleButton;
  QPushButton* m_autoStepButton;

  QHBoxLayout* m_bars;
  QHBoxLayout* m_buttons;
  QHBoxLayout* m_menu;

  QGridLayout* m_mainLayout;

  QTimer* m_timer;
  QThread* m_processThread;
  QWaitCondition m_resume;
  QMutex m_mutex;

  int m_created;
  int m_autoStep;
  abstractSort* m_sort;
};

#endif
