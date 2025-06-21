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
    Q_PROPERTY(int fsmState READ fsmState WRITE setFsmState NOTIFY fsmStateChanged)
    Q_PROPERTY(int blueLedState READ blueLedState WRITE setBlueLedState NOTIFY blueLedStateChanged)
    Q_PROPERTY(int redLedState READ redLedState WRITE setRedLedState NOTIFY redLedStateChanged)
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

    int fsmState() const;
    void setFsmState(int newFsmState);

    int blueLedState() const;
    void setBlueLedState(int newBlueLedState);
    int redLedState() const;
    void setRedLedState(int newRedLedState);

private:
    bool m_adapterConnected = false; // 适配器连接
    float m_batQuatity = 0; // 电池电量
    int m_blueLedState = 0; // 蓝灯状态 同步anim_type_t
    int m_blueLedSpeed = 0; // 蓝灯闪烁速度
    int m_redLedState = 0; // 红灯状态 同步anim_type_t
    int m_redLedSpeed = 0; // 红灯闪烁速度
    int m_cleanerSpeed = 0; // 超声速度
    int m_fsmState = 0; // 系统状态

signals:
    void adapterConnectedChanged();
    void batQuatityChanged();
    void blueLedSpeedChanged();
    void redLedSpeedChanged();
    void cleanerSpeedChanged();
    void fsmStateChanged();
    void blueLedOnChanged();
    void redLedOnChanged();

    void blueLedStateChanged();

    void redLedStateChanged();

public: // 回调
    static void onHalUsCallback(u8 on);
    static void onEnterFsmStateCallback(u8 st);
    static void onSetLedCallback(u8 id, u8 type, u16 period_ms);
};

#endif // SIMVIEWMODEL_H
