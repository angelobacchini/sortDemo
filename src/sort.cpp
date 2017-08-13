
#include "sort.h"

abstractSort::abstractSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume) :
  m_array(_array), m_mutex(_mutex), m_resume(_resume)
{}

abstractSort::~abstractSort()
{}

void abstractSort::sort()
{
  updateAndWait();
  sort(0, m_array->size()-1);
  updateAndWait();
  m_mutex->unlock();
}

void abstractSort::stop()
{
  m_stopped = 1;
}

void abstractSort::updateAndWait()
{
  emit updateUi(m_numSwaps, m_numCompares); // queued connection
  if(!m_stopped) // don't block on wait condition if stop signal has been received
    m_resume->wait(m_mutex);
}

void abstractSort::swap(int _a, int _b)
{
  int temp;
  temp = m_array->at(_a);
  m_array->replace(_a, m_array->at(_b));
  m_array->replace(_b, temp);
  m_numSwaps++;
}

int abstractSort::less(int _a, int _b)
{
  m_numCompares++;
  if(m_array->at(_a) < m_array->at(_b))
    return 1;
  return 0;
}

//SELECTION SORT - N^2/2 COMPARES and N SWAPS
selectionSort::selectionSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume) :
  abstractSort(_array, _mutex, _resume)
{}

selectionSort::~selectionSort()
{}

void selectionSort::sort(const int &_lo, const int &_hi)
{
  int i = _lo;

  while(i <= _hi && !m_stopped)
  {
    int min = i;
    for(int j = i+1; j <= _hi; j++)
    {
      if(!less(min, j))
        min = j;
    }
    swap(i, min);
    updateAndWait();
    i++;
  }
}

//INSERTION SORT - N^2/2 COMPARES and N^2/2 SWAPS (WORST CASE)
insertionSort::insertionSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume) :
  abstractSort(_array, _mutex, _resume)
{}

insertionSort::~insertionSort()
{}

void insertionSort::sort(const int &_lo, const int &_hi)
{
  int i = _lo;

  while(i <= _hi && !m_stopped)
  {
    for(int j = i; j > 0; j--)
    {
      if(!less(j, j-1) or m_stopped)
        break;
      else
        swap(j, j-1);
      updateAndWait();
    }
    updateAndWait();
    i++;
  }
}

//MERGE SORT
mergeSort::mergeSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume) :
  abstractSort(_array, _mutex, _resume)
{
  m_auxArray = new QVector<int>(_array->size());
}

mergeSort::~mergeSort()
{
  delete m_auxArray;
}

void mergeSort::sort(const int &_lo, const int &_hi)
{
  if(_hi <= _lo) return;
  int _mid = _lo +(_hi - _lo)/2;
  sort(_lo, _mid);
  sort(_mid + 1, _hi);
  if(m_stopped) return; // check if stop signal has been recevied and return to avoid
  merge(_lo, _mid, _hi);
  updateAndWait();
}

void mergeSort::merge(const int &_lo, const int &_mid, const int &_hi)
{
  int i = _lo, j = _mid + 1;

  if(!less(_mid + 1, _mid)) // already sorted!!
    return;

  for(int k = _lo; k <= _hi; k++)
  {
    if(i > _mid) m_auxArray->replace(k, m_array->at(j++));
    else if(j > _hi) m_auxArray->replace(k, m_array->at(i++));
    else if(less(j, i)) m_auxArray->replace(k, m_array->at(j++));
    else m_auxArray->replace(k, m_array->at(i++));
    m_numSwaps++;
  }

  for(int i = _lo; i <= _hi; i++)
  {
    m_array->replace(i, m_auxArray->at(i));
    m_numSwaps++;
  }
}

//QUICK SORT
quickSort::quickSort(QVector<int>* _array, QMutex* _mutex, QWaitCondition* _resume) :
  abstractSort(_array, _mutex, _resume)
{}

quickSort::~quickSort()
{}

void quickSort::sort(const int &_lo, const int &_hi)
{
  if(_hi <= _lo)
    return;

  int i = partition(_lo, _hi);
  sort(_lo, i-1);
  sort(i+1, _hi);
}

int quickSort::partition(const int &_lo, const int &_hi)
{
  int i = _lo, j = _hi + 1;

  while(1)
  {
    while(less(++i, _lo))
      if(i == _hi) break;

    while(less(_lo, --j))
      if(j == _lo) break;

    if(i >= j) break;
    swap(j, i);
    updateAndWait();
  }
  swap(_lo, j);
  updateAndWait();
  return j;
}
