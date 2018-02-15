#include "MainWindow.h"
#include "sensordata.h"
#include "LineChart.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>

using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    laser1LineChart = new LineChart({Qt::red});
    laser2LineChart = new LineChart({Qt::red});
    laser3LineChart = new LineChart({Qt::red});
    ultrasoundLineChart = new LineChart({Qt::red});

    laser1LineChart->setTitle("Laser 1");
    laser2LineChart->setTitle("Laser 2");
    laser3LineChart->setTitle("Laser 3");
    ultrasoundLineChart->setTitle("Ultrasound");

    ultrasoundLineChart->setRange(0, 800); // 800 cm max
    overviewBarChart = new QChart();


    auto *vLayout = new QVBoxLayout();

    auto laser1ChartView = new QChartView(laser1LineChart);
    auto laser2ChartView = new QChartView(laser2LineChart);
    auto laser3ChartView = new QChartView(laser3LineChart);
    auto ultrasoundChartView = new QChartView(ultrasoundLineChart);

    vLayout->addWidget(laser1ChartView);
    vLayout->addWidget(laser2ChartView);
    vLayout->addWidget(laser3ChartView);

    laser1Set = new QBarSet("Laser 1");
    laser2Set = new QBarSet("Laser 2");
    laser3Set = new QBarSet("Laser 3");
    ultrasoundSet = new QBarSet("Ultrasound");

    (*laser1Set) << 0;
    (*laser2Set) << 0;
    (*laser3Set) << 0;
    auto *barSeries = new QBarSeries();
    barSeries->append(laser1Set);
    barSeries->append(laser2Set);
    barSeries->append(laser3Set);
    barSeries->append(ultrasoundSet);

    overviewBarChart->addSeries(barSeries);

    QBarCategoryAxis *barAxis = new QBarCategoryAxis();
    barAxis->append(QStringList() << "sensors");

    overviewBarChart->createDefaultAxes();
    overviewBarChart->setAxisX(barAxis, barSeries);
    overviewBarChart->axisY()->setRange(0, BARCHART_INITIAL_MAX);

    QChartView *barView = new QChartView(overviewBarChart);

    auto vLayout2 = new QVBoxLayout();
    vLayout2->addWidget(ultrasoundChartView);
    vLayout2->addWidget(barView);

    auto hLayout = new QHBoxLayout();
    hLayout->addLayout(vLayout);
    hLayout->addLayout(vLayout2);

    ui->sensorBox->setLayout(hLayout);

    serial = new QSerialPort();

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    serialRefreshtimer.setInterval(1000);
    connect(&serialRefreshtimer, &QTimer::timeout, this, &MainWindow::RefreshCOMPorts);
    serialRefreshtimer.start();

    connect (serial, &QSerialPort::readyRead, this, &MainWindow::SampleData);
}


QSerialPort::BaudRate StrTobaud(const QString &str)
{
    if (str == "9600")   return QSerialPort::Baud9600;
    if (str == "115200") return QSerialPort::Baud115200;

    // Default to 9600
    return QSerialPort::Baud9600;
}

MainWindow::~MainWindow()
{
    serial->close();
    delete ui;
}

void MainWindow::RefreshCOMPorts()
{
    ui->COMPorts->clear();
    for (auto Port : QSerialPortInfo::availablePorts())
        ui->COMPorts->addItem(Port.portName());
}

void MainWindow::SampleData()
{
    QByteArray data;
    //Find the starting 's' char
    while (serial->peek(1)[0] != 's' && serial->bytesAvailable() > 1)
        serial->read(1);

    if (serial->bytesAvailable() < sizeof(SensorData) + 1) return;

    //consume the starting byte
    serial->read(1);
    data = serial->read(sizeof(SensorData));

    if (data.size() != sizeof(SensorData))
        return;

    SensorData sdata = *(SensorData*)(data.constData());

    double elapsed = elapsedTimer.elapsed()/1000.0;
    (*ultrasoundLineChart) << QPointF{elapsed, sdata.ultrasound_d};
    (*laser1LineChart) << QPointF{elapsed, sdata.laser1_d};
    (*laser2LineChart) << QPointF{elapsed, sdata.laser2_d};
    (*laser3LineChart) << QPointF{elapsed, sdata.laser3_d};


    // Update the bar charts
    // FIXME: Create a derived class of QChart?
    laser1Set->replace(0, sdata.laser1_d);
    laser2Set->replace(0, sdata.laser2_d);
    laser3Set->replace(0, sdata.laser3_d);
    ultrasoundSet->replace(0, sdata.ultrasound_d);

    auto maxVal = std::max({sdata.laser1_d, sdata.laser2_d, sdata.laser3_d, sdata.ultrasound_d});

    if (maxVal > prevSensorMax || prevSensorMax > maxVal * 1.3f) {
        overviewBarChart->axisY()->setRange(0, maxVal * 1.3f);
        prevSensorMax = maxVal * 1.3f;
    }

}

void MainWindow::on_connectButton_clicked()
{
    if (serial->isOpen()) {
       // We are trying to close the connection.
        serial->close();
        ui->connectButton->setText("connect");
    } else {
        serial->setBaudRate(StrTobaud(ui->BAUDRates->currentText()));
        serial->setPortName(ui->COMPorts->currentText());
        if(!serial->open(QIODevice::ReadWrite)){
            QMessageBox::critical(0, "Error", "Could not open this port");
            return;
        }
        ui->connectButton->setText("disconnect");
        ClearAllCharts();
        prevSensorMax = BARCHART_INITIAL_MAX;
        elapsedTimer.restart();
    }
}

void MainWindow::ClearAllCharts()
{
    laser1LineChart->clear();
    laser2LineChart->clear();
    laser3LineChart->clear();
}
