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
                                                [0, "关机"],
                                                [1, "清洗中"],
                                                [2, "充电"],
                                                [3, "电量低"],
                                                [4, "工作完成"],
                                                [5, "异常"]
                                            ])
                    return statusMap.get(SimViewModel.fsmState) || SimViewModel.fsmState
                }
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

            Rectangle {
                id: blueLed
                Layout.fillHeight: true
                Layout.preferredWidth: height
                Layout.alignment: Qt.AlignHCenter
                radius: width / 2
                color: "white"
                border {
                    width: 20
                    color: "blue"
                }

                Rectangle {
                    id: redLed
                    anchors {
                        fill: parent
                        margins: 20
                    }
                    radius: width / 2
                    color: "white"
                    border {
                        width: 20
                        color: "red"
                    }

                    Rectangle {
                        id: cleaner
                        anchors {
                            centerIn: parent
                        }
                        width: 20
                        height: parent.height - 2 * parent.border.width - 10
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
