/* 
  This file was generated by KreMLin <https://github.com/FStarLang/kremlin>
  KreMLin invocation: krml -verbose -warn-error +9 -drop WasmSupport -drop C_Endianness -drop C -tmpdir ../out -fsopt --cache_dir -fsopt ../out -no-prefix Main -no-prefix Utils -no-prefix Const -no-prefix Common -no-prefix Publish -no-prefix Connect -no-prefix Disconnect -no-prefix FFI -no-prefix Debug -no-prefix Debug_FFI -o ../mqttPacketParser.out main.fst common.fst const.fst publish.fst connect.fst disconnect.fst ffi.fst debug.fst debug_ffi.fst ffi.c debug_ffi.c callMain.c
  F* version: ad3db6d3
  KreMLin version: 2f843633
 */

#include "Connect.h"

struct_fixed_header assemble_connect_struct(struct_connect_parts s)
{
  uint8_t empty_buffer = (uint8_t)0U;
  struct_fixed_header_constant connect_constant = s.connect_connect_constant;
  return
    (
      (struct_fixed_header){
        .message_type = connect_constant.message_type_constant,
        .message_name = connect_constant.message_name_constant,
        .flags = {
          .flag = connect_constant.flags_constant.flag,
          .dup_flag = connect_constant.flags_constant.dup_flag,
          .qos_flag = connect_constant.flags_constant.qos_flag,
          .retain_flag = connect_constant.flags_constant.retain_flag
        },
        .remaining_length = s.connect_remaining_length,
        .connect = s.connect_struct,
        .publish = {
          .topic_length = (uint32_t)0U,
          .topic_name = "",
          .packet_identifier = max_u16,
          .payload = {
            .is_valid_payload = false,
            .payload_value = &empty_buffer,
            .payload_length = (uint32_t)0U
          }
        },
        .disconnect = { .disconnect_reason_code = max_u8, .disconnect_reason_code_name = "" },
        .property = s.connect_property,
        .error = { .code = define_no_error_code, .message = define_no_error }
      }
    );
}

struct_protocol_name
is_valid_protocol_name(uint8_t *packet_data, uint32_t packet_size, uint32_t next_start_index)
{
  uint8_t first_byte = packet_data[next_start_index + (uint32_t)0U];
  uint8_t second_byte = packet_data[next_start_index + (uint32_t)1U];
  uint8_t third_byte = packet_data[next_start_index + (uint32_t)2U];
  uint8_t fourth_byte = packet_data[next_start_index + (uint32_t)3U];
  uint8_t fifth_byte = packet_data[next_start_index + (uint32_t)4U];
  uint8_t sixth_byte = packet_data[next_start_index + (uint32_t)5U];
  if
  (
    !(first_byte == (uint8_t)0x00U)
    || !(second_byte == (uint8_t)0x04U)
    || !(third_byte == (uint8_t)0x4DU)
    || !(fourth_byte == (uint8_t)0x51U)
    || !(fifth_byte == (uint8_t)0x54U)
    || !(sixth_byte == (uint8_t)0x54U)
  )
    return
      (
        (struct_protocol_name){
          .is_valid_protocol_name = false,
          .protocol_version_start_index = (uint32_t)0U
        }
      );
  else
    return
      (
        (struct_protocol_name){
          .is_valid_protocol_name = true,
          .protocol_version_start_index = next_start_index + (uint32_t)6U
        }
      );
}

struct_protocol_version
is_valid_protocol_version(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint32_t next_start_index
)
{
  uint8_t protocol_version = packet_data[next_start_index];
  if (!(protocol_version == (uint8_t)0x05U))
    return
      (
        (struct_protocol_version){
          .is_valid_protocol_version = false,
          .connect_flag_start_index = (uint32_t)0U
        }
      );
  else
    return
      (
        (struct_protocol_version){
          .is_valid_protocol_version = true,
          .connect_flag_start_index = next_start_index + (uint32_t)1U
        }
      );
}

struct_connect_flag
get_connect_flag(uint8_t *packet_data, uint32_t packet_size, uint32_t next_start_index)
{
  uint8_t connect_flag = packet_data[next_start_index];
  return
    (
      (struct_connect_flag){
        .connect_flag_value = connect_flag,
        .keep_alive_start_index = next_start_index + (uint32_t)1U
      }
    );
}

struct_protocol_version
get_protocol_version_struct(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint32_t protocol_version_start_index
)
{
  if (protocol_version_start_index < packet_size - (uint32_t)1U)
    return is_valid_protocol_version(packet_data, packet_size, protocol_version_start_index);
  else
  {
    struct_protocol_version
    protocol_version_struct =
      { .is_valid_protocol_version = false, .connect_flag_start_index = (uint32_t)0U };
    return protocol_version_struct;
  }
}

struct_connect_flag
get_connect_flag_struct(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint32_t connect_flag_start_index
)
{
  if (connect_flag_start_index < packet_size - (uint32_t)1U)
    return get_connect_flag(packet_data, packet_size, connect_flag_start_index);
  else
  {
    struct_connect_flag
    connect_flag = { .connect_flag_value = max_u8, .keep_alive_start_index = (uint32_t)0U };
    return connect_flag;
  }
}

uint32_t get_connect_property_index(uint32_t keep_alive_start_index, uint32_t packet_size)
{
  uint32_t temp_index = keep_alive_start_index + (uint32_t)2U;
  if (temp_index < packet_size)
    return temp_index;
  else
    return (uint32_t)0U;
}

struct_utf8_string
get_connect_id(uint8_t *packet_data, uint32_t packet_size, uint32_t payload_start_index)
{
  uint8_t empty_buffer;
  struct_utf8_string connect_id;
  if
  (
    payload_start_index
    < packet_size - (uint32_t)1U
    && payload_start_index + (uint32_t)2U < max_packet_size
  )
    connect_id = get_utf8_encoded_string(packet_data, packet_size, payload_start_index);
  else
  {
    empty_buffer = (uint8_t)0U;
    connect_id =
      (
        (struct_utf8_string){
          .utf8_string_length = (uint16_t)0U,
          .utf8_string_value = &empty_buffer,
          .utf8_string_status_code = (uint8_t)1U,
          .utf8_next_start_index = (uint32_t)0U
        }
      );
  }
  return connect_id;
}

struct_connect_will
get_connect_will_struct(uint8_t *packet_data, uint32_t packet_size, uint32_t will_start_index)
{
  uint8_t empty_buffer0;
  uint8_t empty_buffer;
  uint8_t buf0;
  uint8_t buf;
  struct_connect_will connect_will_struct0;
  if (will_start_index < packet_size - (uint32_t)1U)
  {
    uint32_t will_property_start_index = will_start_index;
    struct_property
    property_struct = parse_property(packet_data, packet_size, will_property_start_index);
    struct_utf8_string will_topic_name_struct;
    if
    (
      property_struct.payload_start_index
      < packet_size - (uint32_t)1U
      && property_struct.payload_start_index + (uint32_t)2U < max_packet_size
    )
      will_topic_name_struct =
        get_utf8_encoded_string(packet_data,
          packet_size,
          property_struct.payload_start_index);
    else
    {
      empty_buffer0 = (uint8_t)0U;
      will_topic_name_struct =
        (
          (struct_utf8_string){
            .utf8_string_length = (uint16_t)0U,
            .utf8_string_value = &empty_buffer0,
            .utf8_string_status_code = (uint8_t)1U,
            .utf8_next_start_index = (uint32_t)0U
          }
        );
    }
    struct_binary_data will_payload_struct;
    if
    (
      packet_size
      >= (uint32_t)2U
      && will_topic_name_struct.utf8_next_start_index < packet_size - (uint32_t)2U
    )
      will_payload_struct =
        get_binary(packet_data,
          packet_size,
          will_topic_name_struct.utf8_next_start_index);
    else
    {
      empty_buffer = (uint8_t)0U;
      will_payload_struct =
        (
          (struct_binary_data){
            .is_valid_binary_data = false,
            .binary_length = (uint16_t)0U,
            .binary_value = &empty_buffer,
            .binary_next_start_index = (uint32_t)0U
          }
        );
    }
    connect_will_struct0 =
      (
        (struct_connect_will){
          .connect_will_property = property_struct,
          .connect_will_topic_name = will_topic_name_struct,
          .connect_will_payload = will_payload_struct,
          .user_name_or_password_next_start_index = will_payload_struct.binary_next_start_index
        }
      );
  }
  else
  {
    buf0 = (uint8_t)0U;
    buf = (uint8_t)0U;
    struct_connect_will
    connect_will_struct =
      {
        .connect_will_property = property_struct_base,
        .connect_will_topic_name = {
          .utf8_string_length = (uint16_t)0U,
          .utf8_string_value = &buf0,
          .utf8_string_status_code = (uint8_t)1U,
          .utf8_next_start_index = (uint32_t)0U
        },
        .connect_will_payload = {
          .is_valid_binary_data = false,
          .binary_length = (uint16_t)0U,
          .binary_value = &buf,
          .binary_next_start_index = (uint32_t)0U
        },
        .user_name_or_password_next_start_index = (uint32_t)0U
      };
    connect_will_struct0 = connect_will_struct;
  }
  return connect_will_struct0;
}

struct_keep_alive
get_keep_alive(uint8_t *packet_data, uint32_t packet_size, uint32_t keep_alive_start_index)
{
  bool ptr_is_valid_keep_alive = false;
  uint32_t msb_index = keep_alive_start_index;
  uint32_t lsb_index = keep_alive_start_index + (uint32_t)1U;
  uint16_t keep_alive;
  if (msb_index < packet_size && lsb_index < packet_size)
  {
    ptr_is_valid_keep_alive = true;
    uint32_t a = keep_alive_start_index;
    uint32_t b = keep_alive_start_index + (uint32_t)1U;
    keep_alive = get_two_byte_integer_u8_to_u16(packet_data[a], packet_data[b]);
  }
  else
    keep_alive = (uint16_t)0U;
  bool is_valid_keep_alive = ptr_is_valid_keep_alive;
  struct_keep_alive
  keep_alive_struct =
    { .keep_alive_value = keep_alive, .is_valid_keep_alive = is_valid_keep_alive };
  return keep_alive_struct;
}

struct_utf8_string
get_connect_user_name_struct(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint32_t user_name_start_index
)
{
  uint8_t buf;
  struct_utf8_string user_name_struct0;
  if
  (
    user_name_start_index
    < packet_size - (uint32_t)1U
    && user_name_start_index + (uint32_t)2U < max_packet_size
  )
  {
    struct_utf8_string
    user_name_struct = get_utf8_encoded_string(packet_data, packet_size, user_name_start_index);
    user_name_struct0 = user_name_struct;
  }
  else
  {
    buf = (uint8_t)0U;
    struct_utf8_string
    user_name_struct =
      {
        .utf8_string_length = (uint16_t)0U,
        .utf8_string_value = &buf,
        .utf8_string_status_code = (uint8_t)1U,
        .utf8_next_start_index = (uint32_t)0U
      };
    user_name_struct0 = user_name_struct;
  }
  return user_name_struct0;
}

struct_binary_data
get_connect_password_struct(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint32_t password_start_index
)
{
  uint8_t buf;
  struct_binary_data password_struct0;
  if (packet_size >= (uint32_t)2U && password_start_index < packet_size - (uint32_t)2U)
  {
    struct_binary_data
    password_struct = get_binary(packet_data, packet_size, password_start_index);
    password_struct0 = password_struct;
  }
  else
  {
    buf = (uint8_t)0U;
    struct_binary_data
    password_struct =
      {
        .is_valid_binary_data = false,
        .binary_length = (uint16_t)0U,
        .binary_value = &buf,
        .binary_next_start_index = (uint32_t)0U
      };
    password_struct0 = password_struct;
  }
  return password_struct0;
}

struct_ready_connect_will
get_connect_will(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint8_t will_flag,
  uint32_t payload_start_index,
  uint16_t connect_id_length
)
{
  bool ptr_exsist_will = false;
  bool ptr_is_valid_next_start_index = false;
  uint32_t temp = payload_start_index + (uint32_t)2U + (uint32_t)connect_id_length;
  uint32_t next_start_index;
  if (temp < max_packet_size)
    if (will_flag == (uint8_t)1U)
    {
      ptr_exsist_will = true;
      ptr_is_valid_next_start_index = true;
      next_start_index = temp;
    }
    else
    {
      ptr_exsist_will = false;
      ptr_is_valid_next_start_index = true;
      next_start_index = temp;
    }
  else
  {
    ptr_exsist_will = false;
    ptr_is_valid_next_start_index = false;
    next_start_index = (uint32_t)0U;
  }
  bool exsist_will = ptr_exsist_will;
  bool is_valid_next_start_index = ptr_is_valid_next_start_index;
  uint8_t empty_buffer = (uint8_t)0U;
  struct_connect_will connect_will_struct;
  if (exsist_will)
    connect_will_struct = get_connect_will_struct(packet_data, packet_size, next_start_index);
  else if (is_valid_next_start_index)
    connect_will_struct =
      (
        (struct_connect_will){
          .connect_will_property = property_struct_base,
          .connect_will_topic_name = {
            .utf8_string_length = (uint16_t)0U,
            .utf8_string_value = &empty_buffer,
            .utf8_string_status_code = (uint8_t)1U,
            .utf8_next_start_index = (uint32_t)0U
          },
          .connect_will_payload = {
            .is_valid_binary_data = false,
            .binary_length = (uint16_t)0U,
            .binary_value = &empty_buffer,
            .binary_next_start_index = (uint32_t)0U
          },
          .user_name_or_password_next_start_index = next_start_index
        }
      );
  else
    connect_will_struct =
      (
        (struct_connect_will){
          .connect_will_property = property_struct_base,
          .connect_will_topic_name = {
            .utf8_string_length = (uint16_t)0U,
            .utf8_string_value = &empty_buffer,
            .utf8_string_status_code = (uint8_t)1U,
            .utf8_next_start_index = (uint32_t)0U
          },
          .connect_will_payload = {
            .is_valid_binary_data = false,
            .binary_length = (uint16_t)0U,
            .binary_value = &empty_buffer,
            .binary_next_start_index = (uint32_t)0U
          },
          .user_name_or_password_next_start_index = (uint32_t)0U
        }
      );
  return
    (
      (struct_ready_connect_will){
        .ready_connect_will_struct = connect_will_struct,
        .ready_exsist_will = exsist_will
      }
    );
}

struct_ready_connect_user_name
get_connect_user_name(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint8_t user_name_flag,
  uint32_t user_name_start_index
)
{
  bool ptr_exsist_user_name = false;
  uint8_t empty_buffer = (uint8_t)0U;
  uint32_t next_start_index;
  if (user_name_flag == (uint8_t)1U)
  {
    ptr_exsist_user_name = true;
    next_start_index = user_name_start_index;
  }
  else
  {
    ptr_exsist_user_name = false;
    next_start_index = user_name_start_index;
  }
  bool exsist_user_name = ptr_exsist_user_name;
  struct_utf8_string user_name_struct;
  if (exsist_user_name)
    user_name_struct = get_connect_user_name_struct(packet_data, packet_size, next_start_index);
  else
    user_name_struct =
      (
        (struct_utf8_string){
          .utf8_string_length = (uint16_t)0U,
          .utf8_string_value = &empty_buffer,
          .utf8_string_status_code = (uint8_t)1U,
          .utf8_next_start_index = next_start_index
        }
      );
  return
    (
      (struct_ready_connect_user_name){
        .ready_connect_user_name_struct = user_name_struct,
        .ready_exsist_user_name = exsist_user_name
      }
    );
}

struct_ready_connect_password
get_connect_password(
  uint8_t *packet_data,
  uint32_t packet_size,
  uint8_t password_flag,
  uint32_t user_name_start_index
)
{
  bool ptr_exsist_password = false;
  uint8_t empty_buffer = (uint8_t)0U;
  uint32_t next_start_index;
  if (password_flag == (uint8_t)1U)
  {
    ptr_exsist_password = true;
    next_start_index = user_name_start_index;
  }
  else
  {
    ptr_exsist_password = false;
    next_start_index = user_name_start_index;
  }
  bool exsist_password = ptr_exsist_password;
  struct_binary_data password_struct;
  if (exsist_password)
    password_struct = get_connect_password_struct(packet_data, packet_size, next_start_index);
  else
    password_struct =
      (
        (struct_binary_data){
          .is_valid_binary_data = false,
          .binary_length = (uint16_t)0U,
          .binary_value = &empty_buffer,
          .binary_next_start_index = (uint32_t)0U
        }
      );
  return
    (
      (struct_ready_connect_password){
        .ready_connect_password_struct = password_struct,
        .ready_exsist_password = exsist_password
      }
    );
}

struct_connect_packet_seed
connect_packet_parser(uint8_t *packet_data, uint32_t packet_size, uint32_t next_start_index)
{
  struct_protocol_name
  protocol_name_struct = is_valid_protocol_name(packet_data, packet_size, next_start_index);
  struct_protocol_version
  protocol_version_struct =
    get_protocol_version_struct(packet_data,
      packet_size,
      protocol_name_struct.protocol_version_start_index);
  struct_connect_flag
  connect_flag_struct =
    get_connect_flag_struct(packet_data,
      packet_size,
      protocol_version_struct.connect_flag_start_index);
  struct_keep_alive
  keep_alive_struct =
    get_keep_alive(packet_data,
      packet_size,
      connect_flag_struct.keep_alive_start_index);
  uint32_t
  property_start_index =
    get_connect_property_index(connect_flag_struct.keep_alive_start_index,
      packet_size);
  struct_property property_struct;
  if (property_start_index < packet_size - (uint32_t)1U)
    property_struct = parse_property(packet_data, packet_size, property_start_index);
  else
    property_struct =
      (
        (struct_property){
          .property_id = max_u8,
          .property_type_id = max_u8,
          .property_type_struct = property_struct_type_base,
          .payload_start_index = (uint32_t)0U
        }
      );
  uint32_t payload_start_index = property_struct.payload_start_index;
  struct_utf8_string connect_id = get_connect_id(packet_data, packet_size, payload_start_index);
  uint8_t connect_flag = connect_flag_struct.connect_flag_value;
  uint8_t user_name_flag = slice_byte(connect_flag, (uint8_t)0U, (uint8_t)1U);
  uint8_t password_flag = slice_byte(connect_flag, (uint8_t)1U, (uint8_t)2U);
  uint8_t will_retain_flag = slice_byte(connect_flag, (uint8_t)2U, (uint8_t)3U);
  uint8_t will_qos_flag = slice_byte(connect_flag, (uint8_t)3U, (uint8_t)5U);
  uint8_t will_flag = slice_byte(connect_flag, (uint8_t)5U, (uint8_t)6U);
  uint8_t clean_start_flag = slice_byte(connect_flag, (uint8_t)6U, (uint8_t)7U);
  uint8_t resreved_flag = slice_byte(connect_flag, (uint8_t)7U, (uint8_t)8U);
  struct_ready_connect_will
  ready_connect_will_struct =
    get_connect_will(packet_data,
      packet_size,
      will_flag,
      payload_start_index,
      connect_id.utf8_string_length);
  struct_connect_will connect_will_struct = ready_connect_will_struct.ready_connect_will_struct;
  struct_ready_connect_user_name
  ready_connect_user_name_struct =
    get_connect_user_name(packet_data,
      packet_size,
      user_name_flag,
      connect_will_struct.user_name_or_password_next_start_index);
  struct_utf8_string
  user_name_struct = ready_connect_user_name_struct.ready_connect_user_name_struct;
  struct_ready_connect_password
  ready_connect_password_struct =
    get_connect_password(packet_data,
      packet_size,
      password_flag,
      user_name_struct.utf8_next_start_index);
  struct_binary_data
  password_struct = ready_connect_password_struct.ready_connect_password_struct;
  return
    (
      (struct_connect_packet_seed){
        .connect_seed_is_valid_protocol_name = protocol_name_struct.is_valid_protocol_name,
        .connect_seed_is_valid_protocol_version = protocol_version_struct.is_valid_protocol_version,
        .connect_seed_connect_flag = connect_flag_struct.connect_flag_value,
        .connect_seed_is_valid_keep_alive = keep_alive_struct.is_valid_keep_alive,
        .connect_seed_keep_alive = keep_alive_struct.keep_alive_value,
        .connect_seed_is_valid_property_length = true,
        .connect_seed_property = property_struct,
        .connect_seed_connect_id = connect_id,
        .connect_seed_will_struct = connect_will_struct,
        .connect_seed_user_name_struct = user_name_struct,
        .connect_seed_password_struct = password_struct
      }
    );
}

struct_fixed_header connect_packet_parse_result(struct_share_common_data share_common_data)
{
  struct_connect_packet_seed
  connect_packet_seed =
    connect_packet_parser(share_common_data.common_packet_data,
      share_common_data.common_packet_size,
      share_common_data.common_next_start_index);
  struct_fixed_header_constant
  connect_constant =
    get_struct_fixed_header_constant_except_publish(share_common_data.common_message_type);
  uint8_t connect_flag = connect_packet_seed.connect_seed_connect_flag;
  uint8_t user_name_flag = slice_byte(connect_flag, (uint8_t)0U, (uint8_t)1U);
  uint8_t password_flag = slice_byte(connect_flag, (uint8_t)1U, (uint8_t)2U);
  uint8_t will_retain_flag = slice_byte(connect_flag, (uint8_t)2U, (uint8_t)3U);
  uint8_t will_qos_flag = slice_byte(connect_flag, (uint8_t)3U, (uint8_t)5U);
  uint8_t will_flag = slice_byte(connect_flag, (uint8_t)5U, (uint8_t)6U);
  uint8_t clean_start_flag = slice_byte(connect_flag, (uint8_t)6U, (uint8_t)7U);
  uint8_t resreved_flag = slice_byte(connect_flag, (uint8_t)7U, (uint8_t)8U);
  uint16_t keep_alive = connect_packet_seed.connect_seed_keep_alive;
  bool
  have_error =
    !connect_packet_seed.connect_seed_is_valid_protocol_name
    || !connect_packet_seed.connect_seed_is_valid_protocol_version
    || !(resreved_flag == (uint8_t)0U)
    || !connect_packet_seed.connect_seed_is_valid_keep_alive
    || !connect_packet_seed.connect_seed_is_valid_property_length
    ||
      connect_packet_seed.connect_seed_property.property_type_struct.property_type_error.property_error_code
      > (uint8_t)0U
    || connect_packet_seed.connect_seed_connect_id.utf8_string_status_code > (uint8_t)0U;
  if (have_error)
  {
    struct_error_struct error_struct;
    if (!connect_packet_seed.connect_seed_is_valid_protocol_name)
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_protocol_name_invalid_code,
            .message = define_error_protocol_name_invalid
          }
        );
    else if (!connect_packet_seed.connect_seed_is_valid_protocol_version)
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_protocol_version_invalid_code,
            .message = define_error_protocol_version_invalid
          }
        );
    else if (!(resreved_flag == (uint8_t)0U))
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_connect_flag_invalid_code,
            .message = define_error_connect_flag_invalid
          }
        );
    else if (!connect_packet_seed.connect_seed_is_valid_keep_alive)
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_connect_invalid_keep_alive_code,
            .message = define_error_connect_keep_alive_invalid
          }
        );
    else if (!connect_packet_seed.connect_seed_is_valid_property_length)
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_property_length_invalid_code,
            .message = define_error_property_length_invalid
          }
        );
    else if
    (
      connect_packet_seed.connect_seed_property.property_type_struct.property_type_error.property_error_code
      > (uint8_t)0U
    )
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_property_error_code,
            .message = connect_packet_seed.connect_seed_property.property_type_struct.property_type_error.property_error_code_name
          }
        );
    else
      error_struct =
        (
          (struct_error_struct){
            .code = define_error_connect_id_invalid_code,
            .message = define_error_connect_id_invalid
          }
        );
    return error_struct_fixed_header(error_struct);
  }
  else
  {
    struct_connect
    connect_struct =
      {
        .protocol_name = "MQTT", .protocol_version = (uint8_t)5U,
        .flags = {
          .connect_flag = connect_flag, .user_name = user_name_flag, .password = password_flag,
          .will_retain = will_retain_flag, .will_qos = will_qos_flag, .will_flag = will_flag,
          .clean_start = clean_start_flag
        }, .keep_alive = keep_alive, .connect_id = connect_packet_seed.connect_seed_connect_id,
        .will = connect_packet_seed.connect_seed_will_struct,
        .user_name = connect_packet_seed.connect_seed_user_name_struct,
        .password = connect_packet_seed.connect_seed_password_struct
      };
    struct_connect_parts
    ed_fixed_header_parts =
      {
        .connect_remaining_length = share_common_data.common_remaining_length,
        .connect_connect_constant = connect_constant,
        .connect_struct = connect_struct,
        .connect_property = connect_packet_seed.connect_seed_property
      };
    return assemble_connect_struct(ed_fixed_header_parts);
  }
}
