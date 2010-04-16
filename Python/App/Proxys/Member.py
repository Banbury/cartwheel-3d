'''
Created on 2009-09-22

@author: beaudoin

This module contains classes representing different possible type of data members within proxy object.
A member knows how to get/set itself from/to the wrapped object.
'''

import MathLib
import PyUtils

class TopLevel(object):
    
    def __init__(self, type, name, default, editable = False, fancyName = None, isObject = False ):
        self.name = name
        self.default = default
        self.editable = editable
        self.isObject = isObject
        self.type = type
        self.fancyName = fancyName
        if fancyName == None:
            self.fancyName = PyUtils.unCamelCase(name)

    def basicInterpret(self,value):
        """Performs basic interpretation, does not coerce into type."""
        if self.type not in (str,unicode) and isinstance(value, basestring) :
            return eval(value,dict(__builtins__=__builtins__))
        return value

    def interpret(self, value):
        """This method lets a user have different ways to input a member."""
        value = self.basicInterpret(value)
        if self.type is None or value is None or isinstance(value, self.type): return value
        return self.type(value)
    

    def format(self, value):
        """This method returns the preferred representation of the data."""
        return value

class Basic(TopLevel):
    
    def __init__( self, type, name, default, getter, setter, editable = True, fancyName = None, isObject = False ):
        super( Basic, self ).__init__( type, name, default, editable, fancyName, isObject )
        self._getter = getter
        self._setter = setter
    
    def get(self, object):
        if self._getter is None : return None
        return self._getter(object)

    def set(self, object, value, container = None):
        if self._setter is None : return
        if value is None : return
        try: self._setter(object, value, container)
        except ( TypeError, NotImplementedError ): self._setter(object, value)
                 
class Tuple(Basic):
    """Can be used when setter takes multiple parameters and getter returns multiple parameters. Otherwise use Basic with type tuple."""
    
    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Tuple, self ).__init__( tuple, name, default, getter, setter, editable, fancyName, False )
        
    def get(self, object):
        if self._getter is None : return None
        # TODO do something here!
        return None

    def set(self, object, value, container = None):
        if self._setter is None : return
        if value is None : return
        self._setter(object, *value)
    
class Enum(Basic):
    
    def __init__( self, enum, name, default, getter, setter, editable = True, fancyName = None ):
        """Pass the enum class you want to use for this member"""
        super( Enum, self ).__init__( int, name, default, getter, setter, editable, fancyName, False )
        self.enum = enum

    def interpret(self, value):
        if value is None : return None
        elif isinstance(value, basestring) : return self.enum.toInt( value )
        else : return int( value )

    def format(self, value):
        return self.enum.toStr(value)
    
class Vector3d(Basic):
    
    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Vector3d, self ).__init__( MathLib.Vector3d, name, default, getter, setter, editable, fancyName, False )

    def interpret(self, value):
        value = self.basicInterpret(value)
        if isinstance(value, MathLib.Vector3d) : return value
        else : return PyUtils.toVector3d( value )

    def format(self, value):
        return PyUtils.fromVector3d(value)
    
class Point3d(Basic):
    
    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Point3d, self ).__init__( MathLib.Point3d, name, default, getter, setter, editable, fancyName, False )

    def interpret(self, value):
        value = self.basicInterpret(value)
        if isinstance(value, MathLib.Point3d) : return value
        else : return PyUtils.toPoint3d( value )

    def format(self, value):
        return PyUtils.fromPoint3d(value)

class Quaternion(Basic):
    
    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Quaternion, self ).__init__( MathLib.Quaternion, name, default, getter, setter, editable, fancyName, False )

    def interpret(self, value):
        value = self.basicInterpret(value)
        if isinstance(value, MathLib.Quaternion) : return value
        else : return PyUtils.angleAxisToQuaternion( value )

    def format(self, value):
        return PyUtils.angleAxisFromQuaternion(value)
        
class Trajectory1d(Basic):
    
    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Trajectory1d, self ).__init__( MathLib.Trajectory1d, name, default, getter, setter, editable, fancyName, False )

    def interpret(self, value):
        value = self.basicInterpret(value)
        if isinstance(value, MathLib.Trajectory1d) : return value
        else : return PyUtils.toTrajectory1d( value )

    def format(self, value):
        return PyUtils.fromTrajectory1d(value)        

class Object(Basic):

    def __init__( self, name, default, getter, setter, editable = True, fancyName = None ):
        super( Object, self ).__init__( None, name, default, getter, setter, editable, fancyName, True )
    
    def getObject(self, object):
        """Return unwrapped object."""
        return super( Object, self ).get(object)

    def get(self, object):
        return PyUtils.wrap( self.getObject(object) )

    def set(self, object, valueProxy, container = None):
        if valueProxy is None : return
        try:
            # Try to re-fill the existing object
            value = self.getObject(object)
            try: value.beginBatchChanges()
            except AttributeError: pass
            try: 
                valueProxy.fillObject( value, object )
            finally:
                try: value.endBatchChanges()
                except AttributeError: pass
        except(AttributeError, TypeError):
            value = valueProxy.createObject()
            valueProxy.fillObject( value, object )
            super( Object, self ).set(object,value,container)

class ObjectList(TopLevel):
    
    def __init__( self, name, default, getAtIndex, getCount, addToContainer, editable = True, fancyName = None, embedInParentNode = False, groupIcon = "../data/ui/objectListIcon.png" ):
        super( ObjectList, self ).__init__( None, name, default, editable, fancyName, True )
        self._getAtIndex = getAtIndex
        self._getCount = getCount
        self._addToContainer = addToContainer
        self.embedInParentNode = embedInParentNode
        self.groupIcon = groupIcon
    
    def getObject(self, object, i):
        """Return unwrapped object."""
        return self._getAtIndex(object,i)

    def getCount(self, object):
        return self._getCount(object)

    def get(self, object):
        result = []
        for i in range( self.getCount(object) ) :
            result.append( PyUtils.wrap( self.getObject(object,i) ) )
        return result

    def _set(self, object, i, valueProxy, container):
        if valueProxy is None : return
        try:
            # Try to re-fill the existing object
            if i >= self.getCount(object) : raise IndexError;
            value = self.getObject(object, i)
            try: value.beginBatchChanges()
            except AttributeError: pass
            try: 
                valueProxy.fillObject( value, object )
            finally:
                try: value.endBatchChanges()
                except AttributeError: pass
        except(AttributeError, TypeError, IndexError, ValueError):
            value = valueProxy.createObject()
            if value is None : return
            valueProxy.fillObject( value, object )
            try: self._addToContainer(object, value, container)
            except ( TypeError, NotImplementedError ): self._addToContainer(object, value)

    def set(self, object, valueProxyList, container = None):
        if valueProxyList is None : return
        PyUtils.callOnObjectOrListAndEnumerate( valueProxyList, lambda i, valueProxy: self._set(object,i,valueProxy,container) ) 
        
        