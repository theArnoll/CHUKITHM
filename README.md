# A touch controller based on a Japanese Arcade

English version | [繁體中文版本 (Traditional Chinese version)](README_zh-TW.md)

## Table of Contents

- [A touch controller based on a Japanese Arcade](#a-touch-controller-based-on-a-japanese-arcade)
  - [Table of Contents](#table-of-contents)
  - [BOM](#bom)
  - [Development platform](#development-platform)
    - [This project is develop with PlatformIO + Arduino C++ code](#this-project-is-develop-with-platformio--arduino-c-code)
  - [Third Party Libraries](#third-party-libraries)
  - [Sensing Concepts, PCB Design, and Dimensional References (Japanese)](#sensing-concepts-pcb-design-and-dimensional-references-japanese)
  - [Usage](#usage)
    - [Software Prerequisite](#software-prerequisite)
    - [Step](#step)
    - [Enabling Keyboard Input](#enabling-keyboard-input)
  - [Prototype Touchpad Making Step](#prototype-touchpad-making-step)
  - [Program flow chart](#program-flow-chart)
    - [Config](#config)
    - [Running code (`CY8C_code.ino`)](#running-code-cy8c_codeino)
  - [Development stage](#development-stage)
  - [AI Usage \& Attribution](#ai-usage--attribution)

## BOM

<details>

<summary>Using test PCB</summary>

1. Testing PCB [motherboard type B](./PCB/TestPCB/TestMotherboardB.kicad_pcb) × 1
1. Testing PCB [childboard type B](./PCB/TestPCB/TestChildboardB.kicad_pcb) × 1
   1. CY8CMBR3116 w/ [DIP convertion board](./PCB/CY8CMBR3116_PCB/CY8CMBR3116_PCB.kicad_pro) × 1 `! The KiCad project is created with KiCad version 9 or 8.`
   2. 0.1uF × 2
   3. 1uF × 1
   4. 2.2nF X7R × 1
   5. 4.7kΩ × 2
   6. 560Ω × 16
2. IR LED `! Function haven't implented yet`
3. ~IRM-3638~ TSSP58038 IR reciever `! Function haven't implented yet, likely to switch to TSSP58038`
4. RP2040-Zero × 1
   (Refer as RP2040 below)

</details>

<details>

<summary>Using prototype touchpad</summary>

1. CY8CMBR3116 w/ DIP convertion board × 1
   1. 0.1uF × 2
   2. 1uF × 1
   3. 2.2nF × 1
   4. 4.7kΩ × 2
2. Capacitive touch pad × 1 set
   1. Double side conductive copper tape (making how-to look below)
   2. 560Ω × 6/set or 8/set according to [which you choose](#prototype-touchpad-making-step)
3. IR LED `! Function haven't implented yet`
4. ~IRM-3638~ TSSP58038 IR reciever `! Function haven't implented yet, likely to switch to TSSP58038`
5. RP2040-Zero × 1
   (Refer as RP2040 below)

</details>

## Development platform

### This project is develop with PlatformIO + Arduino C++ code

<!-- TODO: explain it more -->

1. VSCode -> Extensions, install PlatformIO
2. Add project: each project folder in this Repo <!-- for ex: [Code/IRcode](Code/IRcode) -->

## Third Party Libraries

Big thank to the author(s) of these libraries

- [CypressCY8CMBR3116](https://github.com/sebastianregelmann/CypressCY8CMBR3116) by GitHub [@sebastianregelmann](https://github.com/sebastianregelmann)
    The way installing the library:
  1. Download the repo as `.zip`
  2. Rename the `.zip` as `CypressCY8CMBR3116.zip`
  3. Arduino IDE 2 > Sketch > Library > Include Library > Add .ZIP Libray...
  4. Navigate to the .zip and select it to install the library.
  5. !IMPORTANT! Current version of this library need to edit the .cpp file to be able to run correctly. Here's how to fix:
      1. Navigate to line 699, where `uint8_t CY8CMBR3116::activateSettings() {` is at
      2. After `uint8_t error = calculateCRC();`, add a line and write `delay(250);`.
      3. After `error = applyRegister();` and `error = resetIC();`, add a line and write `delay(200);`.

      This will increase the possibility of the setting being activated if nothing seem to go wrong.

- [Arduino-Pico](https://github.com/earlephilhower/arduino-pico)
  For developing RP2040-Zero with Arduino IDE and Arduino code.  
  Install the board:
  1. `Arduino IDE 2` > `Arduino IDE` > `Preference` > `Additional boards manager URLs:`
   Add below in the end:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
  2. `Arduino IDE 2` > `Boards Manager`
  Search "RP2040", install "Raspberry Pi Pico/RP2040/RP2350" by "Earle F. Phithower, III".
  - Adafruit TinyUSB library is going to install along installing this board.  
  The library for USB communication with the computer in [CY8C_code/NKRO_TinyUSB.h](./Codes/forCY8C/CY8C_code/NKRO_TinyUSB.h).  
  ***HOWEVER***, before flashing [CY8C_code.ino](./Codes/forCY8C/CY8C_code/CY8C_code.ino), please go to `Arduino IDE 2` > `Tools` > `USB Stack` > `Adafruit Tiny USB` to switch USB function.

## Sensing Concepts, PCB Design, and Dimensional References (Japanese)

[Full dimensions and informations](https://mizucoffee.blogspot.com/2018/05/1-chunithm.html) (Blog, multiple pages)  
[Detailed dimensions and sensing method](https://gist.github.com/mizucoffee/2f6263656d174fb2284a9e49c44bfabc)  
[Touch panel (ground slider) length](https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q14172146159)  
[Sensing method](https://x.com/QmanEnobikto/status/2049293495903629316) (Newer information as confirmation)  


## Usage

### Software Prerequisite

- Arduino IDE (Tested v.2.3.8 on Windows 11 & macOS Tahoe)
  Have the third party library mentioned above installed (either install via .zip or install in the Library Manager in Arduino IDE and make sure you're installing "CypressCY8CMBR3116 by sebastianregelmann".)

### Step

1. Wire your CY8CMBR3116 well on breadboard. I²C SDA → Pin 0, I²C SCL → Pin 1. Make sure you've pulled these pins up with 4.7kΩ resistor.
2. Connect your RP2040 to your computer with a USB cable.
3. Upload [ConfigCode.ino](./CY8C_code/ConfigCode/ConfigCode.ino)
4. Upload [CY8C_code.ino](./CY8C_code/CY8C_code/CY8C_code.ino)
5. Open serial monitor and tap the touch pad.

### Enabling Keyboard Input

Ground Pin 8 on your RP2040 to enable keyboard input.

## Prototype Touchpad Making Step

Prepare a wooden board, print the image below that need to ensure the thickness of black line in the image are 8mm, and paste the paper with white glue.

![./Images/MultipleButtonTest/MultipleButtonTest.png](./Images/MultipleButtonTest/MultipleButtonTest.png)

Choose one of the designs to test the result. The sticks in the middle are touch buttons, and the outter ring is ground. The 6 touch buttons one is good for playing rhythm games on PC.

After you decided the pattern you want to test, cover the black area with 8mm width copper tape and solder wires to these to pins:

- Touch buttons → CS0 ~ CS5 on CY8CMBR3116 via a 560Ω resistor. (CS0 ~ CS7 if you choose 8 buttons one)
- Ground → GND on RP2040

Beware that the space between touch pad and ground ring is the maximum distance between your finger and the touch pad that can be detected as touched.

## Program flow chart

Only Chinese version is available at the time.

### Config

![Images/FlowChart/Config_Code.svg](Images/FlowChart/Config_Code.svg)

### Running code (`CY8C_code.ino`)

![Images/FlowChart/CY8C_code.svg](Images/FlowChart/CY8C_code.svg)
<!-- ![Images/FlowChart/CY8C_Code_setup.svg](Images/FlowChart/CY8C_Code_setup.svg)
![Images/FlowChart/CY8C_Code_loop.svg](Images/FlowChart/CY8C_Code_loop.svg) -->

## Development stage

- [X] Configuration
- [X] Start sensing and print something that makes some sort of sense (≈ MVP)
- [X] Introduce `HID-Project.h`
- [X] Test multiple buttons
- [X] Test IR Code
- [X] Improve latency → Latency is now improved from around 70~75ms to around 40 ms. (Updated 2026 May 11, 1st update)
- [X] Finish IR Code <!-- TODO: To be test -->
- [X] Print and test PCB
- [X] Finalize PCB design
      Integrating CY8CMBR3116 the IC itself to the PCB and include RP2040 to the PCB itself.
      Find the solution of IR sensing connection to the motherboard.
- [ ] Finalize the case
- [ ] Refine the README and documents and Public the Repo
   - [ ] Upload photos and close source pictures to Imgur
   - [ ] Document the usage of CY8CMBR3116 in the same or another repo
   - [X] Public the repo
   - [ ] Refine the final README

---

## AI Usage & Attribution

This project incorporates AI-assisted development tools to optimize workflow and efficiency:

* **Code Completion**: GitHub Copilot and Continue + `granite4:7b-a1b-h` (VS Code) were used for real-time autocompletion.
* **Architectural Scaffolding**: Leveraged `phi4:14b` and `phi4-reasoning:14b` for generating initial boilerplate code and verifying logic in remote environments where hardware access was limited.
* **Information Verification**:
  * Google Gemini was utilized for conceptual research and documentation verification (no project source code was uploaded to Gemini platform)
  * Gemini models and Gemma 4 models on Google AI Studio is used, with paid API key used to ensure legal data privacy.

All AI-generated code has been manually reviewed, refactored, and tested to ensure logic integrity and project-specific requirements.