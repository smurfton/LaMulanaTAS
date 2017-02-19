# Installation

Pending creation of an installer, copy LaMulanaTAS.dll to your 1.3.3.1 installation directory, for instance `C:\GOG Games\La-Mulana`, and apply TAS.bps to LaMulanaWin.exe to create a LaMulanaTAS.exe using Floating IPS.  You can acquire Floating IPS from SMW Central or try to build the source from https://github.com/Alcaro/Flips (quite the adventure).

# Use

## Usage notes

Using fast forward turns off vsync for the rest of that session and the game's frame rate limiter is very bad at keeping a steady frame rate without vsync.  To record accurate video, do not use this function at any point in the session you are recording.

For TASing it can be important to know the order of operations in a frame (e.g. seeing hitbox intersections a frame before damage is dealt), so the game's main loop goes approximately like this:

1. Read input (TAS command and input processing)
2. Pre-collision logic
3. Draw (TAS overlay)
4. Calculate intersections + accumulate damage
5. Post-collision logic
6. Sleep

The intersections of the lines on Lemeza shown with L are the points tested for tile collision.  Notably dynamic collision boxes test again the middle of Lemeza, not his left or right.  It hasn't been verified that this line is the actual coordinate tested for dynamic collision boxes, all the offsets are from the function for tile collision.

Saves and time attack records go to the `tsav` and `tasrec` directories alongside the `save` and `record` for the unmodded game.

## Key bindings (not configurable):

Key | Action
:---:|:---
R | Reload script in place
T | Reset game (imperfect) and restart TAS script from beginning
O | Toggle main overlay
P | Fast forward (also disables vsync for the rest of the session)
[ | Normal speed
] | Frame advance
1 | Toggle Lemeza's hitbox
2 | Toggle Lemeza's weapon hurtboxes
3 | Toggle Lemeza's shield hitbox
4 | Toggle enemy hitboxes
5 | Toggle enemy hurtboxes
6 | Toggle blockable projectile hurtboxes
7 | Toggle enemy shield hitboxes
8 | Toggle omnidirectional scaling damage source hurtboxes
9 | Toggle unidirectional scaling damage source hurtboxes
0 | Toggle item hitboxes
- | Toggle dynamic collision boxes (separate system from hitboxes)
= | Toggle tile collision data
K | Toggle screen exit display
L | Toggle visual indication of Lemeza's location

# Scripting

The TAS script goes in the game installation directory and must be named `script.txt`.  For now having a script is mandatory even if it's an empty file.

It'd be nice if this was documented.  Probably only needs sparse documentation here covering non-obvious things like order of operations and a sample script.txt in the distribution.

# Legal

See the LICENSE file.
