#ifndef NETCDFSUPPORT_GLOBAL_H
#define NETCDFSUPPORT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NETCDFSUPPORT_LIBRARY)
#  define NETCDFSUPPORTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NETCDFSUPPORTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NETCDFSUPPORT_GLOBAL_H