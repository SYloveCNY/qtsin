#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QtMath>

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // 重写绘图事件
    void paintEvent(QPaintEvent *event) override;
    // 重写鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;
    // 重写鼠标拖动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    // 重写鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // 判断鼠标点是否命中曲线
    bool isPointOnCurve(const QPointF& mousePos);
    // 计算点到线段的最短距离
    double pointToLineDistance(const QPointF& p, const QPointF& p1, const QPointF& p2);

    // 曲线参数
    double m_xMin;           // x轴数学范围（起始）
    double m_xMax;           // x轴数学范围（结束）
    double m_yAmp;           // y轴振幅（控制高度）
    const int m_pointCount;  // 采样点数量（固定为500）
    const int m_hitThreshold;  // 点击命中阈值（5px）

    // 鼠标状态变量
    bool m_isMouseDown;      // 鼠标是否按下
    bool m_isHitCurve;       // 是否点中曲线
    QPointF m_mouseStartPos; // 鼠标按下初始位置
    double m_startXMin;      // 按下时的x范围（起始）
    double m_startXMax;      // 按下时的x范围（结束）
    double m_startYAmp;      // 按下时的y振幅
};
#endif // WIDGET_H
