#include "orionoverlaywidget.h"
#include "openvroverlaycontroller.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OverlayWidget *pOverlayWidget = new OverlayWidget;

    COpenVROverlayController::SharedInstance()->Init();

    COpenVROverlayController::SharedInstance()->SetWidget( pOverlayWidget );

    return a.exec();
}
