#include "C3D.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

int main()
{
	try
	{
		C3D application(WINDOW_WIDTH, WINDOW_HEIGHT, "C3D");
		application.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}