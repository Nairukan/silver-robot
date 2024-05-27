#ifndef VIRTUALWINDOW_H
#define VIRTUALWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "object.h"

class VirtualWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VirtualWindow(QWidget *parent = nullptr, QRect Desctop = QRect(0, 0, 0, 0))
        : QMainWindow(parent)
    {}
    Object *SelectObject = nullptr;
};

#endif // VIRTUALWINDOW_H
