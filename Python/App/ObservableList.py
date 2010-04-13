'''
Created on 2009-09-28

@author: beaudoin
'''
import PyUtils

class ObservableList(PyUtils.Observable):
    """An object that contains a list of observable objects of the application (i.e. controllers, characters...)"""

    def __init__(self):
        super(ObservableList,self).__init__()      
        self._objects = []
    
    def add(self, object):
        """Adds an object to the list, notify observers."""
        if PyUtils.sameObjectInList(object, self._objects) :
            raise KeyError ('Cannot add the same object twice to application.')
        self._objects.append( object )
        self.notifyObservers()
        return object

    def delete(self, object):
        """Removes an object from the list. Specify either a name, an index, or an instance of the object."""        
        self._objects.remove( self.get(object) )
        self.notifyObservers()

    def clear(self):
        """Removes all objects from the list."""        
        del self._objects[:]
        self.notifyObservers()
        
    def get(self, description):
        """
        Gets the object at the specified index, or with the specified name.
        
        If description is a string, an object with the specified name will be searched.
        If search fails or objects do not have a getName method, a ValueError is raised.
        
        If description is an int, the controller at the specified index will be returned.
        If the index is invalid, an IndexError is raised.
        
        Otherwise, the input is returned unmodified. (So, if a controller object is passed, it is returned.)
        """
        if isinstance(description,basestring) :
            try: 
                description = [obj.getName() for obj in self._objects].index(description)
            except (AttributeError, ValueError): raise ValueError( "No object found with the specified name." )
        if isinstance(description,int) :
            return self._objects[description]
        return description

    def getCount(self):
        """Returns the number of controllers."""
        return len( self._objects )
