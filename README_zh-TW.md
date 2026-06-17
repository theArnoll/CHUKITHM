# 基於日系街機設計的觸控控制器

[English version](README.md) | 繁體中文版本

以 gemma4:e4b 模型翻譯自英文

## 目錄

- [基於日系街機設計的觸控控制器](#基於日系街機設計的觸控控制器)
  - [目錄](#目錄)
  - [物料清單 (BOM)](#物料清單-bom)
  - [開發平台 (Development platform)](#開發平台-development-platform)
    - [本專案使用 PlatformIO + Arduino C++ 程式碼開發](#本專案使用-platformio--arduino-c-程式碼開發)
  - [第三方函式庫 (Third Party Libraries)](#第三方函式庫-third-party-libraries)
  - [感測概念、PCB 設計與尺寸參考（日文）](#感測概念pcb-設計與尺寸參考日文)
  - [使用方法 (Usage)](#使用方法-usage)
    - [軟體先決條件 (Software Prerequisite)](#軟體先決條件-software-prerequisite)
    - [步驟 (Step)](#步驟-step)
    - [啟用鍵盤輸入 (Enabling Keyboard Input)](#啟用鍵盤輸入-enabling-keyboard-input)
  - [原型觸控板製作步驟 (Prototype Touchpad Making Step)](#原型觸控板製作步驟-prototype-touchpad-making-step)
  - [程式流程圖 (Program flow chart)](#程式流程圖-program-flow-chart)
    - [配置設定 (Config)](#配置設定-config)
    - [執行程式碼 `CY8C_code.ino`](#執行程式碼-cy8c_codeino)
  - [開發階段 (Development stage)](#開發階段-development-stage)
  - [AI 使用與署名 (AI Usage \& Attribution)](#ai-使用與署名-ai-usage--attribution)

## 物料清單 (BOM)
1. [PCB](./PCB/FormalPCB/Touchpad.kicad_pcb) × 2
   1. CY8CMBR3116 × 1
   2. 0.1uF 電容器 × 2
   3. 1uF 電容器 × 1
   4. 2.2nF X7R 電容器 × 1
   5. 4.7kΩ 電阻器 × 2
   6. 560Ω 電阻器 × 16
2. 紅外 LED (IR LED) × 6 `! 功能尚未實作`
3. ~IRM-3638~ TSSP58038 紅外接收器 (IR receiver) × 6 `! 功能尚未實作，可能會切換至 TSSP58038`
4. RP2040-Zero × 1 (下文簡稱 RP2040)
   1. 精密打孔排針腳座 9 pin × 2
   2. 精密打孔母座腳座 9 pin × 2
   3. 精密打孔排針腳座 5 pin × 1
   4. 精密打孔母座腳座 5 pin × 1
5. [壓克力外殼](./Case/Acrylic.dwg) (觸控板用，厚度 2mm) × 2
   1. [壓克力蓋](./Case/AcrylicCap.dwg) (覆蓋 CY8CMBR3116 用，厚度 2mm) × 1
   2. M2 螺絲（長度 10mm）× 6
      1. M2 六角螺母 × 6
      2. M2 座墊圈 (建議使用橡膠材質) × 6
   3. M3 螺絲 (≥9mm 長度) × 7
      1. M3 六角螺母 × 7
      2. M3 座墊圈 (建議使用橡膠材質) × 7
    *(總厚度計算：2mm 壓克力 + 1.6mm PCB = 5.6mm)*
    *(粗估尺寸：5.6mm + 1.6mm M2 螺母 + 0.5mm 橡膠座墊圈 = 7.7mm)*
    *(粗估尺寸：5.6mm + 2.4mm M3 螺母 + 0.5mm 橡膠座墊圈 = 8.5mm)*

<!-- TODO: 在實作紅外功能後，更新此處資訊 -->
<details>
<summary>使用測試用 PCB (Using test PCB)</summary>
1. 測試主機板 [motherboard type B](./PCB/TestPCB/TestMotherboardB.kicad_pcb) × 1
2. 測試子板 [childboard type B](./PCB/TestPCB/TestChildboardB.kicad_pcb) × 1
   1. CY8CMBR3116 w/ [DIP 轉接電路板](./PCB/CY8CMBR3116_PCB/CY8CMBR3116_PCB.kicad_pro) × 1 `! KiCad 專案使用 KiCad 版本 9 或 8 生成。`
   2. 0.1uF 電容器 × 2
   3. 1uF 電容器 × 1
   4. 2.2nF X7R 電容器 × 1
   5. 4.7kΩ 電阻器 × 2
   6. 560Ω 電阻器 × 16
3. 紅外 LED (IR LED) `! 功能尚未實作`
4. ~IRM-3638~ TSSP58038 紅外接收器 `! 功能尚未實作，可能會切換至 TSSP58038`
5. RP2040-Zero × 1
   (下文簡稱 RP2040)
</details>
<details>
<summary>使用手工原型觸控板 (Using handmade prototype touchpad)</summary>
1. CY8CMBR3116 w/ DIP 轉接電路板 × 1
   1. 0.1uF 電容器 × 2
   2. 1uF 電容器 × 1
   3. 2.2nF 電容器 × 1
   4. 4.7kΩ 電阻器 × 2
2. 電容式觸控板 (Capacitive touch pad) × 1 組
   1. 雙面導電銅箔 (製作教學見下文)
   2. 560Ω 電阻器 × 6/組 或 8/組 (取決於您選擇的模式)
3. 紅外 LED (IR LED) `! 功能尚未實作`
4. ~IRM-3638~ TSSP58038 紅外接收器 `! 功能尚未實作，可能會切換至 TSSP58038`
5. RP2040-Zero × 1
   (下文簡稱 RP2040)
</details>

## 開發平台 (Development platform)
### 本專案使用 PlatformIO + Arduino C++ 程式碼開發
<!-- TODO: 詳細說明 -->
1. VSCode -> Extensions，安裝 PlatformIO
2. 添加專案：本儲存庫中的每個專案資料夾（例如：[Code/IRcode](Code/IRcode)）

## 第三方函式庫 (Third Party Libraries)
非常感謝這些函式庫的作者們。
- [CypressCY8CMBR3116](https://github.com/sebastianregelmann/CypressCY8CMBR3116) 由 GitHub 提供的 [@sebastianregelmann](https://github.com/sebastianregelmann)：
    安裝此函式庫的方法：
  1. 將儲存庫下載為 `.zip` 檔案。
  2. 將 `.zip` 的名稱重新命名為 `CypressCY8CMBR3116.zip`。
  3. 開啟 Arduino IDE 2 > Sketch > Library > Include Library > Add .ZIP Library... (新增 .ZIP 函式庫...)
  4. 導航至該 .zip 檔案並選擇它來安裝函式庫。
  5. !重要！此版本需要編輯 `.cpp` 檔案才能正常運行。修復方法如下：
      1. 導航到第 699 行，即 `uint8_t CY8CMBR3116::activateSettings() {` 的位置。
      2. 在 `uint8_t error = calculateCRC();` 後方，新增一行並寫入 `delay(250);`。
      3. 在 `error = applyRegister();` 和 `error = resetIC();` 之後各新增一行，並寫入 `delay(200);`。
      這樣能增加設定被激活的機率（即使看起來一切正常）。

- [Arduino-Pico](https://github.com/earlephilhower/arduino-pico)：
  用於使用 Arduino IDE 和 Arduino 程式碼開發 RP2040-Zero 的環境。
  安裝板級支援：
  1. `Arduino IDE 2` > `Arduino IDE` > `Preference` (偏好設定) > `Additional boards manager URLs:` (額外板管理員 URL:)
     在結尾處新增以下內容：
     ```
     https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
     ```
  2. `Arduino IDE 2` > `Boards Manager` (板級管理員)
  搜尋 "RP2040"，安裝 "Raspberry Pi Pico/RP2040/RP2350" (作者：Earle F. Phithower, III)。
  - 安裝此板級支援時會一併安裝 Adafruit TinyUSB 函式庫。
  用於與電腦進行 USB 通訊的函式庫位於 [CY8C_code/NKRO_TinyUSB.h](./Codes/forCY8C/CY8C_code/NKRO_TinyUSB.h)。
  ***然而***，在燒錄 [CY8C_code.ino](./Codes/forCY8C/CY8C_code/CY8C_code.ino) 之前，請務必到 `Arduino IDE 2` > `Tools` (工具) > `USB Stack` (USB 堆疊) > `Adafruit Tiny USB` 來切換 USB 功能。

## 感測概念、PCB 設計與尺寸參考（日文）
[完整尺寸和資訊](https://mizucoffee.blogspot.com/2018/05/1-chunithm.html) (部落格，多頁面)
[詳細尺寸和感測方法](https://gist.github.com/mizucoffee/2f6263656d174fb2284a9e49c44bfabc)
[觸控板（接地滑動器）長度參考](https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q14172146159)
[感測方法討論](https://x.com/QmanEnobikto/status/2049293495903629316) (新的確認資訊)

## 使用方法 (Usage)
### 軟體先決條件 (Software Prerequisite)
- Arduino IDE (測試版本 v.2.3.8，適用於 Windows 11 及 macOS Tahoe)
  已安裝上方提及的第三方函式庫（可透過 .zip 或在 Arduino IDE 的函式庫管理員中安裝。請務必確保您安裝的是 "CypressCY8CMBR3116 by sebastianregelmann"）。

### 步驟 (Step)
1. 將您的 CY8CMBR3116 穩定地接到麵包板上：I²C SDA → Pin 0，I²C SCL → Pin 1。請務必使用 4.7kΩ 電阻器將這些引腳拉高（Pull up）。
2. 使用 USB 線將您的 RP2040 連接到電腦。
3. 上傳 [ConfigCode.ino](./CY8C_code/ConfigCode/ConfigCode.ino)
4. 上傳 [CY8C_code.ino](./CY8C_code/CY8C_code/CY8C_code.ino)
5. 開啟序列監控器 (Serial monitor)，並輕觸觸控板。

### 啟用鍵盤輸入 (Enabling Keyboard Input)
將 RP2040 的 Ground Pin 8 接上即可啟用鍵盤輸入功能。

## 原型觸控板製作步驟 (Prototype Touchpad Making Step)
準備一塊木板，列印下面圖片所示的圖案（確保圖中黑色線條的厚度為 8mm），然後用白膠將紙貼上去。
![./Images/MultipleButtonTest/MultipleButtonTest.png](./Images/MultipleButtonTest/MultipleButtonTest.png)
選擇一個設計模式進行測試。中間的桿子是觸控按鈕，外部圓圈是接地電極 (ground)。這款 6 個觸控按鈕的配置適合在 PC 上玩節奏遊戲。
決定了想要測試的圖案後，用 8mm 寬度的銅箔覆蓋黑色區域，並將電線焊接到以下腳位：
- 觸控按鈕 → CY8CMBR3116 的 CS0 ~ CS5，通過一個 560Ω 電阻器。 (如果選擇 8 個按鈕，則為 CS0 ~ CS7)
- 接地 (Ground) → RP2040 的 GND。
注意：觸控板和接地環之間的間隙是偵測到手指接觸的最大距離。

## 程式流程圖 (Program flow chart)
目前僅有中文版本可用。
### 配置設定 (Config)
![Images/FlowChart/Config_Code.svg](Images/FlowChart/Config_Code.svg)
### 執行程式碼 `CY8C_code.ino`
![Images/FlowChart/CY8C_code.svg](Images/FlowChart/CY8C_code.svg)
<!-- ![Images/FlowChart/CY8C_Code_setup.svg](Images/FlowChart/CY8C_Code_setup.svg)
![Images/FlowChart/CY8C_Code_loop.svg](Images/FlowChart/CY8C_Code_loop.svg) -->

## 開發階段 (Development stage)
- [X] 配置設定 (Configuration)
- [X] 開始感測和印出有意義的內容（≈ MVP）
- [X] 引入 `HID-Project.h`
- [X] 測試多個按鈕
- [X] 測試紅外代碼 (IR Code)
- [X] 改善延遲時間 → 延遲時間已從約 70~75ms 改善至約 40 ms。（2026 年 5 月 11 日更新，首次更新）
- [X] 完成紅外代碼 (IR Code) <!-- TODO: 待測試 -->
- [X] 列印和測試 PCB
- [X] 定義最終 PCB 設計
      將 CY8CMBR3116 IC 本身整合到 PCB 上，並將 RP2040 也納入 PCB。
      找出紅外感測連接至主板的解決方案。
- [ ] 確定外殼設計 (Finalize the case)
- [ ] 優化 README 和文件，並公開儲存庫 (Public the Repo)
   - [ ] 將照片和閉源圖片上傳到 Imgur
   - [ ] 在本專案或另一個儲存庫中記錄 CY8CMBR3116 的使用方法
   - [X] 公開儲存庫
   - [ ] 優化最終 README 文件

---
## AI 使用與署名 (AI Usage & Attribution)
本專案整合了 AI 輔助開發工具，以優化工作流程和效率：
* **程式碼自動完成 (Code Completion)**：使用 GitHub Copilot 和 Continue + `granite4:7b-a1b-h` (VS Code) 進行即時自動補全。
* **架構骨架生成 (Architectural Scaffolding)**：利用 `phi4:14b` 和 `phi4-reasoning:14b` 在硬體存取受限的遠端環境中，生成初步的範例程式碼和驗證邏輯。
* **資訊驗證 (Information Verification)**：
  * 使用 Google Gemini 進行概念性研究和文件查核（未將任何專案原始碼上傳至 Gemini 平台）。
  * 使用 Google AI Studio 中的 Gemini 和 Gemma 4 模型，並使用付費 API 金鑰確保資料隱私權。

所有由 AI 生成的程式碼都經過人工審閱、重構和測試，以確保邏輯完整性以及專案特定的需求要求。