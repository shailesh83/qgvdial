#!/bin/bash

rm cm-* connection-* protocol-*
qdbusxml2cpp -v -a cm-adapter -p cm-proxy -v cm.xml
echo cm.xml done.
qdbusxml2cpp -v -a connection-adapter -p connection-proxy -v connection.xml
echo connection.xml done.
qdbusxml2cpp -v -a protocol-adapter -p protocol-proxy -v protocol.xml
echo protocol.xml done.

