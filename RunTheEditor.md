# Introduction #

This page explains how to download and run the style and character editor presented at Siggraph 2010. This application only works under Windows.

Get `cartwheel.zip` from the [downloads section](http://code.google.com/p/cartwheel-3d/downloads/list). Unzip it in the directory of your choice and run `simbicon.bat`.

<img src='http://cartwheel-3d.googlecode.com/hg/web/editorScreenShot.jpg' />

# How to use the editor #

You can click the `Play` button to start the default walk. At any point in time, for example if the character falls down, click the `Rewind` button to restart from the initial pose.

Showing and hiding the style editor is controlled by checking the `Edit style` checkbox. You can edit the character shape by clicking the `Edit character` checkbox.

# Editing the gait parameters #

You can use the `Speed`, `Duration` and `Width` sliders to modify the character's walking speed, the stepping frequency and the step width, respectively. Be careful: extreme values will generally make the character fall down.

# Editing the motion style #

In the style editor, you can manipulate any yellow handle in the side or front view. If you want to add or remove a keyframe, simply check one of the three box in the top row. You cannot edit the swing foot position at the beginning or end of a stride, but you can edit it mid-stride.

# Editing the character shape #

In the character editor, play with the yellow handles to change the body shape. Once you're satisfied, click on the `Create` button. If you want to go back to the initial character, click `Reset character`. To create an asymmetrical character, uncheck `Symmetrical`.

Extremely large or small limbs will result in overly light or heavy body part. These usually don't behave well and may cause the character to fall.

# Going further #

The editor is full of hidden features that are not quite ready for prime time. If you want to push things further, [download the code](http://code.google.com/p/cartwheel-3d/wiki/GettingStarted) and start playing with the python scripts. You can also discuss the project or ask questions on the [Forum](http://groups.google.com/group/cartwheel-3d). Don't hesitate to propose ways in which the editor could be made better!