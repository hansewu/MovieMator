#ifndef VIDEOFILTERDOCK_GLOBAL_H
#define VIDEOFILTERDOCK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VIDEOFILTERDOCK_LIBRARY)
#  define VIDEOFILTERDOCKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VIDEOFILTERDOCKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // VIDEOFILTERDOCK_GLOBAL_H
