import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    property alias textField1: textField1
    width: 960
    height: 720
    property alias button3: button3

    RowLayout {
        anchors.horizontalCenterOffset: 154
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 8
        anchors.top: parent.top

        TextField {
            id: textField1
            placeholderText: qsTr("Text Field")
        }

        Button {
            id: nope
            text: qsTr("Press Me")
        }
    }

    Grid {
        id: grid
        x: 660
        y: 400
        width: 240
        height: 240
        rows: 3
        columns: 3

        Button {
            id: button1
            width: 80
            height: 80
            text: qsTr("1")
            checked: false
            checkable: false
        }
        Button {
            id: button2
            width: 80
            height: 80
            text: qsTr("2")
        }

        Button {
            id: button3
            width: 80
            height: 80
            text: qsTr("3")
        }

        Button {
            id: button4
            width: 80
            height: 80
            text: qsTr("4")
        }

        Button {
            id: button5
            width: 80
            height: 80
            text: qsTr("5")
        }

        Button {
            id: button6
            width: 80
            height: 80
            text: qsTr("6")
        }

        Button {
            id: button7
            width: 80
            height: 80
            text: qsTr("7")
        }

        Button {
            id: button8
            width: 80
            height: 80
            text: qsTr("8")
        }

        Button {
            id: button9
            width: 80
            height: 80
            text: qsTr("9")
        }


    }

    ElementGrid {
        id: elementGrid1
        x: 40
        y: 100
    }

    ElementGrid {
        id: elementGrid2
        x: 220
        y: 100
    }

    ElementGrid {
        id: elementGrid3
        x: 400
        y: 100
    }

    ElementGrid {
        id: elementGrid4
        x: 40
        y: 280
    }

    ElementGrid {
        id: elementGrid5
        x: 220
        y: 280
    }

    ElementGrid {
        id: elementGrid6
        x: 400
        y: 280
    }

    ElementGrid {
        id: elementGrid7
        x: 40
        y: 460
    }

    ElementGrid {
        id: elementGrid8
        x: 220
        y: 460
    }

    ElementGrid {
        id: elementGrid9
        x: 400
        y: 460
    }
}
