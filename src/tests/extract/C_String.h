/* 
  This file was generated by KreMLin <https://github.com/FStarLang/kremlin>
  KreMLin invocation: krml -verbose -warn-error +9 -drop WasmSupport -drop C_Endianness -drop C -tmpdir ../out -fsopt --cache_dir -fsopt ../out -no-prefix Main -no-prefix Utils -no-prefix Const -no-prefix Common -no-prefix Publish -no-prefix Connect -no-prefix Disconnect -no-prefix FFI -no-prefix Debug -no-prefix Debug_FFI -o ../mqttPacketParser.out main.fst common.fst const.fst publish.fst connect.fst disconnect.fst ffi.fst debug.fst debug_ffi.fst ffi.c debug_ffi.c callMain.c
  F* version: ad3db6d3
  KreMLin version: 2f843633
 */

#include "kremlib.h"
#ifndef __C_String_H
#define __C_String_H




extern void C_String_print(C_String_t uu____1293);

extern uint32_t C_String_strlen(C_String_t s);

extern void C_String_memcpy(uint8_t *dst, C_String_t src, uint32_t n1);

#define __C_String_H_DEFINED
#endif