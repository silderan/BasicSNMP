#ifndef QCUSTOMHEADEREDTABLE_H
#define QCUSTOMHEADEREDTABLE_H

#include <QtCore/qglobal.h>
#include <QTableWidget>

#include "QCustomHorizontalHeader.h"

class QCustomHeaderedTable : public QTableWidget
{
	public:
		QCustomHeaderedTable(QWidget * parent = Q_NULLPTR);

		void scrollContentsBy(int dx, int dy);

		void SetHorizontalHeaderItemWidget(int column, QWidget * widget);
		void SetHorizontalHeaderItemMargins(int column, QCustomTableHeader::Margins margins);

	private:
		QCustomTableHeader * mHorizontalHeader;
};

#endif // QCUSTOMHEADEREDTABLE_H
