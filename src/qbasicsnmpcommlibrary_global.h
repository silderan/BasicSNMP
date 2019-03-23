/**************************************************************************

  Copyright 2015-2019 Rafael Dellà Bort. silderan (at) gmail (dot) com

  This file is part of BasicSNMP

  BasicSNMP is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  QMikPPPManager is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  and GNU Lesser General Public License. along with QMikPPPManager.
  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/
#ifndef QBASICSNMPCOMMLIBRARY_GLOBAL_H
#define QBASICSNMPCOMMLIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QBASICSNMPCOMMLIBRARY_LIBRARY)
#  define QBASICSNMPCOMMLIBRARYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QBASICSNMPCOMMLIBRARYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QBASICSNMPCOMMLIBRARY_GLOBAL_H
