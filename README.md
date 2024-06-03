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

*******

<div id="Running"/>

## Running

<sup>(All paths must probably have a trailing '/' due to laziness)</sup>

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

There are two main files:
- main.tb: the first table/menu to be parsed
- color-icons: contains a path per line, where line [0] contains the path to the icon representing theme [0]

All data in the tables is contained in a simple CSV format, where each line represents an entry in the table. Their usual format is 

```
entry_name;type;number of currently selected theme;option for theme 0; option for theme 1;...
```

For now the parsing is very limited, whitespace is not ignored and there are no comments, however I might change it to json or something and implement better features

`type` can be one of these 5 (the literal strings):

### apply:

Means selecting this option immediately applies the theme to it. It is the simplest, and you can think of it as the base option.

Syntax: `entry_name;apply;number of currently selected theme;option for theme 0; option for theme 1;...`

### apply_list:

The same as above, except the options have many values instead of one.

I made this to allow applying many options together, with a single click on this entry.

Syntax: `entry_name;apply_list;number of currently selected theme;[blablabla;blablabla;...];[a;b;c];...`

In this case, for example, `a;b;c` are options that are all selected at once when clicking this entry with theme [1].

### sub:

Recurse into a subtable. The table is in a new file.

Syntax: `entry_name;sub`

A file `<data folder>/<entry_name>.tb` must exist, to be parsed into the new table.

### list:

Recurse into a list, where only 1 option can be applied at once.

Acts similarly to a subtable, however, like I said, only 1 option can be selected.

Syntax: `entry_name;list;number of currently selected theme;number of index selected inside the [list] (the list corresponding to the theme);[dsadsadsa;dsadas;dsa];[a;b;c];............`

### list_picture:

Exactly the same as above, but the entries are displayed as images.

I made this to allow, for example, previewing background images when selecting them.

Syntax: same as above, but `list_picture` instead of `list`


#### Empty entries:

To leave something blank, use ;; or ;[]; in the case of lists. Empty options cannot be applied.

*******

<div id="Example_usage">

## Example usage

The way I use this is just to store symlinks that point to the config files of a certain theme.

I use i3, so in the config file(s), I have to first execute the daemon, passing in the path do the data folder:

```
exec --no-startup-id $HOME/Desktop/Rofi-Themer/Scripts/daemon.sh $HOME/Desktop/Rofi-Themer/data/
```

I also bind Win+Menu to open up the menu:

```
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
