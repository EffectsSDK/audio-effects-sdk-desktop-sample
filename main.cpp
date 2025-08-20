#include <QApplication>

#include "ui/main_window.h"
#include "core/core.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    core::Core core;
    ui::MainWindow window(&core);
    window.show();

    return app.exec();
}
