#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

namespace plt {

class Application : public QApplication
{
	Q_OBJECT

public:
	explicit Application(int& argc, char** argv);
	~Application() override = default;

	explicit Application(const Application& )    = delete;
	explicit Application(Application&& )         = delete;

public:
	Application& operator= (const Application& ) = delete;
	Application& operator= (Application&& )      = delete;
};

} // namespace plt

#endif // APPLICATION_H
