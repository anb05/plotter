#ifndef PLOTTER_H
#define PLOTTER_H

#include <QMap>
#include <QPixmap>
#include <QVector>
#include <QWidget>

class QToolButton;

namespace plt {
class PlotSettings;
} // namespace plt

namespace plt {

class Plotter : public QWidget
{
	Q_OBJECT

public:
	explicit Plotter(QWidget *parent = nullptr);
	~Plotter() override = default;

	explicit Plotter(const Plotter& )    = delete ;
	explicit Plotter(Plotter&& )         = delete ;

public:
	Plotter& operator= (const Plotter& ) = delete;
	Plotter& operator= (Plotter&& )      = delete;

public:
	void setPlotSettings(const PlotSettings& settings);
	void setCurveData(int id, const QVector<QPointF>& data);
	void clearCurve(int id);

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

public slots:
	void zoomIn();
	void zoomOut();

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	void updateRubberBandRegion();
	void refreshPixmap();
	void drawGrid(QPainter* painter);
	void drawCurves(QPainter* painter);

	enum {MARGIN = 50};

	QToolButton* _pZoomInBtn;
	QToolButton* _pZoomOutBtn;
	QMap<int, QVector<QPointF>> _curveMap {};
	QVector<PlotSettings> _zoomStack {};
	int _curZoom {0};
	bool _rubberBandIsShown {false};
	QRect _rubberBandRect {};
	QPixmap _pixmap {};
};

} // namespace plt

namespace plt {

class PlotSettings
{
public:
	PlotSettings();
	~PlotSettings() = default;

	PlotSettings(const PlotSettings& ) = default;
	PlotSettings(PlotSettings&& )      = default;

public:
	PlotSettings& operator= (const PlotSettings& ) = default;
	PlotSettings& operator= (PlotSettings&& )      = default;

public:
	void scroll(int dx, int dy);
	void adjust();
	double spanX() const {return maxX - minX;}
	double spanY() const {return maxY - minY;}
	double minX;
	double maxX;
	int numXTicks;
	double minY;
	double maxY;
	int numYTicks;

private:
	static void adjustAxis(double& min, double& max, int& numTicks);
};

} // namespace plt

#endif // PLOTTER_H
