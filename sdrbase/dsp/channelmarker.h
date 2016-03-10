#ifndef INCLUDE_CHANNELMARKER_H
#define INCLUDE_CHANNELMARKER_H

#include <QObject>
#include <QColor>
#include "util/export.h"

class SDRANGEL_API ChannelMarker : public QObject {
	Q_OBJECT

public:
	typedef enum sidebands_e
	{
		dsb,
		lsb,
		usb
	} sidebands_t;

	ChannelMarker(QObject* parent = NULL);

	void setTitle(const QString& title);
	const QString& getTitle() const { return m_title; }

	void setCenterFrequency(int centerFrequency);
	int getCenterFrequency() const { return m_centerFrequency; }

	void setBandwidth(int bandwidth);
	int getBandwidth() const { return m_bandwidth; }

	void setLowCutoff(int lowCutoff);
	int getLowCutoff() const { return m_lowCutoff; }

	void setSidebands(sidebands_t sidebands);
	sidebands_t getSidebands() const { return m_sidebands; }

	void setVisible(bool visible);
	bool getVisible() const { return m_visible; }

	void setHighlighted(bool highlighted);
	bool getHighlighted() const { return m_highlighted; }

	void setColor(const QColor& color);
	const QColor& getColor() const { return m_color; }

protected:
	static QRgb m_colorTable[];
	static int m_nextColor;

	QString m_title;
	int m_centerFrequency;
	int m_bandwidth;
	int m_lowCutoff;
	sidebands_t m_sidebands;
	bool m_visible;
	bool m_highlighted;
	QColor m_color;

signals:
	void changed();
};

#endif // INCLUDE_CHANNELMARKER_H