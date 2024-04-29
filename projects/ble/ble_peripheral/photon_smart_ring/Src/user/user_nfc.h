#ifndef USER_NFC_H
#define USER_NFC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint16_t user_nfc_init(void);
uint16_t user_nfc_read_user_mem(uint16_t user_memory_addr, uint8_t* buffer, uint16_t size);
uint16_t user_nfc_write_user_mem(uint16_t user_memory_addr, uint8_t* data, uint16_t size);
uint16_t user_nfc_read_sys_reg(uint16_t reg_addr, uint8_t* buffer, uint16_t size);
uint16_t user_nfc_write_sys_reg(uint16_t reg_addr, uint8_t* data, uint16_t size);

void user_nfc_test(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif