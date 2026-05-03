# A touch controller based on a Japanese Arcade

## BOM

1. CY8CMBR3116 w/ DIP convertion board x 1
   1. 0.1uF x 2
   2. 1uF x 1
   3. 2.2nF x 1
   4. 4.7kΩ x 2
2. Capacitive touch pad x 1 set (for now)
   1. Double side conductive copper tape (making how-to look below)
   2. 560Ω x 1 / set
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
- HID-Project

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

Make a pattern like below with conductive paint or copper tape:

```
┌───────┐
│  ───  │
└───────┘
```

The inner line is the touch point, and the outter ring is ground. Solder wires to these to pins:

- Touch pad → CS0 on CY8CMBR3116
- Ground → GND on Arduino

Beware that the space between touch pad and ground ring is the maximum distance between your finger and the touch pad that can be detected as touched.

## Development stage

### Finished

- Configuration
- Start sensing and print something that makes some sort of sense (≈ MVP)
- Introduce `HID-Project.h`

### To do

- Test multiple buttons

## AI Usage & Attribution

This project incorporates AI-assisted development tools to optimize workflow and efficiency:

* **Code Completion**: GitHub Copilot and Continue (VS Code) were used for real-time autocompletion.
* **Architectural Scaffolding**: Leveraged `phi4:14b` and `phi4-reasoning:14b` for generating initial boilerplate code and verifying logic in remote environments where hardware access was limited.
* **Information Verification**: Google Gemini was utilized for conceptual research and documentation verification (no project source code was uploaded to this service).

All AI-generated code has been manually reviewed, refactored, and tested to ensure logic integrity and project-specific requirements.