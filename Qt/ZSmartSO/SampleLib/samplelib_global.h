#ifndef SAMPLELIB_GLOBAL_H
#define SAMPLELIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SAMPLELIB_LIBRARY)
#  define SAMPLELIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SAMPLELIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SAMPLELIB_GLOBAL_H