# A touch controller based on a Japanese Arcade

## BOM

1. CY8CMBR3116 w/ DIP convertion board x 1
   1. 0.1uF x 2
   2. 1uF x 1
   3. 2.2nF x 1
   4. 4.7kΩ x 2
2. Capacitive touch pad x 1 set (for now)
   1. Double side conductive copper tape (making how-to look below)
   2. 560Ω x 6 / set
3. IR LED
4. IRM-3638 IR reciever
5. Arduino Pro Micro (or Micro / Leonardo) x 1
   (Refer as "Arduino" below)

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

- HID-Project

## Sensing method idea and PCB and dimensions ref (Japanese)

[Full dimensions and informations](https://mizucoffee.blogspot.com/2018/05/1-chunithm.html) (Blog, multiple pages)  
[Detailed dimensions and sensing method](https://gist.github.com/mizucoffee/2f6263656d174fb2284a9e49c44bfabc)  
[Touch panel (ground slider) length](https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q14172146159)  
[Sensing method](https://x.com/QmanEnobikto/status/2049293495903629316) (Newer information as confirmation)  


## Usage

### Software Prerequisite

- Arduino IDE (Tested v.2.3.8)
  Have the third party library mentioned above installed (either install via .zip or install in the Library Manager in Arduino IDE and make sure you're installing "CypressCY8CMBR3116 by sebastianregelmann".)

### Step

1. Wire your CY8CMBR3116 well on breadboard. I²C SDA → Pin D2, I²C SCL → Pin D3. Make sure you've pulled these pins up with 4.7kΩ resistor.
2. Connect your Arduino to PC via USB cable.
3. Upload [ConfigCode.ino](./CY8C_code/ConfigCode/ConfigCode.ino)
4. Upload [CY8C_code.ino](./CY8C_code/CY8C_code/CY8C_code.ino)
5. Open serial monitor and tap the touch pad.

### Enabling Keyboard Input

Connect Pin 6 on your Arduino to enable keyboard input.

## Touchpad Making Step

Prepare a wooden board, print the image below that need to ensure the thickness of black line in the image are 8mm, and paste the paper with white glue.

![./Images/MultipleButtonTest/MultipleButtonTest.png](./Images/MultipleButtonTest/MultipleButtonTest.png)

Choose one of the designs to test the result. The sticks in the middle are touch buttons, and the outter ring is ground. The 6 touch buttons is good for playing rhythm games on PC.

After you decided the pattern you want to test, cover the black area with 8mm width copper tape and solder wires to these to pins:

- Touch buttons → CS0 ~ CS5 on CY8CMBR3116 via a 560Ω resistor. (CS0 ~ CS7 if you choose 8 buttons one)
- Ground → GND on Arduino

Beware that the space between touch pad and ground ring is the maximum distance between your finger and the touch pad that can be detected as touched.

## Program flow chart

Only Chinese version is available at the time.

### Config

![Images/FlowChart/Config_Code.svg](Images/FlowChart/Config_Code.svg)

### Running code (`CY8C_code.ino`)

![Images/FlowChart/CY8C_Code_setup.svg](Images/FlowChart/CY8C_Code_setup.svg)
![Images/FlowChart/CY8C_Code_loop.svg](Images/FlowChart/CY8C_Code_loop.svg)

## Development stage

- [X] Configuration
- [X] Start sensing and print something that makes some sort of sense (≈ MVP)
- [X] Introduce `HID-Project.h`
- [X] Test multiple buttons
- [X] Test IR Code
- [ ] Improve latency → Latency is now improved from around 70~75ms to around 40 ms. (Updated 2026 May 11, 1st update)
- [ ] Finish IR Code
- [X] Print and test PCB
- [ ] Finalize PCB design
      Integrating CY8CMBR3116 the IC itself to the PCB and include Arduino Pro Micro to the PCB itself.
      Find the solution of IR sensing connection to the motherboard.
- [ ] Finalize the case
- [ ] Refine the README and documents and Public the Repo
   - [ ] Upload photos and closesource pictures to Imgur
   - [ ] Document the usage of CY8CMBR3116 in the same or another repo
   - [ ] Public the repo
   - [ ] Refine the final README

---

## AI Usage & Attribution

This project incorporates AI-assisted development tools to optimize workflow and efficiency:

* **Code Completion**: GitHub Copilot and Continue (VS Code) were used for real-time autocompletion.
* **Architectural Scaffolding**: Leveraged `phi4:14b` and `phi4-reasoning:14b` for generating initial boilerplate code and verifying logic in remote environments where hardware access was limited.
* **Information Verification**: Google Gemini was utilized for conceptual research and documentation verification (no project source code was uploaded to this service).

All AI-generated code has been manually reviewed, refactored, and tested to ensure logic integrity and project-specific requirements.