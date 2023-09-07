/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    sdm845.c

Abstract:

    This module contains support for the Qualcomm SDM 845 serial UART.

--*/

// ------------------------------------------------------------------- Includes

#include "common.h"

// ---------------------------------------------------------------- Definitions

// Different base addresses offset
#define GENI4_CFG           0x0
#define GENI4_IMAGE_REGS    0x100
#define GENI4_DATA          0x600
#define QUPV3_SE_DMA        0xC00
#define GENI4_IMAGE         0x1000

//
// QUPV3_SE
//

#define TX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK
#define TX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT

#define RX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK
#define RX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT

//
// FIFO
//

#define RX_LAST_VALID_BYTES_MASK     HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_BMSK
#define RX_LAST_VALID_BYTES_SHIFT    HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_SHFT
#define RX_FIFO_WC                   HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_BMSK
#define RX_FIFO_WC_SHIFT             HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_SHFT

//
// IRQ
//

#define TF_FIFO_WATERMARK_IRQ       HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_BMSK
#define M_CMD_DONE_IRQ              HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_BMSK
#define SEC_IRQ                     HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_BMSK

#define RX_LAST_IRQ                 HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_BMSK
#define RX_FIFO_WATERMARK_IRQ       HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK

//
// MODULE: GENI4_CFG
//

#define GENI4_CFG_REG_BASE                                                                      (QUPV3_NORTH_QUPV3_ID_1_BASE      + 0x00080000)
#define GENI4_CFG_REG_BASE_OFFS                                                                 0x00080000

#define GENI4_CFG_REG_BASE                                                                      (QUPV3_NORTH_QUPV3_ID_1_BASE      + 0x00080000)
#define GENI4_CFG_REG_BASE_OFFS                                                                 0x00080000


#define HWIO_GENI_OUTPUT_CTRL_ADDR(x)                                                           ((x) + 0x00000024)
#define HWIO_GENI_OUTPUT_CTRL_OFFS                                                              (0x00000024)
#define HWIO_GENI_OUTPUT_CTRL_RMSK                                                                    0x7f
#define HWIO_GENI_OUTPUT_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_OUTPUT_CTRL_ADDR(x), HWIO_GENI_OUTPUT_CTRL_RMSK)
#define HWIO_GENI_OUTPUT_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_OUTPUT_CTRL_ADDR(x), m)
#define HWIO_GENI_OUTPUT_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_OUTPUT_CTRL_ADDR(x),v)
#define HWIO_GENI_OUTPUT_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_OUTPUT_CTRL_ADDR(x),m,v,HWIO_GENI_OUTPUT_CTRL_IN(x))
#define HWIO_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_BMSK                                                     0x7f
#define HWIO_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_SHFT                                                      0x0

#define HWIO_GENI_CGC_CTRL_ADDR(x)                                                              ((x) + 0x00000028)
#define HWIO_GENI_CGC_CTRL_OFFS                                                                 (0x00000028)
#define HWIO_GENI_CGC_CTRL_RMSK                                                                      0x37f
#define HWIO_GENI_CGC_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_CGC_CTRL_ADDR(x), HWIO_GENI_CGC_CTRL_RMSK)
#define HWIO_GENI_CGC_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CGC_CTRL_ADDR(x), m)
#define HWIO_GENI_CGC_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_CGC_CTRL_ADDR(x),v)
#define HWIO_GENI_CGC_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CGC_CTRL_ADDR(x),m,v,HWIO_GENI_CGC_CTRL_IN(x))
#define HWIO_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK                                                    0x200
#define HWIO_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_SHFT                                                      0x9
#define HWIO_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK                                                    0x100
#define HWIO_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_SHFT                                                      0x8
#define HWIO_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK                                                        0x40
#define HWIO_GENI_CGC_CTRL_EXT_CLK_CGC_ON_SHFT                                                         0x6
#define HWIO_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK                                                         0x20
#define HWIO_GENI_CGC_CTRL_RX_CLK_CGC_ON_SHFT                                                          0x5
#define HWIO_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK                                                         0x10
#define HWIO_GENI_CGC_CTRL_TX_CLK_CGC_ON_SHFT                                                          0x4
#define HWIO_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK                                                            0x8
#define HWIO_GENI_CGC_CTRL_SCLK_CGC_ON_SHFT                                                            0x3
#define HWIO_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK                                                    0x4
#define HWIO_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_SHFT                                                    0x2
#define HWIO_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK                                                  0x2
#define HWIO_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_SHFT                                                  0x1
#define HWIO_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK                                                     0x1
#define HWIO_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_SHFT                                                     0x0


#define HWIO_GENI_STATUS_ADDR(x)                                                                ((x) + 0x00000040)
#define HWIO_GENI_STATUS_OFFS                                                                   (0x00000040)
#define HWIO_GENI_STATUS_RMSK                                                                     0x1fffff
#define HWIO_GENI_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_STATUS_ADDR(x), HWIO_GENI_STATUS_RMSK)
#define HWIO_GENI_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_STATUS_ADDR(x), m)
#define HWIO_GENI_STATUS_S_GENI_CMD_FSM_STATE_BMSK                                                0x1f0000
#define HWIO_GENI_STATUS_S_GENI_CMD_FSM_STATE_SHFT                                                    0x10
#define HWIO_GENI_STATUS_NOT_USED_BITS_3_BMSK                                                       0xe000
#define HWIO_GENI_STATUS_NOT_USED_BITS_3_SHFT                                                          0xd
#define HWIO_GENI_STATUS_S_GENI_CMD_ACTIVE_BMSK                                                     0x1000
#define HWIO_GENI_STATUS_S_GENI_CMD_ACTIVE_SHFT                                                        0xc
#define HWIO_GENI_STATUS_NOT_USED_BITS_2_BMSK                                                        0xe00
#define HWIO_GENI_STATUS_NOT_USED_BITS_2_SHFT                                                          0x9
#define HWIO_GENI_STATUS_M_GENI_CMD_FSM_STATE_BMSK                                                   0x1f0
#define HWIO_GENI_STATUS_M_GENI_CMD_FSM_STATE_SHFT                                                     0x4
#define HWIO_GENI_STATUS_NOT_USED_BITS_1_BMSK                                                          0xe
#define HWIO_GENI_STATUS_NOT_USED_BITS_1_SHFT                                                          0x1
#define HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK                                                        0x1
#define HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_SHFT                                                        0x0


#define HWIO_GENI_SER_M_CLK_CFG_ADDR(x)                                                         ((x) + 0x00000048)
#define HWIO_GENI_SER_M_CLK_CFG_OFFS                                                            (0x00000048)
#define HWIO_GENI_SER_M_CLK_CFG_RMSK                                                                0xfff1
#define HWIO_GENI_SER_M_CLK_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_SER_M_CLK_CFG_ADDR(x), HWIO_GENI_SER_M_CLK_CFG_RMSK)
#define HWIO_GENI_SER_M_CLK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_SER_M_CLK_CFG_ADDR(x), m)
#define HWIO_GENI_SER_M_CLK_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_SER_M_CLK_CFG_ADDR(x),v)
#define HWIO_GENI_SER_M_CLK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_SER_M_CLK_CFG_ADDR(x),m,v,HWIO_GENI_SER_M_CLK_CFG_IN(x))
#define HWIO_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_BMSK                                                  0xfff0
#define HWIO_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_SHFT                                                     0x4
#define HWIO_GENI_SER_M_CLK_CFG_SER_CLK_EN_BMSK                                                        0x1
#define HWIO_GENI_SER_M_CLK_CFG_SER_CLK_EN_SHFT                                                        0x0

#define HWIO_GENI_SER_S_CLK_CFG_ADDR(x)                                                         ((x) + 0x0000004c)
#define HWIO_GENI_SER_S_CLK_CFG_OFFS                                                            (0x0000004c)
#define HWIO_GENI_SER_S_CLK_CFG_RMSK                                                                0xfff1
#define HWIO_GENI_SER_S_CLK_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_SER_S_CLK_CFG_ADDR(x), HWIO_GENI_SER_S_CLK_CFG_RMSK)
#define HWIO_GENI_SER_S_CLK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_SER_S_CLK_CFG_ADDR(x), m)
#define HWIO_GENI_SER_S_CLK_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_SER_S_CLK_CFG_ADDR(x),v)
#define HWIO_GENI_SER_S_CLK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_SER_S_CLK_CFG_ADDR(x),m,v,HWIO_GENI_SER_S_CLK_CFG_IN(x))
#define HWIO_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_BMSK                                                  0xfff0
#define HWIO_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_SHFT                                                     0x4
#define HWIO_GENI_SER_S_CLK_CFG_SER_CLK_EN_BMSK                                                        0x1
#define HWIO_GENI_SER_S_CLK_CFG_SER_CLK_EN_SHFT                                                        0x0

#define HWIO_GENI_DFS_IF_CFG_ADDR(x)                                                            ((x) + 0x00000080)
#define HWIO_GENI_DFS_IF_CFG_OFFS                                                               (0x00000080)
#define HWIO_GENI_DFS_IF_CFG_RMSK                                                                    0x701
#define HWIO_GENI_DFS_IF_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_DFS_IF_CFG_ADDR(x), HWIO_GENI_DFS_IF_CFG_RMSK)
#define HWIO_GENI_DFS_IF_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_DFS_IF_CFG_ADDR(x), m)
#define HWIO_GENI_DFS_IF_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_DFS_IF_CFG_ADDR(x),v)
#define HWIO_GENI_DFS_IF_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_DFS_IF_CFG_ADDR(x),m,v,HWIO_GENI_DFS_IF_CFG_IN(x))
#define HWIO_GENI_DFS_IF_CFG_NUM_WAIT_STATES_BMSK                                                    0x700
#define HWIO_GENI_DFS_IF_CFG_NUM_WAIT_STATES_SHFT                                                      0x8
#define HWIO_GENI_DFS_IF_CFG_DFS_IF_EN_BMSK                                                            0x1
#define HWIO_GENI_DFS_IF_CFG_DFS_IF_EN_SHFT                                                            0x0

#define HWIO_GENI_FORCE_DEFAULT_REG_ADDR(x)                                                     ((x) + 0x00000020)
#define HWIO_GENI_FORCE_DEFAULT_REG_OFFS                                                        (0x00000020)
#define HWIO_GENI_FORCE_DEFAULT_REG_RMSK                                                               0x1
#define HWIO_GENI_FORCE_DEFAULT_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_FORCE_DEFAULT_REG_ADDR(x),v)
#define HWIO_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK                                                 0x1
#define HWIO_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_SHFT                                                 0x0

//
// MODULE: GENI4_DATA
//

#define GENI4_DATA_REG_BASE                                                               (QUPV3_NORTH_QUPV3_ID_1_BASE      + 0x00080600)
#define GENI4_DATA_REG_BASE_OFFS                                                          0x00080600

#define HWIO_GENI_M_CMD0_ADDR(x)                                                          ((x) + 0x00000000)
#define HWIO_GENI_M_CMD0_OFFS                                                             (0x00000000)
#define HWIO_GENI_M_CMD0_RMSK                                                             0xf9ffffff
#define HWIO_GENI_M_CMD0_IN(x)      \
        in_dword_masked(HWIO_GENI_M_CMD0_ADDR(x), HWIO_GENI_M_CMD0_RMSK)
#define HWIO_GENI_M_CMD0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_CMD0_ADDR(x), m)
#define HWIO_GENI_M_CMD0_OUT(x, v)      \
        out_dword(HWIO_GENI_M_CMD0_ADDR(x),v)
#define HWIO_GENI_M_CMD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_M_CMD0_ADDR(x),m,v,HWIO_GENI_M_CMD0_IN(x))
#define HWIO_GENI_M_CMD0_OPCODE_BMSK                                                      0xf8000000
#define HWIO_GENI_M_CMD0_OPCODE_SHFT                                                            0x1b
#define HWIO_GENI_M_CMD0_GNRL_PURP_BMSK                                                    0x1000000
#define HWIO_GENI_M_CMD0_GNRL_PURP_SHFT                                                         0x18
#define HWIO_GENI_M_CMD0_PARAM_BMSK                                                         0xffffff
#define HWIO_GENI_M_CMD0_PARAM_SHFT                                                              0x0

#define HWIO_GENI_M_IRQ_STATUS_ADDR(x)                                                    ((x) + 0x00000010)
#define HWIO_GENI_M_IRQ_STATUS_OFFS                                                       (0x00000010)
#define HWIO_GENI_M_IRQ_STATUS_RMSK                                                       0xffc07fff
#define HWIO_GENI_M_IRQ_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_M_IRQ_STATUS_ADDR(x), HWIO_GENI_M_IRQ_STATUS_RMSK)
#define HWIO_GENI_M_IRQ_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_IRQ_STATUS_ADDR(x), m)
#define HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_BMSK                                               0x80000000
#define HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_SHFT                                                     0x1f
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_BMSK                                     0x40000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_SHFT                                           0x1e
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_BMSK                                        0x20000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_SHFT                                              0x1d
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_BMSK                                        0x10000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_SHFT                                              0x1c
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_LAST_BMSK                                           0x8000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_LAST_SHFT                                                0x1b
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                                      0x4000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                                           0x1a
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                                         0x2000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                                              0x19
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                                         0x1000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                                              0x18
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_BMSK                                          0x800000
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_SHFT                                              0x17
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                                        0x400000
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                                            0x16
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_5_BMSK                                                0x4000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_5_SHFT                                                   0xe
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_4_BMSK                                                0x2000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_4_SHFT                                                   0xd
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_3_BMSK                                                0x1000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_3_SHFT                                                   0xc
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_2_BMSK                                                 0x800
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_2_SHFT                                                   0xb
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_1_BMSK                                                 0x400
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_1_SHFT                                                   0xa
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_0_BMSK                                                 0x200
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_0_SHFT                                                   0x9
#define HWIO_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_BMSK                                            0x100
#define HWIO_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_SHFT                                              0x8
#define HWIO_GENI_M_IRQ_STATUS_M_RX_IRQ_BMSK                                                    0x80
#define HWIO_GENI_M_IRQ_STATUS_M_RX_IRQ_SHFT                                                     0x7
#define HWIO_GENI_M_IRQ_STATUS_M_TIMESTAMP_BMSK                                                 0x40
#define HWIO_GENI_M_IRQ_STATUS_M_TIMESTAMP_SHFT                                                  0x6
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_ABORT_BMSK                                                 0x20
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_ABORT_SHFT                                                  0x5
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_CANCEL_BMSK                                                0x10
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_CANCEL_SHFT                                                 0x4
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_FAILURE_BMSK                                                0x8
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_FAILURE_SHFT                                                0x3
#define HWIO_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_BMSK                                                0x4
#define HWIO_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_SHFT                                                0x2
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_BMSK                                                0x2
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_SHFT                                                0x1
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_BMSK                                                   0x1
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_SHFT                                                   0x0

#define HWIO_GENI_M_IRQ_ENABLE_ADDR(x)                                                    ((x) + 0x00000014)
#define HWIO_GENI_M_IRQ_ENABLE_OFFS                                                       (0x00000014)
#define HWIO_GENI_M_IRQ_ENABLE_RMSK                                                       0xffc07fff
#define HWIO_GENI_M_IRQ_ENABLE_IN(x)      \
        in_dword_masked(HWIO_GENI_M_IRQ_ENABLE_ADDR(x), HWIO_GENI_M_IRQ_ENABLE_RMSK)
#define HWIO_GENI_M_IRQ_ENABLE_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_IRQ_ENABLE_ADDR(x), m)
#define HWIO_GENI_M_IRQ_ENABLE_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_ENABLE_ADDR(x),v)
#define HWIO_GENI_M_IRQ_ENABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_M_IRQ_ENABLE_ADDR(x),m,v,HWIO_GENI_M_IRQ_ENABLE_IN(x))
#define HWIO_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_BMSK                                            0x80000000
#define HWIO_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_SHFT                                                  0x1f
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_BMSK                                  0x40000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_SHFT                                        0x1e
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_BMSK                                     0x20000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_SHFT                                           0x1d
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_BMSK                                     0x10000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_SHFT                                           0x1c
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                                        0x8000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                                             0x1b
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                                   0x4000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                                        0x1a
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                                      0x2000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                                           0x19
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                                      0x1000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                                           0x18
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                                       0x800000
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                                           0x17
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                                     0x400000
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                                         0x16
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_BMSK                                             0x4000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_SHFT                                                0xe
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_BMSK                                             0x2000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_SHFT                                                0xd
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_BMSK                                             0x1000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_SHFT                                                0xc
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_BMSK                                              0x800
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_SHFT                                                0xb
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_BMSK                                              0x400
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_SHFT                                                0xa
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_BMSK                                              0x200
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_SHFT                                                0x9
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_BMSK                                         0x100
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_SHFT                                           0x8
#define HWIO_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_BMSK                                                 0x80
#define HWIO_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_SHFT                                                  0x7
#define HWIO_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_BMSK                                              0x40
#define HWIO_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_SHFT                                               0x6
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_BMSK                                              0x20
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_SHFT                                               0x5
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_BMSK                                             0x10
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_SHFT                                              0x4
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_BMSK                                             0x8
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_SHFT                                             0x3
#define HWIO_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_BMSK                                             0x4
#define HWIO_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_SHFT                                             0x2
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_BMSK                                             0x2
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_SHFT                                             0x1
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_BMSK                                                0x1
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_SHFT                                                0x0

#define HWIO_GENI_M_IRQ_CLEAR_ADDR(x)                                                     ((x) + 0x00000018)
#define HWIO_GENI_M_IRQ_CLEAR_OFFS                                                        (0x00000018)
#define HWIO_GENI_M_IRQ_CLEAR_RMSK                                                        0xffc07fff
#define HWIO_GENI_M_IRQ_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_CLEAR_ADDR(x),v)
#define HWIO_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_BMSK                                          0x80000000
#define HWIO_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_SHFT                                                0x1f
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_BMSK                                0x40000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_SHFT                                      0x1e
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_BMSK                                   0x20000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_SHFT                                         0x1d
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_BMSK                                   0x10000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_SHFT                                         0x1c
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                                      0x8000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                                           0x1b
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                                 0x4000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                                      0x1a
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                                    0x2000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                                         0x19
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                                    0x1000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                                         0x18
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                                     0x800000
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                                         0x17
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                                   0x400000
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                                       0x16
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_BMSK                                           0x4000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_SHFT                                              0xe
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_BMSK                                           0x2000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_SHFT                                              0xd
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_BMSK                                           0x1000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_SHFT                                              0xc
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_BMSK                                            0x800
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_SHFT                                              0xb
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_BMSK                                            0x400
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_SHFT                                              0xa
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_BMSK                                            0x200
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_SHFT                                              0x9
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_BMSK                                       0x100
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_SHFT                                         0x8
#define HWIO_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_BMSK                                               0x80
#define HWIO_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_SHFT                                                0x7
#define HWIO_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_BMSK                                            0x40
#define HWIO_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_SHFT                                             0x6
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_BMSK                                            0x20
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_SHFT                                             0x5
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_BMSK                                           0x10
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_SHFT                                            0x4
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_BMSK                                           0x8
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_SHFT                                           0x3
#define HWIO_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_BMSK                                           0x4
#define HWIO_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_SHFT                                           0x2
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_BMSK                                           0x2
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_SHFT                                           0x1
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_BMSK                                              0x1
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_SHFT                                              0x0

#define HWIO_GENI_S_CMD0_ADDR(x)                                                          ((x) + 0x00000030)
#define HWIO_GENI_S_CMD0_OFFS                                                             (0x00000030)
#define HWIO_GENI_S_CMD0_RMSK                                                             0xf9ffffff
#define HWIO_GENI_S_CMD0_IN(x)      \
        in_dword_masked(HWIO_GENI_S_CMD0_ADDR(x), HWIO_GENI_S_CMD0_RMSK)
#define HWIO_GENI_S_CMD0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_CMD0_ADDR(x), m)
#define HWIO_GENI_S_CMD0_OUT(x, v)      \
        out_dword(HWIO_GENI_S_CMD0_ADDR(x),v)
#define HWIO_GENI_S_CMD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_CMD0_ADDR(x),m,v,HWIO_GENI_S_CMD0_IN(x))
#define HWIO_GENI_S_CMD0_OPCODE_BMSK                                                      0xf8000000
#define HWIO_GENI_S_CMD0_OPCODE_SHFT                                                            0x1b
#define HWIO_GENI_S_CMD0_GNRL_PURP_BMSK                                                    0x1000000
#define HWIO_GENI_S_CMD0_GNRL_PURP_SHFT                                                         0x18
#define HWIO_GENI_S_CMD0_PARAM_BMSK                                                         0xffffff
#define HWIO_GENI_S_CMD0_PARAM_SHFT                                                              0x0

#define HWIO_GENI_S_IRQ_STATUS_ADDR(x)                                                    ((x) + 0x00000040)
#define HWIO_GENI_S_IRQ_STATUS_OFFS                                                       (0x00000040)
#define HWIO_GENI_S_IRQ_STATUS_RMSK                                                        0xfc07f3f
#define HWIO_GENI_S_IRQ_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_S_IRQ_STATUS_ADDR(x), HWIO_GENI_S_IRQ_STATUS_RMSK)
#define HWIO_GENI_S_IRQ_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_IRQ_STATUS_ADDR(x), m)
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_BMSK                                           0x8000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_SHFT                                                0x1b
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                                      0x4000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                                           0x1a
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                                         0x2000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                                              0x19
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                                         0x1000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                                              0x18
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_BMSK                                          0x800000
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_SHFT                                              0x17
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                                        0x400000
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                                            0x16
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_5_BMSK                                                0x4000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_5_SHFT                                                   0xe
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_4_BMSK                                                0x2000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_4_SHFT                                                   0xd
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_BMSK                                                0x1000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_SHFT                                                   0xc
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_BMSK                                                 0x800
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_SHFT                                                   0xb
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_1_BMSK                                                 0x400
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_1_SHFT                                                   0xa
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_0_BMSK                                                 0x200
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_0_SHFT                                                   0x9
#define HWIO_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_BMSK                                            0x100
#define HWIO_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_SHFT                                              0x8
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_ABORT_BMSK                                                 0x20
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_ABORT_SHFT                                                  0x5
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_CANCEL_BMSK                                                0x10
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_CANCEL_SHFT                                                 0x4
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_FAILURE_BMSK                                                0x8
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_FAILURE_SHFT                                                0x3
#define HWIO_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_BMSK                                                0x4
#define HWIO_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_SHFT                                                0x2
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_BMSK                                                0x2
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_SHFT                                                0x1
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_DONE_BMSK                                                   0x1
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_DONE_SHFT                                                   0x0

#define HWIO_GENI_S_IRQ_ENABLE_ADDR(x)                                                    ((x) + 0x00000044)
#define HWIO_GENI_S_IRQ_ENABLE_OFFS                                                       (0x00000044)
#define HWIO_GENI_S_IRQ_ENABLE_RMSK                                                        0xfc07f3f
#define HWIO_GENI_S_IRQ_ENABLE_IN(x)      \
        in_dword_masked(HWIO_GENI_S_IRQ_ENABLE_ADDR(x), HWIO_GENI_S_IRQ_ENABLE_RMSK)
#define HWIO_GENI_S_IRQ_ENABLE_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_IRQ_ENABLE_ADDR(x), m)
#define HWIO_GENI_S_IRQ_ENABLE_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_ENABLE_ADDR(x),v)
#define HWIO_GENI_S_IRQ_ENABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_IRQ_ENABLE_ADDR(x),m,v,HWIO_GENI_S_IRQ_ENABLE_IN(x))
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                                        0x8000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                                             0x1b
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                                   0x4000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                                        0x1a
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                                      0x2000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                                           0x19
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                                      0x1000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                                           0x18
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                                       0x800000
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                                           0x17
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                                     0x400000
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                                         0x16
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_BMSK                                             0x4000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_SHFT                                                0xe
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_BMSK                                             0x2000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_SHFT                                                0xd
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_BMSK                                             0x1000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_SHFT                                                0xc
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_BMSK                                              0x800
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_SHFT                                                0xb
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_BMSK                                              0x400
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_SHFT                                                0xa
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_BMSK                                              0x200
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_SHFT                                                0x9
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_BMSK                                         0x100
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_SHFT                                           0x8
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_BMSK                                              0x20
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_SHFT                                               0x5
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_BMSK                                             0x10
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_SHFT                                              0x4
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_BMSK                                             0x8
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_SHFT                                             0x3
#define HWIO_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_BMSK                                             0x4
#define HWIO_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_SHFT                                             0x2
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_BMSK                                             0x2
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_SHFT                                             0x1
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_BMSK                                                0x1
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_SHFT                                                0x0

#define HWIO_GENI_S_IRQ_CLEAR_ADDR(x)                                                     ((x) + 0x00000048)
#define HWIO_GENI_S_IRQ_CLEAR_OFFS                                                        (0x00000048)
#define HWIO_GENI_S_IRQ_CLEAR_RMSK                                                         0xfc07f3f
#define HWIO_GENI_S_IRQ_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_CLEAR_ADDR(x),v)
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                                      0x8000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                                           0x1b
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                                 0x4000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                                      0x1a
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                                    0x2000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                                         0x19
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                                    0x1000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                                         0x18
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                                     0x800000
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                                         0x17
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                                   0x400000
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                                       0x16
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_BMSK                                           0x4000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_SHFT                                              0xe
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_BMSK                                           0x2000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_SHFT                                              0xd
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_BMSK                                           0x1000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_SHFT                                              0xc
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_BMSK                                            0x800
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_SHFT                                              0xb
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_BMSK                                            0x400
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_SHFT                                              0xa
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_BMSK                                            0x200
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_SHFT                                              0x9
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_BMSK                                       0x100
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_SHFT                                         0x8
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_BMSK                                            0x20
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_SHFT                                             0x5
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_BMSK                                           0x10
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_SHFT                                            0x4
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_BMSK                                           0x8
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_SHFT                                           0x3
#define HWIO_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_BMSK                                           0x4
#define HWIO_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_SHFT                                           0x2
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_BMSK                                           0x2
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_SHFT                                           0x1
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_BMSK                                              0x1
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_SHFT                                              0x0

#define HWIO_GENI_TX_FIFOn_ADDR(base,n)                                                   ((base) + 0x00000100 + 0x4 * (n))
#define HWIO_GENI_TX_FIFOn_OFFS(base,n)                                                   (0x00000100 + 0x4 * (n))
#define HWIO_GENI_TX_FIFOn_RMSK                                                           0xffffffff
#define HWIO_GENI_TX_FIFOn_MAXn                                                                   15
#define HWIO_GENI_TX_FIFOn_OUTI(base,n,val)    \
        out_dword(HWIO_GENI_TX_FIFOn_ADDR(base,n),val)
#define HWIO_GENI_TX_FIFOn_TX_DATA_BMSK                                                   0xffffffff
#define HWIO_GENI_TX_FIFOn_TX_DATA_SHFT                                                          0x0

#define HWIO_GENI_RX_FIFOn_ADDR(base,n)                                                   ((base) + 0x00000180 + 0x4 * (n))
#define HWIO_GENI_RX_FIFOn_OFFS(base,n)                                                   (0x00000180 + 0x4 * (n))
#define HWIO_GENI_RX_FIFOn_RMSK                                                           0xffffffff
#define HWIO_GENI_RX_FIFOn_MAXn                                                                   15
#define HWIO_GENI_RX_FIFOn_INI(base,n)        \
        in_dword_masked(HWIO_GENI_RX_FIFOn_ADDR(base,n), HWIO_GENI_RX_FIFOn_RMSK)
#define HWIO_GENI_RX_FIFOn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_GENI_RX_FIFOn_ADDR(base,n), mask)
#define HWIO_GENI_RX_FIFOn_RX_DATA_BMSK                                                   0xffffffff
#define HWIO_GENI_RX_FIFOn_RX_DATA_SHFT                                                          0x0

#define HWIO_GENI_RX_FIFO_STATUS_ADDR(x)                                                  ((x) + 0x00000204)
#define HWIO_GENI_RX_FIFO_STATUS_OFFS                                                     (0x00000204)
#define HWIO_GENI_RX_FIFO_STATUS_RMSK                                                     0xffffffff
#define HWIO_GENI_RX_FIFO_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_FIFO_STATUS_ADDR(x), HWIO_GENI_RX_FIFO_STATUS_RMSK)
#define HWIO_GENI_RX_FIFO_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_FIFO_STATUS_ADDR(x), m)
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BMSK                                             0x80000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_SHFT                                                   0x1f
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_BMSK                                  0x70000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_SHFT                                        0x1c
#define HWIO_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_BMSK                                          0xe000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_SHFT                                               0x19
#define HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_BMSK                                           0x1ffffff
#define HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_SHFT                                                 0x0

#define HWIO_GENI_TX_WATERMARK_REG_ADDR(x)                                                ((x) + 0x0000020c)
#define HWIO_GENI_TX_WATERMARK_REG_OFFS                                                   (0x0000020c)
#define HWIO_GENI_TX_WATERMARK_REG_RMSK                                                         0x3f
#define HWIO_GENI_TX_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_WATERMARK_REG_ADDR(x), HWIO_GENI_TX_WATERMARK_REG_RMSK)
#define HWIO_GENI_TX_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_TX_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_TX_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_TX_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TX_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_TX_WATERMARK_REG_IN(x))
#define HWIO_GENI_TX_WATERMARK_REG_TX_WATERMARK_BMSK                                            0x3f
#define HWIO_GENI_TX_WATERMARK_REG_TX_WATERMARK_SHFT                                             0x0

#define HWIO_GENI_RX_WATERMARK_REG_ADDR(x)                                                ((x) + 0x00000210)
#define HWIO_GENI_RX_WATERMARK_REG_OFFS                                                   (0x00000210)
#define HWIO_GENI_RX_WATERMARK_REG_RMSK                                                         0x3f
#define HWIO_GENI_RX_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_WATERMARK_REG_ADDR(x), HWIO_GENI_RX_WATERMARK_REG_RMSK)
#define HWIO_GENI_RX_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_RX_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_RX_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_RX_WATERMARK_REG_IN(x))
#define HWIO_GENI_RX_WATERMARK_REG_RX_WATERMARK_BMSK                                            0x3f
#define HWIO_GENI_RX_WATERMARK_REG_RX_WATERMARK_SHFT                                             0x0

#define HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x)                                            ((x) + 0x00000214)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OFFS                                               (0x00000214)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RMSK                                                     0x3f
#define HWIO_GENI_RX_RFR_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x), HWIO_GENI_RX_RFR_WATERMARK_REG_RMSK)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_RX_RFR_WATERMARK_REG_IN(x))
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_BMSK                                    0x3f
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_SHFT                                     0x0

#define HWIO_UART_TX_TRANS_CFG_ADDR(x)                                                          ((x) + 0x0000015c)
#define HWIO_UART_TX_TRANS_CFG_OFFS                                                             (0x0000015c)
#define HWIO_UART_TX_TRANS_CFG_RMSK                                                                   0x3f
#define HWIO_UART_TX_TRANS_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_TX_TRANS_CFG_ADDR(x), HWIO_UART_TX_TRANS_CFG_RMSK)
#define HWIO_UART_TX_TRANS_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_TRANS_CFG_ADDR(x), m)
#define HWIO_UART_TX_TRANS_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_TX_TRANS_CFG_ADDR(x),v)
#define HWIO_UART_TX_TRANS_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_TRANS_CFG_ADDR(x),m,v,HWIO_UART_TX_TRANS_CFG_IN(x))
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG3_BMSK                                                0x20
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG3_SHFT                                                 0x5
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG2_BMSK                                                0x10
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG2_SHFT                                                 0x4
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG1_BMSK                                                 0x8
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG1_SHFT                                                 0x3
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG_BMSK                                                  0x4
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG_SHFT                                                  0x2
#define HWIO_UART_TX_TRANS_CFG_UART_CTS_MASK_BMSK                                                      0x2
#define HWIO_UART_TX_TRANS_CFG_UART_CTS_MASK_SHFT                                                      0x1
#define HWIO_UART_TX_TRANS_CFG_UART_PARITY_EN_BMSK                                                     0x1
#define HWIO_UART_TX_TRANS_CFG_UART_PARITY_EN_SHFT                                                     0x0

#define HWIO_UART_TX_WORD_LEN_ADDR(x)                                                           ((x) + 0x00000168)
#define HWIO_UART_TX_WORD_LEN_OFFS                                                              (0x00000168)
#define HWIO_UART_TX_WORD_LEN_RMSK                                                                   0x3ff
#define HWIO_UART_TX_WORD_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_WORD_LEN_ADDR(x), HWIO_UART_TX_WORD_LEN_RMSK)
#define HWIO_UART_TX_WORD_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_WORD_LEN_ADDR(x), m)
#define HWIO_UART_TX_WORD_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_WORD_LEN_ADDR(x),v)
#define HWIO_UART_TX_WORD_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_WORD_LEN_ADDR(x),m,v,HWIO_UART_TX_WORD_LEN_IN(x))
#define HWIO_UART_TX_WORD_LEN_UART_TX_WORD_LEN_BMSK                                                  0x3ff
#define HWIO_UART_TX_WORD_LEN_UART_TX_WORD_LEN_SHFT                                                    0x0

#define HWIO_UART_TX_STOP_BIT_LEN_ADDR(x)                                                       ((x) + 0x0000016c)
#define HWIO_UART_TX_STOP_BIT_LEN_OFFS                                                          (0x0000016c)
#define HWIO_UART_TX_STOP_BIT_LEN_RMSK                                                            0xffffff
#define HWIO_UART_TX_STOP_BIT_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x), HWIO_UART_TX_STOP_BIT_LEN_RMSK)
#define HWIO_UART_TX_STOP_BIT_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x), m)
#define HWIO_UART_TX_STOP_BIT_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x),v)
#define HWIO_UART_TX_STOP_BIT_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x),m,v,HWIO_UART_TX_STOP_BIT_LEN_IN(x))
#define HWIO_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_BMSK                                       0xffffff
#define HWIO_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_SHFT                                            0x0

#define HWIO_UART_TX_TRANS_LEN_ADDR(x)                                                          ((x) + 0x00000170)
#define HWIO_UART_TX_TRANS_LEN_OFFS                                                             (0x00000170)
#define HWIO_UART_TX_TRANS_LEN_RMSK                                                               0xffffff
#define HWIO_UART_TX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_TRANS_LEN_ADDR(x), HWIO_UART_TX_TRANS_LEN_RMSK)
#define HWIO_UART_TX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_TRANS_LEN_ADDR(x), m)
#define HWIO_UART_TX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_TRANS_LEN_ADDR(x),v)
#define HWIO_UART_TX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_TRANS_LEN_ADDR(x),m,v,HWIO_UART_TX_TRANS_LEN_IN(x))
#define HWIO_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_BMSK                                             0xffffff
#define HWIO_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_SHFT                                                  0x0

#define HWIO_UART_RX_TRANS_CFG_ADDR(x)                                                          ((x) + 0x00000180)
#define HWIO_UART_RX_TRANS_CFG_OFFS                                                             (0x00000180)
#define HWIO_UART_RX_TRANS_CFG_RMSK                                                                   0x3f
#define HWIO_UART_RX_TRANS_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_RX_TRANS_CFG_ADDR(x), HWIO_UART_RX_TRANS_CFG_RMSK)
#define HWIO_UART_RX_TRANS_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_TRANS_CFG_ADDR(x), m)
#define HWIO_UART_RX_TRANS_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_RX_TRANS_CFG_ADDR(x),v)
#define HWIO_UART_RX_TRANS_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_TRANS_CFG_ADDR(x),m,v,HWIO_UART_RX_TRANS_CFG_IN(x))
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG3_BMSK                                                0x20
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG3_SHFT                                                 0x5
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG2_BMSK                                                0x10
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG2_SHFT                                                 0x4
#define HWIO_UART_RX_TRANS_CFG_UART_PARITY_EN_BMSK                                                     0x8
#define HWIO_UART_RX_TRANS_CFG_UART_PARITY_EN_SHFT                                                     0x3
#define HWIO_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_BMSK                                          0x4
#define HWIO_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_SHFT                                          0x2
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG1_BMSK                                                 0x2
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG1_SHFT                                                 0x1
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG0_BMSK                                                 0x1
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG0_SHFT                                                 0x0

#define HWIO_UART_RX_WORD_LEN_ADDR(x)                                                           ((x) + 0x0000018c)
#define HWIO_UART_RX_WORD_LEN_OFFS                                                              (0x0000018c)
#define HWIO_UART_RX_WORD_LEN_RMSK                                                                   0x3ff
#define HWIO_UART_RX_WORD_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_RX_WORD_LEN_ADDR(x), HWIO_UART_RX_WORD_LEN_RMSK)
#define HWIO_UART_RX_WORD_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_WORD_LEN_ADDR(x), m)
#define HWIO_UART_RX_WORD_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_RX_WORD_LEN_ADDR(x),v)
#define HWIO_UART_RX_WORD_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_WORD_LEN_ADDR(x),m,v,HWIO_UART_RX_WORD_LEN_IN(x))
#define HWIO_UART_RX_WORD_LEN_UART_RX_WORD_LEN_BMSK                                                  0x3ff
#define HWIO_UART_RX_WORD_LEN_UART_RX_WORD_LEN_SHFT                                                    0x0

#define HWIO_UART_RX_STALE_CNT_ADDR(x)                                                          ((x) + 0x00000194)
#define HWIO_UART_RX_STALE_CNT_OFFS                                                             (0x00000194)
#define HWIO_UART_RX_STALE_CNT_RMSK                                                               0xffffff
#define HWIO_UART_RX_STALE_CNT_IN(x)      \
        in_dword_masked(HWIO_UART_RX_STALE_CNT_ADDR(x), HWIO_UART_RX_STALE_CNT_RMSK)
#define HWIO_UART_RX_STALE_CNT_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_STALE_CNT_ADDR(x), m)
#define HWIO_UART_RX_STALE_CNT_OUT(x, v)      \
        out_dword(HWIO_UART_RX_STALE_CNT_ADDR(x),v)
#define HWIO_UART_RX_STALE_CNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_STALE_CNT_ADDR(x),m,v,HWIO_UART_RX_STALE_CNT_IN(x))
#define HWIO_UART_RX_STALE_CNT_UART_RX_STALE_CNT_BMSK                                             0xffffff
#define HWIO_UART_RX_STALE_CNT_UART_RX_STALE_CNT_SHFT                                                  0x0

#define HWIO_UART_TX_PARITY_CFG_ADDR(x)                                                         ((x) + 0x000001a4)
#define HWIO_UART_TX_PARITY_CFG_OFFS                                                            (0x000001a4)
#define HWIO_UART_TX_PARITY_CFG_RMSK                                                                  0x1f
#define HWIO_UART_TX_PARITY_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_TX_PARITY_CFG_ADDR(x), HWIO_UART_TX_PARITY_CFG_RMSK)
#define HWIO_UART_TX_PARITY_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_PARITY_CFG_ADDR(x), m)
#define HWIO_UART_TX_PARITY_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_TX_PARITY_CFG_ADDR(x),v)
#define HWIO_UART_TX_PARITY_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_PARITY_CFG_ADDR(x),m,v,HWIO_UART_TX_PARITY_CFG_IN(x))
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG1_BMSK                                               0x10
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG1_SHFT                                                0x4
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG0_BMSK                                                0x8
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG0_SHFT                                                0x3
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_MODE_BMSK                                                       0x6
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_MODE_SHFT                                                       0x1
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_BMSK                                                    0x1
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_SHFT                                                    0x0

#define HWIO_UART_RX_PARITY_CFG_ADDR(x)                                                         ((x) + 0x000001a8)
#define HWIO_UART_RX_PARITY_CFG_OFFS                                                            (0x000001a8)
#define HWIO_UART_RX_PARITY_CFG_RMSK                                                                  0x1f
#define HWIO_UART_RX_PARITY_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_RX_PARITY_CFG_ADDR(x), HWIO_UART_RX_PARITY_CFG_RMSK)
#define HWIO_UART_RX_PARITY_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_PARITY_CFG_ADDR(x), m)
#define HWIO_UART_RX_PARITY_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_RX_PARITY_CFG_ADDR(x),v)
#define HWIO_UART_RX_PARITY_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_PARITY_CFG_ADDR(x),m,v,HWIO_UART_RX_PARITY_CFG_IN(x))
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG1_BMSK                                               0x10
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG1_SHFT                                                0x4
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG0_BMSK                                                0x8
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG0_SHFT                                                0x3
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_MODE_BMSK                                                       0x6
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_MODE_SHFT                                                       0x1
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_BMSK                                                    0x1
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_SHFT                                                    0x0


#define HWIO_GENI_DMA_MODE_EN_ADDR(x)                                                           ((x) + 0x00000158)
#define HWIO_GENI_DMA_MODE_EN_OFFS                                                              (0x00000158)
#define HWIO_GENI_DMA_MODE_EN_RMSK                                                                     0x1
#define HWIO_GENI_DMA_MODE_EN_IN(x)      \
        in_dword_masked(HWIO_GENI_DMA_MODE_EN_ADDR(x), HWIO_GENI_DMA_MODE_EN_RMSK)
#define HWIO_GENI_DMA_MODE_EN_INM(x, m)      \
        in_dword_masked(HWIO_GENI_DMA_MODE_EN_ADDR(x), m)
#define HWIO_GENI_DMA_MODE_EN_OUT(x, v)      \
        out_dword(HWIO_GENI_DMA_MODE_EN_ADDR(x),v)
#define HWIO_GENI_DMA_MODE_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_DMA_MODE_EN_ADDR(x),m,v,HWIO_GENI_DMA_MODE_EN_IN(x))
#define HWIO_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK                                                    0x1
#define HWIO_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_SHFT                                                    0x0

#define HWIO_SE_IRQ_EN_ADDR(x)                                                           ((x) + 0x0000021c)
#define HWIO_SE_IRQ_EN_OFFS                                                              (0x0000021c)
#define HWIO_SE_IRQ_EN_RMSK                                                                     0xf
#define HWIO_SE_IRQ_EN_IN(x)      \
        in_dword_masked(HWIO_SE_IRQ_EN_ADDR(x), HWIO_SE_IRQ_EN_RMSK)
#define HWIO_SE_IRQ_EN_INM(x, m)      \
        in_dword_masked(HWIO_SE_IRQ_EN_ADDR(x), m)
#define HWIO_SE_IRQ_EN_OUT(x, v)      \
        out_dword(HWIO_SE_IRQ_EN_ADDR(x),v)
#define HWIO_SE_IRQ_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_IRQ_EN_ADDR(x),m,v,HWIO_SE_IRQ_EN_IN(x))
#define HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_BMSK                                                       0x8
#define HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_SHFT                                                       0x3
#define HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_BMSK                                                       0x4
#define HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_SHFT                                                       0x2
#define HWIO_SE_IRQ_EN_DMA_TX_IRQ_EN_BMSK                                                       0x2
#define HWIO_SE_IRQ_EN_DMA_TX_IRQ_EN_SHFT                                                       0x1
#define HWIO_SE_IRQ_EN_DMA_RX_IRQ_EN_BMSK                                                       0x1
#define HWIO_SE_IRQ_EN_DMA_RX_IRQ_EN_SHFT                                                       0x0

#define HWIO_SE_HW_PARAM_0_ADDR(x)                                                       ((x) + 0x00000224)
#define HWIO_SE_HW_PARAM_0_OFFS                                                          (0x00000224)
#define HWIO_SE_HW_PARAM_0_RMSK                                                          0x3f3f79ff
#define HWIO_SE_HW_PARAM_0_IN(x)      \
        in_dword_masked(HWIO_SE_HW_PARAM_0_ADDR(x), HWIO_SE_HW_PARAM_0_RMSK)
#define HWIO_SE_HW_PARAM_0_INM(x, m)      \
        in_dword_masked(HWIO_SE_HW_PARAM_0_ADDR(x), m)
#define HWIO_SE_HW_PARAM_0_TX_FIFO_WIDTH_BMSK                                            0x3f000000
#define HWIO_SE_HW_PARAM_0_TX_FIFO_WIDTH_SHFT                                                  0x18
#define HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK                                              0x3f0000
#define HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT                                                  0x10
#define HWIO_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_BMSK                                          0x7000
#define HWIO_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_SHFT                                             0xc
#define HWIO_SE_HW_PARAM_0_TX_FIFO_EN_BMSK                                                    0x800
#define HWIO_SE_HW_PARAM_0_TX_FIFO_EN_SHFT                                                      0xb
#define HWIO_SE_HW_PARAM_0_GEN_I3C_BMSK                                                       0x100
#define HWIO_SE_HW_PARAM_0_GEN_I3C_SHFT                                                         0x8
#define HWIO_SE_HW_PARAM_0_GEN_PROG_ROM_BMSK                                                   0x80
#define HWIO_SE_HW_PARAM_0_GEN_PROG_ROM_SHFT                                                    0x7
#define HWIO_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_BMSK                                             0x40
#define HWIO_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_SHFT                                              0x6
#define HWIO_SE_HW_PARAM_0_AHB_M_ADDR_W_BMSK                                                   0x3f
#define HWIO_SE_HW_PARAM_0_AHB_M_ADDR_W_SHFT                                                    0x0

#define HWIO_SE_HW_PARAM_1_ADDR(x)                                                       ((x) + 0x00000228)
#define HWIO_SE_HW_PARAM_1_OFFS                                                          (0x00000228)
#define HWIO_SE_HW_PARAM_1_RMSK                                                          0x3f3f7fff
#define HWIO_SE_HW_PARAM_1_IN(x)      \
        in_dword_masked(HWIO_SE_HW_PARAM_1_ADDR(x), HWIO_SE_HW_PARAM_1_RMSK)
#define HWIO_SE_HW_PARAM_1_INM(x, m)      \
        in_dword_masked(HWIO_SE_HW_PARAM_1_ADDR(x), m)
#define HWIO_SE_HW_PARAM_1_RX_FIFO_WIDTH_BMSK                                            0x3f000000
#define HWIO_SE_HW_PARAM_1_RX_FIFO_WIDTH_SHFT                                                  0x18
#define HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK                                              0x3f0000
#define HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT                                                  0x10
#define HWIO_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_BMSK                                          0x7000
#define HWIO_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_SHFT                                             0xc
#define HWIO_SE_HW_PARAM_1_RX_FIFO_EN_BMSK                                                    0x800
#define HWIO_SE_HW_PARAM_1_RX_FIFO_EN_SHFT                                                      0xb
#define HWIO_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_BMSK                                             0x600
#define HWIO_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_SHFT                                               0x9
#define HWIO_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_BMSK                                          0x1ff
#define HWIO_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_SHFT                                            0x0

#define HWIO_DMA_GENERAL_CFG_ADDR(x)                                                     ((x) + 0x00000230)
#define HWIO_DMA_GENERAL_CFG_OFFS                                                        (0x00000230)
#define HWIO_DMA_GENERAL_CFG_RMSK                                                             0x1ff
#define HWIO_DMA_GENERAL_CFG_IN(x)      \
        in_dword_masked(HWIO_DMA_GENERAL_CFG_ADDR(x), HWIO_DMA_GENERAL_CFG_RMSK)
#define HWIO_DMA_GENERAL_CFG_INM(x, m)      \
        in_dword_masked(HWIO_DMA_GENERAL_CFG_ADDR(x), m)
#define HWIO_DMA_GENERAL_CFG_OUT(x, v)      \
        out_dword(HWIO_DMA_GENERAL_CFG_ADDR(x),v)
#define HWIO_DMA_GENERAL_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_GENERAL_CFG_ADDR(x),m,v,HWIO_DMA_GENERAL_CFG_IN(x))
#define HWIO_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_BMSK                                            0x1c0
#define HWIO_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_SHFT                                              0x6
#define HWIO_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_BMSK                                       0x20
#define HWIO_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_SHFT                                        0x5
#define HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK                                        0x8
#define HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_SHFT                                        0x3
#define HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK                                        0x4
#define HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_SHFT                                        0x2
#define HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK                                             0x2
#define HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_SHFT                                             0x1
#define HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK                                             0x1
#define HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_SHFT                                             0x0

#define HWIO_SE_GSI_EVENT_EN_ADDR(x)                                                     ((x) + 0x00000218)
#define HWIO_SE_GSI_EVENT_EN_OFFS                                                        (0x00000218)
#define HWIO_SE_GSI_EVENT_EN_RMSK                                                               0xf
#define HWIO_SE_GSI_EVENT_EN_IN(x)      \
        in_dword_masked(HWIO_SE_GSI_EVENT_EN_ADDR(x), HWIO_SE_GSI_EVENT_EN_RMSK)
#define HWIO_SE_GSI_EVENT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SE_GSI_EVENT_EN_ADDR(x), m)
#define HWIO_SE_GSI_EVENT_EN_OUT(x, v)      \
        out_dword(HWIO_SE_GSI_EVENT_EN_ADDR(x),v)
#define HWIO_SE_GSI_EVENT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_GSI_EVENT_EN_ADDR(x),m,v,HWIO_SE_GSI_EVENT_EN_IN(x))
#define HWIO_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_BMSK                                               0x8
#define HWIO_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_SHFT                                               0x3
#define HWIO_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_BMSK                                               0x4
#define HWIO_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_SHFT                                               0x2
#define HWIO_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_BMSK                                               0x2
#define HWIO_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_SHFT                                               0x1
#define HWIO_SE_GSI_EVENT_EN_DMA_RX_EVENT_EN_BMSK                                               0x1

#define HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x)                                                  ((x) + 0x00000118)
#define HWIO_QUPV3_SE_AHB_M_CFG_OFFS                                                     (0x00000118)
#define HWIO_QUPV3_SE_AHB_M_CFG_RMSK                                                            0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x), HWIO_QUPV3_SE_AHB_M_CFG_RMSK)
#define HWIO_QUPV3_SE_AHB_M_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x), m)
#define HWIO_QUPV3_SE_AHB_M_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x),v)
#define HWIO_QUPV3_SE_AHB_M_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x),m,v,HWIO_QUPV3_SE_AHB_M_CFG_IN(x))
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK                                           0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_SHFT                                           0x0

#define MAX_RX_FIFO_SIZE 128

// --------------------------------------------------------------------- Macros

#define UART_DM_READ_REG(addr, offset)          \
    UartHardwareAccess.ReadRegister32((ULONG*)((PUCHAR)addr + offset))

#define UART_DM_WRITE_REG(addr, offset, val)    \
    UartHardwareAccess.WriteRegister32((ULONG*)((PUCHAR)addr + offset), val)

// -------------------------------------------------------------------- Globals

//
// This structure indicates the local stored data read/write from/to FIFO HW.
//

typedef struct _FIFO_TX_BLOCK {
    __declspec(align(32)) UCHAR FifoBuffer[MAX_RX_FIFO_SIZE];
    ULONG AvailableBytes;
    PUCHAR PtrToFifoBuffer;
} FIFO_TX_BLOCK, *PFIFO_TX_BLOCK;

//
// This is the control transfer data block.
//

static FIFO_TX_BLOCK Transfer;

// ----------------------------------------------- Internal Function Prototypes

BOOLEAN
SDM845SetBaud (
    _Inout_ PCPPORT Port,
    ULONG Rate
    );

// ------------------------------------------------------------------ Functions

BOOLEAN
SDM845InitializePort (
    _In_opt_ _Null_terminated_ PCHAR LoadOptions,
    _Inout_ PCPPORT Port,
    BOOLEAN MemoryMapped,
    UCHAR AccessSize,
    UCHAR BitWidth
    )

/*++

Routine Description:

    This routine performs the initialization of an SDM845 serial UART.

Arguments:

    LoadOptions - Optional load option string. Currently unused.

    Port - Supplies a pointer to a CPPORT object which will be filled in as
        part of the port initialization.

    MemoryMapped - Supplies a boolean which indicates if the UART is accessed
        through memory-mapped registers or legacy port I/O.

    AccessSize - Supplies an ACPI Generic Access Size value which indicates the
        type of bus access that should be performed when accessing the UART.

    BitWidth - Supplies a number indicating how wide the UART's registers are.

Return Value:

    TRUE if the port has been successfully initialized, FALSE otherwise.

--*/

{

    UNREFERENCED_PARAMETER(LoadOptions);
    UNREFERENCED_PARAMETER(AccessSize);
    UNREFERENCED_PARAMETER(BitWidth);

    ULONG ConfigMask;

    if (MemoryMapped == FALSE) {
        return FALSE;
    }

    Port->Flags = 0;

    //
    // Wait for the state machine to clear up.
    //

    while((UART_DM_READ_REG(Port->Address + GENI4_CFG, HWIO_GENI_STATUS_OFFS) & HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK));

    UART_DM_WRITE_REG(Port->Address + GENI4_CFG, HWIO_GENI_DFS_IF_CFG_OFFS, 0x0);

    ConfigMask = (HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK << HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_SHFT) |
                 (HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK << HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_SHFT) |
                 (HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK << HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_SHFT) |
                 (HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK << HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_SHFT);

    UART_DM_WRITE_REG(Port->Address + QUPV3_SE_DMA, HWIO_DMA_GENERAL_CFG_OFFS, ConfigMask);
    UART_DM_WRITE_REG(Port->Address + GENI4_CFG, HWIO_GENI_CGC_CTRL_OFFS, 0x7F);
    UART_DM_WRITE_REG(Port->Address + GENI4_CFG, HWIO_GENI_FORCE_DEFAULT_REG_OFFS, 0x1);
    UART_DM_WRITE_REG(Port->Address + GENI4_CFG, HWIO_GENI_OUTPUT_CTRL_OFFS, 0x7F);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_GENI_DMA_MODE_EN_OFFS, 0);
    UART_DM_WRITE_REG(Port->Address + QUPV3_SE_DMA, HWIO_SE_IRQ_EN_OFFS, 0xFFFFFFFF);
    UART_DM_WRITE_REG(Port->Address + QUPV3_SE_DMA, HWIO_SE_GSI_EVENT_EN_OFFS, 0);
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_M_IRQ_ENABLE_OFFS, 0xB300005F);
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_S_IRQ_ENABLE_OFFS, 0xB3007E5F);

    //
    // Get TX FIFO width.
    //

    ConfigMask = UART_DM_READ_REG(Port->Address + QUPV3_SE_DMA, HWIO_SE_HW_PARAM_0_OFFS);
    ConfigMask = (ConfigMask & TX_FIFO_DEPTH_MASK) >> TX_FIFO_DEPTH_SHIFT;
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_TX_WATERMARK_REG_OFFS, 4);

    //
    // Get RX FIFO width.
    //

    ConfigMask = UART_DM_READ_REG(Port->Address + QUPV3_SE_DMA, HWIO_SE_HW_PARAM_1_OFFS);
    ConfigMask = (ConfigMask & RX_FIFO_DEPTH_MASK) >> RX_FIFO_DEPTH_SHIFT;
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_RX_WATERMARK_REG_OFFS, (ConfigMask - 8));
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_RX_RFR_WATERMARK_REG_OFFS, (ConfigMask - 4));

    SDM845SetBaud(Port, Port->BaudRate);

    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_TX_WORD_LEN_OFFS, 0x8);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_RX_WORD_LEN_OFFS, 0x8);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_TX_PARITY_CFG_OFFS, 0x0);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_TX_TRANS_CFG_OFFS, 0x2);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_RX_PARITY_CFG_OFFS, 0x0);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_RX_TRANS_CFG_OFFS, 0x0);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_TX_STOP_BIT_LEN_OFFS, 0x0);
    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_RX_STALE_CNT_OFFS, (0x16 * 10));
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_S_CMD0_OFFS, 0x8000000);
    return TRUE;
}

BOOLEAN
SDM845SetBaud (
    _Inout_ PCPPORT Port,
    ULONG Rate
    )

/*++

Routine Description:

    Set the baud rate for the UART hardware and record it in the port object.

Arguments:

    Port - Supplies the address of the port object that describes the UART.

    Rate - Supplies the desired baud rate in bits per second.

Return Value:

    TRUE if the baud rate was programmed, FALSE if it was not.

--*/

{

    UINT32 DivisorLatch = 0;
    UINT32 ValueTempMask = 0;

    if ((Port == NULL) || (Port->Address == NULL)) {
        return FALSE;
    }

    //
    // Clock rate is 1843200.
    //

    switch (Rate) {
    case 7200:
        DivisorLatch = 0x20;
        break;
    case 9600:
        DivisorLatch = 0x18;
        break;
    case 14400:
        DivisorLatch = 0x10;
        break;
    case 19200:
        DivisorLatch = 0xC;
        break;
    case 28800:
        DivisorLatch = 0x8;
        break;
    case 38400:
        DivisorLatch = 0x6;
        break;
    case 57600:
        DivisorLatch = 0x4;
        break;
    case 115200:
        DivisorLatch = 0x2;
        break;
    default:
        DivisorLatch = 0x2;
        break;
    }

    ValueTempMask = (DivisorLatch << (HWIO_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_SHFT)) |
                     HWIO_GENI_SER_M_CLK_CFG_SER_CLK_EN_BMSK;

    UART_DM_WRITE_REG(Port->Address + GENI4_CFG,
                      HWIO_GENI_SER_M_CLK_CFG_OFFS,
                      ValueTempMask);

    UART_DM_WRITE_REG(Port->Address + GENI4_CFG,
                      HWIO_GENI_SER_S_CLK_CFG_OFFS,
                      ValueTempMask);

    Port->BaudRate = Rate;
    return TRUE;
}

UART_STATUS
SDM845GetByte (
    _Inout_ PCPPORT Port,
    _Out_ PUCHAR Byte
    )

/*++

Routine Description:

    Fetch a data byte from the UART device and return it.

Arguments:

    Port - Supplies the address of the port object that describes the UART.

    Byte - Supplies the address of variable to hold the result.

Return Value:

    UART_STATUS code.

--*/

{

    PUCHAR BaseAddress;
    ULONG IrqStatus;
    ULONG RxFifoStatus;
    ULONG PartialBytesToRead;
    ULONG WordsToRead;
    ULONG ArrayIndex;
    ULONG Index;
    ULONG RxFifo;

    if ((Port == NULL) || (Port->Address == NULL)) {
        return UartNotReady;
    }

    BaseAddress = Port->Address;
    ArrayIndex = 0;

    //
    // Check if there is any available data, if no, then
    // read from the fifo and fill the local transfer buffer.
    //

    if (Transfer.AvailableBytes == 0) {
        Transfer.PtrToFifoBuffer = (UCHAR *)Transfer.FifoBuffer;
        IrqStatus = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_S_IRQ_STATUS_OFFS);
        UART_DM_WRITE_REG(BaseAddress + GENI4_DATA, HWIO_GENI_S_IRQ_CLEAR_OFFS, IrqStatus);
        RxFifoStatus = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_FIFO_STATUS_OFFS);

        //
        // Calculate the number of bytes to read.
        //

        PartialBytesToRead = (RxFifoStatus & RX_LAST_VALID_BYTES_MASK) >> RX_LAST_VALID_BYTES_SHIFT;
        WordsToRead = RxFifoStatus & RX_FIFO_WC;
        if ((PartialBytesToRead > 0) || (WordsToRead > 0)) {
            if ((PartialBytesToRead != 0) && (PartialBytesToRead != 4)) {
                WordsToRead -= 1;
            }

        } else if ((IrqStatus & RX_FIFO_WATERMARK_IRQ) != 0) {
            WordsToRead = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_WATERMARK_REG_OFFS);
        }

        Transfer.AvailableBytes = (WordsToRead * 4) + PartialBytesToRead;
        for (Index = 0; Index < WordsToRead; Index += 1) {
            RxFifo = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_FIFOn_OFFS(BaseAddress, Index));
            Transfer.FifoBuffer[0 + ArrayIndex] = (UCHAR)(RxFifo >>  0);
            Transfer.FifoBuffer[1 + ArrayIndex] = (UCHAR)(RxFifo >>  8);
            Transfer.FifoBuffer[2 + ArrayIndex] = (UCHAR)(RxFifo >> 16);
            Transfer.FifoBuffer[3 + ArrayIndex] = (UCHAR)(RxFifo >> 24);
            ArrayIndex += 4;
        }

        if (PartialBytesToRead > 0) {
            RxFifo = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_FIFOn_OFFS(BaseAddress, Index));
            for (Index = 0; Index < PartialBytesToRead; Index += 1) {
                Transfer.FifoBuffer[Index] = (UCHAR)(RxFifo >> Index * 8);
            }
        }
    }

    if (Transfer.AvailableBytes != 0) {
        *Byte = *Transfer.PtrToFifoBuffer;
        Transfer.PtrToFifoBuffer += 1;
        Transfer.AvailableBytes -= 1;
        return UartSuccess;
    }

    return UartNoData;
}

UART_STATUS
SDM845PutByte (
    _Inout_ PCPPORT Port,
    UCHAR Byte,
    BOOLEAN BusyWait
    )

/*++

Routine Description:

    Write a data byte out to the UART device.

Arguments:

    Port - Supplies the address of the port object that describes the UART.

    Byte - Supplies the data to emit.

    BusyWait - Supplies a flag to control whether this routine will busy
        wait (spin) for the UART hardware to be ready to transmit.

Return Value:

    UART_STATUS code.

--*/

{
    PUCHAR Address;

    if ((Port == NULL) || (Port->Address == NULL)) {
        return UartNotReady;
    }

    Address = Port->Address;

    //
    // Wait for the state machine to clear up.
    //

    if (BusyWait != FALSE) {
        while((UART_DM_READ_REG(Port->Address + GENI4_CFG, HWIO_GENI_STATUS_OFFS) & HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK));

    } else if ((UART_DM_READ_REG(Port->Address + GENI4_CFG, HWIO_GENI_STATUS_OFFS) & HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK)) {

        return UartNotReady;
    }


    UART_DM_WRITE_REG(Port->Address + GENI4_IMAGE_REGS, HWIO_UART_TX_TRANS_LEN_OFFS, 1);
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_M_CMD0_OFFS, 0x8000000);

    UINT32 word_value = 0;
    word_value = word_value | Byte;
    UART_DM_WRITE_REG(Port->Address + GENI4_DATA, HWIO_GENI_TX_FIFOn_OFFS(Port->Address, 0), word_value);
    return UartSuccess;
}

BOOLEAN
SDM845RxReady (
    _Inout_ PCPPORT Port
    )

/*++

Routine Description:

    This routine determines if there is data pending in the UART.

Arguments:

    Port - Supplies the address of the port object that describes the UART.

Return Value:

    TRUE if data is available, FALSE otherwise.

--*/

{

    PUCHAR BaseAddress;
    ULONG FifoStatReg;
    ULONG PartialBytesToRead;
    ULONG WordsToRead;
    BOOLEAN IsAvailableBytes;
    ULONG IrqStatus;

    if ((Port == NULL) || (Port->Address == NULL)) {
        return FALSE;
    }

    //
    // Check if there is any waiting pending data.
    //

    if (Transfer.AvailableBytes != 0) {
        IsAvailableBytes = TRUE;
        goto SDM845ReceiveDataAvailableEnd;
    }

    //
    // Read the FIFO status register
    //

    BaseAddress = Port->Address;
    FifoStatReg = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_FIFO_STATUS_OFFS);

    //
    // Calculate the number of bytes to read
    //

    PartialBytesToRead = (FifoStatReg & RX_LAST_VALID_BYTES_MASK) >> RX_LAST_VALID_BYTES_SHIFT;
    WordsToRead = FifoStatReg & RX_FIFO_WC;
    IsAvailableBytes = FALSE;
    if ((PartialBytesToRead > 0) || (WordsToRead > 0)) {
        IsAvailableBytes = TRUE;

    } else {
        IrqStatus = UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_S_IRQ_STATUS_OFFS);
        if ((IrqStatus & RX_FIFO_WATERMARK_IRQ) != 0) {
            if (UART_DM_READ_REG(BaseAddress + GENI4_DATA, HWIO_GENI_RX_WATERMARK_REG_OFFS) > 0) {
                IsAvailableBytes = TRUE;
            }
        }
    }

SDM845ReceiveDataAvailableEnd:
    return IsAvailableBytes;
}

// -------------------------------------------------------------------- Globals

UART_HARDWARE_DRIVER SDM845HardwareDriver = {
    SDM845InitializePort,
    SDM845SetBaud,
    SDM845GetByte,
    SDM845PutByte,
    SDM845RxReady
};
