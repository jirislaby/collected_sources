#ifndef MAIN_H
#define MAIN_H

#include <QWheelEvent>
#include <QtWidgets/QListWidget>

class my_list : public QListWidget {
    Q_OBJECT
public:
    my_list() : QListWidget() { }

    void wheelEvent(QWheelEvent *we);
};

#endif
