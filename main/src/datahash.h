#ifndef DATAHASH_H
#define DATAHASH_H

#define HASH_LENGTH_BYTES 20

#include <QByteArray>
#include <QHash>
#include <QMetaType>

class DataHash {
public:
  DataHash();
  DataHash(const DataHash &other);
  explicit DataHash(const QByteArray &data);

  uint num() const;
  const QString & toString() const;

  DataHash & operator=(const DataHash &other);
  bool operator==(const DataHash &other) const;

private:
  uint8_t m_data[HASH_LENGTH_BYTES];
  uint m_numHash;
  QString m_hexString;
};
Q_DECLARE_METATYPE(DataHash)

uint qHash(const DataHash &dh, uint seed);

#endif // DATAHASH_H
