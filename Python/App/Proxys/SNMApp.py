'''
Created on 2009-09-02

@author: beaudoin
'''

import Proxy, Member
import App 

cls = App.ObservableList
ObservableList = Proxy.create( cls, 
    members = [
        Member.ObjectList( 'objects', None, cls.get, cls.getCount, cls.add, embedInParentNode = True ),
    ] )


cls = App.SnapshotBranch
SnapshotBranch = Proxy.create( cls, 
    nameGetter = lambda object: "Branch",
    icon = "../data/ui/snapshotBranch.png",    
    members = [
        Member.ObjectList( 'snapshots', None, cls.getSnapshot, cls.getSnapshotCount, None, embedInParentNode = True ),
    ] )

cls = App.Snapshot
Snapshot = Proxy.create( cls,
    nameGetter = cls.getName,
    icon = "../data/ui/snapshotIcon.png",    
    members = [
        Member.ObjectList( 'branches', None, cls.getBranch, cls.getBranchCount, None, embedInParentNode = True ),
    ] )
