#include "linechart.h"

#include <QRandomGenerator>

#define MAX_X 100

CLineChart::CLineChart(QWidget* parent)
    : QChartView(parent)
    , m_series_red(nullptr)
    , m_series_green(nullptr)
    , m_axisX(new QValueAxis())
    , m_axisY(new QValueAxis())
    , m_step(0)
    , m_x(0)
    , m_y(1)
{
    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->createDefaultAxes();
    m_chart->setTitle("Simple line chart example");
    // m_chart->setAnimationOptions(QChart::AllAnimations);

    // this->setRenderHint(QPainter::Antialiasing);
    this->setChart(m_chart);

    m_series_red = new QSplineSeries(this);
    QPen red(Qt::red);
    red.setWidth(3);
    m_series_red->setPen(red);
    m_series_red->append(m_x, m_y);

    m_series_green = new QSplineSeries(this);
    QPen green(Qt::green);
    green.setWidth(3);
    m_series_green->setPen(green);
    m_series_green->append(m_x, m_y);

    m_chart->addSeries(m_series_red);
    m_chart->addSeries(m_series_green);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_axisX->setTickCount(5);
    m_axisX->setRange(0, MAX_X);
    m_axisY->setRange(-500, 500);

    m_series_red->attachAxis(m_axisX);
    m_series_red->attachAxis(m_axisY);

    m_series_green->attachAxis(m_axisX);
    m_series_green->attachAxis(m_axisY);
}

void CLineChart::WriteBuffer(const uint8_t* buffer, const int& size)
{
    (void*)buffer;
    (void)size;
}

void CLineChart::Clear()
{
    m_x = 0;
    m_series_red->clear();
    m_series_green->clear();
}

void CLineChart::AddValue(const qreal& xx, const qreal& yy, const Qt::GlobalColor& color)
{
    (void)xx;

    qreal x = m_chart->plotArea().width() / m_axisX->tickCount();
    qreal y = (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
    m_x++; //  += y;
    m_y = yy;

    switch(color) {
        case Qt::red:
            m_series_red->append(m_x, m_y);
            break;
        case Qt::green:
            m_series_green->append(m_x, m_y);
            break;
        default:
            break;
    }

    if(m_x > MAX_X)
        Clear();

    // m_chart->scroll(x, 0);
    // if (m_x == 100)
    // m_timer.stop();
}

QSize CLineChart::sizeHint() const
{
    return this->size();
}