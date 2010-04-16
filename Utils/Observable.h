// An observable is a class that can hold a list of observers
// Based on the java.util.Observable class
#pragma once 

#include <Utils/UtilsDll.h>
#include <Utils/Utils.h>
#include <Utils/Observer.h>

#include <typeinfo>
#include <algorithm>


class UTILS_DECLSPEC Observable {

private:

	DynamicArray< Observer* > observers;
	bool hasChangedFlag;
	int batchChangeDepth;

protected:

	void setChanged() {
		hasChangedFlag = true;
	}

	void clearChanged() {
		hasChangedFlag = false;
	}

	// Notify observers that a change has occured
	// Doesn't notify if a batch change is going on.
	void notifyObservers( void* data = NULL ) {
		setChanged();
		notifyObserversIfChanged(data);
	}

	// Notify observers only if the object has been modified
	// Doesn't notify if a batch change is going on.
	void notifyObserversIfChanged( void* data = NULL ) {
		if( !isDoingBatchChanges() && hasChanged() ) {
			DynamicArray< Observer* >::iterator iter;
			for( iter = observers.begin(); iter != observers.end(); ++iter )
				(*iter)->update( data );
			clearChanged();
		}
	}

public:

	Observable() : hasChangedFlag(false), batchChangeDepth(0) {}

	virtual ~Observable() { 
		observers.clear(); 
	}

	bool hasChanged() const {
		return hasChangedFlag;
	}

	bool isDoingBatchChanges() const {
		return batchChangeDepth > 0;
	}

	void beginBatchChanges() {
		batchChangeDepth++;
	}

	void endBatchChanges() {
		batchChangeDepth--;
		notifyObserversIfChanged();
	}

	void addObserver( Observer* observer ) {
		observers.push_back( observer );
	}

	void deleteObserver( Observer* observer ) {
		observers.erase( std::remove( observers.begin(), observers.end(), observer ), observers.end() );
	}

	unsigned int countObservers() const {
		return observers.size();
	}

	const char* typeName(){
		return typeid(*this).name();
	}

};
