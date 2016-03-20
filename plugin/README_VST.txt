== Installation ==
To install the DrumGizmo vst plugin, follow these simple steps:
1. Copy “drumgizmo_vst.dll” into the folder containing your VST plugins.
2. Done!

When opening your VST host software (Cubase or the like), the plugin
should now show up as a VST Instrument.

== How to use it ==
1.  Load the plugin as a VST Instrument.
2.  Now create a midi track to use with the plugin.
2a. (Optional) Choose “GM map” to use with the midi track.
3.  Select DrumGizmo as midi output for the track.
4.  Now open up the DrumGizmo VST interface.
5.  Load a drumkit by clicking on the "Load kit..." button and browse for
    the drumkit xml file. (Example: “C:\drumkit\drumkit.xml”).
    When the “led” to the right of the line edit turns green, it means
    that the drumkit has succesfully begun loading in the background. It
    might take a while before all drums are loaded, so be patient.
8.  Now click on the “Load map...” button and browse for the midimap xml
    file. (Example: “C:\drumkit\midimap.xml”). When the “led” to the
    right of the line edit turns green, it means that the midimap has
    been loaded succesfully.

NOTE: DrumGizmo currently uses 16 channels of output, which should be
mapped manually in your software, just like you would do with any other
plugin. We can't describe this step since it varies depending on your
software.

Now plot some midi notes, and play them. You should hear the sound of
the DrumGizmo drumkit coming from your speakers. Keep in mind that the
drumkit needs to load fully before everything will play as
expected. So be patient...!

== More info ==
If you have suggestions, bugs or comments, feel free to visit
http://www.drumgizmo.org or visit the official DrumGizmo irc channel at
#drumgizmo on the freenode network.

== Known problems ==
*   On Win7 64bit with 32bit Cubase you might have to install the dll
    in "c:\Program Files(x86)\Steinberg\VSTPlugins" instead of
    "c:\Program Files(x86)\Steinberg\Cubase Studio 5\VSTPlugins".

Have fun!
// The Drumgizmo Team
