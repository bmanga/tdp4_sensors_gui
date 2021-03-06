#include "LineChart.h"

#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDebug>

LineChart::LineChart(QPen pen) : QtCharts::QChart(QChart::ChartTypeCartesian, nullptr, 0)
{
    m_series = new QLineSeries(this);
    m_axis = new QValueAxis();

    m_series->setPen(pen);

    addSeries(m_series);
    createDefaultAxes();
    setAxisX(m_axis, m_series);
    m_axis->setTickCount(5);
    axisX()->setRange(0,10);
    axisY()->setRange(0,130);

    legend()->hide();

    *m_series<< QPointF{0,0};
    *m_series<< QPointF{0,0};
}

QXYSeries &LineChart::operator<<(QPointF p)
{
    axisX()->setTitleText(QString().sprintf("%.2f", p.y()));
    auto elapsed = p.x();
    if (elapsed > 10) {
        // We dont really want to remove the points, we'll store them in csv
        // m_series->removePoints(0, 1);
        axisX()->setRange(elapsed - 10, elapsed);
    }

    return *m_series << p;
}

void LineChart::clear()
{
    m_series->clear();
    *m_series<< QPointF{0,0};
    *m_series<< QPointF{0,0};
    axisX()->setRange(0,10);
    zoomReset();
}
