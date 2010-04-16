'''
Created on 2009-09-26

@author: beaudoin
'''

import UI, wx
from ToolsetBase import ToolsetBase

class Camera(ToolsetBase):
    
    def __init__(self, toolPanel):
        """Adds a tool set for camera information to a toolpanel."""
        
        super(Camera, self).__init__()
        
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Camera" )
        app = wx.GetApp()
        
        self.addOption( "Follow character", app.doesCameraFollowCharacter, app.setCameraFollowCharacter )
        self.addOption( "Auto orbit", app.doesCameraAutoOrbit, app.setCameraAutoOrbit )
        
        # Add this as an observer
        app.addCameraObserver(self)
        
        # Initial update
        self.update()
        