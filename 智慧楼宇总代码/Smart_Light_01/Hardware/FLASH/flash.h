#ifndef __FLASH_H__
#define __FLASH_H__
#include "main.h"

#define Flash_Number_Old 24
#define Flash_Number_New 25

extern unsigned char Flash_Once_Flag;

extern uint32_t WriteFlashData1[4];//数据
extern uint32_t WriteFlashData2[4];//数据
extern uint32_t WriteFlashData3[4];//数据
extern uint32_t WriteFlashData4[4];//数据
extern uint32_t WriteFlashData5[4];//数据
extern uint32_t WriteFlashData6[4];//数据
extern uint32_t WriteFlashData7[4];//数据

extern uint32_t Flash_Addr1 ;//
extern uint32_t Flash_Addr2 ;//
extern uint32_t Flash_Addr3 ;//
extern uint32_t Flash_Addr4 ;//
extern uint32_t Flash_Addr5 ;//
extern uint32_t Flash_Addr6 ;//
extern uint32_t Flash_Addr7 ;//

/** FLASH解锁 **/
void Flash_Unlock(void);
/** FLASH擦除 **/
void Flash_Erasure(uint32_t addr);
/** 写入数据 **/
void Flash_Write(uint32_t L,uint32_t Data[],uint32_t addr);
/** FLASH_锁住 **/
void Flash_Lock(void);

void PrintFlashTest(uint32_t L,uint32_t addr);
void Flash_Write_Data(uint32_t L,uint32_t Data[],uint32_t addr);

void FlashWrite_Para1(void);//存储
void FlashRead_Para1(void);//读取
void Flash_Write_Residue_Degree(void);//写入剩余次数

void FlashSave_Production(void);
//用于调试的存储数据代码
void FlashSave_Debug(void);


#endif

