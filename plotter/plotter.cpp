#include "plotter.h"

#include <QIcon>
#include <QToolButton>
#include <QStylePainter>
#include <QMouseEvent>
#include <QStyleOptionFocusRect>
#include <QPen>

#include <cmath>

namespace plt {

Plotter::Plotter(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    _rubberBandIsShown = false;

    _pZoomInBtn = new QToolButton(this);
    _pZoomInBtn->setIcon(QIcon(":/images/zoomIn"));
    _pZoomInBtn->adjustSize();
    connect(_pZoomInBtn, &QToolButton::clicked,
            this,        &Plotter::zoomIn);

    _pZoomOutBtn = new QToolButton(this);
    _pZoomOutBtn->setIcon(QIcon(":/images/zoomOut"));
    _pZoomOutBtn->adjustSize();
    connect(_pZoomOutBtn, &QToolButton::clicked,
            this,         &Plotter::zoomOut);

    setPlotSettings(PlotSettings());
}

void Plotter::setPlotSettings(const PlotSettings& settings)
{
    _zoomStack.clear();
    _zoomStack.append(settings);
    _curZoom = 0;
    _pZoomInBtn->hide();
    _pZoomOutBtn->hide();
    refreshPixmap();
}

void Plotter::setCurveData(int id, const QVector<QPointF>& data)
{
    _curveMap[id] = data;
    refreshPixmap();
}

void Plotter::clearCurve(int id)
{
    _curveMap.remove(id);
    refreshPixmap();
}

QSize Plotter::minimumSizeHint() const
{
    return QSize(6 * MARGIN, 4 * MARGIN);
}

QSize Plotter::sizeHint() const
{
    return QSize(12 * MARGIN, 8 * MARGIN);
}

void Plotter::zoomIn()
{
    if (_curZoom < _zoomStack.count() - 1) {
        ++_curZoom;
        _pZoomInBtn->setEnabled(_zoomStack.count() - 1);
        _pZoomOutBtn->setEnabled(true);
        _pZoomOutBtn->show();
        refreshPixmap();
    }
}

void Plotter::zoomOut()
{
    if (_curZoom > 0 ) {
        --_curZoom;
        _pZoomOutBtn->setEnabled(_curZoom > 0);
        _pZoomInBtn->setEnabled(true);
        _pZoomInBtn->show();
        refreshPixmap();
    }
}

void Plotter::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, _pixmap);

    if (_rubberBandIsShown) {
        painter.setPen(palette().light().color());
        painter.drawRect(_rubberBandRect.normalized().adjusted(0, 0, -1, -1));
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void Plotter::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    int x = width() - (_pZoomInBtn->width() + _pZoomOutBtn->width() + 10);
    _pZoomInBtn->move(x, 5);
    _pZoomOutBtn->move(x + _pZoomInBtn->width() + 5, 5);
    refreshPixmap();
}

void Plotter::mousePressEvent(QMouseEvent* event)
{
    QRect rect(MARGIN, MARGIN, width() - 2 * MARGIN, height() - 2 * MARGIN);

    if (event->button() != Qt::LeftButton){
        return;
    }
    if (!rect.contains(event->pos())) {
        return;
    }

    _rubberBandIsShown = true;
    _rubberBandRect.setTopLeft(event->pos());
    _rubberBandRect.setBottomRight(event->pos());
    updateRubberBandRegion();
    setCursor(Qt::CrossCursor);
}

void Plotter::mouseMoveEvent(QMouseEvent* event)
{
    if (_rubberBandIsShown == false) {
        return;
    }
    updateRubberBandRegion();
    _rubberBandRect.setBottomRight(event->pos());
    updateRubberBandRegion();
}

void Plotter::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton) && _rubberBandIsShown) {
        _rubberBandIsShown = false;
        unsetCursor();
        QRect rect = _rubberBandRect.normalized();
        if ((rect.width() < 4) || (rect.height() < 4)) {
            return;
        }
        rect.translate(-MARGIN, -MARGIN);
        PlotSettings prevSettings = _zoomStack[_curZoom];
        PlotSettings settings;
        double dx = prevSettings.spanX() / (width() - 2 * MARGIN);
        double dy = prevSettings.spanY() / (height() - 2 * MARGIN);
        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.minY + dy * rect.bottom();
        settings.maxY = prevSettings.minY + dy * rect.top();
        settings.adjust();
        _zoomStack.resize(_curZoom + 1);
        _zoomStack.append(settings);
        zoomIn();
    }
}

void Plotter::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        return;
    case Qt::Key_Minus:
        zoomOut();
        return;
    case Qt::Key_Left:
        _zoomStack[_curZoom].scroll(-1, 0);
        refreshPixmap();
        return;
    case Qt::Key_Right:
        _zoomStack[_curZoom].scroll(+1, 0);
        refreshPixmap();
        return;
    case Qt::Key_Down:
        _zoomStack[_curZoom].scroll(0, -1);
        refreshPixmap();
        return;
    case Qt::Key_Up:
        _zoomStack[_curZoom].scroll(0, +1);
        refreshPixmap();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Plotter::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;

    if (event->orientation() == Qt::Horizontal) {
        _zoomStack[_curZoom].scroll(numTicks, 0);
    }
    else {
        _zoomStack[_curZoom].scroll(0, numTicks);
    }
    refreshPixmap();
}

void Plotter::updateRubberBandRegion()
{
    QRect rect = _rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void Plotter::refreshPixmap()
{
    _pixmap = QPixmap(size());
//	_pixmap.fill(this, 0, 0);
    _pixmap.fill(backgroundRole());
    QPainter painter(&_pixmap);
    painter.begin(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}

void Plotter::drawGrid(QPainter* painter)
{
    QRect rect (MARGIN, MARGIN, width() - 2 * MARGIN, height() - 2 * MARGIN);
    if (!rect.isValid()) {
        return;
    }

    PlotSettings settings = _zoomStack[_curZoom];
    QPen quiteDark = palette().dark().color().lighter();
    QPen light     = palette().light().color();

    for (int i = 0; i < settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1) / settings.numXTicks);
        double lbl = settings.minX + (i * settings.spanX()/settings.numXTicks);
        painter->setPen(quiteDark);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
        painter->drawLine(x, rect.bottom() + 5, x, rect.bottom()+ 5);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(lbl));
    }

    for (int j = 0; j < settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)/settings.numYTicks);
        double lbl = settings.minY + (j * settings.spanY()/settings.numYTicks);

        painter->setPen(quiteDark);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);

        painter->drawText(rect.left() - MARGIN, y -10, MARGIN -5, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(lbl));
    }
    painter->drawRect(rect.adjusted(0,0,-1,-1));
}

void Plotter::drawCurves(QPainter* painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    PlotSettings settings = _zoomStack[_curZoom];
    QRect rect(MARGIN, MARGIN,
               width() - 2 * MARGIN, height() - 2 * MARGIN);
    if (!rect.isValid()) {
        return;
    }

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    QMapIterator<int, QVector<QPointF> > i(_curveMap);
    while (i.hasNext()) {
        i.next();

        int id = i.key();
        QVector<QPointF> data = i.value();
        QPolygonF polyline(data.count());

        for (int j = 0; j < data.count(); ++j) {
            double dx = data[j].x() - settings.minX;
            double dy = data[j].y() - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                      / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                        / settings.spanY());
            polyline[j] = QPointF(x, y);
        }
        painter->setPen(colorForIds[uint(id) % 6]);
        painter->drawPolyline(polyline);
    }
}

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 10.0;
    numXTicks = 5;

    minY = 0.0;
    maxY = 10.0;
    numYTicks = 5;
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double& min, double& max, int& numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    min = std::floor(min / step) * step;
    max = std::ceil(max / step) * step;
}

} // namespace plt

