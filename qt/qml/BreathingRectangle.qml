import QtQuick 2.15

Item {
    id: root

    // 默认尺寸
    implicitWidth: 200
    implicitHeight: 200

    /** 外部可设置属性 **/
    // 尺寸
    property alias rectWidth: rect.width
    property alias rectHeight: rect.height

    // 背景色、圆角
    property alias color: rect.color
    property alias radius: rect.radius

    // 边界
    property alias border: rect.border

    // 呼吸周期（ms）
    property int breathDuration: 200
    property int realBreathDuration: breathDuration * 128 // 传入的是呼吸灯分频因子，故这里乘上一个系数
    onBreathDurationChanged: {
        console.log("duration " + realBreathDuration)
    }

    property int ledState: 0 // 同步anim_type_t
    onLedStateChanged: {
        console.log("led state " + ledState)
    }


    // 透明度范围
    property real minOpacity: 0.3
    property real maxOpacity: 1.0

    Rectangle {
        id: rect
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height)
        height: Math.min(parent.width, parent.height)
        radius: Math.min(width, height) / 2
        visible: ledState > 0

        transformOrigin: Item.Center

        // 初始状态
        opacity: minOpacity

        // 透明度动画
        SequentialAnimation {
            id: breathAnim
//            running: ledState === 2
            loops: Animation.Infinite
            NumberAnimation {
                target: rect; property: "opacity"
                from: minOpacity;
                to: maxOpacity;
                duration: realBreathDuration/2;
                easing.type: Easing.InOutQuad
                onDurationChanged: {
                    if( ledState === 2) {
                        console.log("start 1st half")
                        breathAnim.restart()
                    } else {
                        console.log("stop 1st half")
                        breathAnim.stop()
                    }
                }
            }
            NumberAnimation {
                target: rect; property: "opacity"
                from: maxOpacity;
                to: minOpacity;
                duration: realBreathDuration/2;
                easing.type: Easing.InOutQuad
                onDurationChanged: {
                    if( ledState === 2) {
                        console.log("start 2nd half")
                        breathAnim.restart()
                    } else {
                        console.log("stop 2nd half")
                        breathAnim.stop()
                    }
                }
            }
            onStopped: {
                rect.opacity = maxOpacity
            }
        }
    }
}
