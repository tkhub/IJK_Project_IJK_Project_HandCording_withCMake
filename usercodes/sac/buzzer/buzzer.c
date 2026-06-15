/**
 * @file buzzer.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "buzzer.h"
#include "gpio.h"

#if SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
#include <xprintf.h>
#include <string.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/

static buzzerSchedule_t    buzzerScheduleBuffer[BUZZERSCHEDULE_BUFFER_SIZE];
static buzzerSchedule_t     buzzerScheduleExec;
volatile static uint8_t buzzerBufferTail;
volatile static uint8_t buzzerBufferHead;
volatile static uint16_t buzzerOnCount;
volatile static uint16_t buzzerOffCount;
volatile static bool buzzerExecFlag;

#if SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
static uint8_t testModeCnt;
#endif /* SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST */

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/

/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/

void buzzerInit(void) {
    buzzerBufferHead = 0;
    buzzerBufferTail = 0;
    buzzerOnCount = 0;
    buzzerOffCount = 0;
    buzzerExecFlag = false;
#if SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
    testModeCnt = 0;
#endif /* SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST */
}

void buzzer_10ms(void) {
    if (buzzerExecFlag) {
        if (buzzerOnCount != 0) {
            HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,1);
            buzzerOnCount--;
        }
        else if (buzzerOffCount != 0) {
            HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,0);
            buzzerOffCount--;
        }
        else {
            buzzerExecFlag = false;
        }

    }
    else {
        if (buzzerBufferHead != buzzerBufferTail) {
            buzzerBufferHead = (buzzerBufferHead + 1) % BUZZERSCHEDULE_BUFFER_SIZE;
            buzzerScheduleExec = *(buzzerScheduleBuffer + buzzerBufferHead);
            buzzerOnCount = buzzerScheduleExec.oncount10ms;
            buzzerOffCount = buzzerScheduleExec.offcount10ms;
            buzzerExecFlag = true;
        }
    }
}

void buzzerSetSchedule(buzzerSchedule_t bzsch) {
    __disable_irq();
    *(buzzerScheduleBuffer + buzzerBufferTail) = bzsch;
    buzzerBufferTail = (buzzerBufferTail + 1) % BUZZERSCHEDULE_BUFFER_SIZE;
    __enable_irq();
}

void buzzerSetScheduleMs(uint16_t onMs, uint16_t offMs) {
    buzzerSchedule_t tmp;
    tmp.oncount10ms = onMs / 10;
    tmp.offcount10ms = offMs / 10;
    buzzerSetSchedule(tmp);
}

#if SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
uint8_t buzzerTest(char* strBuffer, uint8_t maxBufferSize) {
    switch (testModeCnt) {
    case 0:
        buzzerSetScheduleMs(100, 100);
        xsnprintf(strBuffer, maxBufferSize,"0: on = 100ms, off = 100ms");
        testModeCnt++;
        HAL_Delay(500);
        break;

    case 1:
        buzzerSetScheduleMs(500, 250);
        buzzerSetScheduleMs(500, 250);
        xsnprintf(strBuffer, maxBufferSize,"1:on = 500ms, off = 250ms\n\r2:on = 500ms, off = 250ms\n\r");
        testModeCnt++;
        HAL_Delay(2000);
        break;

    case 2:
        buzzerSetScheduleMs(50, 50);
        buzzerSetScheduleMs(50, 50);
        buzzerSetScheduleMs(50, 50);
        xsnprintf(strBuffer, maxBufferSize,"1: on = 50ms, off = 50ms\n\r2: on = 50ms, off = 50ms\n\r3: on = 50ms, off = 50ms\n\r");
        testModeCnt++;
        HAL_Delay(1000);
        break;

    default :
        buzzerSetScheduleMs(10, 10);
        xsnprintf(strBuffer, maxBufferSize,"x: on = 10ms, off = 10ms");
        testModeCnt = 0;
        HAL_Delay(2000);
        break;
    }


    return 0;
}
#endif /* SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/

/*========AAAA Private Function Definition END AAAA==========================*/
