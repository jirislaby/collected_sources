#ifndef MAIN_H
#define MAIN_H

#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

class my_tray : public QSystemTrayIcon {
	Q_OBJECT
public:
	my_tray(const QIcon &icon);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void activate(QSystemTrayIcon::ActivationReason reason);
private:
	QMenu menu;
};

#endif
