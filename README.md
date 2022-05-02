# The Stanley Parable Button Renamer
This is a mod for **The Stanley Parable: Ultra Deluxe** which modifies the "Jim" button such that it actually says your name.

## Installation
Download the latest version of the mod from Releases and extract all the files from the zip archive into the installation directory of **The Stanley Parable: Ultra Deluxe**, overwriting the file `The Stanley Parable Ultra Deluxe.exe`.

## Usage
By default, the mod will use the name of your Steam profile as the name the button says. If you want to choose a custom name yourself, you can do so by setting it in the `force_name` property inside `TSPBR_config.json`. When you change your profile name on Steam, you have to restart the Steam client first before the change is reflected in-game.

If the installation folder of the game is outside the installation folder of Steam itself, you have to specify the path to the Steam installation folder in the `steam_install` property inside `TSPBR_config.json` (e.g. *C:/Program Files (x86)/Steam*). ⚠️ **Make sure the specified Steam installation path uses forward slashes.** ⚠️

## Uninstall
To revert the game back to its original state, all you have to do is run the `TSPBR_uninstall.exe` executable, located in the game's installation folder.
