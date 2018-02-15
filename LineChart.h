#ifndef LINECHART_H
#define LINECHART_H

#include <QtCharts/QChart>
#include <QPen>
#include <QPointF>

namespace QtCharts
{
class QLineSeries;
class QValueAxis;
class QXYSeries;
}

using namespace QtCharts;

class LineChart : public QtCharts::QChart
{
public:
    LineChart(QPen pen);

    QXYSeries &operator<<(QPointF p);

    void clear();

    void setRange(qreal x, qreal y)
    {
        axisY()->setRange(x, y);
    }
private:
    QLineSeries *m_series;
    QValueAxis *m_axis;
};

#endif // LINECHART_H
