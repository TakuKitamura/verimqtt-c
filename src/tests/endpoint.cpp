// Usual programs
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <chrono>
#include <fmt/core.h>

// We need Logger
// #include "Logger/Logger.hpp"
// We need FastString
#include "Strings/Strings.hpp"

extern "C" {
    #include "Main.h"
}

// We need MQTT client 
// Swap our own string class here and don't depend on the selected build flags
#define MQTTDumpCommunication 1
#undef MQTTString
#define MQTTString Strings::FastString
#undef MQTTROString
#define MQTTROString Strings::VerySimpleReadOnlyString
#undef MQTTStringGetData
#define MQTTStringGetData(X)    X.getData()
#undef MQTTStringGetLength
#define MQTTStringGetLength(X)  X.getLength()
#include "Protocol/MQTT/MQTT.hpp"

#define PACKET_COUNT 1

typedef Strings::FastString String;

inline int asHex(char ch) { return ch >= '0' && ch <= '9' ? (ch - '0') : (ch >= 'a' && ch <= 'f' ? (ch - 'a' + 10) : (ch >= 'A' && ch <= 'F' ? (ch - 'A' + 10) : 0)); }

int emqtt5(uint8 * packetBuffer, size_t packetByte) {
    Protocol::MQTT::V5::FixedHeader header;
    header.raw = packetBuffer[0];
    // printf("Detected %s packet\n", Protocol::MQTT::V5::Helper::getControlPacketName((Protocol::MQTT::Common::ControlPacketType)(uint8)header.type));
    Protocol::MQTT::Common::VBInt len;
    uint32 r = len.readFrom(&packetBuffer[1], packetByte - 1);
    if (r == Protocol::MQTT::Common::BadData)
        return fprintf(stderr, "Invalid packet length at pos: 1\n"); // Close the socket here, the given data are wrong or not the right protocol
    if (r == Protocol::MQTT::Common::NotEnoughData)
        return fprintf(stderr, "Packet is too short at pos: 1\n");
    // Check packet size
    if ((uint32)len + 1 + len.getSize() < packetByte)
        printf("Warning: Got additional %d bytes but packet size is coded as: %u\n", (int)packetByte, (uint32)len + 1 + len.getSize());
    // else printf("with size: %u\n", (uint32)len + 1 + len.getSize()); 

    // Then dump it now
    Protocol::MQTT::V5::registerAllProperties();
    Protocol::MQTT::V5::ControlPacketSerializable * packet; 
    switch ((uint8)header.type)
    {
    case Protocol::MQTT::V5::RESERVED: return fprintf(stderr, "Can not parse further...\n");
    case Protocol::MQTT::V5::CONNECT:     packet = new Protocol::MQTT::V5::ConnectPacket; break;
    case Protocol::MQTT::V5::CONNACK:     packet = new Protocol::MQTT::V5::ROConnACKPacket; break;
    case Protocol::MQTT::V5::PUBLISH:     packet = new Protocol::MQTT::V5::ROPublishPacket; break;
    case Protocol::MQTT::V5::PUBACK:      packet = new Protocol::MQTT::V5::ROPubACKPacket; break;
    case Protocol::MQTT::V5::PUBREC:      packet = new Protocol::MQTT::V5::ROPubRecPacket; break;
    case Protocol::MQTT::V5::PUBREL:      packet = new Protocol::MQTT::V5::ROPubRelPacket; break;
    case Protocol::MQTT::V5::PUBCOMP:     packet = new Protocol::MQTT::V5::ROPubCompPacket; break;
    case Protocol::MQTT::V5::SUBSCRIBE:   packet = new Protocol::MQTT::V5::ControlPacket<Protocol::MQTT::Common::SUBSCRIBE>; break;
    case Protocol::MQTT::V5::SUBACK:      packet = new Protocol::MQTT::V5::ROSubACKPacket; break;
    case Protocol::MQTT::V5::UNSUBSCRIBE: packet = new Protocol::MQTT::V5::ControlPacket<Protocol::MQTT::Common::UNSUBSCRIBE>; break;
    case Protocol::MQTT::V5::UNSUBACK:    packet = new Protocol::MQTT::V5::ROUnsubACKPacket; break;
    case Protocol::MQTT::V5::PINGREQ:     packet = new Protocol::MQTT::V5::PingReqPacket; break;
    case Protocol::MQTT::V5::PINGRESP:    packet = new Protocol::MQTT::V5::PingRespPacket; break;
    case Protocol::MQTT::V5::DISCONNECT:  packet = new Protocol::MQTT::V5::RODisconnectPacket; break;
    case Protocol::MQTT::V5::AUTH:        packet = new Protocol::MQTT::V5::ROAuthPacket; break;
    }

    // error check
    r = packet->readFrom(packetBuffer, packetByte);

    if (Protocol::MQTT::Common::isError(r))
        return fprintf(stderr, "Could not parse packet with error: %u\n", r);

    // Then dump the packet
    String out;
    packet->dump(out);
    printf("%s\n", (const char*)out);

    return 0;
}

int verimqtt(uint8 *packetBuffer, size_t packetByte) {
    kremlinit_globals();
    struct_fixed_header d = mqtt_packet_parse(packetBuffer, packetByte);

/* すべてのパケットに共通のフォーマット
PUBLISH control packet (rlength: 20)
  Header: (type PUBLISH, retain 0, QoS 0, dup 0)
*/

    // std::string message = format("PUBLISH control packet (rlength: {})", d.remaining_length);

    std::string flag_message = 
        (d.flags.dup_flag == 255 &&
        d.flags.qos_flag == 255 &&
        d.flags.dup_flag == 255) ? 
            "no flags": 
            fmt::format("retain {retain_flag}, QoS {qos_flag}, dup {dup_flag}",
                fmt::arg("retain_flag", d.flags.retain_flag),
                fmt::arg("qos_flag", d.flags.qos_flag),
                fmt::arg("dup_flag", d.flags.dup_flag)
            );

    std::string variable_data = "";
    if (!strcmp(d.message_name, "CONNECT")) {

    } else if (!strcmp(d.message_name, "PUBLISH")) {
        variable_data = fmt::format(
            "PUBLISH packet (id {packet_identifier}): Str ({topic_length} bytes): {topic_name}",
            fmt::arg("packet_identifier",
                fmt::format(
                    "{:#06x}",
                    d.publish.packet_identifier == 0xffff ? 0x0000 : d.publish.packet_identifier
                )
            ),
            fmt::arg("topic_length", d.publish.topic_length),
            fmt::arg("topic_name", d.publish.topic_name)
        );
    } else if (!strcmp(d.message_name, "DISCONNECT")) {

    }

    std::string base_format = fmt::format(
            "{message_name} control packet (rlength: {remaining_length})\n  Header: (type {message_name}, {flag_message})\n  {variable_data}",
            fmt::arg("message_name", d.message_name),
            fmt::arg("remaining_length", d.remaining_length),
            fmt::arg("flag_message", flag_message),
            fmt::arg("variable_data", variable_data)
        );

    std::cout << base_format << std::endl;

    // printf("%s\n", out.c_str());


    return 0;
}

int main(int argc, char ** argv)
{
    int binary_count = 16;

    std::string mqttPacketBinaryPath[binary_count] = {
        "bin/property/property_four_byte/property_four_byte.bin",
        "bin/property/property_utf8_pair_string/property_utf8_pair_string.bin",
        "bin/property/property_variable_byte_integer/property_variable_byte.bin",
        "bin/property/property_two_byte/property_two_byte.bin",
        "bin/property/property_binary_data/property_binary_data.bin",
        "bin/property/property_one_byte/property_one_byte.bin",
        "bin/property/property_utf8_string/property_utf8_string.bin",
        "bin/connect/connect_will/connect_will.bin",
        "bin/connect/connect_all/connect_all.bin",
        "bin/connect/normal_connect/normal_connect.bin",
        "bin/connect/connect_user/connect_user.bin",
        "bin/connect/connect_password/connect_password.bin",
        "bin/disconnect/normal_disconnect/normal_disconnect.bin",
        "bin/disconnect/disconnect_reason/disconnect_reason.bin",
        "bin/publish/normal_publish/normal_publish.bin",
        "bin/publish/publish_packet_identifier/publish_packet_identifier.bin",
    };

    // std::ofstream log;
    // log.open("log.csv", ios::trunc)
    std::ofstream log("log.csv");

    log << "file_name,packet_count,packet_byte,emqtt5_ave_time[μs],verimqtt_ave_time[μs]" << std::endl;

    for(int i=0; i < binary_count; i++) {
        std::string file_path = mqttPacketBinaryPath[i];
        std::cout << file_path << std::endl;
        
        // Read Binary file
        std::ifstream ifs(file_path, std::ios::binary);
        if (!ifs) { 
            return fprintf(stderr, "can't read %s\n", file_path.c_str()); 
        }

        // seek to end
        ifs.seekg(0,std::ios::end);

        // get file bytes
        size_t packetByte = ifs.tellg();

        // seek to start
        ifs.seekg(0,std::ios::beg);

        // packet size check
        if (packetByte < 2 || packetByte > 268435461) {
            return fprintf(stderr, "packet size is invalid\n"); 
        }

        uint8 *packetBuffer = new uint8[packetByte];

        // read binary
        ifs.read((char*)packetBuffer, packetByte);

        double totalEmqtt5Time = 0.0;
        double totalVerimqttTime = 0.0;

        for(int j=0; j < PACKET_COUNT; j++) {

            // exec emqtt5 and verimqtt

            std::chrono::system_clock::time_point start, end;
            double mesureTime = 0.0;

            // emqtt5 time, C++
            start = std::chrono::system_clock::now();
            emqtt5(packetBuffer, packetByte);
            end = std::chrono::system_clock::now();

            mesureTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            totalEmqtt5Time += mesureTime;

            // verimqtt time, F*
            start = std::chrono::system_clock::now();
            verimqtt(packetBuffer, packetByte);
            end = std::chrono::system_clock::now();

            mesureTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            totalVerimqttTime += mesureTime;
        }

        int baseNameIndex = file_path.find_last_of("/")+1;
        int dotIndex = file_path.find_last_of(".");
        std::string baseName = 
            file_path.substr(baseNameIndex,dotIndex-baseNameIndex);

        // printf("%s\npacket count %d\npacket byte %ld\nemqtt5 ave time %lf[μs]\nverimqtt ave time %lf[μs]\n\n", 
        //     baseName.c_str(),
        //     PACKET_COUNT,
        //     packetByte,
        //     totalEmqtt5Time / PACKET_COUNT / 1000.0,
        //     totalVerimqttTime / PACKET_COUNT / 1000.0);

        // log << baseName << ","
        //     << PACKET_COUNT << "," 
        //     << packetByte << "," 
        //     << totalEmqtt5Time / PACKET_COUNT / 1000.0 << "," 
        //     << totalVerimqttTime / PACKET_COUNT / 1000.0 << std::endl;
    }

    return 0;
}