'''
Created on 2009-08-24

This module contains the main OpenGL application window that is used by all SNM applications

@author: beaudoin
'''

import wx
import UI

        
class MainWindow(wx.Frame):
    """The class for the main window."""
    MIN_TOOLPANEL_WIDTH = 200
    MIN_CONSOLE_HEIGHT  = 100

    def __init__(self, parent, id, title, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
                 name='frame', fps=30, glCanvasSize=wx.DefaultSize,
                 showConsole=True,
                 consoleEnvironment={} ):
        
        # Check if a fixed glWindow was asked
        fixedGlWindow = glCanvasSize != wx.DefaultSize
        self._glCanvasSize = glCanvasSize
        
        #
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style |= wx.NO_FULL_REPAINT_ON_RESIZE
        
        # Not resizable if GL canvas is fixed size
        if fixedGlWindow :
            style &= ~wx.RESIZE_BORDER & ~wx.MAXIMIZE_BOX
            
        super(MainWindow, self).__init__(parent, id, title, pos, size, style, name)

        #
        # Create the menu
        
        self._menuBar = wx.MenuBar()
        self._fileMenu = wx.Menu()
        self._fileMenu.Append( wx.ID_OPEN, "&Open" )
        self._fileMenu.Append( wx.ID_SAVE, "&Save" )
        self._fileMenu.AppendSeparator()
        self._fileMenu.Append( wx.ID_EXIT, "&Quit" )
        self._menuBar.Append(self._fileMenu, "&File" )
        
        self._helpMenu = wx.Menu()
        self._helpMenu.Append( wx.ID_ABOUT, "&About" )
        self._menuBar.Append(self._helpMenu, "&Help" )

        self.SetMenuBar( self._menuBar )

        #
        # Create the GL canvas
        attribList = (wx.glcanvas.WX_GL_RGBA, # RGBA
                      wx.glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      wx.glcanvas.WX_GL_DEPTH_SIZE, 24,  # 24 bit depth
                      wx.glcanvas.WX_GL_STENCIL_SIZE, 8 ) # 8 bit stencil

        self._glCanvas = UI.GLPanel(self, fps = fps, size = glCanvasSize, attribList = attribList)      

        # Create the right window (sashed) where the tool panel will be
        self._rightWindow = wx.SashLayoutWindow(self)
        self._rightWindow.SetDefaultSize((MainWindow.MIN_TOOLPANEL_WIDTH * 1.3,-1))
        self._rightWindow.SetMinimumSizeX(MainWindow.MIN_TOOLPANEL_WIDTH)
        self._rightWindow.SetOrientation( wx.LAYOUT_VERTICAL )
        self._rightWindow.SetAlignment( wx.LAYOUT_RIGHT )
        if not fixedGlWindow:
            self._rightWindow.SetSashVisible( wx.SASH_LEFT, True )
        self._rightWindow.Bind( wx.EVT_SASH_DRAGGED, self.onSashDragRightWindow )
        
        #
        # Create the tool panel
        self._toolPanel = UI.ToolPanel(self._rightWindow)

        # Create the bottom window (sashed) where the console will be
        self._bottomWindow = wx.SashLayoutWindow(self)
        self._bottomWindow.SetDefaultSize((-1,MainWindow.MIN_CONSOLE_HEIGHT*2))
        self._bottomWindow.SetMinimumSizeY(MainWindow.MIN_CONSOLE_HEIGHT)
        self._bottomWindow.SetOrientation( wx.LAYOUT_HORIZONTAL )
        self._bottomWindow.SetAlignment( wx.LAYOUT_BOTTOM )
        if not fixedGlWindow:
            self._bottomWindow.SetSashVisible( wx.SASH_TOP, True )
        self._bottomWindow.Bind( wx.EVT_SASH_DRAGGED, self.onSashDragBottomWindow )
        
        #
        # Create the console window
        self._console = UI.PythonConsole(self._bottomWindow, size=(-1,220), consoleEnvironment = consoleEnvironment )
        if not showConsole:
            self._bottomWindow.Hide()
        
        self.Bind( wx.EVT_SIZE, self.onSize )


    #
    # Private methods
    
    def _layoutFrame(self):
        """Private. Perform frame layout"""
        wx.LayoutAlgorithm().LayoutFrame(self, self._glCanvas)

        
    #
    # Event handlers 
        
    def onSize(self, event):
        self._layoutFrame()
        if self._glCanvasSize != wx.DefaultSize :
            currGlCanvasSize = self._glCanvas.GetSize()
            diff = ( currGlCanvasSize[0] - self._glCanvasSize[0], currGlCanvasSize[1] - self._glCanvasSize[1] )
            if diff == (0,0) :
                return
            currentSize = event.GetSize()
            newSize= ( currentSize[0] - diff[0], currentSize[1] - diff[1] )
            if newSize == currentSize :
                return
            self.SetSize( newSize )
            self.SendSizeEvent()



    def onSashDragRightWindow(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
        self._rightWindow.SetDefaultSize((event.GetDragRect().width,-1))
        self._layoutFrame()

    def onSashDragBottomWindow(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
        self._bottomWindow.SetDefaultSize((-1,event.GetDragRect().height))
        self._layoutFrame()

    #
    # Accessors
    
    def getGLCanvas(self):
        """Return the associated GL canvas."""
        return self._glCanvas

    def getToolPanel(self):
        """Return the associated tool panel."""
        return self._toolPanel


    def getFps(self):
        """Return the desired frame per second for this window."""
        return self._glCanvas.getFps()
