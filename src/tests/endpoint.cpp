// Usual programs
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <chrono>

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

#define PACKET_COUNT 1000000

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
    // printf("%s\n", (const char*)out);

    return 0;
}

int verimqtt(uint8 *packetBuffer, size_t packetByte) {
    kremlinit_globals();
    struct_fixed_header data = mqtt_packet_parse(packetBuffer, packetByte);

    /*
    printf("data.message_type = 0x%02x\n", data.message_type);
    printf("data.message_name = %s\n", data.message_name);
    printf("data.flags.flag = 0x%02x\n", data.flags.flag);
    printf("data.flags.dup_flag = 0x%02x\n", data.flags.dup_flag);
    printf("data.flags.qos_flag = 0x%02x\n", data.flags.qos_flag);
    printf("data.flags.retain_flag = 0x%02x\n", data.flags.retain_flag);
    printf("data.remaining_length = %u\n", data.remaining_length);

    puts("");

    if (data.message_type == 1) {
        printf("data.connect.protocol_name = %s\n", data.connect.protocol_name);
        printf("data.connect.protocol_version = %u\n", data.connect.protocol_version);
        printf("data.connect.flags.connect_flag = 0x%02x\n", data.connect.flags.connect_flag);
        printf("data.connect.flags.user_name = 0x%02x\n", data.connect.flags.user_name);
        printf("data.connect.flags.password = 0x%02x\n", data.connect.flags.password);
        printf("data.connect.flags.will_retain = 0x%02x\n", data.connect.flags.will_retain);
        printf("data.connect.flags.will_qos = 0x%02x\n", data.connect.flags.will_qos);
        printf("data.connect.flags.will_flag = 0x%02x\n", data.connect.flags.will_flag);
        printf("data.connect.flags.clean_start = 0x%02x\n", data.connect.flags.clean_start);
        printf("data.connect.keep_alive = 0x%02x\n", data.connect.keep_alive);

        if (data.connect.connect_id.utf8_string_status_code == 0) {
            printf("data.connect.connect_id.utf8_string_length = %u\n", data.connect.connect_id.utf8_string_length);
            printf("data.connect.connect_id.utf8_string_value = \n [");
            if (data.connect.connect_id.utf8_string_length > 0) {
                for (int i=0; i < data.connect.connect_id.utf8_string_length; i++) {
                    printf("0x%02X", data.connect.connect_id.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.connect.connect_id.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }
            } else {
                puts("]");
            }

        }


        if (data.connect.will.connect_will_topic_name.utf8_string_status_code == 0) {
            if (data.connect.will.connect_will_property.property_id != 255) {
                if (data.connect.will.connect_will_property.property_type_struct.property_type_error.property_error_code == 0) {
                    printf("data.property.payload_start_index = %u\n", data.connect.will.connect_will_property.payload_start_index);
                    printf("data.connect.will.connect_will_property.property_id = %u\n", data.connect.will.connect_will_property.property_id);
                    printf("data.connect.will.connect_will_property.property_type_id = %u\n", data.connect.will.connect_will_property.property_type_id);

                    if (data.connect.will.connect_will_property.property_type_id == 1) {
                        printf("data.connect.will.connect_will_property.property_type_struct.one_byte_integer_struct.one_byte_integer_value = %u\n", data.connect.will.connect_will_property.property_type_struct.one_byte_integer_struct);
                    } else if (data.connect.will.connect_will_property.property_type_id == 2) {
                        printf("data.connect.will.connect_will_property.property_type_struct.two_byte_integer_struct.two_byte_integer_value = %u\n", data.connect.will.connect_will_property.property_type_struct.two_byte_integer_struct);           
                    } else if (data.connect.will.connect_will_property.property_type_id == 3) {
                        printf("data.connect.will.connect_will_property.property_type_struct.four_byte_integer_struct.four_byte_integer_value = %u\n", data.connect.will.connect_will_property.property_type_struct.four_byte_integer_struct);  
                    } else if (data.connect.will.connect_will_property.property_type_id == 4) {
                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_length = %u\n", data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_length);
                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_value = \n [");
                        for (int i=0; i < data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_length; i++) {
                            printf("0x%02X", data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_value[i] & 0x000000FF);
                            if (i + 1 == data.connect.will.connect_will_property.property_type_struct.utf8_encoded_string_struct.utf8_string_length) 
                                puts("]");
                            else
                                printf(", ");
                        }
                    } else if (data.connect.will.connect_will_property.property_type_id == 5) {
                        printf("data.connect.will.connect_will_property.property_type_struct.variable_byte_integer_struct.variable_integer_value = %u\n", data.connect.will.connect_will_property.property_type_struct.variable_byte_integer_struct);  
                    } else if (data.connect.will.connect_will_property.property_type_id == 6) {
                        printf("data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_length = %u\n", data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_length);
                        printf("data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_value = \n [");
                        for (int i=0; i < data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_length; i++) {
                            printf("0x%02X", data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_value[i] & 0x000000FF);
                            if (i + 1 == data.connect.will.connect_will_property.property_type_struct.binary_data_struct.binary_length) 
                                puts("]");
                            else
                                printf(", ");
                        }
                    } else if (data.connect.will.connect_will_property.property_type_id == 7) {
                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length = %u\n", data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length);
                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_value = \n [");
                        for (int i=0; i < data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length; i++) {
                            printf("0x%02X", data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_value[i] & 0x000000FF);
                            if (i + 1 == data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length) 
                                puts("]");
                            else
                                printf(", ");
                        }

                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length = %u\n", data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length);
                        printf("data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_value = \n [");
                        for (int i=0; i < data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length; i++) {
                            printf("0x%02X", data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_value[i] & 0x000000FF);
                            if (i + 1 == data.connect.will.connect_will_property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length) 
                                puts("]");
                            else
                                printf(", ");
                        }
                        

                    }
                    puts("");
                } else {
                    // puts("property error");
                    printf("property error code = %u\n", data.connect.will.connect_will_property.property_type_struct.property_type_error.property_error_code);
                    printf("property error code name = %s\n", data.connect.will.connect_will_property.property_type_struct.property_type_error.property_error_code_name);
                }
            } else {
                puts("property type id is invalid");
            }

            printf("data.connect.will.connect_will_topic_name.utf8_string_length = %u\n", data.connect.will.connect_will_topic_name.utf8_string_length);
            printf("data.connect.will.connect_will_topic_name.utf8_string_value = \n [");
            if (data.connect.will.connect_will_topic_name.utf8_string_length > 0) {
                for (int i=0; i < data.connect.will.connect_will_topic_name.utf8_string_length; i++) {
                    printf("0x%02X", data.connect.will.connect_will_topic_name.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.connect.will.connect_will_topic_name.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }
            } else {
                puts("]");
            }

            printf("data.connect.will.connect_will_payload.binary_length = %u\n", data.connect.will.connect_will_payload.binary_length);
            printf("data.connect.will.connect_will_payload.binary_value = \n [");
            for (int i=0; i < data.connect.will.connect_will_payload.binary_length; i++) {
                printf("0x%02X", data.connect.will.connect_will_payload.binary_value[i] & 0x000000FF);
                if (i + 1 == data.connect.will.connect_will_payload.binary_length) 
                    puts("]");
                else
                    printf(", ");
            }

            puts("");            
        }

        puts("");

        if (data.connect.user_name.utf8_string_status_code == 0) {
            printf("data.connect.user_name.utf8_string_length = %u\n", data.connect.user_name.utf8_string_length);
            printf("data.connect.user_name.utf8_string_value = \n [");
            if (data.connect.user_name.utf8_string_length > 0) {
                for (int i=0; i < data.connect.user_name.utf8_string_length; i++) {
                    printf("0x%02X", data.connect.user_name.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.connect.user_name.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }
            } else {
                puts("]");
            }
        }

        if (data.connect.password.is_valid_binary_data) {
            printf("data.connect.password.binary_length = %u\n", data.connect.password.binary_length);
            printf("data.connect.password.binary_value = \n [");
            for (int i=0; i < data.connect.password.binary_length; i++) {
                printf("0x%02X", data.connect.password.binary_value[i] & 0x000000FF);
                if (i + 1 == data.connect.password.binary_length) 
                    puts("]");
                else
                    printf(", ");
            }
        }


        puts("");
    } else if (data.message_type == 3) {

        printf("data.publish.topic_length = %u\n", data.publish.topic_length);
        printf("data.publish.topic_name =\n [");
        for (int i=0; i < data.publish.topic_length; i++) {
            printf("0x%02X", data.publish.topic_name[i] & 0x000000FF);
            if (i + 1 == data.publish.topic_length) 
                puts("]\n");
            else
                printf(", ");
        }

        if (data.flags.qos_flag > 0) {
            printf("data.publish.packet_identifier = %d\n", data.publish.packet_identifier);
        }

        printf("data.publish.payload =\n [");
        for (int i=0; i < data.publish.payload.payload_length; i++) {
            printf("0x%02X", data.publish.payload.payload_value[i] & 0x000000FF);
            if (i + 1 == data.publish.payload.payload_length) 
                puts("]");
            else
                printf(", ");
        }
        printf("data.publish.payload_length = %u\n", data.publish.payload.payload_length);

        puts("");
    } else if (data.message_type == 14) {
        printf("data.disconnect.disconnect_reason_code = 0x%02x\n", data.disconnect.disconnect_reason_code);
        printf("data.disconnect.disconnect_reason_code_name = %s\n", data.disconnect.disconnect_reason_code_name);
        puts("");
    }

    puts("");
 
    if (data.property.property_type_id != 255) {
        if (data.property.property_type_struct.property_type_error.property_error_code == 0 &&
            data.property.property_type_id != 0) {
            printf("data.property.payload_start_index = %u\n", data.property.payload_start_index);
            printf("data.property.property_id = %u\n", data.property.property_id);
            printf("data.property.property_type_id = %u\n", data.property.property_type_id);

            if (data.property.property_type_id == 1) {
                printf("data.property.property_type_struct.one_byte_integer_struct.one_byte_integer_value = %u\n", data.property.property_type_struct.one_byte_integer_struct);
            } else if (data.property.property_type_id == 2) {
                printf("data.property.property_type_struct.two_byte_integer_struct.two_byte_integer_value = %u\n", data.property.property_type_struct.two_byte_integer_struct);           
            } else if (data.property.property_type_id == 3) {
                printf("data.property.property_type_struct.four_byte_integer_struct.four_byte_integer_value = %u\n", data.property.property_type_struct.four_byte_integer_struct);  
            } else if (data.property.property_type_id == 4) {
                printf("data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_length = %u\n", data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_length);
                printf("data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_value = \n [");
                for (int i=0; i < data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_length; i++) {
                    printf("0x%02X", data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.property.property_type_struct.utf8_encoded_string_struct.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }
            } else if (data.property.property_type_id == 5) {
                printf("data.property.property_type_struct.variable_byte_integer_struct.variable_integer_value = %u\n", data.property.property_type_struct.variable_byte_integer_struct);  
            } else if (data.property.property_type_id == 6) {
                printf("data.property.property_type_struct.binary_data_struct.binary_length = %u\n", data.property.property_type_struct.binary_data_struct.binary_length);
                printf("data.property.property_type_struct.binary_data_struct.binary_value = \n [");
                for (int i=0; i < data.property.property_type_struct.binary_data_struct.binary_length; i++) {
                    printf("0x%02X", data.property.property_type_struct.binary_data_struct.binary_value[i] & 0x000000FF);
                    if (i + 1 == data.property.property_type_struct.binary_data_struct.binary_length) 
                        puts("]");
                    else
                        printf(", ");
                }
            } else if (data.property.property_type_id == 7) {
                printf("data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length = %u\n", data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length);
                printf("data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_value = \n [");
                for (int i=0; i < data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length; i++) {
                    printf("0x%02X", data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_key.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }

                printf("data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length = %u\n", data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length);
                printf("data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_value = \n [");
                for (int i=0; i < data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length; i++) {
                    printf("0x%02X", data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_value[i] & 0x000000FF);
                    if (i + 1 == data.property.property_type_struct.utf8_string_pair_struct.utf8_string_pair_value.utf8_string_length) 
                        puts("]");
                    else
                        printf(", ");
                }
                

            }
            puts("");
        } else {
            // puts("property error");
            printf("property error code = %u\n", data.property.property_type_struct.property_type_error.property_error_code);
            printf("property error code name = %s\n", data.property.property_type_struct.property_type_error.property_error_code_name);
        }
    } else {
        puts("property type id is invalid");
    }

    printf("data.error.code=%u\n", data.error.code);
    printf("data.error.message=%s\n", data.error.message);

    */
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

        printf("%s\npacket count %d\npacket byte %ld\nemqtt5 ave time %lf[μs]\nverimqtt ave time %lf[μs]\n\n", 
            baseName.c_str(),
            PACKET_COUNT,
            packetByte,
            totalEmqtt5Time / PACKET_COUNT / 1000.0,
            totalVerimqttTime / PACKET_COUNT / 1000.0);

        log << baseName << ","
            << PACKET_COUNT << "," 
            << packetByte << "," 
            << totalEmqtt5Time / PACKET_COUNT / 1000.0 << "," 
            << totalVerimqttTime / PACKET_COUNT / 1000.0 << std::endl;
    }

    return 0;
}