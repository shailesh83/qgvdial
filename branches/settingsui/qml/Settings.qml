import Qt 4.7
import "helper.js" as Code

Item {
    id: container

    signal sigProxySupport(bool enable)
    signal sigUseSystemProxy(bool enable)

    Column {
        id: mainColumn
        anchors.fill: parent
        anchors.topMargin: 2

        property bool bUserProxy: ((proxySupport.check == true) && (proxySystem.check  != true))

        Row {
            width: parent.width
            spacing: 2

            Text {
                text: "Username:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: textUsername
                anchors.verticalCenter: parent.verticalCenter
                text: "Username"
                color: "white"
            }
        }

        Row {
            width: parent.width
            spacing: 2

            Text {
                text: "Username:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: textpassword
                anchors.verticalCenter: parent.verticalCenter
                text: "Username"
                color: "white"
                echoMode: TextInput.Password
            }
        }

        RadioButton {
            id: proxySupport
            width: parent.width

            text: "Enable proxy support"
            onCheckChanged: container.sigProxySupport(check)
        }// RadioButton (proxySupport)

        RadioButton {
            id: proxySystem
            width: parent.width
            opacity: (proxySupport.check == true ? 1 : 0)

            text: "Use system proxy settings"
            onCheckChanged: container.sigUseSystemProxy(check)
        }// RadioButton (proxySystem)

        Row {
            width: parent.width
            spacing: 2

            opacity: (mainColumn.bUserProxy == true ? 1 : 0)

            Text {
                text: "Host:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: textUserProxyHost
                anchors.verticalCenter: parent.verticalCenter
                text: "Enter proxy host"
                color: "white"
            }
        }// Row (user proxy host)

        Row {
            width: parent.width
            spacing: 2

            opacity: (mainColumn.bUserProxy == true ? 1 : 0)

            Text {
                text: "Port:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: textUserProxyPort
                anchors.verticalCenter: parent.verticalCenter
                text: "Enter proxy port"
                color: "white"
                validator: IntValidator { bottom: 0; top: 65535 }
            }
        }// Row (user proxy port)

    }// Column
}
