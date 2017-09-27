#include "RocketLauncherControlCenter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RocketLauncherControlCenter w;
	w.show();
	return a.exec();
}
