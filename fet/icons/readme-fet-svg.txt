Begin: 5 March 2020.

Last modified on: 5 March 2020.

Instructions by Liviu Lalescu about the file fet.svg:

This file was created a long time ago in an older version of Inkscape (0.48.4). I cannot update it to the latest Inkscape version (currently 0.92.4)
because it does not open/save nicely, and because it might raise (minor) cosmetic changes to the current icon of FET.
fet.svg still remains a usable file, but you need to:

1) If you do not have it, you need to install the font "URW Chancery L" or "URW Chancery L Medium Italic" - I am not sure which is the exact name.
On my openSUSE GNU/Linux it seems to be present by default, but it seems that the latest Inkscape version does not accept the default provided
implementation (which seems to be in an old format). But I could download freely the font from the internet, in the ttf form, installed it,
and Inkscape saw it.

2) You need to edit the file fet.svg with a text or XML editor, replace the string "font-style:italic" (without quotes) with
the string "font-style:normal" (again, without quotes), in the two places in which it appears.

After these two steps you should be able to open and use the file with the latest Inkscape version. You'll still have some things when you open
the file (Inkscape will ask conversion questions), but the file should be usable. Unfortunately, when I save the file using Inkscape 0.92.4 and
open it again, the position of the items is incorrect - I need to move them.
