#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QSystemTrayIcon>

#include "main.h"

my_tray::my_tray(const QIcon &icon) : QSystemTrayIcon(icon) {
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(activate(QSystemTrayIcon::ActivationReason)));
	installEventFilter(this);
}

bool my_tray::eventFilter(QObject *obj, QEvent *event)
{
	qDebug() << __func__ << (obj == this) << event->type();
	return QObject::eventFilter(obj, event);
}

void my_tray::activate(QSystemTrayIcon::ActivationReason reason)
{
	qDebug() << __func__ << reason;
//	if (reason == QSystemTrayIcon::Context)
		abort();
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QIcon icon("/usr/share/icons/hicolor/32x32/apps/package_edutainment_chemical.png");
	my_tray tray(icon);

	tray.show();

	return app.exec();
}
