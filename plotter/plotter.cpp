#include "plotter.h"

#include <QIcon>
#include <QToolButton>
#include <QStylePainter>
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

}

void Plotter::mousePressEvent(QMouseEvent* event)
{

}

void Plotter::mouseMoveEvent(QMouseEvent* event)
{

}

void Plotter::mouseReleaseEvent(QMouseEvent* event)
{

}

void Plotter::keyPressEvent(QKeyEvent* event)
{

}

void Plotter::wheelEvent(QWheelEvent* event)
{

}

void Plotter::updateRubberBandRegion()
{

}

void Plotter::refreshPixmap()
{

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

