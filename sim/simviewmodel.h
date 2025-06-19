#ifndef SIMVIEWMODEL_H
#define SIMVIEWMODEL_H

#include "types.h"
#include <QObject>

class SimViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool adapterConnected READ adapterConnected WRITE setAdapterConnected NOTIFY adapterConnectedChanged)
    Q_PROPERTY(float batQuatity READ batQuatity WRITE setBatQuatity NOTIFY batQuatityChanged)
    Q_PROPERTY(int blueLedSpeed READ blueLedSpeed WRITE setBlueLedSpeed NOTIFY blueLedSpeedChanged)
    Q_PROPERTY(int redLedSpeed READ redLedSpeed WRITE setRedLedSpeed NOTIFY redLedSpeedChanged)
    Q_PROPERTY(int cleanerSpeed READ cleanerSpeed WRITE setCleanerSpeed NOTIFY cleanerSpeedChanged)
public:
    explicit SimViewModel(QObject *parent = nullptr);
    Q_INVOKABLE void pressed(); // 按下按键
    Q_INVOKABLE void released(); // 松开按键

    bool adapterConnected() const;
    void setAdapterConnected(bool newAdapterConnected);

    float batQuatity() const;
    void setBatQuatity(float newBatQuatity);

    int blueLedSpeed() const;
    void setBlueLedSpeed(int newBlueLedSpeed);

    int redLedSpeed() const;
    void setRedLedSpeed(int newRedLedSpeed);

    int cleanerSpeed() const;
    void setCleanerSpeed(int newCleanerSpeed);

private:
    bool m_adapterConnected; // 适配器连接
    float m_batQuatity; // 电池电量
    int m_blueLedSpeed; // 蓝灯闪烁速度
    int m_redLedSpeed; // 红灯闪烁速度
    int m_cleanerSpeed; // 超声速度

signals:
    void adapterConnectedChanged();
    void batQuatityChanged();
    void blueLedSpeedChanged();
    void redLedSpeedChanged();
    void cleanerSpeedChanged();

public: // 回调
    static void onHalUsCallback(u8 on);
};

#endif // SIMVIEWMODEL_H
