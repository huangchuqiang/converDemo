#include "converdemo.h"
#include <QtGui/QApplication>

#pragma comment(lib, "GdiPlus.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	converDemo w;
	w.show();
	return a.exec();
}
