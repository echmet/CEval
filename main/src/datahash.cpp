#include "datahash.h"
#include <cstring>

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

  memcpy_s(m_data, HASH_LENGTH_BYTES, data.data(), HASH_LENGTH_BYTES);

  m_numHash = 0;
  for (int idx = 0; idx < HASH_LENGTH_BYTES; idx++)
    m_numHash += m_data[idx];

  m_hexString = data.toHex();
}

DataHash::DataHash(const DataHash &other)
{
  memcpy_s(m_data, HASH_LENGTH_BYTES, other.m_data, HASH_LENGTH_BYTES);
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
  memcpy_s(m_data, HASH_LENGTH_BYTES, other.m_data, HASH_LENGTH_BYTES);
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
