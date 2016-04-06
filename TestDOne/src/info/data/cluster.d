module info.data.cluster;
////////////////////////////
import std.format;
/////////////////////
import info.data.distance;
import info.data.indiv;
//////////////////////////////////
enum DistanceMode {modeInvalid, modeUpUp, modeUpDown, modeDownUp, modeDownDown};
//////////////////////////////////////
enum LinkMode {linkInvalid, linkCenter, LinkMin, linkMax, linkMean};
////////////////////////////////
class Cluster(T=int,U=long) : Indiv!(T,U) {
	bool dirty;
	Indiv!(T,U)[] members;
	//
	this(in U aIndex=0, in T[] aData=[], in string sId=""){
		super(aIndex,aData,sId);
		members=[];
		dirty = false;
	}// this
	this(in Indiv!(T,U) aIndiv)
	in {
		assert(!(aIndiv is null));
		assert(aIndiv.is_valid);
	}
	body{
		members = [];
		members ~= aIndiv;
		dirty = true;
	}// this
} // class Cluster!(T,U)
///////////////////////////////////
//eof: cluster.d
