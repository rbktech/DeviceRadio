#pragma once

#include <QSplineSeries>
#include <QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using namespace QtCharts;

class CLineChart : public QChartView
{
    Q_OBJECT

private:
    QSplineSeries* m_series_red;
    QSplineSeries* m_series_green;
    QStringList m_titles;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    qreal m_step;
    qreal m_x;
    qreal m_y;

    QChart* m_chart;

    QSize sizeHint() const override;

public:
    explicit CLineChart(QWidget* parent);
    ~CLineChart() override = default;

    void WriteBuffer(const uint8_t* buffer, const int& size);

    void AddValue(const qreal& x, const qreal& y, const Qt::GlobalColor& color);

    void Clear();
};