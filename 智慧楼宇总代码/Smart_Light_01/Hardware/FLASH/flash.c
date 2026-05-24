#include "flash.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "time.h"

//unsigned char Flash_Once_Flag ; //存储一次标志位

//uint32_t WriteFlashData1[4] = {1,2,3,4};//数据
//uint32_t WriteFlashData2[4] = {5,6,7,8};//数据
//uint32_t WriteFlashData3[4] = {9,10,11,12};//数据
//uint32_t WriteFlashData4[4] = {13,14,15,16};//数据
//uint32_t WriteFlashData5[4] = {17,18,19,20};//数据
//uint32_t WriteFlashData6[4] = {21,22,23,24};//数据
//uint32_t WriteFlashData7[4] = {25,26,27,28};//数据

//uint32_t Flash_Addr1 = 0x0800FC00;//
//uint32_t Flash_Addr2 = 0x0800FBF0;//
//uint32_t Flash_Addr3 = 0x0800FBE0;//
//uint32_t Flash_Addr4 = 0x0800FBD0;//
//uint32_t Flash_Addr5 = 0x0800FBC0;//
//uint32_t Flash_Addr6 = 0x0800FBB0;//
//uint32_t Flash_Addr7 = 0x0800FBA0;//

///** FLASH解锁 **/
//void Flash_Unlock(void)
//{
//	/* 1/4解锁FLASH*/
//	HAL_FLASH_Unlock();
//}

///** FLASH擦除 **/
//void Flash_Erasure(uint32_t addr)
//{
//	/*初始化FLASH_EraseInitTypeDef*/
//	/*擦除方式页擦除FLASH_TYPEERASE_PAGES，块擦除FLASH_TYPEERASE_MASSERASE*/
//	/*擦除页数*/
//	/*擦除地址*/
//	FLASH_EraseInitTypeDef FlashSet;
//	FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
//	FlashSet.PageAddress = addr;
//	FlashSet.NbPages = 1;
//	/*设置PageError，调用擦除函数*/
//	uint32_t PageError = 0;
//	HAL_FLASHEx_Erase(&FlashSet, &PageError);
//}
///** 写入数据 **/
//void Flash_Write(uint32_t L,uint32_t Data[],uint32_t addr)
//{
//	/* 3/4对FLASH烧写*/
//	uint32_t i=0;
//	for(i=0;i<L;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+4*i, Data[i]);
//	}
//}

///** FLASH_锁住 **/
//void Flash_Lock(void)
//{
//	/* 4/4锁住FLASH*/
//	HAL_FLASH_Lock();
//}




///*FLASH写入程序*/
//void Flash_Write_Data(uint32_t L,uint32_t Data[],uint32_t addr)
//{
//	uint32_t i=0;
//	/* 1/4解锁FLASH*/
//	HAL_FLASH_Unlock();
//	/* 2/4擦除FLASH*/
//	/*初始化FLASH_EraseInitTypeDef*/
//	/*擦除方式页擦除FLASH_TYPEERASE_PAGES，块擦除FLASH_TYPEERASE_MASSERASE*/
//	/*擦除页数*/
//	/*擦除地址*/
//	FLASH_EraseInitTypeDef FlashSet;
//	FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
//	FlashSet.PageAddress = addr;
//	FlashSet.NbPages = 1;
//	/*设置PageError，调用擦除函数*/
//	uint32_t PageError = 0;
//	HAL_FLASHEx_Erase(&FlashSet, &PageError);
//	/* 3/4对FLASH烧写*/
//	for(i=0;i<L;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+4*i, Data[i]);
//	}
//	/* 4/4锁住FLASH*/
//	HAL_FLASH_Lock();
//}


///*FLASH读取打印程序*/
//void PrintFlashTest(uint32_t L,uint32_t addr)
//{
//	uint32_t i=0;
//	for(i=0;i<L;i++)
//	{
//		printf("\naddr is:0x%x, data is:0x%x", addr+i*4,  *(__IO uint32_t*)(addr+i*4));
//	}
//	
//}
////读取数据
//void FlashRead_Para1(void)
//{
////	Residue_Degree	= *(__IO uint32_t*)(Flash_Addr1);

////	Work_Time     			= *(__IO uint32_t*)(Flash_Addr2);
////	Turbine_Speed   		= *(__IO uint32_t*)(Flash_Addr2+4);
////	Turbine_On_Temp 		= *(__IO uint32_t*)(Flash_Addr2+4+4);
////	Work_Tike_Last_5     	= *(__IO uint32_t*)(Flash_Addr2+4+4+4);
////	
////	Temp_Coefficient     	= *(__IO uint32_t*)(Flash_Addr3);
////	Temp_Store  			= *(__IO uint32_t*)(Flash_Addr3+4);
////	Temp_Medicine			= *(__IO uint32_t*)(Flash_Addr3+4+4);
////	Flash_Once_Flag			= *(__IO uint32_t*)(Flash_Addr3+4+4+4);
////	
////	Warn_Temp_Store_High = *(__IO uint32_t*)(Flash_Addr4);
////	Pump_In_Time_Delay  	= *(__IO uint32_t*)(Flash_Addr4+4);
////	Pump_Out_Time_Delay		= *(__IO uint32_t*)(Flash_Addr4+4+4);
////	WT2003_Volume_Velue  	= *(__IO uint32_t*)(Flash_Addr4+4+4+4);
//	
////	printf("Residue_Degree   =%d  \r\n",Residue_Degree);
////	
////	printf("Work_Time        =%d  \r\n",Work_Time);
////	printf("Turbine_Speed    =%d  \r\n",Turbine_Speed   );
////	printf("Turbine_On_Temp  =%d  \r\n",Turbine_On_Temp);
////	printf("Work_Tike_Last_5 =%d  \r\n",Work_Tike_Last_5     	);
////	
////	printf("Temp_Coefficient =%d  \r\n",Temp_Coefficient   );
////	printf("Temp_Store  	 =%d  \r\n",Temp_Store  	 );
////	printf("Temp_Medicine	 =%d  \r\n",Temp_Medicine	   );
////	printf("Flash_Once_Flag	 =%d  \r\n",Flash_Once_Flag	   );
////	
////	printf("Warn_Temp_Store_High		=%d  \r\n",Warn_Temp_Store_High 	);
////	printf("Pump_In_Time_Delay			=%d  \r\n",Pump_In_Time_Delay   	);
////	printf("Pump_Out_Time_Delay			=%d  \r\n",Pump_Out_Time_Delay		);
////	printf("WT2003_Volume_Velue			=%d  \r\n",WT2003_Volume_Velue  	);
//	
//}

////存储数据
//void FlashWrite_Para1(void)
//{
////	WriteFlashData1[0]=Residue_Degree;//剩余次数
////	WriteFlashData1[1]=0;
////	WriteFlashData1[2]=0;
////	WriteFlashData1[3]=0;
////	
////	WriteFlashData2[0]=Work_Time; //工作时间
////	WriteFlashData2[1]=Turbine_Speed;
////	WriteFlashData2[2]=Turbine_On_Temp;
////	WriteFlashData2[3]=Work_Tike_Last_5;
////	
////	WriteFlashData3[0]=Temp_Coefficient;
////	WriteFlashData3[1]=Temp_Store;
////	WriteFlashData3[2]=Temp_Medicine;
//	WriteFlashData3[3]=Flash_Once_Flag;
////	
////	WriteFlashData4[0]=Warn_Temp_Store_High;
////	WriteFlashData4[1]=Pump_In_Time_Delay;
////	WriteFlashData4[2]=Pump_Out_Time_Delay;
////	WriteFlashData4[3]=WT2003_Volume_Velue;


//	/** 解锁FLASH **/
//	Flash_Unlock();
//	/** 擦除FLASH **/
//	Flash_Erasure(Flash_Addr1);//专门给剩余次数的
//	Flash_Erasure(Flash_Addr2);//余下的参数放这里
//	/** 写入数据 **/
//	/* FLASH烧写 WriteFlashData1*/
//	for(unsigned int i=0;i<4;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Addr1+4*i, WriteFlashData1[i]);
//	}
//	
//	/* FLASH烧写 WriteFlashData2*/
//	for(unsigned int i=0;i<4;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Addr2+4*i, WriteFlashData2[i]);
//	}
//	/* FLASH烧写 WriteFlashData3*/
//	for(unsigned int i=0;i<4;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Addr3+4*i, WriteFlashData3[i]);
//	}
//	/* FLASH烧写 WriteFlashData4*/
//	for(unsigned int i=0;i<4;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Addr4+4*i, WriteFlashData4[i]);
//	}
//	
//	
//	/** 锁住FLASH **/
//	Flash_Lock();
//}

///*FLASH写入次数程序*/
//void Flash_Write_Residue_Degree(void)
//{
//	WriteFlashData1[0]=Residue_Degree;//剩余次数
//	WriteFlashData1[1]=0;
//	WriteFlashData1[2]=0;
//	WriteFlashData1[3]=0;
//	
//	/** 解锁FLASH **/
//	Flash_Unlock();
//	/** 擦除FLASH **/
//	Flash_Erasure(Flash_Addr1);//专门给剩余次数的
//	/** 写入数据 **/
//	/* FLASH烧写 WriteFlashData1*/
//	for(unsigned int i=0;i<4;i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Addr1+4*i, WriteFlashData1[i]);
//	}
//	/** 锁住FLASH **/
//	Flash_Lock();
//}

////用于生产的存储数据代码
//void FlashSave_Production(void)
//{
//	
//	Flash_Once_Flag			= *(__IO uint32_t*)(Flash_Addr3+4+4+4);
//	
//	if(Flash_Once_Flag==Flash_Number_New)
//	{
//		Flash_Once_Flag=Flash_Number_New;
//	}else{
//		Flash_Once_Flag=Flash_Number_Old;
//	}
//	
//	//判断下载一次
//  if(Flash_Once_Flag == Flash_Number_Old)
//  {
//	 HAL_Delay(100);
//	 Flash_Once_Flag=Flash_Number_New;
//	 FlashWrite_Para1();  //存储默认参数
//	  
////	 printf("Flash_Once_Flag = %d \r\n",Flash_Once_Flag);
//  }
//  
//   HAL_Delay(100);
//   FlashRead_Para1();//初始化读取内部FLASH的参数值
//  
//}

////用于调试的存储数据代码
//void FlashSave_Debug(void)
//{
//	HAL_Delay(100);
//	FlashWrite_Para1();  //存储默认参数
//	
//	HAL_Delay(100);
//    FlashRead_Para1();//初始化读取内部FLASH的参数值
//}


