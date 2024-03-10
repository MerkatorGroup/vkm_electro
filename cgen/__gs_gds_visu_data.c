
/**-----------------------------------------------
 * __gs_gds_visu_data.c
 * This file is automatically generated, don't try to edit it
 * all modifications will be lost with exporting the project
 **-----------------------------------------------*/

#include <stdint.h>

typedef int32_t tObjNum;
typedef struct tagVisMaskColorTabEntry
{
  tObjNum  Number;  // Container or Mask number
  uint16_t BkColorNameIdx;
  uint16_t FrColorNameIdx;
  uint32_t Transparent;
}tVisMaskColorTabEntry;

const uint32_t _gs_ColorTableOffset = 512;
const uint32_t _gs_ContainerCnt = 4;
const uint32_t _gs_MaskCnt = 4;

const tVisMaskColorTabEntry _gs_visMaskColorTab[] =
{
  //  Co/Ma  BkColor  FrColor  Trans-
  // Number, NameIdx, NameIdx, parent
          0,   65535,   65535,     1,  // container: VelcheIcons_1
          1,   65535,   65535,     1,  // container: DashCont
          2,   65535,   65535,     1,  // container: Container2
          3,   65535,   65535,     1,  // container: ContParam
          0,   65535,   65535,     0,  // mask: 0
          1,   65535,   65535,     0,  // mask: Mask1
          2,   65535,   65535,     0,  // mask: DashBoard
          3,   65535,   65535,     0   // mask: Mask_PARAM
};

