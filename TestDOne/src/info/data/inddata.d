//inddata.d
module info.data.inddata;
///////////////
import std.format;
//////////////////////
import info.data.distance;
import info.data.indiv;
//////////////////////////
class IndData(T=int, U = int) {
	private:	
		Indiv!(T,U)[U] indivs;
	public:
	//
	this(W)(in size_t r, in size_t c, in W[] dd,
		in U[] ni = null,
		in string[] nn = null)
	in {
		assert(r > 0);
		assert(c > 0);
		assert(!(dd is null));
		assert(dd.length >= cast(size_t)(r * c));
		if (!(nn is null)){
			assert(nn.length >= r);
		}
		if (!(ni is null)){
			assert(ni.length >= r);
		}
	}
	body {
		T[] data = [];
		string[] names = [];
		U[] ids = [];
		const size_t n = cast(size_t)(r * c);
		data.length = n;
		names.length = r;
		ids.length = r;
		for (size_t i = 0; i < n; ++i){
			data[i] = cast(T)dd[i];
		}// i
		if (!(nn is null)){
			for (size_t i = 0; i < r; ++i){
				const U index = cast(U)(i + 1);
				string s = nn[i];
				if (s is null){
					names[i] = format("id%s",index);
				} else if (s.length < 1){
					names[i] = format("id%s",index);
				} else {
					names[i] = s;
				}
			}// i
		} else {
			for (size_t i = 0; i < r; ++i){
				const U index = cast(U)(i + 1);
				string sx = format("id%s",index);
				names[i] = sx;
			}
		}
		if (!(ni is null)){
			U idmax = 0;
			for (size_t i = 0; i < r; ++i){
				U key = ni[i];
				if (key < 1){
					key = 1;
				}
				if (key > idmax){
					idmax = key;
				}
			}//i
			++idmax;
			U[U] temp = [0:0];
			for (size_t i = 0; i < r; ++i){
				U key = cast(U)ni[i];
				if (key < 1){
					key = 1;
				}
				if (key in temp){
					temp[idmax] = idmax;
					key = idmax++;
				} else {
					temp[key] = key;
				}
				ids[i] = key;
			}// i
		} else {
			for (size_t i = 0; i < r; ++i){
				ids[i] = cast(U)(i + 1);
			}//
		}
		U zero = 0;
		Indiv!(T,U) dummy = new Indiv!(T,U);
		indivs = [zero:dummy];
		for (size_t irow = 0; irow < r; ++irow){
			U aIndex = ids[irow];
			string sId = names[irow];
			indivs[aIndex] = new Indiv!(T,U)(aIndex,data[irow * c..(irow + 1)*c], sId);
		}// i
		indivs.remove(zero);
	}// this
	//
	@property bool is_valid() const {
		return (!(indivs is null)) && (indivs.length > 0);
	}// is_valid
	@property size_t count() const
	in {
		assert(this.is_valid);
	}
	body {
		return indivs.length;
	}
	@property U[] keys() const 
	in {
		assert(this.is_valid());
	}
	body {
		return indivs.keys;
	}// keys
	Indiv!(T,U) get(in U key) const 
	in {
		assert(this.is_valid);
	}
	body {
		if (!(key in indivs)){
			return null;
		}
		auto p = indivs[key];
		return new Indiv!(T,U)(p.index,p.data, p.id);
	}
	Z distance(Z) (in U index1, in U index2,
		in DistanceFunc!(T,Z) func,
	in size_t irow1 = 0, in size_t irow2 = 0) const
	in{
		assert(!(func is null));
	}
	out(result){
		assert(result >= 0);
	}
	body{
			Indiv!(T,U) p1 = this.get(index1);
			assert(!(p1 is null));
			Indiv!(T,U) p2 = this.get(index2);
			assert(!(p2 is null));
			return func(p1.data, p2.data,irow1,irow2);
		} // distnce
	Z[] compute_distances(Z)(in DistanceFunc!(T,Z) func) const 
	in {
		assert(this.is_valid);
		assert(!(func is null));
	}
	body{
		U[] kkeys = indivs.keys;
		assert(!(kkeys is null));
		const size_t n = kkeys.length;
		assert(n > 0);
		Z[] vRes = [];
		vRes.length = cast(size_t)(n * n);
		for (size_t i = 0; i < n; ++i){
			vRes[i * n + i] = 0;
			const U index1 = kkeys[i];
			Indiv!(T,U) p1 = this.get(index1);
			assert(!(p1 is null));
			for (size_t j = cast(size_t)(i + 1); j < n; ++j){
				const U index2 = kkeys[j];
				Indiv!(T,U) p2 = this.get(index2);
				assert(!(p2 is null));
				const Z r = func(p1.data,p2.data,i,j);
				vRes[i*n + j] = r;
				vRes[j*n + i] = r;
			}// j
		}// i
		return vRes;
	}//commpute_distances
}// class IndData(T)
////////////////////////////////
unittest
{
	//import std.stdio;
	import info.data.testdata;
	//
	const size_t nRows = TestData.socmortal_rows;
	const size_t nCols = TestData.socmortal_cols;
	const int[] gdata = TestData.socmortal_data;
	const string[] names = TestData.socmortal_inds;
	/////////////////////////////
	DistanceFunc!(int,long)[] dists = [];
	dists ~= new DistanceFunc!(int,long);
	dists ~= new ManhattanDistanceFunc!(int,long);
	dists ~= new EuclideDistanceFunc!(int,long);
	dists ~= new MaxDistanceFunc!(int,long);
	DistanceFunc!(int,float)[] distsf = [];
	distsf ~= new DivergenceDistanceFunc!(int,float);
	distsf ~= new KhiDeuxDistanceFunc!(int,float)(nRows, nCols,gdata);
	foreach (f ; dists){
		assert(f.is_valid);
	}// f
	foreach (f ; distsf){
		assert(f.is_valid);
	}// f
	/////////////////////////////
	auto pData = new IndData!(int,int)(nRows, nCols,gdata,null,names);
	assert(!(pData is null));
	assert(pData.is_valid);
	assert(pData.count == nRows);
	int[] keys = pData.keys;
	assert(!(keys is null));
	const size_t n = keys.length;
	assert(n == nRows);
	foreach (index; keys){
		assert(index > 0);
		Indiv!(int,int) p = pData.get(index);
		assert(!(p is null));
		assert(p.is_valid);
	}// key
    /////////////////////////////
    for (size_t i = 0; i < n; ++i){
    	const int index1 = keys[i];
    	Indiv!(int,int) p1 = pData.get(index1);
    	assert(!(p1 is null));
    	assert(p1.is_valid);
    	for (size_t j = 0; j < i; ++j){
    		const int index2 = keys[j];
    		Indiv!(int,int) p2 = pData.get(index2);
    	     assert(!(p2 is null));
    	     assert(p2.is_valid);
    	    long[] res = [];
    	    float[] resf = []; 
    	    foreach (f ; dists){
				res ~= pData.distance(index1,index2,f,i,j);
				}// f
			foreach (f ; distsf){
				resf ~= pData.distance(index1,index2,f,i,j);
				}// f
		//string sout = format("%s --> %s\t%s\t%s",p1.id,p2.id, res,resf);
		//writeln(sout);	
    	} // j
    }// i
    ////////////////////////////////
    long[] dd = pData.compute_distances(dists[0]);
    assert(!(dd is null));
    assert(dd.length == cast(size_t)(n * n));
    ////////////////////////////////
}// unittest
