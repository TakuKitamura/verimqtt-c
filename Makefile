all:
	cmake src && make -C src && cd ./src/tests/ && ./MQTTParsePacket