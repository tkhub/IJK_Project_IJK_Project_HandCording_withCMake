/**
 * @file app.h
 * @brief 簡単な説明
 * @author USER
 * @date 2025/11/30
 * @Version 0.00
 */

#ifndef __APP_H___
#define __APP_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/* #define MAX_NANKA 256 */
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/* extern int global_var; */
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/* int testFunc(int* buffer, int size); */

void appInit(void);
void appIntervalHandler_1ms(void);
void appIntervalHandler_10ms(void);
void appMainLoop(void);

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __APP_H___ */