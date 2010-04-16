'''
Created on 2009-09-26

@author: beaudoin
'''

import UI, wx
from ToolsetBase import ToolsetBase

class Options(ToolsetBase):
    
    def __init__(self, toolPanel):
        """Adds a tool set for various options information to a toolpanel."""
        
        super(Options, self).__init__()
        
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Options" )

        app = wx.GetApp()

        self.addOption( "Show collision volumes", app.getDrawCollisionVolumes, app.drawCollisionVolumes )
        self.addOption( "Capture screenshots", app.getCaptureScreenShots, app.captureScreenShots )
        self.addOption( "Kinematic motion", app.getKinematicMotion, app.setKinematicMotion)

        # Add this as an observer
        app.addOptionsObserver(self)
        
        # Initial update
        self.update()        