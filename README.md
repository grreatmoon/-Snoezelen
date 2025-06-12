# スヌーズレン修理(傘)

このリポジトリは,LEDテープを用いたスヌーズレンのための傘玩具の修理/開発に用いたプログラムの保存を目的としたプロジェクトです

## 使用物
- ws2812b(LEDテープ)
- Arduino Leonardo(マイコン)
- FastLEDライブラリ(ws2812b制御用)
- バッテリー
- 基盤(自作)
- その他(傘,同線,スイッチなど)


## 注意点
- **Leonardoへの書き込み**: Leonardoへの書き込みの際は,Resetボタンを二回連続で押さないとPortが認識されないので注意が必要です
    [Arduino Leonardoへのガイド](https://trac.switch-science.com/wiki/Guide/ArduinoLeonardo)
- **Port表示**: Resetボタンを二連続で押した後,一定期間のみPortが表示されるので,その際に確認してください
- **ボード選択**: ボードマネージャー > Arduino AVR Boards > Arduino Leonardo
- **消費電力**: 明るさMaxでws2812bを光らせると,消費電力が大きすぎて電流が足りなくなる可能性があるので,明るさ制限推奨
