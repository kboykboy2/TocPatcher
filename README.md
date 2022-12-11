# TocPatcher
 This is a tool to patch utocs for games using signing. There are bypasses for the .sig files themselves.
 however even if they are bypassed if the game uses IoStore, then the mod still wont work cause the .utoc's contain an signature within them.
 It works by merging two toc files, A custom one generated for a mod, And one from the game.
 It does not yet support encrypted toc files. It may not work in every circumstance, However it should work most of the time.
 Raise an issue if you run into any bugs glitches or other issues like that, Cause you will.
 this is not a user friendly tool, and is meant to be used by experianced modders.

# Usage
 copy a .utoc from the game and replace "Master.utoc" with it. make sure the toc is larger than the target toc.

 now you can run "TocPatcher.exe" and it will tell you the cooking settings to need to set
 then once those settings are set, cook your mod then drag and drop the target toc(your mods .utoc) on "TocPatcher.exe"
 if it was successful it will say so, if it just closes with no message then it crashed.

# Acknowledgements 
 The source code for UEViewer was a massive help for this, Thanks Gildor.
 https://github.com/gildor2/UEViewer

 As was the utoc format description here.
 https://github.com/gitMenv/UEcastoc
