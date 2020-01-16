#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QTimer>

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

    void on_Calibration_clicked();
    void execute_repeat();

    void on_Connection_clicked();

    void on_backmain_clicked();

private:
    Ui::OverlayWidget *ui;
    QTimer *timer;
    QPrinter *printdevicepose;
    bool isconn;
};

#endif // OVERLAYWIDGET_H
