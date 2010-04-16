'''
Created on 2009-09-22

@author: beaudoin
'''

import PyUtils

def create( wrappedClass = None, members = [], parent = None, caster = None, loader = None, verbose = True, icon = None, nameGetter = None ):
    """
    Creates a new proxy class (NOT an instance).
    
    This proxy class is meant as a wrapper to the specified wrappedClass.
    
    It contains a specific list of members, these members should be instances of objects in App.Proxys.Member.
    The order of the members is important, and will stay as specified.
    If it has a parent class, then it inherits all the members from the parent class, as well as the newly specified ones,
    the order is: parent's members first, then child's members.
    If a member exists both in the parent and the child, then the child's takes precedence, but it appears at the position of the parent 
    (this allows changing member's default value, for example).
    
    If a caster is specified, then it will be used to cast down the wrapped object whenever it's needed.
    
    If a loader is specified, then the wrapped object can be loaded directly using the load method.
    
    If the wrapper is verbose (default), then its representation will use the long format:  
        memberName = value
    Also, fields having default values will not be represented. 
        
    If the wrapper is not verbose, then its representation will be short, all the values will be listed in their correct order.
    
    In icon, specify the filename of a png to use as an icon to represent the object. Pass None to use parent's icon.
    
    In nameGetter, specify the method to call to obtain a string representing the name of this object.       
    """
    
    kwargs = {}
    for name, var in locals().items():
        if name == 'kwargs' : continue
        kwargs["_"+name] = var
    return _create(**kwargs)


def _create( _wrappedClass, _members, _parent, _caster, _loader, _verbose, _icon, _nameGetter ):
    """Private, forwarded from create for convenience in member name."""
    
    @classmethod
    def wrap( cls, object, recursive = True ):
        """
        This class method creates an instance of this proxy class wrapping the specified object.
        To make sure you use the right proxy class, consider calling App.PyUtils.wrap instead.
        Pass recursive = False to only wrap non-object members. The members containing object references or list of objects will not be wrapped and will be None.  
        """
        if cls._caster is not None : object = cls._caster(object)
        kwargs = {}
        for member in cls._members:
            if not recursive and member.isObject: continue
            kwargs[ member.name ] = member.get(object)
        return cls( **kwargs )

    @classmethod
    def getIcon( cls ):
        return cls._icon

    @classmethod
    def getName( cls, object ):
        if cls._caster is not None : object = cls._caster(object)
        try: return cls._nameGetter( object )
        except TypeError: return cls.__name__
    
    @classmethod
    def getObjectMembers(cls):
        return filter( lambda m : m.isObject, cls._members )
    
    def __init__(self, *args, **kwargs):
        """
        Initialize a proxy object of this proxy class.
        All the members must be initialized in the order in which they appear or using keyword arguments.
        For a specific list of members, look at:
           self.members
        """

        if len(args) > len( self._members ):
            raise TypeError( "Too many arguments when creating proxy '%s'." % type(self).__name__ )
        
        for i, member in enumerate( self._members ):
            if i < len(args):
                value = args[i]
            else :
                try: 
                    value = kwargs[ member.name ]
                    del kwargs[ member.name ]
                except KeyError: 
                    value = member.default
            self.__setattr__( member.name, member.interpret( value ) )

        if len(kwargs) > 0 :
            raise AttributeError( "Attribute '%s' not a member of proxy '%s'." % (kwargs.keys()[0], type(self).__name__) )

    def __repr__(self) :
        """
        Creates a representation of this proxy object. This can be evaluated provided the following includes:
          from App.Proxys.All import *
        """
        outMembers = []
        for member in self._members:
            value = self.__getattribute__(member.name)
            if self._verbose and PyUtils.safeEqual(value, member.default) : continue 
            outMember = repr( member.format(value) )
            if self._verbose: outMember = member.name + " = " + outMember
            outMembers.append( outMember )
        return type(self).__name__ + "(" + ','.join(outMembers) + ")"


    def setValueAndUpdateObject(self, memberName, value, object, container = None):
        """
        Sets the specified value of the specified member and updates the object.
        As a container, specify the object (not proxy object) to which this object is attached with an HAS_A or HAS_MANY relationship.
        """
        info = self._memberDict[memberName]
        newValue = info.interpret(value)
        if PyUtils.safeEqual( newValue, self.__getattribute__(memberName) ) : return
        self.__setattr__(memberName, newValue)
        info.set(object, newValue, container )

    
    def getValueAndInfo(self, index):
        """
        Returns a 2-tuple  (value, info)  for the ith member of this wrapped object.
        
        'value' is the member's value
        'info' is a class from module App.Proxys.Member that describes this member
        """
        info = self._members[index]
        value = self.__getattribute__(info.name)
        
        return ( value, info )

    def getMemberCount(self):
        """
        Returns the number of members of this proxy class.
        """
        return len( self._members )

    def createAndFillObject(self, container = None, *args):
        """Same as calling createObject followed by fillObject."""
        return self.fillObject( self.createObject(*args), container )
            
    def createObject(self, *args):
        """
        Creates a new instance of the object wrapped by this class.
        The object will not be filled, you have to call fillObject.
        If any extra parameters are specified, they are passed to the object constructor.

        Returns the newly created object.
        """ 
        return self._wrappedClass(*args)  

    def fillObject(self, object, container = None):
        """
        Fills every wrapped field in the specified object with the content of this proxy object.
        All members are iterated over in the order they were specified.
        If the object is an Observable, then the changes are batched.
        As a container, specify the object (not proxy object) to which this object is (or will be) attached with an HAS_A or HAS_MANY relationship.

        Returns the filled object, the exact same object that was passed in parameter.
        """ 
        try: object.beginBatchChanges() 
        except AttributeError: pass
        try:
            for member in self._members:
                member.set(object, self.__getattribute__(member.name), container )
        finally:
            try: object.endBatchChanges()
            except AttributeError: pass
        return object
    
    def extractFromObject( self, object, recursive = True ):
        """
        This method extract the content of the passed object and places it in this wrapper.
        Pass recursive = False to only extract non-object members. The members containing object references or list of objects will not be extracted and will remain unchanged.  
        """
        if self._caster is not None : object = self._caster(object)
        for member in self._members:
            if not recursive and member.isObject: continue
            self.__setattr__( member.name, member.get(object) )
    
    def load(self, *args):
        """
        Creates a new instance of the object wrapped by this class,
        Fill it with its content,
        Then, add it to the framework or to the container object using 
        the specified loader method.
        
        Returns the newly loaded object. 
        """
        object = self.createObject(*args)
        self.fillObject(object)
        if self._loader is not None :
            self._loader( object )
        return object

    # Inherits the parent icon
    if _icon is None :
        if _parent is not None:
            _icon = _parent._icon
                
    # Merge the parent and child's member list
    if _parent is not None :
        # Merge parent and child members
        _members = _parent._members + _members
        # Move the child's member to the parent if they have the same name
        for i in range( len(_parent._members) ):
            for j in range( len(_parent._members), len(_members) ):
                if _members[i].name == _members[j].name :
                    _members[i] = _members[j]
                    del _members[j]

                    break
    try: 
        del i
        del j
    except UnboundLocalError: pass
                
    # Build a dictionnary of members for efficient look-up
    _memberDict = {}
    for member in _members:
        _memberDict[member.name] = member
    try: del member
    except UnboundLocalError: pass
    _nameGetter = staticmethod(_nameGetter)
    return type( _wrappedClass.__name__.split('.')[-1], (object,), locals() )
