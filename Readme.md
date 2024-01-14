# Hacking away on Hanshow Electronic Shelf Labels (based on the TI MSP430G2775 MCU)

This is about getting you own baremetal code running on two very similar e-paper based elctronic shelf labels (ESLs):

- [Hanshow Stallar M3@](Readme-m3.md) - This is where it started, using serial Boot Strap Loader access.

![M3](pics/front.jpg)

- [Hanshow Stellar S3T@](Readme-s3t.md) - Based on the findings above, using spy-bi-wire/JTAG access.

![S3T](pics/st3-front.jpg)


## Open points / unclear

- Haven't touched the radio yet
- Initialization of epd needs `0xf7` for intitial power-up in display update control 2 command (`0x22`). This is only necessary after the display was powered off for alonger period of time. However, most other epd code sources rely on `0xc4` or `0xc7` (which also works on the succeeding updates).
- ST3 display with 200x200 would need 5kB for framebuffer operation, which is obviously not going to work
- Using Spy-bi-wire as UART backchannel - Don't know if that is possible at all


