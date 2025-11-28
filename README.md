# cmsc131_st_firesentinel

Quick Start Instructions
Connect the hardware:
  -  Plug the Arduino Uno into your laptop with a USB cable.
  -  Plug the 12V water pump into its designated power supply and connect it to the relay module.
  -  Plug a separate power supply for the servo motors and GSM module. Make sure all grounds are common.
  -  Confirm there are no loose wires in any connections.

Prepare and position the system:
  -  Place the FireSentinel apparatus on a stable, flat surface with unobstructed frontal space.
  -  Insert a filled water reservoir for the pump.
  -  Put a valid SIM card into the GSM module, ensuring good signal.

Upload code:
  -  Download the project code from this repository.
  -  Upload the Arduino .ino sketch using the Arduino IDE.
  -  Update PHONE_NUMBER and calibration settings in the code if needed.

Test the detection:
  -  Place a fire source (e.g., small candle) anywhere within the 180° scanning angle of the IR sensor.
  -  The system will sweep to detect the flame, aim the nozzle, send an SMS alert, and spray water to extinguish the fire.

Safety Notes
  -  Only test with small flames in controlled environments.
  -  Ensure water and electronics are isolated to avoid short-circuits.
  -  Monitor connections during operation; never leave unattended during burns.
