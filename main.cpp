#include "plotter/plotter.h"

#include "app/application.h"

int main(int argc, char *argv[])
{
	plt::Application a(argc, argv);
	plt::Plotter w;
	w.show();
	return a.exec();
}
