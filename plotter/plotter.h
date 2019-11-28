#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>

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
};

} // namespace plt

#endif // PLOTTER_H
