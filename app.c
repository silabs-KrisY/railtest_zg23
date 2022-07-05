/***************************************************************************//**
 * @file
 * @brief Application-level Functions
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "app.h"
#include "em_cmu.h"
#include "em_msc.h"
#include "sl_cli.h" //custom CLI
#include "sl_cli_handles.h"
#include "response_print.h"

#define MAX_CTUNE 0xff

// Fetch CTUNE value from USERDATA page as a manufacturing token
#define USERDATA ((uint32_t*)USERDATA_BASE)
#define MFG_CTUNE_ADDR 0x0FE00100UL
#define MFG_CTUNE_VAL  (*((uint16_t *) (MFG_CTUNE_ADDR)))
#define MFG_CTUNE_FLASH ((uint32_t*) MFG_CTUNE_ADDR)
#define MFG_CTUNE_USERDATA_OFFSET 0x100UL

void mfgSetCtuneToken(sl_cli_command_arg_t *arguments);
void mfgGetCtuneToken(sl_cli_command_arg_t *arguments);

static const sl_cli_command_info_t cmd_setCtuneToken = \
    SL_CLI_COMMAND(mfgSetCtuneToken,
                   "set HFXO CTUNE non-volatile token with optional USERDATA page erase",
                   "CTUNE token value" SL_CLI_UNIT_SEPARATOR "1 to erase USERDATA page first, 0 not to erase page" SL_CLI_UNIT_SEPARATOR,
                   {SL_CLI_ARG_UINT16, SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd_getCtuneToken = \
   SL_CLI_COMMAND(mfgGetCtuneToken,
                  "get Ctune Token (non-volatile) value",
                  "",
                  {SL_CLI_ARG_END, });


// Create the array of commands, containing three elements in this example
static sl_cli_command_entry_t a_table[] = {
  { "setCtuneToken", &cmd_setCtuneToken, false },
  { "getCtuneToken", &cmd_getCtuneToken, false },
  { NULL, NULL, false },
};

// Create the command group at the top level
static sl_cli_command_group_t a_group_0 = {
  { NULL },
  false,
  a_table
};

void app_init(void)
{

  //  install the custom commands.
    sl_cli_command_add_command_group(sl_cli_default_handle, &a_group_0);
}

void app_process_action(void)
{
}

void mfgSetCtuneToken(sl_cli_command_arg_t *arguments)
{
  MSC_Status_TypeDef status;

  uint32_t ctune = (uint32_t) sl_cli_get_argument_uint16(arguments, 0);
  uint8_t erase_userdata = sl_cli_get_argument_uint8(arguments, 1);

  if (ctune > MAX_CTUNE)
  {
      responsePrint(sl_cli_get_command_string(arguments, 0),"Error: crystal tuning value out of range. Input 0x%x is greater than max 0x%x\r\n\r\n",
                            ctune, MAX_CTUNE);
    return;
  }

  // Enable MSC Clock
  CMU_ClockEnable(cmuClock_MSC, true);

  if (erase_userdata == true) {
    // Clear the Userdata page of any previous data stored
    MSC_ErasePage(USERDATA);
  }

  // Write the CTUNE value as a 32-bit word
  MSC_Init();
  status = MSC_WriteWord((MFG_CTUNE_FLASH), &ctune, 4);
  MSC_Deinit();

   responsePrint(sl_cli_get_command_string(arguments, 0),"ctune msc write status:%d",status);
}

void mfgGetCtuneToken(sl_cli_command_arg_t *arguments)
{
  responsePrint(sl_cli_get_command_string(arguments, 0),
                        "ctuneTokenVal:0x%04x", MFG_CTUNE_VAL);
}
