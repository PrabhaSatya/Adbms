#include "CacheUI.h"
#include "Cache.h"

int main ()
{
    Cache* objCache = NULL;
    CacheUI objUI;
    while ( objUI.runConsoleUI(objCache) );

    return 0;
}

/*

#include <QApplication>
#include <iostream>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

*/
