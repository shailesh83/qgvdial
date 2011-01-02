import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: container

    property string text: "Yes or no question?"
    property bool check: false

    Row {
        anchors.fill: parent

        Rectangle {
            id: imageRect

            color: "white"
            border.color: "black"

            height: textLabel.height
            width: height

            Image {
                id: imageTick
                source: "tick.png"

                opacity: (check == true ? 1 : 0)
            }
        }

        Text {
            id: textLabel
            text: container.text

            color: "white"
        }
    }

    MouseArea {
        onClicked: {
            if (container.check == true) {
                container.check = false;
            } else {
                container.check = false;
            }
        }
    }

    states: [
        State {
            name: "pressed"
            PropertyChanges { target: container; color: "orange"}
        }

    ]
}
