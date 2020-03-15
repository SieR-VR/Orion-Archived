#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <Windows.h>
#include <QtWidgets/QWidget>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QPalette>
#include <QTimer>

enum TrackerType {
    CHEST = 1, WAIST, RIGHT_THIGH, LEFT_THIGH, RIGHT_CALF, LEFT_CALF, RIGHT_FOOT, LEFT_FOOT
};

struct OverlayShereMem {
    bool connecting[9];
    bool connection[9];
    double position[3][9];
    bool calibration;
};

namespace Ui {
class OverlayWidget;
}

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget *parent = 0);
    ~OverlayWidget();

private slots:
    void execute_repeat();
    void on_btnConnection_clicked();
    void on_btnbackmain_clicked();
    void on_btnQuit_clicked();

private:
    Ui::OverlayWidget *ui;

    QTimer *timer;
    QPalette RedPalette;
    QPalette GreenPalette;

    HANDLE hMemoryMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"SH_MEM");
    LPBYTE pMemoryMap = (BYTE*)MapViewOfFile(hMemoryMap, FILE_MAP_ALL_ACCESS,0, 0, 0);
    OverlayShereMem* pStruct = (OverlayShereMem*)pMemoryMap;
};
#endif // OVERLAYWIDGET_H
