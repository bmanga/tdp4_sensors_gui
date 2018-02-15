#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
namespace QtCharts
{
class QChart;
class QBarSet;
}

class LineChart;
class QSerialPort;

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void RefreshCOMPorts();
    void SampleData();
    void on_connectButton_clicked();

private:
    static constexpr float BARCHART_INITIAL_MAX = 5.f;
    float prevSensorMax = BARCHART_INITIAL_MAX;
    void ClearAllCharts();
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer serialRefreshtimer;
    QElapsedTimer elapsedTimer;

    QChart *overviewBarChart;
    LineChart *laser1LineChart;
    LineChart *laser2LineChart;
    LineChart *laser3LineChart;
    LineChart *ultrasoundLineChart;
    
    QBarSet *laser1Set;
    QBarSet *laser2Set;
    QBarSet *laser3Set;
    QBarSet *ultrasoundSet;

};

#endif // MAINWINDOW_H
