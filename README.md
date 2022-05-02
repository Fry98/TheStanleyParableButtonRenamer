# The Stanley Parable Button Renamer
This is a mod for **The Stanley Parable: Ultra Deluxe** which modifies the "Jim" button such that it actually says your name.

**Demo:** https://youtu.be/dHq9_lhJI8I

## Installation
Download the latest version of the mod from Releases and extract all the files from the zip archive into the installation directory of **The Stanley Parable: Ultra Deluxe**, overwriting the file `The Stanley Parable Ultra Deluxe.exe`.

## Usage
By default, the mod will use the name of your Steam profile as the name the button says. If you want to choose a custom name yourself, you can do so by setting it in the `force_name` property inside `TSPBR_config.json`. When you change your profile name on Steam, you have to restart the Steam client first before the change is reflected in-game.

If the installation folder of the game is outside the installation folder of Steam itself, you have to specify the path to the Steam installation folder in the `steam_install` property inside `TSPBR_config.json` (e.g. *C:/Program Files (x86)/Steam*). ⚠️ **Make sure the specified Steam installation path uses forward slashes.** ⚠️

## Uninstall
To revert the game back to its original state, all you have to do is run the `TSPBR_uninstall.exe` executable, located in the game's installation folder.

## How does it work?
The app extracts your profile name from the Steam installation folder and then uses the [voice.exe](https://www.elifulkerson.com/projects/commandline-text-to-speech.php) utility to create an audio clip of it using the OS's native text-to-speech system. That also means you can actually change the voice of the button by installing a different TTS voice on Windows. The resulting `.wav` file is then converted into an FMOD bank through the [fsbankcl.exe](https://www.fmod.com/download#fmodengine) CLI tool.

The app then locates the file `sharedassets21.resource` in the game's installation folder and appends the aforementioned FMOD bank to the end of that file. After that, the app has to modify the entry inside `sharedassets21.assets` pointing to the original sound bank and update it to point to the newly injected one. It does that by searching the file for the string **"ud_ncp2_jim_button"**, which is the name of the resource being played when the "Jim" button is pressed, and it then modifies the offset and the size specified in this entry to match our sound bank. The offset and the size are stored in the `.assets` file as little-endian uint64 numbers and are right next to each other exactly 62 bytes after the end of the resource name.

This whole process only happens on the first run or when the player's name changes. Information about the previous run are stored in the file `TSPBR_data/mdat.bin` and when the current player name matches the previously injected one, the app immediately launches the game without any modifications.
