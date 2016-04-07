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
	this(W,Z)(in size_t r, in size_t c, in W[] dd,
		in Z[] ni = null,
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
				string s = nn[i];
				if (s is null){
					names[i] = format("id%s",i+1);
				} else if (s.length < 1){
					names[i] = format("id%s",i+1);
				} else {
					names[i] = s;
				}
			}// i
		} else {
			for (size_t i = 0; i < r; ++i){
				ids[i] = format("id%s",i+1);
			}
		}
		if (!(ni is null)){
			U idmax = 0;
			for (size_t i = 0; i < r; ++i){
				U key = cast(U)ni[i];
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
		for (size_t irow = 0; irow < r; ++i){
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
	@property Indiv!(T,U)[] values() const 
	in {
		assert(this.is_valid());
	}
	body {
		return indivs.values;
	}// values
	Indiv!(T,U) get(in U key) const 
	in {
		assert(this.is_valid);
	}
	body {
		return (key in indivs) ? indivs[key] : null;
	}
	Z distance(Z) (in U index1, in U index2,
		in DistanceFunc!(T,Z) func) const
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
			return p1.distance(p2, func);
		} // distnce
	Z[] compute_distances(Z)(DistanceFunc!(T,Z) func = null) const 
	in {
		assert(this.is_valid);
	}
	body{
		if (func is null){
			func = new DistanceFunc!(T,U);
		}
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
				const Z r = p1.distance(p2,func);
				vRes[i*n + j] = r;
				vRes[j*n + i] = r;
			}// j
		}// i
		return vRes;
	}//commpute_distances
}// class IndData(T)