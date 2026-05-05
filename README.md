
[株式会社アールティー](https://www.rt-shop.jp/)製ロボットキットの[TrainingTracer_V2](https://github.com/rt-net/TrainingTracer_V2_Hardware)用ソフト。
 
# Requirement
## Software
* STM32CubeMX
* VSCode
* [STM32 VS code extension](https://www.st.com/content/st_com/ja/campaigns/stm32-vs-code-extension-z11.html)
  
## Hardware
キットをベースに以下の改造を実施。
### 意図しないタイヤの回転を抑制
ソフトの書き換え中など、マイコンが起動していない際にタイヤが回転してしまうため、PWM端子を10kΩでプルダウンして電位を固定。

<img width="684" height="611" alt="image" src="https://github.com/user-attachments/assets/82094fe9-7a02-4378-ba7e-194b9c97ff3d" />

### バッテリー電圧の計測精度を向上
左端センサの反応にバッテリー電圧の計測結果が左右される問題があるため、マイコンの端子に0.1μFのコンデンサを追加。
<img width="738" height="588" alt="image" src="https://github.com/user-attachments/assets/fd129ee7-a396-4fd7-941c-6d260d8605aa" />

（おそらく、ADコンバータのサンプリングコンデンサの電荷がバッテリー電圧計測用分圧抵抗を通して充放電するのが間に合わず、計測結果が変わってしまうためだと思われます。）
 
# Usage
## ドライバ機能
### UISW
割り込み内でチャタリングを除去するとともにエッジを検出し、普通に押す場合、長押し、両押しのイベントを発行・キューイング。
メインルーチンでキューイングしたデータを取り出す。

### ブザー
メインルーチンで鳴動パターン(ON/OFFの時間)をキューイング。割り込み内でキューイングされた鳴動パターンに応じてブザーを制御。

### ライン・マーカーセンサー
割り込み内で各センサの値を取得。白黒を判定するとともに、機体中央を0としてラインが左ズレ時に正、右ズレ時に負となるズレ量をmmで計測。

### バッテリー電圧計測
割り込み内でバッテリー電圧のAD値を取得。フィルターしたうえで実電圧に公開するとともに、バッテリー状態を電圧で判断して公開する。

### モータ・ドライバ
モータとエンコーダを制御する。
（速度検出と速度フィードバックは未実装）

## デバッグ・テスト
usercodes/common/test_debug.hの_ENABLE_SAC_TEST_を有効にすることで、ドライバのデバッグ機能を有効化する。

### UISW
_ENABLE_UISW_TEST_を有効にすることで、スイッチ押下状態とキューイングされたイベントをテストできる。

### ブザー
_ENABLE_BUZZER_TEST_を有効にすることで、キューイングされた鳴動パターンがテストできる

### ライン・マーカーセンサー
_ENABLE_LINEMKR_TEST_を有効にすることで、ラインセンサーのズレ計測値と、ラインセンサー・マーカーセンサーの白黒値をテストできる。

### バッテリー電圧計測
_ENABLE_BATTERY_TEST_を有効にすることで、バッテリー電圧を計測し、バッテリーステータスをテストできる。

### モータ・ドライバ
_ENABLE_MOTOR_TEST_を有効にすることで、モータを回転させつつ、エンコーダの回転量を計測できる。

# License
IJKProject under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
 
