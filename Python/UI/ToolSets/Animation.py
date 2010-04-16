'''
Created on 2009-09-26

@author: beaudoin
'''

import wx, UI

class Animation(object):
    
    # Available speeds on the buttons
    _buttonSpeeds = ( '1/16', '1/8', '1/4', '1/2', '1', '2', '4' )

    def __init__(self, toolPanel):
        """Adds a tool set for animation information to a toolpanel."""
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Animation" )       
        app = wx.GetApp()

        speedButtonPanel = self._toolSet.addTool( wx.Panel )
        animButtonPanel  = self._toolSet.addTool( wx.Panel )
        self._speedButtons = [ SpeedButton( speedButtonPanel, label = speed, size=(28,28) ) for speed in Animation._buttonSpeeds ] 
        self._restartButton = wx.BitmapButton( animButtonPanel, bitmap = wx.Bitmap('../data/ui/restart.png',   wx.BITMAP_TYPE_PNG) )
        self._playButton = wx.BitmapButton( animButtonPanel, bitmap = wx.Bitmap('../data/ui/play.png',   wx.BITMAP_TYPE_PNG) )
        self._pauseButton = wx.BitmapButton( animButtonPanel, bitmap = wx.Bitmap('../data/ui/pause.png',   wx.BITMAP_TYPE_PNG) )
        self._stepButton = wx.BitmapButton( animButtonPanel, bitmap = wx.Bitmap('../data/ui/step.png',   wx.BITMAP_TYPE_PNG) )
        self._stepButton.SetBitmapDisabled( wx.Bitmap('../data/ui/stepDisabled.png',   wx.BITMAP_TYPE_PNG) )
        
        for speedButton in self._speedButtons:            
            speedButton.Bind( wx.EVT_BUTTON, lambda event: app.setSimulationSecondsPerSecond( event.GetEventObject()._labelValue ) )
        self._restartButton.Bind( wx.EVT_BUTTON, lambda e: app.restoreActiveSnapshot(False) )
        self._playButton.Bind( wx.EVT_BUTTON, lambda e: app.setAnimationRunning(True) )
        self._pauseButton.Bind( wx.EVT_BUTTON, lambda e: app.setAnimationRunning(False) )
        self._stepButton.Bind( wx.EVT_BUTTON, lambda e: app.simulationFrame() )
        
        hBoxSpeedButton = wx.BoxSizer( wx.HORIZONTAL )
        hBoxSpeedButton.AddStretchSpacer()
        for speedButton in self._speedButtons:
            hBoxSpeedButton.Add( speedButton,0, wx.EXPAND ) 
        hBoxSpeedButton.AddStretchSpacer()
        speedButtonPanel.SetSizerAndFit(hBoxSpeedButton)
                       
        self._hBoxAnimButtons = wx.BoxSizer( wx.HORIZONTAL )
        self._hBoxAnimButtons.AddStretchSpacer()
        self._hBoxAnimButtons.Add(self._restartButton)
        self._hBoxAnimButtons.Add(self._playButton)
        self._hBoxAnimButtons.Add(self._pauseButton)
        self._hBoxAnimButtons.Add(self._stepButton)        
        self._hBoxAnimButtons.AddStretchSpacer()
        animButtonPanel.SetSizerAndFit(self._hBoxAnimButtons)
        
        # Add this as an observer
        app.addAnimationObserver(self)

        # Initial update
        self.update()
        

    def update(self, data = None):
        """Called whenever the animation is updated in the application."""

        app = wx.GetApp()
        simulationSecondsPerSeconds = app.getSimulationSecondsPerSecond()
        for speedButton in self._speedButtons:
            speedButton.Enable( simulationSecondsPerSeconds != speedButton._labelValue )
        
        self._hBoxAnimButtons.Show( self._playButton, not app.isAnimationRunning() )
        self._hBoxAnimButtons.Show( self._pauseButton, app.isAnimationRunning() )
        self._stepButton.Enable( not app.isAnimationRunning() )
        self._hBoxAnimButtons.Layout()
        

class SpeedButton(wx.Button):
    """A private internal class that keeps a button together with its numeric speed"""


    def __init__(self, parent, id = wx.ID_ANY, label = '', pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0,
                 validator = wx.DefaultValidator, name='' ):
        
        super(SpeedButton, self).__init__(parent, id, label, pos, size, style, validator, name)
        self._labelValue = eval(label+".0")