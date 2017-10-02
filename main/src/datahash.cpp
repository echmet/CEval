#include "datahash.h"
#include <cstring>

#ifdef Q_OS_WIN
  #define COPYHASH(src) memcpy_s(m_data, HASH_LENGTH_BYTES, src, HASH_LENGTH_BYTES)
#else
  #define COPYHASH(src) memcpy(m_data, src, HASH_LENGTH_BYTES)
#endif // Q_OS_

DataHash::DataHash()
{
  memset(m_data, 0, HASH_LENGTH_BYTES);
  m_numHash = 0;
  m_hexString = QString('0', 2 * HASH_LENGTH_BYTES);
}

DataHash::DataHash(const QByteArray &data)
{
  if (data.length() != HASH_LENGTH_BYTES)
    throw std::runtime_error("Invalid hash length");

  COPYHASH(data.data());

  m_numHash = 0;
  for (int idx = 0; idx < HASH_LENGTH_BYTES; idx++)
    m_numHash += m_data[idx];

  m_hexString = data.toHex();
}

DataHash::DataHash(const DataHash &other)
{
  COPYHASH(other.m_data);
  m_numHash = other.m_numHash;
  m_hexString = other.m_hexString;
}

uint DataHash::num() const
{
  return m_numHash;
}

const QString & DataHash::toString() const
{
  return m_hexString;
}

DataHash & DataHash::operator=(const DataHash &other)
{
  COPYHASH(other.m_data);
  m_numHash = other.m_numHash;
  m_hexString = other.m_hexString;

  return *this;
}

bool DataHash::operator==(const DataHash &other) const
{
  return memcmp(m_data, other.m_data, HASH_LENGTH_BYTES) == 0;
}

uint qHash(const DataHash &dh, uint seed)
{
  return dh.num() ^ seed;
}
