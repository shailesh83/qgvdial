import Qt 4.7

Row {
    MyButton {
        id: btnCall
        mainText: "Call / Text"

        width: parent.width * 2 / 3
        height: parent.height
    }
    MyButton {
        id: btnDel
        mainText: "\u2190"

        width: parent.width / 3
        height: parent.height
    }
}
