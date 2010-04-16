'''
Created on 2009-09-24

@author: beaudoin
'''

import Utils, wx, PyUtils
import Member
from UI.InfoTree import getIconIndex
    
class NodeData(Utils.Observer):
    """
    This class wraps one object presented in a node of the tree.
    The wrapped object must be observable (either derived from Utils.Observable for a C++ object
    or App.Observable for a Python object).
    An object keeps an ordered list of its child objects. 
    An object also has a list of properties that can be edited or not.
    """

    def __init__(self, memberName, object, tree, treeItemId, container = None, index = None):
        """Subclasses must calls this constructor.
        Pass a string giving the member name for this node (member should be either App.Proxys.Member.Object or App.Proxys.Member.ObjectList.          
        Pass the object to observe, the member information for this object (from App.Proxys.Member).
        Also pass the tree and the treeItemId where this object is found.
        In container, pass the object that contains this node (in an HAS_A or HAS_MANY relationship)
        In index, pass the index of this node within its container (in an HAS_MANY relationship)        
        """
        super( NodeData, self ).__init__()
        self._memberName = memberName
        self._object = object
        self._tree = tree
        self._treeItemId = treeItemId
        self._container = container
        self._index = index
        if object is None :
            self._proxyClass = None
            iconIndex = getIconIndex( '../data/ui/noneIcon.png' )
        else:
            object.addObserver( self )
            self._proxyClass = PyUtils.getProxyClass(object)
            iconIndex = getIconIndex( self._proxyClass.getIcon() )
        self._tree.SetItemImage( self._treeItemId, iconIndex )

        self._subMemberNodes = []
        if self._proxyClass is not None :
            for subMember in self._proxyClass.getObjectMembers():
                self._subMemberNodes.append( _createSubMemberNode(subMember, self._tree, self._treeItemId, object) )

        tree.SetItemPyData( treeItemId, self )
        
        # Initial update
        self.update()

    def __del__(self):
        try: self._object.deleteObserver( self )
        except AttributeError: pass

    def update(self, data = None):
        """Called whenever the attached object is updated."""
        self._tree.updateLock()
        if self._object is not None :
            name = self._proxyClass.getName(self._object)
        else:
            name = "None (%s)" % self._memberName
        self._tree.SetItemText( self._treeItemId, name )
        for subMemberNode in self._subMemberNodes:
            subMemberNode.update( self._object )
        self._tree.updateUnlock()
     
    def getObject(self):
        """Access the object attached to this NodeData."""
        return self._object
    
def _createSubMemberNode( subMember, tree, parentTreeItemId, container ):
    """
    Private. 
    Adds a node to the tree if the subMember is a Member.Object or a non-embedded Member.ObjectList.
    If it is an embedded Member.ObjectList, no node is added.
    Then creates a subMemberNode that contains enough information to watch this member.
    """
    if isinstance( subMember, Member.Object ):        
        return subMemberObjectNode(subMember, tree, container, parentTreeItemId)
    elif isinstance( subMember, Member.ObjectList ):
        if subMember.embedInParentNode:
            return subMemberEmbeddedObjectListNode(subMember, tree, container, parentTreeItemId)            
        else:
            return subMemberObjectListNode(subMember, tree, container, parentTreeItemId)
    else:
        raise TypeError( "Unknown object member type: '%s'." % subMember.__class__.__name__ )

class subMemberNode(object):
    """Private class. Information for a submember node."""
    
    def __init__(self, subMember, tree, container):
        self._member = subMember
        self._tree = tree
        self._container = container
       
class subMemberObjectNode(subMemberNode):
    """Private class. Information for a submember node that wraps a single object."""
    
    def __init__(self, subMember, tree, container, parentTreeItemId):
        """Create a new node inside the tree that wraps the specified submember"""
        super(subMemberObjectNode,self).__init__(subMember, tree, container)
        self._treeItemId = tree.AppendItem( parentTreeItemId, "" )
        if tree.isAutoVisible():
            tree.EnsureVisible(self._treeItemId)

    def getObject(self):
        """Returns the object associated with this node, or None if no object is associated."""
        nodeData = self._tree.GetItemPyData( self._treeItemId )
        if nodeData is None : 
            raise AttributeError( 'No data attached to tree node!' );
        return nodeData.getObject()

    def update(self, object):
        """Updates the node so that it contains the specified object."""
        subObject = self._member.getObject(object)
        try:
            currentObject = self.getObject() 
            if PyUtils.sameObject(currentObject, subObject) : return
        except AttributeError: pass
        nodeData = NodeData( self._member.name, subObject, self._tree, self._treeItemId, self._container )
    
class subMemberAbstractObjectListNode(subMemberNode):
    """
    Private class. provides some methods shared by embedded and non-embedded node lists.
    Subclasses must define insertChild()
    """

    def __init__(self, subMember, tree, container):
        super(subMemberAbstractObjectListNode,self).__init__(subMember, tree, container)    
        self._subTreeItemIds = []

    def getObject(self, index):
        """Returns the object number index associated with this node, or None if no object is associated.
        Raises a IndexError if the index is out of bound"""
        nodeData = self._tree.GetItemPyData( self._subTreeItemIds[index] )
        if nodeData is None : 
            raise AttributeError( 'No data attached to tree node!' );
        return nodeData.getObject()

    def removeChildren(self, start=None, end=None):
        """Private. This method removes objects from a tree and a children list.
        Objects removed are in the range [start,end).
        Runs from object 0 when start==None and until the end when end==None."""
        if start is None : start = 0
        if end is None : end = len( self._subTreeItemIds )
        for child in self._subTreeItemIds[start:end]:
            self._tree.Delete(child)
        del self._subTreeItemIds[start:end]
        
    def update(self, object):
        """Adjusts the node's _subTreeItemIds list so that it matches an input object list.
        TreeItemId s will be deleted or inserted into _subTreeItemIds based on its current content and
        that of the input object list."""

        count = self._member.getCount(object)
        for i in range(count):
            inObject = self._member.getObject(object,i)
            try: 
                outObject = self.getObject(i)
                areSameObject = PyUtils.sameObject(inObject, outObject)
            except IndexError: 
                outObject = None
                areSameObject = False        
            if not areSameObject:
                # Need to delete or insert
                # First, check how many objects we should delete
                delete = 1 
                try: 
                    while not PyUtils.sameObject( inObject, self.getObject(i+delete) ) :
                        delete += 1
                except IndexError:
                    delete = 0
            
                if delete > 0 :
                    # Delete the specified objects
                    self.removeChildren(i,i+delete)
                else :
                    # Insert the specified object                                                        
                    self.insertChild(i, inObject)
        
        # Delete any remaining objects
        self.removeChildren(count)
        
class subMemberObjectListNode(subMemberAbstractObjectListNode):
    """Private class. Information for a submember node that wraps a single object."""
    
    def __init__(self, subMember, tree, container, parentTreeItemId):
        """Create a new node inside the tree that wraps the specified submember"""
        super(subMemberObjectListNode,self).__init__(subMember, tree, container)
        self._listNodeTreeItemId = tree.AppendItem( parentTreeItemId, subMember.fancyName )
        tree.SetItemPyData( self._listNodeTreeItemId, (subMember, container) )
        iconIndex = getIconIndex( subMember.groupIcon )
        tree.SetItemImage( self._listNodeTreeItemId, iconIndex )
        if tree.isAutoVisible():
            tree.EnsureVisible(self._listNodeTreeItemId)

    def insertChild(self, pos, object):
        """Private. Wraps the specified object in a new NodeData and inserts it 
        at the specified position in the tree."""
        treeItemId = self._tree.InsertItemBefore( self._listNodeTreeItemId, pos, "" )
        nodeData = NodeData( self._member.name, object, self._tree, treeItemId, self._container, pos )
        self._subTreeItemIds.insert(pos, treeItemId)
        if self._tree.isAutoVisible():
            self._tree.EnsureVisible(treeItemId)
         
    
class subMemberEmbeddedObjectListNode(subMemberAbstractObjectListNode):
    """Private class. Information for a submember node that wraps a single object."""
    
    def __init__(self, subMember, tree, container, parentTreeItemId):
        """Create a new node inside the tree that wraps the specified submember"""
        super(subMemberEmbeddedObjectListNode,self).__init__(subMember, tree, container)
        self._parentTreeItemId = parentTreeItemId
        self._previousTreeItemId = tree.GetLastChild(parentTreeItemId)  # Will be invalid if there is no last child
        
    def insertChild(self, pos, object):
        """Private. Wraps the specified object in a new NodeData and inserts it 
        at the specified position in the tree."""
        if pos == 0:
            # Insertion at the beginning, tricky.
            if len(self._subTreeItemIds) == 0 :
                # Nothing in the list, add after self._previousTreeItemId
                if not self._previousTreeItemId.IsOk():
                    # At the very top of the tree
                    treeItemId = self._tree.InsertItemBefore( self._parentTreeItemId, 0, "" )
                else:
                    treeItemId = self._tree.InsertItem( self._parentTreeItemId, self._previousTreeItemId, "" )
            else:
                # Insert before first object in the tree
                prevItemId = self.tree.GetPrevSibling( self._parentTreeItemId, self._subTreeItemIds[0], "" )
                if not prevItemId.IsOk():
                    # At the very top of the tree
                    treeItemId = self._tree.InsertItemBefore( self._parentTreeItemId, 0, "" )
                else:
                    treeItemId = self._tree.InsertItem( self._parentTreeItemId, prevItemId, "" )
        else:
            # Insertion in the middle, easy
            treeItemId = self._tree.InsertItem( self._parentTreeItemId, self._subTreeItemIds[pos-1], "" )
        
        nodeData = NodeData( self._member.name, object, self._tree, treeItemId, self._container, pos )
        self._subTreeItemIds.insert(pos, treeItemId)
        if self._tree.isAutoVisible():
            self._tree.EnsureVisible(treeItemId)
        