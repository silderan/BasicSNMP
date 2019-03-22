#include "QCustomHeaderedTable.h"

QCustomHeaderedTable::QCustomHeaderedTable(QWidget * parent)
	: QTableWidget(parent)
	, mHorizontalHeader(new QCustomTableHeader(Qt::Orientation::Horizontal, this))
{
	setHorizontalHeader(mHorizontalHeader);
}

void QCustomHeaderedTable::scrollContentsBy(int dx, int dy)
{
	QTableWidget::scrollContentsBy(dx, dy);

	if( dx != 0 )
		mHorizontalHeader->FixComboPositions();
}

void QCustomHeaderedTable::SetHorizontalHeaderItemWidget(int column, QWidget * widget)
{
	mHorizontalHeader->SetItemWidget(column, widget);
}

void QCustomHeaderedTable::SetHorizontalHeaderItemMargins(int column, QCustomTableHeader::Margins margins)
{
	mHorizontalHeader->SetItemMargins(column, margins);
}
