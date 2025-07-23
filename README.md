# Garage Clicker

I cannibalized the remote I had for the garage door opener in my car to be able to trigger a relay and press a button via MQTT. It is annoying to refresh Home Assistant as soon as I cross the threshold of my home network, so I decided to put a discrete clicker in my car again instead of using the app.

This project utilizes 2 esp32 boards -- one for the base and one for the remote -- with LoRa modules for reliable, low-bandwith communications. On receiving a valid code from the trigger,  the base node updates the MQTT listeners with the new code. This is very rudamentary since HomeAssistant is able to trigger an automation on a state change.

The code is determined and validated by using pseudorandom generators. The logic is similar to a rolling car-key code. The base station has a queue of valid codes to receive. The base starts with the same seed as the trigger, and they share the same pseudorandom generation algorithm. Thus, it will be incredibly difficult to generate valid codes without knowing the starting random state and the algorithm. This is NOT a secure implementation; I figured there are easier ways for someone to break into my garage than to crack this, so I did not put that much effort into it. 

## TODO:
  - [ ] state on the trigger needs to be stored in non-volatile memory; on restarting the car the state resets and both devices lose sync at this point
  - [ ] clean up the code -- too much stuff is in the main file
  - [ ] make a better random algorithm; I should be outputting a 32bit signed int; I am outputting an 8-bit signed int right now.
  - [ ] abstract pin definitions -- led status lights are hardcoded right now. 
  - [ ] add an option to use the internal antenna on seeed xiao esp32c6 -- cannot assume all users have additional antennae.
