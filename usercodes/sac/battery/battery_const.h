/**
 * @file battery_const.h
 * @brief 簡単な説明(定数宣言)  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_BATTERY_BATTERY_CONST_H___
#define __SAC_BATTERY_BATTERY_CONST_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define BATTERY_THRESHOLDS_TABLES_LEN 10
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief 上昇時としきい値を配列
 */
typedef struct {
    float rise_thresholds[BATTERY_THRESHOLDS_TABLES_LEN];
    float fall_thresholds[BATTERY_THRESHOLDS_TABLES_LEN];
}BatteryThresholdsTables_t;
/*========AAAA Typedef Definition END AAAA===================================*/



/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

/**
 * @brief バッテリー電圧計測ゲイン
 * @details バッテリー計測ADC結果を正しい電圧に変換するための調整値
 */
extern const float BATTERY_GAIN;
/**
 * @brief バッテリー電圧計測オフセット
 * @details バッテリー計測ADC結果を正しい電圧に変換するための調整値
 */
extern const float BATTERY_OFFSET;

/**
 * @brief バッテリー電圧フィルタゲイン
 * @details バッテリー電圧のノイズを除去するためのローパスフィルターゲイン
 */
extern const float BATTERY_IIRLPF_GAIN;

extern const BatteryThresholdsTables_t BATTERY_THRESHOLDS_TABLES;

/**
 * @brief 初期化時の電圧
 * @details バッテリー電圧のフィルタ収束の早期化とシーケンスが狂わないように念の為設定
 */
extern const float BATTERY_INIT_VOL;

/**
 * @brief 電圧安定待ち時間
 * @details ハード上・ソフト上フィルタの影響を排除するためのマスク時間
 */
extern const uint16_t BATTERY_MASK_10MSCNT;
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_BATTERY_BATTERY_CONST_H___ */
