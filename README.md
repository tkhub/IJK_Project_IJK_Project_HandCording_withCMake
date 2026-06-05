
[株式会社アールティー](https://www.rt-shop.jp/)製ロボットキットの[TrainingTracer_V3](https://www.rt-shop.jp/index.php?main_page=product_info&products_id=4248)用ソフト。
(V2の場合、改造が必要)
 
# Requirement
## Software
* STM32CubeMX
* STM32CubeCLT
* STM32CubeProgramer
* VSCode
* [STM32 VS code extension](https://www.st.com/content/st_com/ja/campaigns/stm32-vs-code-extension-z11.html)
  
## [Hardware](https://github.com/rt-net/TrainingTracer_V3_Hardware)
V2キットの場合、以下の改造を実施。(TrainingTracerV3相当)
### 意図しないタイヤの回転を抑制
ソフトの書き換え中など、マイコンが起動していない際にタイヤが回転してしまうため、PWM端子を10kΩでプルダウンして電位を固定。

<img width="684" height="611" alt="image" src="https://github.com/user-attachments/assets/82094fe9-7a02-4378-ba7e-194b9c97ff3d" />

### バッテリー電圧の計測精度を向上
左端センサの反応にバッテリー電圧の計測結果が左右される問題があるため、マイコンの端子に0.1μFのコンデンサを追加。
<img width="738" height="588" alt="image" src="https://github.com/user-attachments/assets/fd129ee7-a396-4fd7-941c-6d260d8605aa" />

（おそらく、ADコンバータのサンプリングコンデンサの電荷がバッテリー電圧計測用分圧抵抗を通して充放電するのが間に合わず、計測結果が変わってしまうためだと思われます。）
 
### IMUセンサをICM42688Pに換装
搭載方向はV3と同等とし、適宜両面テープ等で固定する。

# Usage
## ドライバ機能
### UISW
割り込み内でチャタリングを除去するとともにエッジを検出し、普通に押す場合、長押し、両押しのイベントを発行・キューイング。
メインルーチンでキューイングしたデータを取り出す。

#### API
 - uiswevent_t uiswRead(void)
    - スイッチ押下状態を取得する(uiswevent_t型)
        - NON_UISW_EVENT : スイッチイベントなし
        - UISW1_PUSH_EVENT : SW1が押されて離されるイベント
        - UISW2_PUSH_EVENT : SW2が押されて離されるイベント
        - UISW1_LONG_EVENT : SW1が長押し押されてるイベント
        - UISW2_LONG_EVENT : SW2が長押し押されてるイベント
        - UISW1_LONG_RELEASE_EVENT : SW1が長押し押され離されたイベント
        - UISW2_LONG_RELEASE_EVENT : SW2が長押し押され離されたイベント
        - UISW_BOTH_EVENT : SWが両押しされたイベント

### ブザー
メインルーチンで鳴動パターン(ON/OFFの時間)をキューイング。割り込み内でキューイングされた鳴動パターンに応じてブザーを制御。

#### API
 - void buzzerSetScheduleMs(uint16_t onMs, uint16_t offMs)
    - ON時間、OFF時間をまとめて依頼するとブザーが鳴動する

### ライン・マーカーセンサー
割り込み内で各センサの値を取得。白黒を判定するとともに、機体中央を0としてラインが左ズレ時に正、右ズレ時に負となるズレ量をmmで計測。

#### API
- float linesensorsReadPosition(void)
    - ラインからのズレをmm単位で計測する関数(左にズレたらマイナス)
- uint8_t linesensorsReadBin(void)
    - 各ラインセンサの白黒判定値を読み出す関数
- uint8_t markersensorsRead(void)
    - 各マーカセンサの白黒判定値を読み出す関数
- void linesensorsCalibration(const uint16_t llsw, const uint16_t llsb, const uint16_t lcsw, const uint16_t lcsb, const uint16_t rcsw, const uint16_t rcsb, const uint16_t rrsw, const uint16_t rrsb)
    - ラインセンサ毎の白黒値を渡すことで、AD値を0.0～1.0の正規化した値で扱える。
- void markersensorsCalibration(const uint16_t mklw, const uint16_t mklb, const uint16_t mkrw, const uint16_t mkrb);
    - マーカセンサ毎の白黒値を渡すことで、AD値を0.0～1.0の正規化した値で扱える。

### バッテリー電圧計測
割り込み内でバッテリー電圧のAD値を取得。フィルターしたうえで実電圧に公開するとともに、バッテリー状態を電圧で判断して公開する。

#### API
- float batteryRead(void)
    - バッテリ電圧の計測値[V]
- batterystatus_t batteryReadStateatus(void)
    - バッテリの状態を電圧で判定(batterystatus_t)
        - BATTERY_AAA_EMPTY : 単3電池過放電警告
        - BATTERY_AAA_WRN : 単3電池消耗注意
        - BATTERY_AAA_NORM : 単3電池通常状態
        - BATTERY_AAA_FULL : 単3電池新品
        - BATTERY_LIPO_EMPTY : リポバッテリー過放電警告
        - BATTERY_LIPO_WRN : リポバッテリー消耗注意
        - BATTERY_LIPO_NORM : リポバッテリー通常状態
        - BATTERY_LIPO_FULL : リポバッテリー満充電


### モータ・ドライバ
モータとエンコーダを制御する。
#### API
- void motorsDrive(const float nrmPwrL, const float nrmPwrR)
    - 左右のモーター回転出力を指示する(-1.0～+1.0)。正の値を設定すると各タイヤが前進する。
- void motorsReadPower(float* nrmPwrL, float* nrmPwrR)
    - 左右のモータに設定されている回転出力を読み出す
- void motorsResetRound(void)
    - エンコーダの積算回転数をリセットする
- void motorsReadRps(float* rpsL, float* rpsR)
    - 左右のモータ回転数[rps]を返す。正の場合は前進。
- void motorsReadRound(float* roundL, float* roundR)
    - 左右のモータ総回転数[回転]を返す。正の場合は前進。

### IMU
ジャイロと加速度を検出する。
#### API
- gyro_t imuGyro(void)
    - IMUセンサの各軸角速度を取得する[rps]
        - ロール : X軸周りの回転(右側が下がる方向が正)
        - ピッチ : Y軸周りの回転(前方が下がる方向が正)
        - ヨー : Z軸周りの回転(機体が反時計回りに回転する方向が正)
- accel_t imuAccel(void)
    - IMUセンサの各軸加速度を取得する[g]
        - X : 機体前進方向を正
        - Y : 機体左方向を正
        - Z : 機体上方向を正
- imu_t imuAll(void)
    - gyro_t imuGyro(void),accel_t imuAccel(void)の値を一度に読み出せる

## アプリ機能
アプリ機能はブランクとなっている。
### API
- void appInit(void)
    - アプリ機能の初期化を行う
#### リアルタイム制御
タイマ割り込みに差し込まれるので、制御が破綻しないように処理時間のかかる処理を入れない（ブロッキングしない）こと。
Nculeoボードの緑色LEDが点灯している間は割り込みの実行時間である。オシロスコープで処理時間を確認が可能。
- void appIntervalHandler_1ms(void)
    - センサ類の取得とモータ駆動の間で呼び出される(センサ類に対して500μs遅延する)
- void appIntervalHandler_10ms(void)
    - 各センサの読み出しが終わってから読み出される
#### ノン・リアルタイム制御
- void appMainLoop(void)
    - リアルタイム制御ではない処理（UI等）を差し込む

## デバッグ・テスト
usercodes/common/test_debug.hの_ENABLE_SAC_TEST_を有効にすることで、ドライバのデバッグ機能を有効化する。
SAC_DEBUGMODEに各デバッグ用マクロを設定することで各SACがデバッグモードとして動作する

# License
- IJKProject under [MIT license](https://en.wikipedia.org/wiki/MIT_License)
- [xprintf](https://elm-chan.org/fsw/strf/xprintf_j.html)

