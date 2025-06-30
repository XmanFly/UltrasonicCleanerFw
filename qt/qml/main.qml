import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    width: 800
    height: 480
    visible: true
    title: qsTr("超声模拟器 V1.0")

    RowLayout {
        anchors {
            fill: parent
            margins: 10
        }

        GridLayout {
            Layout.preferredWidth: parent.width * 0.5
            columns: 2
            columnSpacing: 8
            Text {
                text: "系统状态"
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: {
                    var statusMap = new Map([
                                                [0, "初始化"],
                                                [1, "空闲"],
                                                [2, "AD采样延时"],
                                                [3, "清洗中"],
                                                [4, "清洗间隔"],
                                                [5, "工作完成"],
                                                [6, "关机"],
                                                [7, "充电电压稳定中"],
                                                [8, "充电初始化"],
                                                [9, "充电开"],
                                                [10, "充电关"],
                                                [11, "充满"],
                                                [12, "电量低"],
                                                [13, "异常"]
                                            ])
                    return statusMap.get(SimViewModel.fsmState) || SimViewModel.fsmState
                }
            }

            Text {
                text: "供电自锁"
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: SimViewModel.power ? "已锁定" : "未锁定"
                color: SimViewModel.power ? "greeen" : "red"
            }

            Text {
                text: "适配器"
            }
            Switch {
                onClicked: {
                    SimViewModel.adapterConnected = checked
                }
            }

            Text {
                text: "电量"
            }
            RowLayout {
                Layout.fillWidth: true
                Slider {
                    id: batQuantity
                    from: 0
                    to: 5000
                    onValueChanged: {
                        SimViewModel.batQuatity = value
                    }
                }
                Text {
                    Layout.preferredWidth: 80
                    text: batQuantity.value
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                BreathingRectangle {
                    id: blueLed
                    anchors {
                        centerIn: parent
                    }
                    width: Math.min(parent.width, parent.height)
                    height: Math.min(parent.width, parent.height)
                    color: "white"
                    border {
                        width: 20
                        color: "blue"
                    }
                    breathDuration: SimViewModel.blueLedSpeed
                    ledState: SimViewModel.blueLedState

                    BreathingRectangle {
                        id: redLed
                        anchors {
                            fill: parent
                            margins: 20
                        }
                        color: "white"
                        border {
                            width: 20
                            color: "red"
                        }
                        breathDuration: SimViewModel.redLedSpeed
                        ledState: SimViewModel.redLedState

                        Rectangle {
                            id: cleaner
                            anchors {
                                centerIn: parent
                            }
                            width: 20
                            height: Math.min(parent.width, parent.height) - 2 * parent.border.width
                            color: "green"
                            /* 关键两行 —— 指定旋转中心并开启动画 */
                            transformOrigin: Item.Center              // 围绕中心
                            RotationAnimation {                       // 无限顺时针旋转
                                target: cleaner
                                property: "rotation"
                                from: 0; to: 360
                                duration: 1000                       // 2.5 s 一圈
                                loops: Animation.Infinite
                                easing.type: Easing.Linear
                                running: SimViewModel.cleanerSpeed
                            }
                        }
                    }
                }

            }
            Item { Layout.preferredHeight: 40 }

            Button {
                Layout.alignment: Qt.AlignHCenter
                text: "开始"
                onPressed: {
                    SimViewModel.pressed()
                }
                onReleased: {
                    SimViewModel.released()
                }
            }
        }
    }
}
