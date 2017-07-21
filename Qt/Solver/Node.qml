import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Rectangle {

    width: 60
    height: 60
    color: "#555555"
    border.color: "black"



    property bool checked: true
    property var nominee: [true, true, true, true, true, true, true, true, true]
    property string value: ""

    Text{
        text: parent.value
        anchors.centerIn: parent
        font.pixelSize: parent.height * .5
        color: "white"
    }

    Rectangle{
        x: 0
        y: 0
        width: 20
        height: 20
        z: 2
        visible: parent.nominee[0]
        color: "#aaaaaa"

        Text{
            text: qsTr("1")
            anchors.centerIn: parent
            font.pixelSize: parent.height * .5
            color: "white"
        }
    }
}
