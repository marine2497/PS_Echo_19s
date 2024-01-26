#ifndef  UART_PROTOCOL_H
#define  UART_PROTOCOL_H
#include <rtthread.h>
#include "board.h"
/*  ±äÁ¿·ÖÅä4×Ö½Ú¶ÔÆë */
ALIGN(RT_ALIGN_SIZE)
//#define	GET						0
//#define	SET						1
//#define	ERR						2
#define	Byte_Type				0
#define	Int_Type				1
#define	Float_Type				2
#define	Str_Type				3

#define	Channel_One				1
#define	Channel_Two				2

#define	str_len					32
#define	Device_Info_Addr		((DEVICE_INFO *) 0x08007800)

//#define	boot_ver_str			"20175625"			// BOOT°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¨°üº¬¡¯\0¡¯£©
//#define sn_str					"S23001111111111"	// 15Î»»úÉíºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define pn_str					"S23001111111111"	// Ó²¼ş°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define pid_str					"729048374639"		// ²úÆ·¸½¼ÓÂë£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ128(°üº¬¡¯\0¡¯)
//#define hid_str					"837364545"			// ÏµÍ³ĞÅÏ¢
//#define brand_str				"SOUTH"				// ²úÆ·Æ·ÅÆ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define model_str				"SDE230DF"			// »úĞÍ,×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define production_date_str		"20180109"			// Éú²úÈÕÆÚ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define app_ver_str				"2017.12.26"		// ¹Ì¼ş°æ±¾ºÅ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//#define gnssboard_str			"256145"			// Ö÷°åÀàĞÍ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¬°üº¬¡¯\0¡¯©

// 1.1.1   INFO	½Úµã(¸Ã½ÚµãÖ§³ÖGET/SET»ñÈ¡Ò¶×ÓĞÅÏ¢)
//typedef struct
//{
//	char boot_ver[sizeof(boot_ver_str)];				// BOOT°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¨°üº¬¡¯\0¡¯£©
//	char sn[sizeof(sn_str)];							// 15Î»»úÉíºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char pn[sizeof(pn_str)];							// Ó²¼ş°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char pid[sizeof(pid_str)];							// ²úÆ·¸½¼ÓÂë£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ128(°üº¬¡¯\0¡¯)
//	char hid[sizeof(hid_str)];							// ÏµÍ³ĞÅÏ¢
//	char brand[sizeof(brand_str)];						// ²úÆ·Æ·ÅÆ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char model[sizeof(model_str)];						// »úĞÍ,×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char production_date[sizeof(production_date_str)];	// Éú²úÈÕÆÚ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char app_ver[sizeof(app_ver_str)];					// ¹Ì¼ş°æ±¾ºÅ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	char gnssboard[sizeof(gnssboard_str)];				// Ö÷°åÀàĞÍ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¬°üº¬¡¯\0¡¯
//}DEVICE_INFO;//
typedef struct
{
	const char boot_ver[str_len];						// BOOT°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¨°üº¬¡¯\0¡¯£©
	const char sn[str_len];							// 15Î»»úÉíºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
	const char pn[str_len];							// Ó²¼ş°æ±¾ºÅ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
//	const char pid[str_len];							// ²úÆ·¸½¼ÓÂë£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ128(°üº¬¡¯\0¡¯)
	const char hid[str_len];							// ÏµÍ³ĞÅÏ¢
	const char brand[str_len];						// ²úÆ·Æ·ÅÆ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
	const char model[str_len];						// »úĞÍ,×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
	const char production_date[str_len];				// Éú²úÈÕÆÚ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
	const char app_ver[str_len];						// ¹Ì¼ş°æ±¾ºÅ, ×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32(°üº¬¡¯\0¡¯)
	const char gnssboard[str_len];					// Ö÷°åÀàĞÍ£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ32£¬°üº¬¡¯\0¡¯	
	char pid[str_len];								// ²úÆ·¸½¼ÓÂë£¬×Ö·û´®ÒÔ¡¯\0¡¯½áÎ²£¬³¤¶È²»´óÓÚ128(°üº¬¡¯\0¡¯)
}DEVICE_INFO;
extern	char Pid[str_len];	
extern	char App_ver[str_len];	

// 1.1.2 TIME.´Ë½ÚµãÊôĞÔÎª²âÉîÒÇµÄ×¢²áÊ±¼äºÍÊ¹ÓÃÊ±¼ä£¬½ö²éÑ¯£¬¼´½öÖ§³ÖGETÃüÁî
typedef struct
{
	char str_reg_time[8];
	uint32_t reg_time;								// ×¢²áÊ±¼ä
	uint32_t used_time;								// ¹¤×÷Ê±¼ä¼ÇÂ¼£¬×ª»¯³Éused_time×Ö·û´®
	uint32_t p_Cur_Addr;							// ¼ÇÂ¼Ğ´ÈëFlashµÄµØÖ·
	uint32_t Cur_Counter_10Min;						// µ±Ç°¼ÇÂ¼µÄ10·ÖÖÓ¸öÊı
}TIME;
// 1.1.3 VOLTAGEÊôĞÔ.´Ë½ÚµãÊôĞÔ·µ»Øµ±Ç°µç³ØµçÑ¹»òÍâ½ÓµçÔ´µÄÕæÊµµçÑ¹£¬ÒÔVÎªµ¥Î»¡£½öÖ§³ÖGETÃüÁî
typedef struct
{
	float voltage;								// ´Ë½ÚµãÊôĞÔ·µ»Øµ±Ç°µç³ØµçÑ¹»òÍâ½ÓµçÔ´µÄÕæÊµµçÑ¹£¬ÒÔVÎªµ¥Î»
}VOLTAGE;
// 1.1.4 REGIÊôĞÔ.´Ë½ÚµãÊôĞÔÓÃÓÚ×¢²á²âÉîÒÇ£¬½öÖ§³ÖSETÃüÁî
typedef struct
{
	char regi[40];									// ´Ë½ÚµãÊôĞÔÓÃÓÚ×¢²á²âÉîÒÇ£¬½öÖ§³ÖSETÃüÁî
	char srt_regi[40];
}REGI;
// 1.1.5SURVEY_STATUS,´Ë½ÚµãÊôĞÔÎªÆô¶¯²âÉîºÍ¹Ø±Õ²âÉîÃüÁî£¬1±íÊ¾¿ªÆô£¬0±íÊ¾¹Ø±Õ£¬¿ÉÖ§³ÖSETºÍGETÃüÁî£¬Ö§³ÖÍ¬Ê±ÉèÖÃËùÓĞµÄÍ¨µÀ¡£
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_STATUS;
// 1.1.7 SURVEY_MODEL,´Ë½ÚµãÊôĞÔÎªÉèÖÃ¸÷Í¨µÀµÄ²âÁ¿Ä£Ê½ÎªÊÖ¶¯²âÁ¿Ä£Ê½»ò×Ô¶¯²âÁ¿Ä£Ê½£¬0±íÊ¾ÊÖ¶¯£¬1±íÊ¾×Ô¶¯£¬Ö§³ÖSETÃüÁî¼°GETÃüÁî,Ö§³ÖÍ¬Ê±ÉèÖÃËùÓĞµÄÍ¨µÀ¡£
typedef struct
{
	uint8_t channel_one;	
	uint8_t channel_two;	
}SURVEY_MODEL;
//1.1.8 DOGNUM.´Ë½ÚµãÊôĞÔÓÃÓÚÓë×ÔÓÉĞĞÎÕÊÖÊ±µÄ½»»¥£¬½öÖ§³ÖSETÃüÁî,ÎÕÊÖÊ±¿ÉÏÈ·¢ËÍPENDINGÃüÁî£¬±íÊ¾ÒÑ½ÓÊÕ£¬¼ÓÃÜºóÒÔGETÓ¦´ğµÄ¸ñÊ½·µ»Ø
typedef struct
{
	char dognum[16];									
}DOGNUM;
//1.2 Parameters.´Ë½ÚµãÊôĞÔÎª²ÎÊıÉèÖÃ£¬¶¼»áÓĞ¸öÎ¨Ò»µÄÍ¨µÀºÅÒ¶×Ó½Úµã£¬Ö»ÓĞÕâ¸öÒ¶×Ó½Úµã²ÅÄÜ×é³ÉÒ»ÌõÓï¾ä¡£
//1.2 Parameters.´Ë½ÚµãÊôĞÔÎª²ÎÊıÉèÖÃ£¬¶¼»áÓĞ¸öÎ¨Ò»µÄÍ¨µÀºÅÒ¶×Ó½Úµã£¬Ö»ÓĞÕâ¸öÒ¶×Ó½Úµã²ÅÄÜ×é³ÉÒ»ÌõÓï¾ä¡£
typedef struct
{
	float draft;									// ³ÔË® µ¥Î»ÎªÃ× È¡ÖµÎª0.00-9.99 Ä¬ÈÏ£º0.50
	float velocity;								   // ÉùËÙ µ¥Î»ÊÇÃ×/Ãë È¡ÖµÎª1300-1700 Ä¬ÈÏ£º1483
	float minadepth;								// Ç³Ë®±¨¾¯Ë®Éî µ¥Î»ÎªÃ× È¡ÖµÎª0.50-9.99 Ä¬ÈÏ£º1
	float maxadepth;								// ÉîË®±¨¾¯Ë®Éî µ¥Î»ÎªÃ× È¡ÖµÎª1.00-600.00 Ä¬ÈÏ£º100
	uint8_t alarmstatus;							// ÎŞ»Ø²¨±¨¾¯±êÊ¶ 0Îª¹Ø£¬1Îª¿ª Ä¬ÈÏ£º0£¨¹Ø±Õ£©
	uint8_t alarmchannel;							// ²Î¿¼±¨¾¯Í¨µÀºÅ È¡Öµ1-16 Ä¬ÈÏ£º1
	uint8_t missreturn;								// ±íÊ¾ÔÊĞí¶ª²¨¸öÊı È¡Öµ1-10 Ä¬ÈÏ£º5									
}PARAMETERS;
// 1.2.2Calibration±È¶Ô°åÉùËÙĞ£Õı²ÎÊı¡£´Ë½ÚµãÊôĞÔÎª±È¶Ô°åÉùËÙ¼ÆËãÏà¹ØÃüÁî£¬½öÖ§³ÖSETÃüÁî
typedef struct
{
	uint16_t DEPTH;									// ±È¶ÔË®Éî
	uint8_t STATUS;									// ¿ªÊ¼ºÍ½áÊø±È¶Ô×´Ì¬£¬1±íÊ¾¿ªÆô£¬0±íÊ¾½áÊø
}CALIBRATION;
//Sounder²âÉîÒÇ²ÎÊı.´Ë½ÚµãÊôĞÔÎªÃ¿¸öÍ¨µÀµÄ²âÉîÒÇ²ÎÊı£¬¿ÉÖ§³ÖGET,SETÃüÁî£¬²¢×Ô¶¯ÉÏ´«£¬ÉÏ´«Ê±¸ñÊ½Í¬GETÓ¦´ğ¸ñÊ½¡£
typedef struct
{
	uint8_t Power_Index;							// ¹¦ÂÊ È¡Öµ1-3 Ä¬ÈÏ£º2
	const uint16_t Power_ADC_Value[8];				// ¹¦ÂÊ¶ÔÓ¦µÄADCÖµ
}POWER_CFG;
typedef struct
{
	uint8_t Gain_Index;								// ÔöÒæ È¡Öµ1-8 Ä¬ÈÏ£º3
	uint16_t Gain_ADC_Value[8]; 					// ÔöÒæ ¶ÔÓ¦µÄADCÖµ
}GAIN_CFG;
//////////////////////////////////////SOUNDER_cfg;
typedef struct	// ·¢ÉäÂö¿í µ¥Î»ÎªºÁÃë È¡ÖµÎª 0.01-1.20 Ä¬ÈÏ£º0.2
{
	float	channel_one;
	float	channel_two;
}TXWIDTH;
typedef struct
{
	float 	channel_one;							// ÒÖÖÆË®Éî µ¥Î»ÎªÃ× È¡ÖµÎª0.2-20 Ä¬ÈÏ£º0.8;0.2-3.0 ²½½ø0.1   3.0-10.0 ²½½ø1  10-20 ²½½ø2
	float 	channel_two;							// ÒÖÖÆË®Éî µ¥Î»ÎªÃ× È¡ÖµÎª0.2-20 Ä¬ÈÏ£º0.8;0.2-3.0 ²½½ø0.1   3.0-10.0 ²½½ø1  10-20 ²½½ø2
}CLEARWIDTH;
typedef struct
{
	POWER_CFG	channel_one;						// ¹¦ÂÊ È¡Öµ1-3 Ä¬ÈÏ£º2
	POWER_CFG	channel_two;						// ¹¦ÂÊ È¡Öµ1-3 Ä¬ÈÏ£º2
}POWER;
typedef struct
{
	GAIN_CFG 	channel_one;						// ÔöÒæ È¡Öµ1-8 Ä¬ÈÏ£º3
	GAIN_CFG 	channel_two;						// ÔöÒæ È¡Öµ1-8 Ä¬ÈÏ£º3
}GAIN;
typedef struct
{
	uint8_t channel_one;							// »Ø²¨ãĞÖµ È¡Öµ1-5 Ä¬ÈÏ£º3
	uint8_t channel_two;							// »Ø²¨ãĞÖµ È¡Öµ1-5 Ä¬ÈÏ£º3
}THRESHOLD;
typedef struct
{
	uint8_t channel_one;							// TVGÇúÏß È¡Öµ0-30 µ¥Î»ÎªlogR ²½½ø5 Ä¬ÈÏ£º20
	uint8_t channel_two;							// TVGÇúÏß È¡Öµ0-30 µ¥Î»ÎªlogR ²½½ø5 Ä¬ÈÏ£º20
}TVGS;
typedef struct
{
	float 	channel_one;							// Ê±¼äÃÅ²ÎÊı È¡Öµ0%-50%  0.0±íÊ¾¹Ø±Õ Ä¬ÈÏ£º0.1
	float 	channel_two;							// Ê±¼äÃÅ²ÎÊı È¡Öµ0%-50%  0.0±íÊ¾¹Ø±Õ Ä¬ÈÏ£º0.1
}FILTER;
typedef struct
{
	uint8_t channel_one;							// ÆğÊ¼ÔöÒæ È¡ÖµÎª0-60 µ¥Î»Îªdb²½½øÎª5 Ä¬ÈÏ£º10
	uint8_t channel_two;							// ÆğÊ¼ÔöÒæ È¡ÖµÎª0-60 µ¥Î»Îªdb²½½øÎª5 Ä¬ÈÏ£º10
}STARTGAIN;
typedef struct
{
	uint8_t channel_one;							// ÆµÂÊ.¸ßÆµ£º100KHZ-1000KHZ Ä¬ÈÏ£º200KHZ;µÍÆµ£º10KHZ-100KHZ   Ä¬ÈÏ£º20KHZ
	uint8_t channel_two;							// ÆµÂÊ.¸ßÆµ£º100KHZ-1000KHZ Ä¬ÈÏ£º200KHZ;µÍÆµ£º10KHZ-100KHZ   Ä¬ÈÏ£º20KHZ
}FREQUENCY;
typedef struct
{
	uint8_t channel_one;							// Ç³Ë®²âÁ¿Ä£Ê½.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
	uint8_t channel_two;							// Ç³Ë®²âÁ¿Ä£Ê½.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
}SHALLOWSURVEYMODE;
typedef struct
{
	uint8_t channel_one;							// Ç¿ÖÆËø¶¨»Ø²¨.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
	uint8_t channel_two;							// Ç¿ÖÆËø¶¨»Ø²¨.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
}FORCEDLOCKINGECHO;

/*upper limit  lower limit*/
//typedef struct
//{
//	uint16_t channel_one_upperlimit;							// Ç¿ÖÆËø¶¨»Ø²¨.ÉÏÏŞ  Ä¬ÈÏ£º0
//	uint16_t channel_one_lowerlimit;							// Ç¿ÖÆËø¶¨»Ø²¨.ÏÂÏŞ  Ä¬ÈÏ£º0
//	
//	uint16_t channel_two_upperlimit;							// Ç¿ÖÆËø¶¨»Ø²¨.ÉÏÏŞ  Ä¬ÈÏ£º0
//	uint16_t channel_two_lowerlimit;							// Ç¿ÖÆËø¶¨»Ø²¨.ÏÂÏŞ  Ä¬ÈÏ£º0	
//}FORCEDLOCKINGECHOVALNU;

typedef struct
{
	uint8_t channel_one;						// »Ø²¨ÅĞ¶Ï.È¡Öµ1¡¢2¡¢3  1£º×î´óÄÜÁ¿»Ø²¨  2£º²Î¿¼¸ßÆµ»Ø²¨ 3£ºµ×²ã»Ø²¨.Ä¬ÈÏ£º1
	uint8_t channel_two;						// »Ø²¨ÅĞ¶Ï.È¡Öµ1¡¢2¡¢3  1£º×î´óÄÜÁ¿»Ø²¨  2£º²Î¿¼¸ßÆµ»Ø²¨ 3£ºµ×²ã»Ø²¨.Ä¬ÈÏ£º1
}INDENTIFYARITHMETIC;
typedef struct
{
	TXWIDTH 			txwidth;								// ·¢ÉäÂö¿í µ¥Î»ÎªºÁÃë È¡ÖµÎª 0.01-1.20 Ä¬ÈÏ£º0.2
	CLEARWIDTH 			clearwidth;								// ÒÖÖÆË®Éî µ¥Î»ÎªÃ× È¡ÖµÎª0.2-20 Ä¬ÈÏ£º0.8;0.2-3.0 ²½½ø0.1   3.0-10.0 ²½½ø1  10-20 ²½½ø2
	POWER				power;									// ¹¦ÂÊ È¡Öµ1-3 Ä¬ÈÏ£º2
	GAIN 				gain;									// ÔöÒæ È¡Öµ1-8 Ä¬ÈÏ£º3
	THRESHOLD			threshold;								// »Ø²¨ãĞÖµ È¡Öµ1-5 Ä¬ÈÏ£º3
	TVGS				tvgs;									// TVGÇúÏß È¡Öµ0-30 µ¥Î»ÎªlogR ²½½ø5 Ä¬ÈÏ£º20
	FILTER	 			filter;									// Ê±¼äÃÅ²ÎÊı È¡Öµ0%-50%  0.0±íÊ¾¹Ø±Õ Ä¬ÈÏ£º0.1
	STARTGAIN			startgain;								// ÆğÊ¼ÔöÒæ È¡ÖµÎª0-60 µ¥Î»Îªdb²½½øÎª5 Ä¬ÈÏ£º10
	FREQUENCY			frequency;								// ÆµÂÊ.¸ßÆµ£º100KHZ-1000KHZ Ä¬ÈÏ£º200KHZ;µÍÆµ£º10KHZ-100KHZ   Ä¬ÈÏ£º20KHZ
	SHALLOWSURVEYMODE	shallowsurveymode;						// Ç³Ë®²âÁ¿Ä£Ê½.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
	FORCEDLOCKINGECHO	forcedlockingecho;						// Ç¿ÖÆËø¶¨»Ø²¨.È¡Öµ0»ò1  0£º¹Ø±Õ  1£º¿ªÆô  Ä¬ÈÏ£º0
//    FORCEDLOCKINGECHOVALNU forcedlockingechovalue;				// Ç¿ÖÆËø¶¨»Ø²¨.ÉÏÏŞÓëÏÂÏŞ
	INDENTIFYARITHMETIC	indentifyarithmetic;					// »Ø²¨ÅĞ¶Ï.È¡Öµ1¡¢2¡¢3  1£º×î´óÄÜÁ¿»Ø²¨  2£º²Î¿¼¸ßÆµ»Ø²¨ 3£ºµ×²ã»Ø²¨.Ä¬ÈÏ£º1
}SOUNDER;
// 1.3 grade ²âÁ¿µµÎ».´Ë½ÚµãÎªµµÎ»Öµ£¬Ö§³ÖSETÃüÁî£¬¼°×Ô¶¯ÉÏ´«£¬ÉÏ´«Ê±¸ñÊ½Í¬GETÓ¦´ğ¸ñÊ½.µµÎ»È¡Öµ0-7Ä¬ÈÏ15Ã×µµ
typedef struct
{
	uint8_t grade;
	uint8_t auto_grade;
}GRADE;
// ÏµÍ³ĞÅÏ¢¼°ÅäÖÃ¿ØÖÆ¿é
typedef struct
{
	DEVICE_INFO		*Device_Info;
	TIME 			Time;
	VOLTAGE 		Voltage;
	DOGNUM 			Dogunm;	
	PARAMETERS 		Parameters;
	REGI 			Regi;
	SURVEY_STATUS 	Survey_Status;
	SURVEY_MODEL	Survey_Model;
	CALIBRATION 	Calibration;
	SOUNDER			Sounder;
	GRADE			Grade;
}SYS_CB;
// ÏµÍ³ĞÅÏ¢¼°ÅäÖÃ¿ØÖÆ¿é¹Ø¼ü±äÁ¿¿½±´
typedef struct
{
	PARAMETERS 		Parameters;
	REGI 			Regi;
	SURVEY_STATUS 	Survey_Status;
	SURVEY_MODEL	Survey_Model;
	CALIBRATION 	Calibration;
	SOUNDER			Sounder;
	GRADE			Grade;
}SYS_CB_COPY;


typedef struct
{
	uint8_t Transform_One;
	uint8_t Transform_Two;
//    uint8_t Transform_Three;
//    uint8_t Transform_Four;
}TRANSFORM;
// ÏµÍ³¿ØÖÆ¿é
extern SYS_CB 		Sys_CB;									
extern SYS_CB_COPY	Sys_CB_Copy;
extern uint8_t CMD_Refresh;
extern TRANSFORM       Transform;
/*************************************************************************************************/
//	×Ö·û´®¿½±´£¬·µ»ØÊµ¼Ê¿½±´¸öÊı
/*************************************************************************************************/
char do_crc(char *src,uint32_t len);				
uint16_t my_strncpy(char *dst, const char *src, uint16_t n);
uint8_t Float_to_Str(char *p,float dat);
uint8_t Str_to_float(char *p,float *dat);
uint8_t Long_to_Str(char *p,uint32_t dat);
uint8_t Str_to_Long(char *p,uint32_t *dat);
uint8_t Str_to_Long_num(char *p,uint32_t *dat,char num);

void getCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void Tran_setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);
void Jump_setCMD_Process(char *p);
void CMD_Process(void);
void Tran_setCMD_ACK(char *Str,void *p_Dat,uint8_t Dat_Type);

#endif
