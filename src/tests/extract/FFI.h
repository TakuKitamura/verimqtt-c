/* 
  This file was generated by KreMLin <https://github.com/FStarLang/kremlin>
  KreMLin invocation: krml -verbose -warn-error +9 -drop WasmSupport -drop C_Endianness -drop C -tmpdir ../out -fsopt --cache_dir -fsopt ../out -no-prefix Main -no-prefix Utils -no-prefix Const -no-prefix Common -no-prefix Publish -no-prefix Connect -no-prefix Disconnect -no-prefix FFI -no-prefix Debug -no-prefix Debug_FFI -o ../mqttPacketParser.out main.fst common.fst const.fst publish.fst connect.fst disconnect.fst ffi.fst debug.fst debug_ffi.fst ffi.c debug_ffi.c callMain.c
  F* version: ad3db6d3
  KreMLin version: 2f843633
 */

#include "kremlib.h"
#ifndef __FFI_H
#define __FFI_H




extern C_String_t uint8_to_c_string(uint8_t *u8_buffer);

#define __FFI_H_DEFINED
#endif
