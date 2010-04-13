'''
Created on 2009-10-06

@author: beaudoin
'''

import PyUtils

class Scenario(PyUtils.Observable):
    """The base class for a scenario. Each scenarios should have the following methods:
          load()
             Called when the scenario has been filled so that its resources can be created (like rigid bodies, etc.)
          update()
             Called whenever a character has moved and the scenario must update the character-related information.""" 
    
    
    def __init__(self, name = "Unnamed scenario"):
        """Initializes a scenario with a specific name"""
        super(Scenario, self).__init__()
        
        self._name = name
        
    #
    # Accessors
    #
    def getName(self):
        """Access the scenario name."""
        return self._name
    
    def setName(self, name):
        """Sets the name of the scenario."""
        self._name = name
