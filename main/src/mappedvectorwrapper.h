#ifndef MAPPEDVECTORWRAPPER_H
#define MAPPEDVECTORWRAPPER_H

#include <QObject>
#include <QVector>

template<typename T, typename I>
class MappedVectorWrapper
{
public:
  explicit MappedVectorWrapper(const QVector<T> &data = QVector<T>()) :
    m_data(data)
  {
    if (data.length() == 0)
      m_data.resize(index(I::LAST_INDEX));

    Q_ASSERT(m_data.length() == index(I::LAST_INDEX));
  }

  MappedVectorWrapper(const MappedVectorWrapper &other)
  {
    Q_ASSERT(other.m_data.length() == index(I::LAST_INDEX));

    m_data = other.m_data;
  }

  const T at(const I item) const
  {
    int idx = index(item);

    return m_data.at(idx);
  }

  QVector<T> *pointer()
  {
    return &m_data;
  }

  void setData(QVector<T> data)
  {
    Q_ASSERT(m_data.length() == index(I::LAST_INDEX));

    m_data = data;
  }

  void setItemAt(const int idx, const T &value)
  {
    Q_ASSERT(idx >= 0 && idx < index(I::LAST_INDEX));

    m_data[idx] = value;
  }

  QVector<T> &operator()()
  {
    return m_data;
  }

  T &operator[](const I item)
  {
    int idx = index(item);

    return m_data[idx];
  }

  MappedVectorWrapper &operator=(const MappedVectorWrapper &other)
  {
    Q_ASSERT(other.m_data.length() == index(I::LAST_INDEX));

    m_data = other.m_data;

    return *this;
  }

private:
  int index(const I item) const
  {
    return static_cast<int>(item);
  }

  QVector<T> m_data;
};

#endif // MAPPEDVECTORWRAPPER_H
