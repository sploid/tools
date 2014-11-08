#include "depth_widget.h"

depth_widget::depth_widget( QWidget* parent, kinect_reader* reader )
	: QVTKWidget( parent )
	, m_reader( reader )
	, m_render_window( vtkSmartPointer<vtkRenderWindow>::New() )
	, m_renderer( vtkSmartPointer<vtkRenderer>::New() )
	, m_point( vtkSmartPointer<vtkPolyData>::New() )
	, m_mapper( vtkSmartPointer<vtkPolyDataMapper>::New() )
	, m_actor( vtkSmartPointer<vtkActor>::New() )
	, m_text_actor( vtkSmartPointer<vtkTextActor>::New() )
	, m_data_buffer( new quint16[ DEPTH_IMAGE_SIZE ], [] ( quint16* buf ) { delete[] buf; } )
	, m_reset_camera( true )
{
	m_mapper->SetInputData( m_point );
	m_actor->SetMapper( m_mapper );
	m_actor->GetProperty()->SetPointSize( 1 );
	m_renderer->AddActor( m_actor );
	m_render_window->AddRenderer( m_renderer );
	SetRenderWindow( m_render_window );

	m_text_actor->GetTextProperty()->SetFontSize( 24 );
	m_text_actor->SetPosition2( 0, 0 );
	m_renderer->AddActor2D( m_text_actor );
	m_text_actor->SetInput( "0." );
	m_text_actor->GetTextProperty()->SetColor( 1.0, 1.0, 1.0 );

	m_renderer->ResetCamera();

	connect( reader, SIGNAL( data_ready() ), SLOT( on_data_ready() ), Qt::ConnectionType::QueuedConnection );
	m_fts_mesurement.start();
}

void depth_widget::on_data_ready()
{
	m_reader->get_data( m_data_buffer.data() );
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	QVector< vtkIdType > pid;
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for ( int nn = 0; nn < DEPTH_IMAGE_HEIGHT; ++nn )
	{
		for ( int mm = 0; mm < DEPTH_IMAGE_WIDTH; ++mm )
		{
			const int curr_index = DEPTH_IMAGE_WIDTH * nn + mm;
			float val = m_data_buffer.data()[ curr_index ];
			if ( val > DEPTH_IMAGE_MIN && val < DEPTH_IMAGE_CMAX )
			{
				const float po[ 3 ] = { (float)nn, (float)mm, val };
				pid.push_back( points->InsertNextPoint( po ) );
			}
		}
	}
	if ( pid.size() )
	{
		vertices->InsertNextCell( pid.size(), &pid[ 0 ] );

		// Set the points and vertices we created as the geometry and topology of the polydata
		m_point->SetPoints( points );
		m_point->SetVerts( vertices );
		if ( m_reset_camera )
		{
			m_renderer->ResetCamera();
			m_reset_camera = false;
		}
		const QString text = QString( "FPS %1" ).arg( 1000. / m_fts_mesurement.restart(), 0, 'f', -1 );
		m_text_actor->SetInput( text.toAscii() );
		update();
	}
}
