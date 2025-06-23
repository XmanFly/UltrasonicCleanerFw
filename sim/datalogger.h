#pragma once
#include <QObject>
#include <QFile>
#include <QTimer>

class DataLogger : public QObject {
    Q_OBJECT
public:
    explicit DataLogger(const QString &filePath,
                        int bufferFlushSize = 64*1024,  // 64KB
                        int flushIntervalMs  = 1000,    // 1 s
                        QObject *parent = nullptr);
    ~DataLogger();

public slots:
    /// 每次接收一个 int，就追加到内存缓冲
    void onDataReceived(int id, int value);

private slots:
    /// 定时把缓冲落盘
    void flushBuffer();

private:
    QFile      m_file;
    QByteArray m_buffer;
    const int  m_bufferFlushSize;
    QTimer     m_flushTimer;
    int		   m_cnt = 0;
};
