#pragma once

#include <QSplineSeries>
#include <QTimer>
#include <QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using namespace QtCharts;

class CLineChart : public QChart
{
private:
    QSplineSeries* m_series;
    QStringList m_titles;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    qreal m_step;
    // qreal m_x;
    // qreal m_y;

public:
    CLineChart();
    ~CLineChart() override = default;

    void WriteBuffer(uint8_t* buffer, const int& size);
};