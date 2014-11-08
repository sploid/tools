#include "kinect_reader.h"
#include "depth_widget.h"

int main( int argc, char* argv[] )
{
	QVTKApplication app( argc, argv );
	kinect_reader reader( nullptr );
	depth_widget widget( nullptr, &reader );
	reader.start();
	widget.showMaximized();
	const int ret = app.exec();
	reader.stop();
	reader.wait();
	return ret;

}