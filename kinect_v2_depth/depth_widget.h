#pragma once
#include "kinect_reader.h"

class depth_widget : public QVTKWidget
{
	Q_OBJECT
public:
	depth_widget( QWidget* parent, kinect_reader* reader );

public Q_SLOTS:
	void on_data_ready();

private:
	kinect_reader* m_reader;
	vtkSmartPointer<vtkRenderWindow> m_render_window;
	vtkSmartPointer<vtkRenderer> m_renderer;
	vtkSmartPointer<vtkPolyData> m_point;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkTextActor> m_text_actor;
	QSharedPointer<quint16> m_data_buffer;
	QTime m_fts_mesurement;
	bool m_reset_camera;
};