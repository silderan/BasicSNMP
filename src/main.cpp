#include "mainwindow.h"
#include <QApplication>

#include <lib/snmplib.h>
#include "snmptests.h"

int main(int argc, char *argv[])
{
//	SNMPTests::doTests();

//	return 0;
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
