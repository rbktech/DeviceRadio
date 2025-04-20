#include <QApplication>

#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    CMainWindow mainWindow;

    mainWindow.show();

    return QApplication::exec();
}