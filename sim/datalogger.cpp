#include "datalogger.h"
#include <QThread>
#include <QTextStream>

DataLogger::DataLogger(const QString &filePath,
                       int bufferFlushSize,
                       int flushIntervalMs,
                       QObject *parent)
    : QObject(parent)
    , m_file(filePath)
    , m_bufferFlushSize(bufferFlushSize)
    , m_flushTimer(this)
{
    // 只写模式 + 追加（不想覆盖就用 Append）
    if (!m_file.open(QIODevice::WriteOnly)) {
        qFatal("无法打开日志文件 %s: %s",
               qPrintable(filePath),
               qPrintable(m_file.errorString()));
    }
    // 先把文件指针移到末尾
    m_file.seek(m_file.size());

    // 写头
    QTextStream t(&m_file);
    t << "D0\n";

    // 启动定时 flush
    m_flushTimer.setInterval(flushIntervalMs);
    connect(&m_flushTimer, &QTimer::timeout,
            this, &DataLogger::flushBuffer);
    m_flushTimer.start();
}

DataLogger::~DataLogger()
{
    m_flushTimer.stop();
    flushBuffer();
    m_file.close();
}

void DataLogger::onDataReceived(int id, int value)
{
    if(id) {
        return;
    }


    // 1ms 内存操作：整数→ASCII + 换行
//    m_buffer.append(QByteArray::number(m_cnt++));
//    m_buffer.append(",");
    m_buffer.append(QByteArray::number(value));
    m_buffer.append('\n');

    // 缓冲满了就写一次盘
    if (m_buffer.size() >= m_bufferFlushSize) {
        flushBuffer();
    }
}

void DataLogger::flushBuffer()
{
    if (m_buffer.isEmpty()) return;
    qint64 written = m_file.write(m_buffer);
    if (written != m_buffer.size()) {
        qWarning("DataLogger 写入失败: 期望 %lld, 实际 %lld",
                 m_buffer.size(), written);
    }
    m_file.flush();
    m_buffer.clear();
}
