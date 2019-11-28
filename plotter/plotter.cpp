#include "plotter.h"

#include <QIcon>
#include <QToolButton>

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

}

void Plotter::setCurveData(int id, const QVector<QPointF>& data)
{

}

void Plotter::clearCurve(int id)
{

}

QSize Plotter::minimumSizeHint() const
{

}

QSize Plotter::sizeHint() const
{

}

void Plotter::zoomIn()
{

}

void Plotter::zoomOut()
{

}

void Plotter::paintEvent(QPaintEvent* event)
{

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

