'''
Created on 2009-09-29

@author: beaudoin
'''

import PyUtils

class Curve(PyUtils.Observable):
    """This class contains a named curve that can be observed."""
    
    def __init__(self, name, trajectory1d, phiPtr = None):
        """Initializes a curve with the given name and attached to the given trajectory."""
        self._name = str(name)  # No unicode string pass this point
        self._trajectory1d = trajectory1d
        self._phiPtr = phiPtr
        
    def getName(self):
        """Returns the curve name."""
        return self._name
        
    def setName(self, name):
        """Returns the curve name."""
        self._name = str(name)  # No unicode string pass this point
        self.notifyObservers()
    
    def getTrajectory1d(self):
        """Returns the attached trajectory."""
        return self._trajectory1d
        
    def getPhiPtr(self):
        """Returns the attached pointer to the phase."""
        return self._phiPtr