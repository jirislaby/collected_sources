#include <QApplication>
#include <QWheelEvent>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

#include <iostream>

#include "main.h"

void my_list::wheelEvent(QWheelEvent *we)
{
    std::cout << "ahoj: " << we->angleDelta().y() << "\n";
    fflush(stdout);
    QListWidget::wheelEvent(we);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget w;
    my_list lst;
    const char dff = 'a' - 'A';

    for (char small = 0; small <= dff; small += dff)
        for (char c = 'A' + small; c <= 'Z' + small; c++)
            lst.addItem(QString(c));

    QHBoxLayout l;
    l.setMargin(0);
    l.addWidget(&lst);
    w.setLayout(&l);
    w.show();

    return a.exec();
}
