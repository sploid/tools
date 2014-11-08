#include "kinect_reader.h"

namespace { namespace auxil {
	template<class T>
	class isafe_deleter
	{
	public:
		isafe_deleter( T** iface )
			: m_iface( iface )
		{
		}

		~isafe_deleter()
		{
			if ( *m_iface != nullptr )
			{
				(*m_iface)->Release();
				*m_iface = nullptr;
			}

		}
	private:
		T** m_iface;
	};
} } // namespace auxil

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define DECLARE_IFACE( type, val )	type* val = nullptr; \
									auxil::isafe_deleter<type> MACRO_CONCAT( deleter, __COUNTER__ )( &val )

kinect_reader::kinect_reader( QObject* parent )
	: QThread( parent )
	, m_data_buffer( new quint16[ DEPTH_IMAGE_SIZE ], [] ( quint16* buf ) { delete[] buf; } )
	, m_data_taken( true )
	, m_stop( false )
{
}

bool kinect_reader::get_data( quint16* buffer )
{
	QMutexLocker lock( &m_mutex );
	if ( !m_data_taken )
	{
		m_data_taken = true;
		memcpy( buffer, m_data_buffer.data(), DEPTH_IMAGE_BYTES );
		return true;
	}
	return false;
}

void kinect_reader::stop()
{
	QMutexLocker lock( &m_mutex );
	m_stop = true;
}

void kinect_reader::run()
{
	{
		QMutexLocker lock( &m_mutex );
		m_data_taken = true;
	}
	DECLARE_IFACE( IKinectSensor, isensor );
	if ( SUCCEEDED( GetDefaultKinectSensor( &isensor ) ) )
	{
		if ( SUCCEEDED( isensor->Open() ) )
		{
			DECLARE_IFACE( IDepthFrameSource, idepth_source );
			if ( SUCCEEDED( isensor->get_DepthFrameSource( &idepth_source ) ) )
			{
				DECLARE_IFACE( IDepthFrameReader, idepth_reader );
				if ( SUCCEEDED( idepth_source->OpenReader( &idepth_reader ) ) )
				{
					DECLARE_IFACE( IFrameDescription, idescription );
					if ( SUCCEEDED( idepth_source->get_FrameDescription( &idescription ) ) )
					{
						// test current values
						int width = 0, height = 0;
						unsigned short min = 0, max = 0;
						if ( SUCCEEDED( idescription->get_Width( &width ) )
							&& width == DEPTH_IMAGE_WIDTH
							&& SUCCEEDED( idescription->get_Height( &height ) )
							&& height == DEPTH_IMAGE_HEIGHT
							&& SUCCEEDED( idepth_source->get_DepthMinReliableDistance( &min ) )
							&& min == DEPTH_IMAGE_MIN
							&& SUCCEEDED( idepth_source->get_DepthMaxReliableDistance( &max ) )
							&& max == DEPTH_IMAGE_MAX )
						{
							while ( !need_stop() ) // TODO(sploid): insert while data not empty
							{
								DECLARE_IFACE( IDepthFrame, idepth_frame );
								if ( SUCCEEDED( idepth_reader->AcquireLatestFrame( &idepth_frame ) ) )
								{
									QMutexLocker lock( &m_mutex );
									if ( m_data_taken )
									{
										UINT buffer_size = 0;
										quint16* buffer = nullptr;
										if ( SUCCEEDED( idepth_frame->AccessUnderlyingBuffer( &buffer_size, &buffer ) ) )
										{
											Q_ASSERT( buffer_size == DEPTH_IMAGE_WIDTH  * DEPTH_IMAGE_HEIGHT );
											Q_ASSERT( buffer );
											memcpy( m_data_buffer.data(), buffer, DEPTH_IMAGE_BYTES );
											m_data_taken = false;
											Q_EMIT data_ready();
										}
									}
								}
							}
							return;
						}
					}
				}
			}
		}
	}
	Q_EMIT error_occurred();
	return;
}

bool kinect_reader::need_stop() const
{
	QMutexLocker lock( &m_mutex );
	return m_stop;
}
