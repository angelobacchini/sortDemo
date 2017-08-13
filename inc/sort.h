
#ifndef SORT_H
#define SORT_H

#include <QtWidgets>

class abstractSort : public QObject
{
  Q_OBJECT

protected:
  abstractSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume);

public:
  virtual ~abstractSort();

public slots:
  void sort();
  void stop();

signals:
  void updateUi(const int& _swaps, const int& _compares);

protected:
  virtual void sort(const int &_lo, const int &_hi) = 0;
  void updateAndWait();
  void swap(int _a, int _b);
  int less(int _a, int _b);

  QVector<int>* m_array;
  QMutex* m_mutex;
  QWaitCondition* m_resume;
  int m_stopped = 0;
  int m_numSwaps = 0;
  int m_numCompares = 0;

public:
};

class selectionSort : public abstractSort
{
  Q_OBJECT

public:
  selectionSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume);
  ~selectionSort();

private:
  void sort(const int &_lo, const int &_hi);
};

class insertionSort : public abstractSort
{
  Q_OBJECT

public:
  insertionSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume);
  ~insertionSort();

private:
  void sort(const int &_lo, const int &_hi);
};

class quickSort : public abstractSort
{
  Q_OBJECT

public:
  quickSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume);
  ~quickSort();

private:
  void sort(const int &_lo, const int &_hi);
  int partition(const int &_lo, const int &_hi);
};

class mergeSort : public abstractSort
{
  Q_OBJECT

public:
  mergeSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume);
  ~mergeSort();

private:
  void sort(const int &_lo, const int &_hi);
  void merge(const int& _lo, const int& _mid, const int& _hi);

  QVector<int>* m_auxArray;
};

#endif
