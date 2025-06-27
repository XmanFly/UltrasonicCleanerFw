#include "platform.h"
#include "simviewmodel.h"
#include "hal/touch.h"
#include "hal/battery.h"
#include <QDebug>

static SimViewModel *g_self = nullptr;

SimViewModel::SimViewModel(QObject *parent)
    : QObject{parent}
{
    g_self = this;
    connect(this, &SimViewModel::batQuatityChanged,
            [this](){
        hal_battery_sim_set_mv(m_batQuatity);
    });
    hal_battery_sim_set_mv(m_batQuatity);
    connect(this, &SimViewModel::adapterConnectedChanged,
            [this](){
        hal_battery_sim_set_chg(m_adapterConnected);
    });
    hal_battery_sim_set_chg(m_adapterConnected);
}

void SimViewModel::pressed()
{
    qDebug();
    hal_touch_sim_set(1);
}

void SimViewModel::released()
{
    qDebug();
    hal_touch_sim_set(0);
}

bool SimViewModel::adapterConnected() const
{
    return m_adapterConnected;
}

void SimViewModel::setAdapterConnected(bool newAdapterConnected)
{
    qDebug() << newAdapterConnected;
    if (m_adapterConnected == newAdapterConnected)
        return;
    m_adapterConnected = newAdapterConnected;
    emit adapterConnectedChanged();
}

float SimViewModel::batQuatity() const
{
    return m_batQuatity;
}

void SimViewModel::setBatQuatity(float newBatQuatity)
{
    qDebug() << newBatQuatity;
    if (qFuzzyCompare(m_batQuatity, newBatQuatity))
        return;
    m_batQuatity = newBatQuatity;
    emit batQuatityChanged();
}

int SimViewModel::blueLedSpeed() const
{
    return m_blueLedSpeed;
}

void SimViewModel::setBlueLedSpeed(int newBlueLedSpeed)
{
    qDebug() << newBlueLedSpeed;
    if (m_blueLedSpeed == newBlueLedSpeed)
        return;
    m_blueLedSpeed = newBlueLedSpeed;
    emit blueLedSpeedChanged();
}

int SimViewModel::redLedSpeed() const
{
    return m_redLedSpeed;
}

void SimViewModel::setRedLedSpeed(int newRedLedSpeed)
{
    qDebug() << newRedLedSpeed;
    if (m_redLedSpeed == newRedLedSpeed)
        return;
    m_redLedSpeed = newRedLedSpeed;
    emit redLedSpeedChanged();
}

int SimViewModel::cleanerSpeed() const
{
    return m_cleanerSpeed;
}

void SimViewModel::setCleanerSpeed(int newCleanerSpeed)
{
    qDebug() << newCleanerSpeed;
    if (m_cleanerSpeed == newCleanerSpeed)
        return;
    m_cleanerSpeed = newCleanerSpeed;
    emit cleanerSpeedChanged();
}

int SimViewModel::fsmState() const
{
    return m_fsmState;
}

void SimViewModel::setFsmState(int newFsmState)
{
    if (m_fsmState == newFsmState)
        return;
    m_fsmState = newFsmState;
    emit fsmStateChanged();
}

int SimViewModel::redLedState() const
{
    return m_redLedState;
}

void SimViewModel::setRedLedState(int newRedLedState)
{
    if (m_redLedState == newRedLedState)
        return;
    m_redLedState = newRedLedState;
    emit redLedStateChanged();
}

bool SimViewModel::power() const
{
    return m_power;
}

void SimViewModel::setPower(bool newPower)
{
    if (m_power == newPower)
        return;
    m_power = newPower;
    emit powerChanged();
}

int SimViewModel::blueLedState() const
{
    return m_blueLedState;
}

void SimViewModel::setBlueLedState(int newBlueLedState)
{
    if (m_blueLedState == newBlueLedState)
        return;
    m_blueLedState = newBlueLedState;
    emit blueLedStateChanged();
}

void SimViewModel::onHalUsCallback(u8 on)
{
    g_self->setCleanerSpeed(on);
}

void SimViewModel::onEnterFsmStateCallback(u8 st)
{
    g_self->setFsmState(st);
}

void SimViewModel::onSetLedCallback(u8 id, u8 type, u8 speed)
{
    qDebug() << "id" << id << "type" << type << "period" << speed;
    switch (id) {
    case 0:
        g_self->setRedLedState(type);
        g_self->setRedLedSpeed(speed);
        break;
    case 1:
        g_self->setBlueLedState(type);
        g_self->setBlueLedSpeed(speed);
        break;
    default:
        break;
    }
}

void SimViewModel::onSetPower(u8 on)
{
    g_self->setPower(on);
}

