#include "linechart.h"

#include <QRandomGenerator>

CLineChart::CLineChart()
    : QChart()
    , m_series(nullptr)
    , m_axisX(new QValueAxis())
    , m_axisY(new QValueAxis())
    , m_step(0)
// m_x(5),
// m_y(1)
{
    this->legend()->hide();
    this->createDefaultAxes();
    this->setTitle("Simple line chart example");
    // this->setAnimationOptions(QChart::AllAnimations);

    m_series = new QSplineSeries(this);
    QPen green(Qt::red);
    green.setWidth(3);
    m_series->setPen(green);
    // m_series->append(m_x, m_y);

    addSeries(m_series);

    addAxis(m_axisX, Qt::AlignBottom);
    addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    m_axisX->setTickCount(5);
    m_axisX->setRange(0, 1024);
    m_axisY->setRange(110, 150);
}

void CLineChart::WriteBuffer(uint8_t* buffer, const int& size)
{
    qreal x = plotArea().width() / m_axisX->tickCount();
    qreal y = (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
    // m_x += y;
    // m_y = QRandomGenerator::global()->bounded(5) - 2.5;

    m_series->clear();
    for(int i = 0; i < 1024; i++) {
        m_series->append(i, buffer[i]);
    }
}
