#!/bin/bash

rm -f cm_* connection_* protocol_*

#qdbusxml2cpp -v -a cm_adapter -p cm_proxy -v -i shared_data_types.h cm.xml
qdbusxml2cpp -v -a cm_adapter -v -i shared_data_types.h cm.xml
echo cm.xml done.
#qdbusxml2cpp -v -a connection_adapter -p connection_proxy -v -i shared_data_types.h connection.xml
qdbusxml2cpp -v -a connection_adapter -v -i shared_data_types.h connection.xml
echo connection.xml done.
#qdbusxml2cpp -v -a protocol_adapter -p protocol_proxy -v -i shared_data_types.h protocol.xml
qdbusxml2cpp -v -a protocol_adapter -v -i shared_data_types.h protocol.xml
echo protocol.xml done.

