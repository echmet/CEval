#ifndef INUMBERFORMATCHANGEABLE_H
#define INUMBERFORMATCHANGEABLE_H

#include "globals.h"
#include <QtPlugin>

class QLocale;

class INumberFormatChangeable {
public:
  virtual ~INumberFormatChangeable() {}
  virtual void onNumberFormatChanged(const QLocale *oldLocale) = 0;

};

Q_DECLARE_INTERFACE(INumberFormatChangeable, SOFTWARE_NAME_INTERNAL_S ".INumberFormatChangeable/1.0")

#endif // INUMBERFORMATCHANGEABLE_H
