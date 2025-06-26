#include "waveemitter.h"

static WaveEmitter *g_self = nullptr;

WaveEmitter::WaveEmitter(QObject *parent)
    : QObject{parent}
{
    g_self = this;
}

void WaveEmitter::onEmitIO(u8 grp, u8 on)
{
    if(grp == 0) {
        return;
    }
    emit g_self->ioChanged(grp, on);
}
