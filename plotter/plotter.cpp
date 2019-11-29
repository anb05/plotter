#include "plotter.h"

#include <QIcon>
#include <QToolButton>
#include <QStylePainter>
#include <QMouseEvent>
#include <QStyleOptionFocusRect>

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

}

void Plotter::drawCurves(QPainter* painter)
{

}

PlotSettings::PlotSettings()
{

}

void PlotSettings::scroll(int dx, int dy)
{

}

void PlotSettings::adjust()
{

}

void PlotSettings::adjustAxis(double& min, double& max, int& numTicks)
{

}

} // namespace plt

