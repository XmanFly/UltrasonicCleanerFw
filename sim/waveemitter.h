#ifndef WAVEEMITTER_H
#define WAVEEMITTER_H

#include <QObject>
#include "types.h"

class WaveEmitter : public QObject
{
    Q_OBJECT
public:
    explicit WaveEmitter(QObject *parent = nullptr);

public: // 回调
    static void onEmitIO(u8 id, u8 on);

signals:
    void ioChanged(u8 id, u8 on);

};

#endif // WAVEEMITTER_H
