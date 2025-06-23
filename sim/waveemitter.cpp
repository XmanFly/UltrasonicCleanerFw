#include "waveemitter.h"

static WaveEmitter *g_self = nullptr;

WaveEmitter::WaveEmitter(QObject *parent)
    : QObject{parent}
{
    g_self = this;
}

void WaveEmitter::onEmitIO(u8 id, u8 on)
{
    emit g_self->ioChanged(id, on);
}
