#ifndef QCUSTOMHORIZONTALHEADER_H
#define QCUSTOMHORIZONTALHEADER_H

#include <QWidget>
#include <QHeaderView>

class QCustomTableHeader : public QHeaderView
{
public:
	struct Margins
	{
		int left;
		int right;
		int top;
		int buttom;

		Margins(int left = 2, int right = 2, int top = 2, int buttom = 2);
	};

	QCustomTableHeader(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);

	void FixComboPositions();

	void SetItemWidget(int index, QWidget * widget);
	void SetItemMargins(int index, Margins margins);

private:
	struct Item
	{
		QWidget * item;
		Margins margins;

		Item();
	};

	QMap<int, Item> mItems;

	void showEvent(QShowEvent *e);
	void HandleSectionResized(int i);
	void HandleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);
};

#endif // QCUSTOMHORIZONTALHEADER_H
