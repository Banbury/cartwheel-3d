'''
Created on 2009-09-02

@author: beaudoin
'''

def callOnObjectOrList( objectOrList, function ):
    """Apply the specified function on :
        - The single element passed as first parameter
        - The list of elements passed as first parameter, if this is a list or tuple
        - The list of values passed as first parameter if this is a dict
       The passed function should receive a single parameter.
       Does not return anything, so whatever is returned by function is lost."""
                
    if hasattr( objectOrList, 'values' ):
        for object in objectOrList.values(): function(object)
    elif hasattr( objectOrList, '__iter__' ):
        for object in objectOrList: function(object)
    else:
        function( objectOrList )


def callOnObjectOrListAndEnumerate( objectOrList, function ):
    """Apply the specified function on :
        - The single element passed as first parameter
        - The list of elements passed as first parameter, if this is a list or tuple
        - The list of values passed as first parameter if this is a dict
       The passed function should receive two parameters: an index or the object.
       Does not return anything, so whatever is returned by function is lost."""
                
    if hasattr( objectOrList, 'values' ):
        for i, object in enumerate(objectOrList.values()): function(i,object)
    elif hasattr( objectOrList, '__iter__' ):
        for i, object in enumerate(objectOrList): function(i,object)
    else:
        function( 0, objectOrList )

def _moduleExist( moduleNames ):
    """Checks that the specified module can be loaded in the current python path.
    moduleNames is a list containing every element of the module name. For example,
    module 'Data.Character.Robot' would be [ 'Data', 'Character', 'Robot' ] """
    import sys, os
    for path in sys.path :
        fileName = os.path.join( path, *moduleNames )
        if os.path.exists( fileName + '.py' ) : return True

    return False


def _getData( dataModule ):
    """Private function. Access the data contained in the specified dataModule"""

    # First check if file exist.
    # This is better than using the python exception handling, since we don't want to
    # inadvertently catch exceptions generated during module importation.
    modules = dataModule.split('.')
    if modules[0] != "Data" :  modules.insert(0, "Data")
    if not _moduleExist( modules ) :
        modules.append( modules[-1] )
        if not _moduleExist( modules ) :
            raise ImportError( "Can't find data module '" + dataModule + "'." )
    moduleName = '.'.join(modules)

    module = __import__( moduleName, globals(), locals(), ["data"] ) 
    reload( module )
    try:
        return module.data
    except( AttributeError ):
        raise ImportError( "Data module '" + dataModule + "' doesn't contain a variable named 'data'." )    

def load( dataModule, *args ):
    """Loads the object contained in the specified data module.
    A data module is a python-style name for a module located under the
    'Data' package.  For example to load the data contained
    into 'Data/Character/Robot/Controllers/Walk.py', call
    load( "Character.Robot.Controllers.Walk" ).
    As a shortcut, if the specified dataModule is a package, the
    function will look for a module having the exact name of the package.
    For example, load( "Character.Robot" ) is the same as load( "Character.Robot.Robot" ).
    Any extra parameter passed to the function are passed to the load method
    of the loaded object. Returns the loaded object. """
    data = _getData( dataModule )
    return data.load(*args)


def loadRename( dataModule, objectName, *args ):
    """Loads the object contained in the specified data module.
    A data module is a python-style name for a module located under the
    'Data' package.  For example to load the data contained
    into 'Data/Character/Robot/Controllers/Walk.py', call
    load( "Character.Robot.Controllers.Walk" ).
    As a shortcut, if the specified dataModule is a package, the
    function will look for a module having the exact name of the package.
    For example, load( "Character.Robot" ) is the same as load( "Character.Robot.Robot" ).
    Any extra parameter passed to the function are passed to the load method
    of the loaded object. Returns the loaded object. """
    data = _getData( dataModule )
    data.name = objectName
    return data.load(*args)

def loadMany( dataModule, count, *args ):
    """Loads 'count' instances of the object contained in the specified data module.
    See 'load' for more details on what is a dataModule.
    If they have a 'name' attribute, an underscore and a number starting at '_0' will be appended to each instance.
    If they have a 'pos' attribute, then the positions will be moved around.
    Returns a list of all the loaded object. """
    from MathLib import Vector3d
    data = _getData( dataModule )
    result = []
    for i in range(0,count):
        dataCopy = data
        try: dataCopy.name = data.name + "_" + str(i)
        except AttributeError: pass
        try: dataCopy.pos += Vector3d(10,10,10) * i
        except AttributeError: pass
        result.append( dataCopy.load(*args) )
    return result

def getClassName( object ):
    """
    Returns the name of the class of the specified object.
    The object must have a method   typeName()  that returns the name of the class (i.e.  "class ClassName" or "ClassName").
    """
    return object.typeName().split(' ')[-1]

def getProxyClass( object ):
    """
    This function returns the proxy class for specified object. This class will
    have the following methods: 
        wrap(object)    : returns the proxy class for the specified object
        getIcon()       : returns the filename of a png icon that represents the object
        getName(object) : returns a string that can be used to identify the object
    Moreover, the proxy class can be constructed using all the accessible members
    of the wrapped object.
    """
    from App import Proxys

    className = getClassName(object)

    try:
        # Create an instance of the equivalent python wrapper class
        return Proxys.__dict__[className]
    except NameError:
        raise NameError( "No proxy class known for object of type '%s'." % className )    

def wrap( object, recursive = True ):
    """
    This function creates an instance of the correct proxy class to wrap the specified object.
    The proxy class will have the exact same name, but the class will come from the App.Proxys package.
    As a container, specify the object (not proxy object) to which this object will be attached with an HAS_A or HAS_MANY relationship.
    As an index, specify the position at which this object appear in the list of his container. Only useful with an HAS_MANY relationship.
    Pass recursive = False to only wrap non-object members. The members containing object references or list of objects will not be wrapped and will be None.  
    """
    if object is None : return None
    return getProxyClass(object).wrap( object, recursive )
    
def serialize(object):
    """Tries to create a serialized version of the object. Wrap it if needed."""
    try: proxyClass = getProxyClass(object)
    except AttributeError: 
        result = repr( object )
        if result[0]=='<' :
            raise TypeError( "Cannot wrap object " + result + ". Missing typeName() or proxy class." )
        return result
    return repr( getProxyClass(object).wrap( object, True ) )

def copy(object, *args):
    """Copy the object."""
    copiedWrapper = wrapCopy( object )
    try: copiedWrapper.name = copiedWrapper.name + "_Copy"
    except AttributeError: pass
    return copiedWrapper.createAndFillObject(None, *args)
    
def wrapCopy(object):
    """Wrap a copy of the object."""
    from App import Proxys
    return eval( serialize(object), Proxys.__dict__ )

def toVector3d( tuple ):
    """Converts a tuple to a Vector3d. If it is already a ThreeTuple, return it as a vector."""
    from MathLib import Vector3d, ThreeTuple
    if tuple is None: return None
    if isinstance( tuple, ThreeTuple ) : return Vector3d(tuple)
    if len(tuple) != 3:
        raise TypeError( "A Vector3D should be a 3-tuple" )
    return Vector3d( tuple[0], tuple[1], tuple[2] )
    
def fromVector3d( vec ):
    """Converts a Vector3D to a tuple"""
    if vec is None: return None
    return (vec.x, vec.y, vec.z)
    
def toPoint3d( tuple ):
    """Converts a tuple to a Point3D. If it is already a Point3D, return it."""
    from MathLib import Point3d, ThreeTuple
    if tuple is None: return None
    if isinstance( tuple, ThreeTuple ) : return Point3d(tuple)
    if len(tuple) != 3:
        raise TypeError( "A Point3D should be a 3-tuple" )
    return Point3d( tuple[0], tuple[1], tuple[2] )

def fromPoint3d( pt ):
    """Converts a Point3d to a tuple"""
    if pt is None: return None
    return (pt.x, pt.y, pt.z)
        
def angleAxisToQuaternion( tuple ):
    """Converts a 2-tuple (angle, (axis_x, axis_y, axis_z)) to a Quaternion. If it is already a Quaternion, return it."""
    from MathLib import Quaternion
    if tuple is None: return None
    if isinstance( tuple, Quaternion ) : return tuple
    if len(tuple) != 2:
        raise TypeError( "An angle-axis quaternion should be a 2-tuple" )
    q = Quaternion()
    q.setToRotationQuaternion( tuple[0], toVector3d( tuple[1] ) )
    return q
    
def angleAxisFromQuaternion( quat ):
    """Converts Quaterion to a 2-tuple (angle, (axis_x, axis_y, axis_z))"""
    if quat is None: return None
    return ( quat.getAngle(), fromVector3d( quat.getAxis() ) )
        
def toTrajectory1d( tupleList ):
    """Converts a list of 2-tuple to a Core.Trajectory1d"""
    from MathLib import Trajectory1d
    if tupleList is None: return None
    traj = Trajectory1d()
    for tuple in tupleList:
        if len(tuple) != 2 : raise TypeError( "A Trajectory1d should be a list of 2-tuples" )
        traj.addKnot( tuple[0], tuple[1] )    
    return traj
    
def fromTrajectory1d( traj ):
    """Converts Core.Trajectory1d to a list of 2-tuple"""
    if traj is None: return None
    return [ (traj.getKnotPosition(i), traj.getKnotValue(i)) for i in range(traj.getKnotCount()) ]
        
def fancify( expr ):
    """Add new line, spaces and intentation to the passed expression"""
    import ast
    import Fancify
    tree = ast.parse( expr )
    return Fancify.Fancify().visit(tree)

def sameObject( obj1, obj2 ):
    """True if the two objects are the same. Very similar to the "is" python keyword, but returns
    true if both objects are different SWIG wrapper of the same object."""
    try: 
        return obj1.this == obj2.this
    except AttributeError:
        return obj1 is obj2
    
def sameObjectInList( object, objectList ):
    """True if the object is in the list. See sameObject for an explanation on how the comparison is made."""
    for other in objectList:
        if sameObject(object, other):
            return True
    return False
    
def safeEqual( obj1, obj2 ):
    """True if both objects are None or both are equals."""
    try: return obj1 == obj2
    except (ValueError, TypeError): return False
    

def deprecated(func):
    """This is a decorator which can be used to mark functions
    as deprecated. It will result in a warning being emmitted
    when the function is used."""

    import warnings

    def newFunc(*args, **kwargs):
        warnings.warn("Call to deprecated function %s." % func.__name__,
                      category=DeprecationWarning, stacklevel = 2)
        return func(*args, **kwargs)
    newFunc.__name__ = func.__name__
    newFunc.__doc__ = func.__doc__
    newFunc.__dict__.update(func.__dict__)
    return newFunc

def unCamelCase(inString):
    """Takes a string in camel case and remove camel case, inserting spaces and capitalize."""
    import string
    result = []
    prev = 0
    for i, char in enumerate(inString):
        if i == 0: continue
        if char in string.uppercase:
            result.append( inString[prev:i] )
            prev = i
    result.append( inString[prev:] )
    return ' '.join(result).capitalize()
        
def convertController(controllerFileName, character = None ):
    """
    Takes the filename to an old-style controller file and converts it to the new format.
    If a character is provided it is used for loading, otherwise the 1st character of the application is used.
    """
    import wx, Core
    character = wx.GetApp().getCharacter( character )
    if character is None:
        character = wx.GetApp().getCharacter(0)
    if character is None:
        raise AttributeError( "No character provided, and none found in the application" )
    controller = Core.SimBiController(character)
    controller.loadFromFile( controllerFileName )
    
    print fancify( repr( wrap( controller ) ) )
    
def _buildGetterOrSetter( prefix, varName ):
    """Utility function used by getterName and setterName."""
    if varName[0] == '_' :
        varName = varName[1:]
    return prefix + varName[0].upper() + varName[1:]
    
def getterName( varName ):
    """
    Given that the string varName holds a variable (prefixed with _ or not), this method returns the standard
    name of the getter for that variable. i.e. 'getVarName'
    """
    return _buildGetterOrSetter( 'get', varName )
    
def setterName( varName ):
    """
    Given that the string varName holds a variable (prefixed with _ or not), this method returns the standard
    name of the setter for that variable. i.e. 'setVarName'
    """
    return _buildGetterOrSetter( 'set', varName )
    