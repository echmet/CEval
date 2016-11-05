#ifndef INUMBERFORMATCHANGEABLE_H
#define INUMBERFORMATCHANGEABLE_H

class QLocale;

class INumberFormatChangeable {
public:
  virtual void onNumberFormatChanged(const QLocale *oldLocale) = 0;
  //virtual ~INumberFormatChangeable();

};

Q_DECLARE_INTERFACE(INumberFormatChangeable, "CEval.INumberFormatChangeable/1.0")

#endif // INUMBERFORMATCHANGEABLE_H
