#include <dsp/filerecord.h>
#include "dsp/dspcommands.h"
#include "util/simpleserializer.h"
#include "util/message.h"

#include <QDebug>

FileRecord::FileRecord() :
	BasebandSampleSink(),
    m_fileName(std::string("test.sdriq")),
    m_sampleRate(0),
    m_centerFrequency(0),
	m_recordOn(false),
    m_recordStart(false),
    m_byteCount(0)
{
	setObjectName("FileSink");
}

FileRecord::FileRecord(const std::string& filename) :
    BasebandSampleSink(),
    m_fileName(std::string(filename)),
    m_sampleRate(0),
    m_centerFrequency(0),
    m_recordOn(false),
    m_recordStart(false),
    m_byteCount(0)
{
    setObjectName("FileRecord");
}

FileRecord::~FileRecord()
{
    stopRecording();
}

void FileRecord::setFileName(const std::string& filename)
{
    if (!m_recordOn)
    {
        m_fileName = filename;
    }
}

void FileRecord::feed(const SampleVector::const_iterator& begin, const SampleVector::const_iterator& end, bool positiveOnly __attribute__((unused)))
{
    // if no recording is active, send the samples to /dev/null
    if(!m_recordOn)
        return;

    if (begin < end) // if there is something to put out
    {
        if (m_recordStart)
        {
            writeHeader();
            m_recordStart = false;
        }

        m_sampleFile.write(reinterpret_cast<const char*>(&*(begin)), (end - begin)*sizeof(Sample));
        m_byteCount += end - begin;
    }
}

void FileRecord::start()
{
}

void FileRecord::stop()
{
    stopRecording();
}

void FileRecord::startRecording()
{
    if (!m_sampleFile.is_open())
    {
    	qDebug() << "FileRecord::startRecording";
        m_sampleFile.open(m_fileName.c_str(), std::ios::binary);
        m_recordOn = true;
        m_recordStart = true;
        m_byteCount = 0;
    }
}

void FileRecord::stopRecording()
{
    if (m_sampleFile.is_open())
    {
    	qDebug() << "FileRecord::stopRecording";
        m_sampleFile.close();
        m_recordOn = false;
        m_recordStart = false;
    }
}

bool FileRecord::handleMessage(const Message& message)
{
	qDebug() << "FileRecord::handleMessage";

	if (DSPSignalNotification::match(message))
	{
		DSPSignalNotification& notif = (DSPSignalNotification&) message;
		m_sampleRate = notif.getSampleRate();
		m_centerFrequency = notif.getCenterFrequency();
		qDebug() << "FileRecord::handleMessage: DSPSignalNotification: m_inputSampleRate: " << m_sampleRate
				<< " m_centerFrequency: " << m_centerFrequency;
		return true;
	}
    else
    {
        return false;
    }
}

void FileRecord::handleConfigure(const std::string& fileName)
{
    if (fileName != m_fileName)
    {
        stopRecording();
    }

	m_fileName = fileName;
}

void FileRecord::writeHeader()
{
    m_sampleFile.write((const char *) &m_sampleRate, sizeof(int));
    m_sampleFile.write((const char *) &m_centerFrequency, sizeof(quint64));
    std::time_t ts = time(0);
    m_sampleFile.write((const char *) &ts, sizeof(std::time_t));
}

void FileRecord::readHeader(std::ifstream& sampleFile, Header& header)
{
    sampleFile.read((char *) &(header.sampleRate), sizeof(int));
    sampleFile.read((char *) &(header.centerFrequency), sizeof(quint64));
    sampleFile.read((char *) &(header.startTimeStamp), sizeof(std::time_t));
}
