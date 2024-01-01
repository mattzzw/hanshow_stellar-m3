# Hacking away on the Hanshow Stellar-M3@ Electronic Shelf Label

I bought a used e-paper based electronic shelf label from the company "Hanshow" marked `MODEL: Stellar-M3@`. 

![front](pics/front.jpg)

![back](pics/back.jpg)

To open the device, remove the battery from the back, then slide an exacto knife or similar between the translucent display cover and the case. Be careful not to damage the display.

![opened](pics/case_opened.jpg)

Opening the device revealed the following:
  
- 3V supplied by 2 CR2450 cells (dead)  
- PCB version: `HS_EL5102-0_V2-4`
- MCU: TI `MSP430G2755` 
- Wireless chip: `A7106`
- SPI flash: `AH1819 25VQ21BT` - 2Mbit/256kB
- Display: 2,13" Hink-E0213A11-A1 212x104 - 
  - Not sure if this is the B version V3, red/black/white, called ["2.13 e-paper (B) V3"](https://www.waveshare.com/w/upload/d/d8/2.13inch_e-Paper_%28B%29_V3_Specification.pdf)
  - or if it's the D version, black/white, called ["2.13inch e-Paper (D)"](https://files.waveshare.com/upload/5/5b/2.13inch_e-Paper_%28D%29_Datasheet.pdf)
- Reed contact connected to P1.1 

![pcb top](pics/pcb_top.png)

## GPIO

| Port | Pin  | Connection |
|------|------|------------|
| P1.0 | 29   | EPD: Power (low active) |
| P1.1 | 30   | reed switch to gnd, BSL TX  |
| P1.2 | 31   | LED green |
| P1.3 | 32   | Radio: power |
| P1.4 | 33   | Radio: RFIO2 |
| P1.5 | 34   | Radio: RFIO1 |
| P1.6 | 35   | Radio: RFDA |
| P1.7 | 36   | Radio: RFCK |
| P2.0 | 6    | LED blue |
| P2.1 | 7    | CS pad backside|
| P2.2 | 8    | BSL RX |
| P2.3 | 27   | EPD: CLK |
| P2.4 | 28   | EPD: DIN |
| P2.5 | 40   | Radio: RFCS |
| P2.6 | 3    | ??? |
| P2.7 | 2    | ??? |
| P3.0 | 9    | SPI flash: SCLK |
| P3.1 | 10   | ?? I2C for unpopulated U4? |
| P3.2 | 11   | ?? I2C for unpopulated U4?|
| P3.3 | 12   | SPI flash: CSn | 
| P3.4 | 23   | SPI flash: SI / UART TX |
| P3.5 | 24   | SPI flash: SO |
| P3.6 | 25   | EPD: D/Cn |
| P3.7 | 26   | EPD: CSn |
| P4.0 | 15   | LED red  |
| P4.1 | 16   | DA pad backside |
| P4.2 | 17   | CK pad backside |
| P4.3 | 18   | ??? |
| P4.4 | 19   | ??? |
| P4.5 | 20   | EPD: BS |
| P4.6 | 21   | EPD: BUSYn |
| P4.7 | 22   | EPD: RSTn |


## MCU Connection via Bootstrap Loader (BSL)

### Physical connection

Using the MSP430 "JTAG" SBW (spy-by-wire) mode would only use `Test` and `nRST` but I don't have an adapter for this, yet.
So to get a connection to the MCU the uart mode of the BSL is used here. 
A couple of testpoints on the battery side come in handy. 

![pcb bottom](pics/pcb_back.jpg)

| Module | Location | RS-232 |
|--------|----|-------|
| `Vcc` | test point battery case | 3.3V |
| `GND` | test point battery case | GND   |
| `Test` | test point battery case | RTS | 
| `nRST` | test point batter case | DTR |
| BSL TX on `P1.1` | P1.1 is routed to the reed switch | RX |
| BSL RX on `P2.2` | P2.2 is routed to R17 which is unpopulated | TX |

I am using a CP2102 based USB to serial converter module.

### Software Access to the MCU

Install a recent version of `mspdebug` (0.25).

To enter BSL mode Test/nRST have to be toggled in a specific way. Refer to https://www.ti.com/lit/pdf/slau319. This can be controlled by mspdebug.

The only way to get access to flash memory is to execute a "mass erase" command, that will unlock the flash and delete all contents (including the calibration data of the internal clock unit as well as the ADC calibration).

Initiate mass erase (adjust serial port accordingly):

`mspdebug -d /dev/ttyUSB0 rom-bsl --bsl-entry-sequence Dr,R,r,R,r,d`

### Compile and upload code

- Download the TI MSP430-GCC from https://www.ti.com/tool/MSP430-GCC-OPENSOURCE#downloads
- Adjust `PROG_PORT` and `MSPDIR` in `src/Makefile`
- Execute `make`

The python script `reset_board.py` will toggle the DTR line to - well... reset the board to start the uploaded code.

## Progress so far
- [x] MCU enters SBL mode
- [x] mass erase via `mspdebug`
- [x] cross compiling code
- [x] upload and code execution
- [x] LED blinks in RGB :-)
- [x] Serial debug output on P3.4 to 2nd RS-232 USB module
- [x] Software UART on P1.1
- [x] Flash access
- [ ] EPD power up, boost conv running
- [ ] EPD access
- [ ] Wireless stuff


## Serial port

Using the built in UART to get some debugging output on P3.4. This pin is conveniently routed to pin 5 of the SPI flash where a micro clamp can be easily connected to a second USB to serial adapter module.

As the calibration data of the digital controlled oscillator (DCO) has been erased during the mass erase, the intneral clock will run slightly faster therefore configuring the UART needed some tweeking (see code).

However, to get access to the SPI flash and to potentially read out its contents over the serial interface a software UART is used on P1.1.

The retrieved flash image shows some graphics contents.

 ![flash contents](pics/flash.png)

# References

- https://github.com/CursedHardware/hanshow/blob/master/HS_EL5101.md
- https://github.com/osresearch/eink-pricetags
- https://www.ti.com/product/MSP430G2755 - Datasheet 
- https://www.ti.com/lit/pdf/slau144 - User's Guide
- https://www.ti.com/lit/pdf/slau319 - TI BSL User's Guide
