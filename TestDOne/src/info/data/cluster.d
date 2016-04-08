module info.data.cluster;
////////////////////////////
import std.format;
/////////////////////
import info.data.distance;
import info.data.indiv;
//////////////////////////////////
enum DistanceMode {modeInvalid, modeUpUp, modeUpDown, modeDownUp, modeDownDown}
enum LinkMode {linkInvalid, linkCenter, LinkMin, linkMax, linkMean}
////////////////////////////////
class Cluster(T=int,U=long) : Indiv!(T,U) {
	static assert((U.stringof == "int")||(U.stringof == "short") || (U.stringof == "long") || (U.stringof == "uint")||(U.stringof == "ushort") || (U.stringof == "ulong"));
	bool dirty;
	Indiv!(T,U)[] members;
	//
	this(){
		super();
		dirty = false;
		members = [];
	}
	this(in U aIndex, in T[] aData, in string sId){
		super(aIndex,aData,sId);
		members=[];
		dirty = false;
	}// this
	this(Indiv!(T,U) aIndiv)
	in {
		assert(!(aIndiv is null));
		assert(aIndiv.is_valid);
	}
	body{
		super();
		members = [];
		members ~= aIndiv;
		this.index = aIndiv.index;
		this.id = aIndiv.id;
		this.data = aIndiv.data.dup;
		dirty = false;
	}// this
	override bool check() const {
		return super.check() && (!(members is null));
	}
	@property bool empty() const
	in {
		assert(this.is_valid);
	}
	body {
		return (this.members.length < 1);
	}
	void clear_indivs(){
		if (this.dirty){
			this.update_center();
		}
		this.members = [];
	}// clear_indivs
	void update_center(){
		this.dirty = false;
	}// update_center
	bool contains_index(in U index) const
	in {
		assert(this.is_valid);
		assert(index > 0);
	}
	body {
		foreach (p ; members){
			assert(!(p is null));
			assert(p.is_valid);
			if (p.index == index){
				return true;
			}
		}// p
		return false;
	}// contains_index
	bool contains_id(in string sId) const
	in {
		assert(this.is_valid);
		assert(!(sId is null));
	}
	body {
		foreach (p ; members){
			assert(!(p is null));
			assert(p.is_valid);
			if (p.id == sId){
				return true;
			}
		}// p
		return false;
	}// contains_index
	bool contains_indiv(in Indiv!(T,U) aIndiv) const
	in {
		assert(this.is_valid);
		assert(!(aIndiv is null));
		assert(aIndiv.is_valid);
	}
	body {
		if (this.contains_index(aIndiv.index)){
			return (true);
		}
		return this.contains_id(aIndiv.id);
	}// contains_indiv
	bool add_indiv(Indiv!(T,U) aIndiv) 
	in {
		assert(this.is_valid);
		assert(!(aIndiv is null));
		assert(aIndiv.is_valid);
	}
	body {
		if (this.contains_indiv(aIndiv)){
			return (false);
		}
		members ~= aIndiv;
		dirty = true;
		return true;
	}// contains_indiv
} // class Cluster!(T,U)
/////////////////////////////////
unittest {
	import std.stdio;
	import info.data.inddata;
	import info.data.testdata;
	//
	const size_t nRows = TestData.socmortal_rows;
	const size_t nCols = TestData.socmortal_cols;
	const int[] gdata = TestData.socmortal_data;
	const string[] names = TestData.socmortal_inds;
	/////////////////////////////
	IndData!(int,int) pData = new IndData!(int,int)(nRows, nCols,gdata,null,names);
	assert(!(pData is null));
	assert(pData.is_valid);
	assert(pData.count == nRows);
	IndDataProvider!(int,int)  pProvider = new IndDataProvider!(int,int)(pData);
    assert(!(pProvider is null));
    assert(pProvider.is_valid);
    
    {
    	pProvider.reset();
    	Cluster!(int,int) c0 = new Cluster!(int,int);
    	assert(!(c0 is null));
    	assert(!c0.is_valid);
    	Indiv!(int,int) p = pProvider.next();
    	assert(!(p is null));
    	assert(p.is_valid);
    	Cluster!(int,int) c1 = new Cluster!(int,int)(p.index,p.data,p.id);
    	assert(!(c1 is null));
    	assert(c1.is_valid);	
    	Cluster!(int,int) c2 = new Cluster!(int,int)(p);
    	assert(!(c2 is null));
    	assert(c2.is_valid);
    }
    {
    	pProvider.reset();
    	Indiv!(int,int) p = pProvider.next();
    	assert(!(p is null));
    	assert(p.is_valid);
    	Cluster!(int,int) c1 = new Cluster!(int,int)(p);
    	assert(!(c1 is null));
    	assert(c1.is_valid);
    	while (true){
    		Indiv!(int,int) px = pProvider.next();
    		if (px is null){
    			break;
    		}
    	 bool b = c1.contains_indiv(px);
    	 assert(!b);
    	 b = c1.add_indiv(px);
    	 assert(b);	
    	 b = c1.contains_indiv(px);
    	 assert(b);
    	}// while
    }
}// unittest
///////////////////////////////////
//eof: cluster.d
