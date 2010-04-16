'''
Created on 2009-09-28

@author: beaudoin
'''


def enum( *args ):
    """
    Creates an enum class. 
    Pass a dictionnary  { choice : value, ... }
    Here, choice needs to be a string and value is an integer
    Can also pass a list or a tuple, then the values will be given automatically, starting at 0
    Instances of this class can have any of the values available in choices
    
    Examples:
    Colors = enum( {"red" : 10, "green" : 20, "yellow" : 30} )
    Colors.int( "red" )  ==>  10
    Colors.str( 20 )     ==>  green    
    x = Colors("red")
    x                    ==> "red"
    str(x)               ==> red
    int(x)               ==>  10
    x.set( "green" )
    x.set( 30 )
    """
    
    @classmethod
    def toInt(cls,choice):
        """Return the value associated with the choice (an integer)."""
        if choice is None : return None
        return cls.choicesDict[choice]
    
    @classmethod
    def toStr(cls,value):
        """Return the choice associated with the value (a string)."""
        if value is None : return None
        return cls.valuesDict[value]

    @classmethod
    def values(cls):
        """Return the choice associated with the value (a string)."""
        return valuesDict.keys()

    @classmethod
    def choices(cls):
        """Return the choice associated with the value (a string)."""
        return choicesDict.keys()
    
    def __init__(self, value=None):
        """Initialize a new instance of this enum class"""
        self.set(value)
        
    def __repr__(self):
        """A representation of this object"""
        if self.value is None : return repr(None)
        return repr( str(self) )
    
    def __int__(self):
        """Return the value currently associated with this variable"""
        return int( self.value )
    
    def __str__(self):
        """Return the value currently associated with this variable"""
        if self.value is None : return str(None)
        return self.toStr( self.value )

    def __eq__(self,other):
        """Checks that this enum is equal to the other, which can be an enum of the same type, a value, or a string."""
        if isinstance(other, basestring) : return other == str(self)
        return self.value == int(other)
    
    def __ne__(self,other):
        """Checks that this enum is equal to the other, which can be an enum of the same type, a value, or a string."""
        return not self == other
    
    def set(self, value):
        """Changes the value of self"""
        if value is None : self.value = None
        try: self.value = value.value
        except AttributeError:
            if isinstance(value, int)          : self.value = value
            elif isinstance(value, basestring) : self.value = self.toInt(value)
            else: raise ValueError( "Trying to set an enum with an invalid type. Only int and str can be used." )
        return self
        
    # Reverse the dictionary
    if len(args) == 1 and isinstance(args[0], dict) :
        choicesDict = args[0]
    else:
        choicesDict = {}
        for i, choice in enumerate( args ) : choicesDict[choice] = i
        try: del i
        except UnboundLocalError: pass
        
    valuesDict = {}
    for choice, value in choicesDict.iteritems(): valuesDict[value] = choice
    try: del choice
    except UnboundLocalError: pass
    try: del value
    except UnboundLocalError: pass
    try: del args
    except UnboundLocalError: pass
    
    return type('Enum',(object,),locals())
    