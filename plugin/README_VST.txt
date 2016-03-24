== Installation ==
To install the DrumGizmo vst plugin, follow these simple steps:
1. Copy “drumgizmo_vst.dll” into the folder containing your VST plugins.
2. Done!

When opening your VST host software (Cubase or the like), the plugin
should now show up as a VST Instrument.

== How to use it ==
1.  Load the plugin as a VST Instrument
    (In Cubase, load it as a "Rack Instrument").
2.  Now create a midi track to use with the plugin.
2a. (Optional) Choose “GM drum map” to use with the midi track.
3.  Select DrumGizmo as midi output for the track.
4.  Now open up the DrumGizmo VST interface.
5.  Load a drumkit by clicking on the "Browse..." button and browse for
    the drumkit xml file. (Example: “C:\drumkit\drumkit.xml”).
    You can follow the loading progress on the green bar. It might take a
    while before all drums are loaded, so be patient.
8.  Load a midimap by clicking the “Browse...” button and browse for the
    midimap xml file. (Example: “C:\drumkit\midimap.xml”). You can follow
    the progress on the green bar (this will load almost instantly).

NOTE: DrumGizmo uses several audio output channels, which should be
mapped manually in your software, just like you would do with any other
plugin. We can't describe this step since it varies depending on your
software. But keep in mind that if you do not map these channels, you
will most likely only hear the output of one of the ambience microphones
and it will sound really bad. So please take your time to get these
mappings set up!

Now plot some midi notes, and play them. You should hear the sound of
the DrumGizmo drumkit coming from your speakers. Keep in mind that the
drumkit needs to load fully before everything will play as
expected.

== More info ==
If you have suggestions, bugs or comments, feel free to visit
http://www.drumgizmo.org or visit the official DrumGizmo irc channel at
#drumgizmo on the freenode network.

Have fun!
// The Drumgizmo Team
