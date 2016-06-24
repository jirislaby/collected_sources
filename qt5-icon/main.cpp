#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QSystemTrayIcon>

#include "main.h"

my_tray::my_tray(const QIcon &icon) : QSystemTrayIcon(icon), menu() {
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(activate(QSystemTrayIcon::ActivationReason)));
	installEventFilter(this);

	connect(menu.addAction("ahoj"), &QAction::triggered,
			QApplication::instance(),
			&QApplication::aboutQt);
	menu.addSeparator();
	connect(menu.addAction("vogo"), &QAction::triggered,
			QCoreApplication::instance(), &QCoreApplication::quit);
	setContextMenu(&menu);
}

bool my_tray::eventFilter(QObject *obj, QEvent *event)
{
	std::cout << __func__ << "\n";
	qDebug() << __func__ << obj << event;
	return QObject::eventFilter(obj, event);
}

void my_tray::activate(QSystemTrayIcon::ActivationReason reason)
{
	qDebug() << __func__ << reason;
//	QSystemTrayIcon::activated(reason);
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QIcon icon("/usr/share/icons/hicolor/32x32/apps/package_edutainment_chemical.png");
	my_tray tray(icon);

	qDebug() << "ahoj";

	tray.show();

	return app.exec();
}
