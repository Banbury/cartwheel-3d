# Introduction #

This page will give you all the steps required to compile and execute the project. Right now it only works under Windows and the document focuses on a Visual Studio + Eclipse environment. Porting the project to different platforms and environments should not be too difficult, and we're definitely interested in hosting a multi-platform version so you're welcome to contribute this!

# Initial set-up #

The project has been tested under Visual Studio 2008 and [Eclipse](http://www.eclipse.org/downloads/) with [PyDev](http://pydev.org/). You can install PyDev using Eclipse update manager from http://pydev.org/updates. Eclipse and PyDev are only useful if you want to edit and debug the Python code. The project uses a mercurial repository. Under windows, you can use [TortoiseHg](http://tortoisehg.bitbucket.org/).

Once you have the above tools, set-up your workspace directory, say `$workspace` (The path shouldn't contain any spaces). Download and unzip `lib.zip` from the [downloads section](http://code.google.com/p/cartwheel-3d/downloads/list) into `$workspace\lib`, make sure you use precisely that directory name. You can then clone the mercurial repository at `https://cartwheel-3d.googlecode.com/hg/` into `$workspace/cartwheel`. For more information, here is an excellent [mercurial tutorial](http://hginit.com/).

**Important!** The project uses the `glew32.dll` and `glut32.dll` found in `$workspace\lib`.  You will therefore need to add that directory to your `PATH` environment variable, or copy the two `dll`s into `C:\Windows\System32`.

# Compiling the C++ code #

Open `$workspace/cartwheel/simbicon.sln` with Visual Studio. Right-click on the `Core` project and `Set as StartUp Project`. Then select the `Release` build in the drop-down box beside the play button and hit `Ctrl+Shift+B` to build the solution. Once built, you should can hit `Ctrl-F5` to start the project without debugging.

If you want to debug the C++ code, you can compile in `Debug` and hit `F5`.

# Writing and debugging the Python code #

If you want to edit and debug the Python code, including all the user interface code, you can use the Eclipse environment.

In Eclipse, first set-up your python environment. Go to `Window --> Preferences --> Pydev --> Interpreter - Python`. Add a `New...` interpreter, name it `Python26` and select `$workspace\lib\Python26\python.exe`.

To import the PyDev project select `File --> Import` then `General --> Existing Projects into Workspace`. As the root directory, select `$workspace` (**not** `$workspace\cartwheel`), make sure you check only the `simbicon` project, and hit `Finish`. Now click the drop-down arrow beside the play button and select `BasicEditor Release`.

To debug the python code, select `BasicEditor Debug` from the debug drop-down. This will only work if you compiled the debug version of the C++ code.

**[NOTES](http://www.funkboxing.com/wordpress/?p=228) FROM SUCCESSFUL COMPILE W/ VS2010EXPRESS**

