# Rofi-Themer

## Interactive menus to pick themes/ dotfiles

Rofi-Themer makes it easy to achieve even higher ricing levels by enabling you to pick different parts of themes using interactive menus.

At its core, it is just a [rofi script](https://davatorium.github.io/rofi/1.7.5/rofi-script.5/), but was not done in shell due to its complexity.

| | |
| ---------------------- | ---------------------- |
| ![all_blue](https://github.com/IVSOP/Rofi-Themer/assets/108223142/a820697f-19f2-45a8-a84e-277be13240b6) | ![blue_red](https://github.com/IVSOP/Rofi-Themer/assets/108223142/87990464-cb70-4690-8311-b39177de9ddb) |
| ![all_red_rofi](https://github.com/IVSOP/Rofi-Themer/assets/108223142/58f00850-9b7f-41a6-8eb4-35617af82c21) | ![blue_purple](https://github.com/IVSOP/Rofi-Themer/assets/108223142/0646a6ff-31d7-4de0-ac81-26bd6f8589db) |


*******
- [Compiling](#Compiling)
- [Running](#Running)
- [Data format](#Data_format)
- [Example usage](#Example_usage)
*******

## Clone recursively
```bash
git clone --recurse-submodules git@github.com:IVSOP/Rofi-Themer.git
```

<div id="Compiling"/>

## Compiling

The script `build.sh` will configure CMake and build the code for you.

Three binaries are created in build/: Rofi-Themer-menu, Rofi-Themer-daemon and Rofi-Themer-read, however you do not need to run them directly.

To specify a location for the socket, pass the full path as an argument: `./build.sh <full path to socket>`

*******

<div id="Running"/>

## Running

<sup>(All paths must have a trailing '/' due to laziness)</sup>

Daemon - parses data and listens on the socket: `./Scripts/daemon.sh <dataset path>`

Menu - creates the menus themselves (feel free to edit it, it has my custom rofi command): `./Scripts/menu.sh`

Read - queries daemon for stored data: `./Scripts/read.sh <string>`. The string can be, for example:
- "dunst": reads the value contained there (Its type is apply)
- "rofi/theme": same, but reads the value from the subtable "rofi" (Its type is also apply)
- "rofi/network-manager": gets all values in network-manager, from subtable "rofi", separated by newline (Its type is apply_list)
- "polybar/*": gets every value contained in the subtable "polybar", recursively, separated by newline.

Please see [Data format](#Data_format) to better understand this behaviour and the meaning of the types


> :warning: Unfortunately rofi messes up passing arguments to the scripts, so some things had to be hardcoded, such as the path to the binaries inside the menu and read scripts, and the location of the socket, defined in every main. However, you can choose a new location for the socket by passing it to build.sh as an argument (use the full path, and name the socket).
> There is also currently no way to properly install this, so you can also have lots of fun editing the paths in the Scripts/ as you see fit.

*******

<div id="Data_format"/>

## Data format

The data files are stored as json. The first table is called `main.json` and is the only one that contains the color icons. Each of the other tables are on a separate file.

These tables have this format:

```json
"theme": "<theme name>", // theme currently applied to the table

"type": "table",

"data": { // everything inside this will be entries in the menu

	"<name of option>": {
		"type": "<type>", // see Data types below
		"theme": "<theme name>", // theme currently applied to this entry
		"selected": <number of selected option, starting at 0>, // only used in lists. see below
		"options": [
			["blabla", "blabla"],
			[], // there are no options for a theme, do this
			...
		]
	},

	..........
}
```


### Color icons

The color icons are stored in this way, **only** in the ma table:

And the color icons:
```json
"color-icons": {
	"<name1>": "icon1.png", // use full paths
	"<name2>": "icon2.png",
}
```

The order of the options will correspond to the order of these icons:

```json
...
options: [
	"option for theme <name1>"
	"option for theme <name2>"
]
```

## Data types

### apply:

Means selecting this option immediately applies the theme to it. It is the simplest type.

Example:

```json
"dunst": {
	"type": "apply",
	"theme": "blue",
	"options": [
		".config/i3/themes/dunst/0/dunstrc-0", // option for first theme
		".config/i3/themes/dunst/1/dunstrc-1", // option for second theme
		".config/i3/themes/dunst/2/dunstrc-2", // ...
		".config/i3/themes/dunst/3/dunstrc-3"
	]
}
```

### apply_list:

The same as above, except the options have many values instead of one.
```json
"network-manager": {
	"type": "apply_list",
	"theme": "blue",
	"options": [
		[".config/i3/themes/rofi/0/rofi-network-manager-0.conf", ".config/i3/themes/rofi/0/rofi-network-manager-0.rasi"],
		[".config/i3/themes/rofi/1/rofi-network-manager-1.conf", ".config/i3/themes/rofi/1/rofi-network-manager-1.rasi"],
		[],
		[".config/i3/themes/rofi/3/rofi-network-manager-3.conf", ".config/i3/themes/rofi/3/rofi-network-manager-3.rasi"]
	]
}
```

For example, instead of returning `something`, it would return
```
something1
something2
something3
...
```

### table:

Recurse into a new menu, from other table. The table is in a new file, called `<name>.json`.

These subtables are exactly the same as the main one, except without color icons.

On the table that 'includes' this one, you should write:

```json
"rofi": {
	"type": "table"
},
```

Which tells it to get the values from another file automatically.

### list:

Show the menu of a list. This differs from tables, as only 1 option is allowed to be selected.

See the example below.

### list_picture:

Exactly the same as above, but the entries are displayed as the icons, instead of showing the theme icon.

I made this to allow, for example, previewing background images when selecting them.

Example:
```json
"background": {
	"type": "list_picture",
	"theme": "blue",
	"selected": 0, // current active option will be options[theme_index][selected]
	"options": [
		["/home/ivsopi3/BG/TF22.jpg","/home/ivsopi3/BG/TF21.jpg"],
		["/home/ivsopi3/BG/KSP1.jpg","/home/ivsopi3/BG/KSP2.png","/home/ivsopi3/BG/KSP3.png"],
		[],
		["/home/ivsopi3/BG/BladeR3.jpg"]
	]
}
```

*******

<div id="Example_usage">

## Example usage

The way I use this is just to store symlinks that point to the config files of a certain theme.

I use i3, so in the config file(s), I have to first execute the daemon, passing in the path do the data folder:

```bash
#                        the script to lauch daemon                  data folder ending in /
exec --no-startup-id $HOME/Desktop/Rofi-Themer/Scripts/daemon.sh $HOME/Desktop/Rofi-Themer/data/
```

I also bind Win+Menu to open up the menu:

```bash
bindsym $mod+Menu exec --no-startup-id $HOME/Desktop/Rofi-Themer/Scripts/menu.sh
```

Lastly, when i3 powers up, I have many scripts getting the values and setting the symlinks.

For example, dunst:
```bash
#!/bin/bash

cd $HOME/.config/dunst

# get path from themer
DUNSTRC_PATH=$($HOME/Desktop/Rofi-Themer/Scripts/read.sh "dunst")

# check error
if [[ -z  $DUNSTRC_PATH ]]
then
	echo "Error"
else
	# set link
    ln -sfT $HOME/$DUNSTRC_PATH dunstrc # &>> /tmp/dunst_theme_script.txt
fi
```

This script reads the value of "dunst", then sets a symlink to it (first placing $HOME at the start)

You might have noticed I check if the value is empty. This is because there can be unexpected errors or, since this is being run right at the start, the daemon might not even be started and listening on the socket yet. You can probably solve that last problem in many different ways, but for now I just do nothing.

A more complex example, involving reading many values, is rofi itself:
```bash
#!/bin/bash

cd $HOME/.config/rofi/current-theme

mapfile -t ROFI_PATHS < <($HOME/Desktop/Rofi-Themer/Scripts/read.sh "rofi/*")

LINK_NAMES=( "dmenu-theme.rasi" "menu-theme.rasi" "network-manager.conf" "network-manager.rasi" "theme.rasi" )

if [ ${#ROFI_PATHS[@]} -eq ${#LINK_NAMES[@]} ] # check if correct number of data was received
then
	for (( i=0; i<${#ROFI_PATHS[@]}; i++ ))
	do
	   if ! [[ -z ${ROFI_PATHS[$i]} ]]
	   then
	       ln -sfT $HOME/${ROFI_PATHS[$i]} ${LINK_NAMES[$i]} # &>> /tmp/rofi_theme_script.txt
	   fi
	done
fi
```

This basically just uses mapfile to get all the values and place them in an array. The length is checked to detect errors, like in the first script.

You can execute these scripts automatically when i3 starts up, for example:
```bash
exec_always --no-startup-id $HOME/.config/rofi/current-theme/get-theme.sh
```

*******

This project is a C++ rewrite of [I-Themer](https://github.com/IVSOP/I-Themer), due to it being too much of a mess, and also has some more features.

After making this I found out about gnu stow, which does something very similar. In the future I might try to use it with this.
