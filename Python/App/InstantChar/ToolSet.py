'''
Created on 2009-11-23

@author: beaudoin
'''

import UI, wx


class ToolSet(UI.ToolSets.ToolsetBase):
    
    def __init__(self, toolPanel, model):
        """Adds a tool set for the instant character to a toolpanel."""
        
        super(ToolSet,self).__init__()
               
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Instant Character" )

        self.addOption( "Edit character", model.getDisplayInterface, model.displayInterface )
        self.addOption( "Symmetrical", model.isSymmetric, model.setSymmetric )
        self.addButton( "Create", model.create )
        self.addButton( "Reset character", model.reset )
        self._speedSlider, self._speedSliderData = self.addSlider( "Speed", min=-1, max=5.0, step=0.01, getter = model.getDesiredSpeed, setter = model.setDesiredSpeed)
        self._durationSlider, self._durationSliderData = self.addSlider( "Duration", min=0.2, max=1.0, step=0.01, getter = model.getDesiredDuration, setter = model.setDesiredDuration)
        self._stepWidthSlider, self._stepWidthSliderData = self.addSlider( "Width", min=0.0, max=0.3, step=0.01, getter = model.getDesiredStepWidth, setter = model.setDesiredStepWidth)


        # Add this as an observer
        model.addOptionsObserver(self)

        # Initial update
        self.update()
        
    def getCurrentSpeed(self):
        """Gets the speed as currently shown on the slider."""
        return self._speedSliderData.getSliderValue()
    
    def getCurrentDuration(self):
        """Gets the duration as currently shown on the slider."""
        return self._durationSliderData.getSliderValue()
        
    def getCurrentStepWidth(self):
        """Gets the step width as currently shown on the slider."""
        return self._stepWidthSliderData.getSliderValue()