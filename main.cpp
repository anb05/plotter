#include "plotter/plotter.h"
#include "app/application.h"

#include <QPointF>
#include <QVector>
#include <QRandomGenerator>

#include <QDebug>

int main(int argc, char *argv[])
{
	QVector<QPointF> v;
	for (int i = 0; i < 100; ++i) {
		QPointF pf(i,i);
		v.push_back(pf);
	}
	plt::Application a(argc, argv);
	plt::Plotter w;
	w.setCurveData(0, v);

	w.show();
	return a.exec();
}
