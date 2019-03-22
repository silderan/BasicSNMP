#include "QCustomHorizontalHeader.h"

QCustomTableHeader::QCustomTableHeader(Qt::Orientation orientation, QWidget * parent)
	: QHeaderView(orientation, parent)
{
	connect(this, &QCustomTableHeader::sectionResized, this, &QCustomTableHeader::HandleSectionResized);
	connect(this, &QCustomTableHeader::sectionMoved,   this, &QCustomTableHeader::HandleSectionMoved);
}

void QCustomTableHeader::showEvent(QShowEvent *e)
{
	for( int i = 0; i < count(); i++ )
	{
		if( !mItems[i].item )
			mItems[i].item = new QWidget(this);
		else
			mItems[i].item->setParent(this);

		mItems[i].item->setGeometry(sectionViewportPosition(i) + mItems[i].margins.left,
									mItems[i].margins.top,
									sectionSize(i) - mItems[i].margins.left - mItems[i].margins.right - 1,
									height() - mItems[i].margins.top - mItems[i].margins.buttom - 1);
		mItems[i].item->show();
	}
	QHeaderView::showEvent(e);
}


void QCustomTableHeader::HandleSectionResized(int i)
{
	int logical;

	for( int j = visualIndex(i); j < count(); j++ )
	{
		logical = logicalIndex(j);
		mItems[logical].item->setGeometry(sectionViewportPosition(logical) + mItems[i].margins.left,
										  mItems[i].margins.top,
										  sectionSize(logical) - mItems[i].margins.left - mItems[i].margins.right - 1,
										  height() - mItems[i].margins.top - mItems[i].margins.buttom - 1);
	}
}


void QCustomTableHeader::HandleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex)
{
	Q_UNUSED(logical);

	for( int i = qMin(oldVisualIndex, newVisualIndex); i < count(); i++ )
	{
		int logical = logicalIndex(i);
		mItems[logical].item->setGeometry(sectionViewportPosition(logical) + mItems[i].margins.left,
										  mItems[i].margins.top,
										  sectionSize(logical) - mItems[i].margins.left - mItems[i].margins.right - 1,
										  height() - mItems[i].margins.top - mItems[i].margins.buttom - 1);
	}
}


void QCustomTableHeader::FixComboPositions()
{
	for( int i = 0; i < count(); i++ )
		mItems[i].item->setGeometry(sectionViewportPosition(i) + mItems[i].margins.left,
									mItems[i].margins.top,
									sectionSize(i) - mItems[i].margins.left - mItems[i].margins.right - 1,
									height() - mItems[i].margins.top - mItems[i].margins.buttom - 1);
}


void QCustomTableHeader::SetItemWidget(int index, QWidget * widget)
{
	widget->setParent(this);
	mItems[index].item = widget;
}


void QCustomTableHeader::SetItemMargins(int index, QCustomTableHeader::Margins margins)
{
	mItems[index].margins = margins;
}


QCustomTableHeader::Margins::Margins(int left, int right, int top, int buttom) :
	left(left),
	right(right),
	top(top),
	buttom(buttom)

{
}


QCustomTableHeader::Item::Item() :
	item(Q_NULLPTR)
{
}
