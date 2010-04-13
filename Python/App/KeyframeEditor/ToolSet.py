'''
Created on 2009-11-23

@author: beaudoin
'''

import UI, wx


class ToolSet(UI.ToolSets.ToolsetBase):
    
    def __init__(self, toolPanel, model):
        """Adds a tool set for the keyframe editor to a toolpanel."""
        
        super(ToolSet,self).__init__()
               
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Style Editor" )

        self.addOption( "Edit style", model.getDisplayInterface, model.displayInterface )

        # Add this as an observer
        model.addOptionsObserver(self)

        # Initial update
        self.update()