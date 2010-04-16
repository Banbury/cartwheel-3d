'''
Created on 2009-09-16

@author: beaudoin

A Python version of the C++ Observable class found in Utils 
'''

class Observable(object):
    
    def __init__(self):
        self._observers = []
        self._hasChanged = False
        self._batchChangeDepth = 0
        
    def setChanged(self):
        """Protected. Indicate that the object has changed."""
        self._hasChanged = True
        
    def clearChanged(self):
        """Protected. Indicate that the object changes have been sent to every observer."""
        self._hasChanged = False
        
    def notifyObservers(self, data = None):
        """Protected. Notify observers that a change has occurred.
        Doesn't notify if a batch change is going on."""
        self.setChanged()
        self.notifyObserversIfChanged( data )
        
    def notifyObserversIfChanged(self, data = None ):
        """Protected. Notify observers only if the object has been modified. 
        Doesn't notify if a batch change is going on."""       
        if not self.isDoingBatchChanges() and self.hasChanged():
            for observer in self._observers:
                observer.update( data )
            self.clearChanged()
    
    def hasChanged(self):
        """Return true if this object has changed since last notification."""
        return self._hasChanged
    
    def isDoingBatchChanges(self):
        """Returns true if this object is currently going through a batch of changes."""
        return self._batchChangeDepth > 0
    
    def beginBatchChanges(self):
        """Indicates that we want to begin performing batch changes on this object.
        Every call to beginBatchChanges() should be matched by a call to endBatchChanges(),
        observers will only be notified when the first begin is closed by the last end.
        The call to endBatchChanges() should usually be in a finally clause."""  
        self._batchChangeDepth += 1

    def endBatchChanges(self):
        """Indicates that we want to end performing batch changes on this object.
        Every call to beginBatchChanges() should be matched by a call to endBatchChanges(),
        observers will only be notified when the first begin is closed by the last end.
        The call to endBatchChanges() should usually be in a finally clause."""  
        self._batchChangeDepth -= 1
        self.notifyObserversIfChanged()

    def addObserver(self, observer):
        """Adds an observer to this object. Observers need to have a method:
              update( observable, data = None )
        Where observable is an observable object and data is an arbitrary data object."""
        self._observers.append( observer )
        
    def deleteObserver(self, observer):
        """Removes the specified observer from the list of observers."""
        self._observers.remove(observer)
        
    def countObservers(self):
        """Returns the number of observers of this object."""
        return len( self._observers )    
    
    @classmethod
    def typeName(cls):
        """Returns the name of the class."""
        return cls.__name__
