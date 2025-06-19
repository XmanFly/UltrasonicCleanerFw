#include "platform.h"
#include "simviewmodel.h"
#include <QDebug>

static SimViewModel *g_self = nullptr;

SimViewModel::SimViewModel(QObject *parent)
    : QObject{parent}
{
    g_self = this;
}

void SimViewModel::pressed()
{
    qDebug();
}

void SimViewModel::released()
{
    qDebug();
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

void SimViewModel::onHalUsCallback(u8 on)
{
    g_self->setCleanerSpeed(on);
}
