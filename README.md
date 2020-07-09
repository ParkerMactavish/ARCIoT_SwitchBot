# ARCIoT_SwitchBot
Repo for Arc IoT contest containing the server and app of the system and the test program for BLE and WiFi on arc OSP.

## 簡介
現在市場上，有許多成熟的智慧家電方案，但使用時需要更換整套系統，不管是替換整組開關或家電，都需要不少的花費，且更換後的傳統電器也會造成環境汙染；
因此在此提案中，我們隊伍想透過外部控制的方式，將舊式傳統家電變換成智慧家電，以Arc IoTDK作為整套系統的主控模組，完成語音辨識以及分析的工作，
並且將相應的指令傳送到指定子機，同時利用安設在按鈕開關或旋鈕上的子機<br/>接收指令並且動作，以達成相應的家電控制；
另外，如果不在空間內無法使用語音進行控制，則也可以選擇使用手機頁面對Arc IoTDK<br/>主控模組下達指令，達成遠距操控的工作；
希望透過非侵入式智慧家電系統，讓更多人享受到現代物聯網所帶來的便利。
## 系統架構
<img src="https://github.com/LittleLaGi/ARCIoT_SwitchBot/blob/master/brief_architecture.PNG" width="90%" />

## 硬體架構


## 軟體架構
#### 透過app控制裝置
原本希望利用MQTT協定完成app和IOTDK之間的溝通，但是目前在IOTDK上的mqtt client架設遇到一些問題，
因此只有app和伺服器之間的溝通是用MQTT，IOTDK和伺服器之間則是利用HTTP協定溝通，
透過不停地像伺服器發送GET請求來取得app發送的指令。<br/>
###### MQTT封包格式
    {action:'reg', target:'Arduino-01'} //註冊ID為"Arduino-01"的裝置
    {action:'trig', target:'Arduino-01'} //觸發ID為"Arduino-01"的裝置
    {action:'del', target:'Arduino-01'} //刪除ID為"Arduino-01"的裝置
###### HTTP回傳狀態
     200: 成功取得app指令
     204: app並沒有發送指令
    


## 使用說明
#### Arc App - 裝置控制
<img src="https://github.com/LittleLaGi/ARCIoT_SwitchBot/blob/master/Arc_app.jpg" width="20%" /> <img src="https://github.com/LittleLaGi/ARCIoT_SwitchBot/blob/master/Arc_app_reg.jpg" width="20%" /> <img src="https://github.com/LittleLaGi/ARCIoT_SwitchBot/blob/master/Arc_app_del.jpg" width="20%" />

1. 在app首頁可以看到已註冊的裝置。
2. 點擊"新裝置"按鈕，可以輸入裝置ID進行註冊。
3. 短按為觸發裝置，長按為刪除裝置。
#### Arc App - wifi設定



1. 點擊"IOTDK連線"按鈕，可以搜尋藍芽裝置。
2. 點擊搜尋到的藍芽名稱，可以進行連線。
3. 連線成功後，點擊"WIFI設定"，可以輸入WIFI名稱以及密碼。
4. 點擊"確認"，將WIFI設定傳送給IOTDK。
