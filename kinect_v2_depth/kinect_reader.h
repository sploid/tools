#pragma once

static const int DEPTH_IMAGE_WIDTH = 512;
static const int DEPTH_IMAGE_HEIGHT = 424;
static const int DEPTH_IMAGE_SIZE = DEPTH_IMAGE_WIDTH * DEPTH_IMAGE_HEIGHT;
static const int DEPTH_IMAGE_BYTES = DEPTH_IMAGE_SIZE * sizeof( quint16 );
static const int DEPTH_IMAGE_MIN = 500;
static const int DEPTH_IMAGE_MAX = 4500;
static const int DEPTH_IMAGE_CMAX = DEPTH_IMAGE_MAX;

class kinect_reader : public QThread
{
	Q_OBJECT
public:
	kinect_reader( QObject* parent );
	bool get_data( quint16* buffer );
	void stop();

Q_SIGNALS:
	void data_ready();
	void error_occurred();

private:
	bool need_stop() const;
	virtual void run() override;

	QSharedPointer<quint16> m_data_buffer;
	bool m_data_taken;
	mutable QMutex m_mutex;
	bool m_stop;
};
