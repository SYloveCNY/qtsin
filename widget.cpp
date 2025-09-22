#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    m_pointCount(500),          // 采样点数量固定为500
    m_hitThreshold(5.0),        // 点击命中阈值（5px）
    m_isMouseDown(false),
    m_isHitCurve(false) {
    // 初始曲线参数（2个周期）
    m_xMin = 0.0;
    m_xMax = 4 * M_PI;  // 4π ≈ 12.56，包含2个完整周期
    m_yAmp = 1.0;       // 初始振幅

    // 设置控件属性
    setFixedSize(800, 400);       // 初始大小
    setStyleSheet("background-color: white;");  // 白色背景
    setWindowTitle("可缩放的Sin曲线");          // 窗口标题

}

void Widget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);  // 忽略事件参数

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);  // 抗锯齿

    const double padding = 20.0;  // 边距
    const double widgetW = width() - 2 * padding;   // 有效宽度
    const double widgetH = height() - 2 * padding;  // 有效高度

    // 生成曲线采样点
    QPointF points[m_pointCount];
    for (int i = 0; i < m_pointCount; ++i)
    {
        // 计算数学坐标
        double xMath = m_xMin + (m_xMax - m_xMin) * i / (m_pointCount - 1);
        double yMath = sin(xMath) * m_yAmp;

        // 映射到屏幕坐标
        double xScreen = padding + (xMath - m_xMin) * widgetW / (m_xMax - m_xMin);
        double yScreen = padding + (widgetH / 2) - yMath * (widgetH / 2 - padding);
        points[i] = QPointF(xScreen, yScreen);
    }

    // 绘制曲线（红色，2px宽）
    QPen curvePen(Qt::red, 2);
    painter.setPen(curvePen);
    painter.drawPolyline(points, m_pointCount);

    // 绘制坐标轴（灰色，1px宽）
    QPen axisPen(Qt::gray, 1);
    painter.setPen(axisPen);
    double xAxisY = padding + widgetH / 2;  // x轴y坐标（垂直居中）
    painter.drawLine(QPointF(padding, xAxisY), QPointF(padding + widgetW, xAxisY));  // x轴
    painter.drawLine(QPointF(padding, padding), QPointF(padding, padding + widgetH));  // y轴
}

// 鼠标按下事件：判断是否点中曲线
void Widget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_mouseStartPos = event->pos();  // 记录初始位置
        m_startXMin = m_xMin;            // 保存当前曲线参数
        m_startXMax = m_xMax;
        m_startYAmp = m_yAmp;
        m_isHitCurve = isPointOnCurve(event->pos());  // 判断是否点中曲线
        m_isMouseDown = true;
    }
}

// 鼠标拖动事件：缩放曲线（仅点中曲线时生效）
void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isMouseDown && m_isHitCurve) {
        // 计算拖动偏移量
        QPointF delta = event->pos() - m_mouseStartPos;

        // 横向拖动：调整x轴范围（控制周期数量）
        double xRangeDelta = delta.x() * 0.05;  // 缩放系数
        m_xMin = m_startXMin - xRangeDelta;
        m_xMax = m_startXMax + xRangeDelta;
        m_xMax = qMax(m_xMin + 2 * M_PI, m_xMax);  // 至少保留1个周期

        // 纵向拖动：调整y轴振幅（控制曲线高度）
        double yAmpDelta = -delta.y() * 0.01;  // 负号：y轴方向反转
        m_yAmp = m_startYAmp + yAmpDelta;
        m_yAmp = qBound(0.1, m_yAmp, 5.0);  // 限制振幅范围

        update();  // 触发重绘
    }
}

// 鼠标释放事件：重置状态
void Widget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isMouseDown = false;
        m_isHitCurve = false;
    }
}

// 判断鼠标点是否命中曲线
bool Widget::isPointOnCurve(const QPointF& mousePos) {
    const double padding = 20.0;
    const double widgetW = width() - 2 * padding;
    const double widgetH = height() - 2 * padding;

    // 遍历所有线段，判断是否有线段距离鼠标点小于阈值
    for (int i = 0; i < m_pointCount - 1; ++i) {
        // 计算线段端点坐标
        double xMath1 = m_xMin + (m_xMax - m_xMin) * i / (m_pointCount - 1);
        double yMath1 = sin(xMath1) * m_yAmp;
        QPointF p1(
            padding + (xMath1 - m_xMin) * widgetW / (m_xMax - m_xMin),
            padding + (widgetH / 2) - yMath1 * (widgetH / 2 - padding) / m_yAmp
            );

        double xMath2 = m_xMin + (m_xMax - m_xMin) * (i+1) / (m_pointCount - 1);
        double yMath2 = sin(xMath2) * m_yAmp;
        QPointF p2(
            padding + (xMath2 - m_xMin) * widgetW / (m_xMax - m_xMin),
            padding + (widgetH / 2) - yMath2 * (widgetH / 2 - padding) / m_yAmp
            );

        // 计算距离，小于阈值则命中
        if (pointToLineDistance(mousePos, p1, p2) < m_hitThreshold) {
            return true;
        }
    }
    return false;
}

// 计算点到线段的最短距离
double Widget::pointToLineDistance(const QPointF& p, const QPointF& p1, const QPointF& p2) {
    // 线段向量
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    // 点到线段起点的向量
    double px = p.x() - p1.x();
    double py = p.y() - p1.y();

    // 计算投影比例（0~1表示在seg内）
    double dot = px * dx + py * dy;  // 点积
    double segLenSq = dx * dx + dy * dy;  // 线段长度平方
    double t = segLenSq == 0 ? 0 : dot / segLenSq;
    t = qBound(0.0, t, 1.0);  // 限制在0~1

    // 投影点坐标
    double projX = p1.x() + t * dx;
    double projY = p1.y() + t * dy;

    // 计算距离（勾股定理）
    double distX = p.x() - projX;
    double distY = p.y() - projY;
    return sqrt(distX * distX + distY * distY);
}

Widget::~Widget() {}
