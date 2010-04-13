'''
Created on 2009-10-02

@author: beaudoin
'''

import PyUtils, Core, time, Physics, Utils, wx

class Snapshot(PyUtils.Observable):
    """This class contains a snapshot. That is, the entire state of the world, the controllers and the character."""
    
    def __init__(self, parentBranch):
        """Takes a shot of a world, add it to the specified branch."""
        super(Snapshot,self).__init__()

        self._time = time.localtime()
        
        # Save the world
        world = Physics.world()
        self._worldState = Utils.DynamicArrayDouble() 
        world.getState( self._worldState )
        
        # Save the controllers
        app = wx.GetApp()        
        self._controllers = []
        for i in range(app.getControllerCount()):
            controller = app.getController(i)
            controllerState = Core.SimBiControllerState()
            controller.getControllerState( controllerState )
            self._controllers.append( (controllerState, PyUtils.wrapCopy( controller )) )

        self._parentBranch = parentBranch
        self._childBranches = []
        self._activeIndex = -1                
    
    #
    # Public methods
    #
    
    def restore(self, restoreControllerParams = True):
        """Restores this snapshot, sets it as active."""
        self._parentBranch._setActiveSnapshot( self )
        self._restore(restoreControllerParams)
        self.notifyObservers()
        
    def getBranch(self, index):
        """Retrieves a branch."""
        return self._childBranches[index]
        
    def getBranchCount(self):
        """Retrieves the number of branches."""
        return len( self._childBranches )
        
    def getName(self):
        """Retrieves a string for that snapshot."""
        return "Snapshot at %(time)s" % { "time" : time.strftime("%H:%M:%S", self._time) }
    
    #
    # Private methods
    #
        
    def _takeSnapshot(self):
        """Takes a snapshot in the active branch, or add a new active branch if required."""
        if self._activeIndex != -1 :
            snapshot = self._childBranches[self._activeIndex].takeSnapshot()
            if snapshot is not None : return snapshot

        # Need to add a new branch
        self._childBranches.append( SnapshotBranch(self) )
        self._activeIndex = len(self._childBranches) - 1        
        self.notifyObservers()
        return self._childBranches[self._activeIndex].takeSnapshot()
    
    def _restore(self, restoreControllerParams = True):
        """Restores this snapshot, does sets it as active. Should only be called by SnapshotBranch."""
        if self._activeIndex >= 0 :
            # Make sure the selected branch is deactivated
            self._childBranches[self._activeIndex]._deactivate()

        # Restore the world
        world = Physics.world()
        world.setState( self._worldState )
        
        # Restore the controllers
        app = wx.GetApp()        
        assert app.getControllerCount() == len(self._controllers), "Controller list doesn't match snapshot!"
        
        for i in range(app.getControllerCount()):
            controller = app.getController(i)
            controllerState, wrappedController = self._controllers[i]
            if restoreControllerParams :
                controller.beginBatchChanges()
                try: wrappedController.fillObject(controller)
                finally: controller.endBatchChanges()
            controller.setControllerState( controllerState )            
        self.notifyObservers()

        
    def _deactivateAllBranches(self):
        """Indicates that no branch should be active. In other words, the main branch is active."""
        self._activeIndex = -1
                    
    def _setActiveBranch(self, branch):
        """Indicates that the specified branch should be the active one."""
        self._parentBranch._setActiveSnapshot(self)
                    
        if self._activeIndex < 0 or branch is not self._childBranches[self._activeIndex] :
            for i, myBranch in enumerate( self._childBranches ) :
                if myBranch is branch :
                    self._activeIndex = i
                    return
                
            raise RuntimeError( "Desired active branch not found!" )

    def _getCurrentSnapshot(self):
        if self._activeIndex < 0 : return self
        return self._childBranches[self._activeIndex].getCurrentSnapshot()
        
    def _restoreActive(self, restoreControllerParams = True):
        """Restores the active snapshot. Can be this one or one in its subbranches."""
        if self._activeIndex >= 0 : 
            snapshot = self._childBranches[self._activeIndex].restoreActive(restoreControllerParams)
            if snapshot is not None:
                return snapshot
        self._restore(restoreControllerParams)
        return self

    def _previousSnapshot(self, restoreControllerParams = True):
        """Restore previous snapshot and set it as active. Return false if no previous snapshot could be restored,
        in which case the previous branch should navigate."""                
        if self._activeIndex < 0 : return None
        return self._childBranches[self._activeIndex].previousSnapshot(restoreControllerParams)
            
    def _nextSnapshot(self, restoreControllerParams = True):
        """Navigates down active branch. Return false if no active branch."""
        if self._activeIndex < 0 : return None
        return self._childBranches[self._activeIndex].nextSnapshot(restoreControllerParams)
            
    
    
class SnapshotBranch(PyUtils.Observable):
    """A list of world snapshots, including controller states and parameters."""
    
    def __init__(self, parentSnapshot = None):
        """Pass the parent snapshot or None if this is the root branch."""
        super(SnapshotBranch,self).__init__()
        
        self._parentSnapshot = parentSnapshot
             
        self._snapshots = []
        self._activeIndex = -1
        
    #
    # Public methods
    #
        
    def takeSnapshot(self):
        """Take a new snapshot and add it at the correct position in this branch, or one of its subbranches.
        Return the snapshot if it was taken, None otherwise."""
        if self._activeIndex == len(self._snapshots) - 1:
            snapshot = Snapshot(self)
            self._snapshots.append( snapshot )
            self._activeIndex = len(self._snapshots) - 1
            self.notifyObservers()
            return snapshot
        elif self._activeIndex > -1 :
            return self._snapshots[self._activeIndex]._takeSnapshot()
        else:
            return None            

    def getCurrentSnapshot(self):
        """Returns the currently active snapshot."""
        if self._activeIndex < 0 : return self._parentSnapshot
        return self._snapshots[self._activeIndex]._getCurrentSnapshot()
    
    def restoreActive(self, restoreControllerParams = True):
        """Restores the currently active snapshot. Returns false if nothing was restored. """
        if self._activeIndex < 0 : 
            return None
        return self._snapshots[self._activeIndex]._restoreActive(restoreControllerParams)
        
    def previousSnapshot(self, restoreControllerParams = True):
        """Restore previous snapshot and set it as active. Return false if no previous snapshot could be restored,
        in which case the previous branch should navigate."""                
        if self._activeIndex < 0 :
            return None
        
        snapshot = self._snapshots[self._activeIndex]._previousSnapshot(restoreControllerParams)
        if snapshot is not None :
            return snapshot
        
        # Cannot be handled by active snapshot, go down this branch
        if self._activeIndex == 0 and self._parentSnapshot is None :
            return self._snapshots[0]
        self._activeIndex -= 1
        if self._activeIndex == -1:
            self._parentSnapshot._restore(restoreControllerParams)
            return self._parentSnapshot

        self._snapshots[self._activeIndex]._deactivateAllBranches()
        self._snapshots[self._activeIndex]._restore(restoreControllerParams)
        return self._snapshots[self._activeIndex]
        
    def nextSnapshot(self, restoreControllerParams = True):
        """Restore next snapshot and set it as active. Return false if no next snapshot could be restored."""                
        if self._activeIndex >= 0 :
            snapshot = self._snapshots[self._activeIndex]._nextSnapshot(restoreControllerParams)
            if snapshot is not None:
                return snapshot
            
        if self._activeIndex < len( self._snapshots ) - 1 :
            self._activeIndex += 1
        
        self._snapshots[self._activeIndex]._restore(restoreControllerParams)
        return self._snapshots[self._activeIndex]
        
    def getSnapshot(self, index):
        """Access the snapshot at the specified index. Will not change the active snapshot.""" 
        return self._snapshots[index]

    def getSnapshotCount(self):
        """Access the total number of snapshots.""" 
        return len(self._snapshots)

    #
    # Private methods
    #

    def _deactivate(self):
        """Indicates that this branch is not active and the parent snapshot should be selected."""
        self._activeIndex = -1


    def _setActiveSnapshot(self, snapshot):
        """Makes sure the specified snapshot is the active one."""
        if self._parentSnapshot is not None:
            self._parentSnapshot._setActiveBranch(self)
                    
        if self._activeIndex < 0 or snapshot is not self._snapshots[self._activeIndex] :
            for i, mySnapshot in enumerate( self._snapshots ) :
                if mySnapshot is snapshot :
                    self._activeIndex = i
                    return
                
            raise RuntimeError( "Desired active snapshot not found!" )
