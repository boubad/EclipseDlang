module info.data.distance;
///////////////////////
import std.math;
////////////////////
enum DistanceType {
	distanceInvalid,
	distanceSquare,
	distanceManhattan,
	distanceEuclide,
	distanceMax,
	distanceDivergence,
	distanceKhiDeux,
	distanceVariance,
	distanceWeighted,
	indiceRusselAndRao,
		indiceJaccard,
		indiceDice,
		indiceSokalAndSneath_2,
		indiceKulczinski_1,
		indiceKulczinski_2,
		indiceOchiai,
		indiceSimpson,
		indiceKochenAndWong,
		indiceSokalAndMichener,
		indiceSokalAndSneath_1,
		indiceRogersAndTanimoto,
		indiceSokalAndSneath_4,
		indiceSokalAndSneath_5,
		indiceRoux_1,
		Hamann,
		indiceYule,
		indicePhiPearson
}
////////////////////
class DistanceFunc(T=int,Z=long) {
	protected:
		bool _valid;
		DistanceType _distanceType;
	public:
	this(){
		_valid = true;
		_distanceType = DistanceType.distanceSquare;
	}
	@property bool is_valid() const {
		return _valid;
	}
	@property DistanceType type() const{
		return _distanceType;
	}
	Z opCall(in T[] data1, in T[] data2, in size_t irow1 = 0, in size_t irow2 = 0) const
	in {
		assert(this.is_valid);
		assert(!(data1 is null));
		assert(!(data2 is null));
		assert(data2.length == data2.length);
	}
	out(result){
		assert(result >= 0);
	}
	body{
		return cast(Z)perform_compute(data1,data2, irow1, irow2);
	}// opCall
	protected:
		real compute_one_step(in T v1, in T v2, in  size_t icol, in size_t irow1, in size_t irow2) const 
		{
			real r = v1 - v2;
			return (r * r);
		}// compute_one_step
		real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
		{
			real s = 0;
			immutable size_t n = data1.length;
			for (size_t i = 0; i < n; ++i){
				s  += compute_one_step(data1[i],data2[i],i, irow1, irow2);
			}// i
			if (n > 1){
				s /= n;
			}
			return s;
		}// opCall
}// class DistanceFunc!(T,Z)
/////////////////////////////////////////
class ManhattanDistanceFunc(T=int,Z=long) : DistanceFunc!(T,Z)
{
    this()
    {
        super();
        _distanceType = DistanceType.distanceManhattan;
    }
protected:
    override real compute_one_step(in T v1, in T v2, in size_t icol, in size_t irow1, in size_t irow2) const
    {
        real r = v1 - v2;
        return abs(r);
    }// compute_one_step
}// class ManhattanDistanceFunc
////////////////////////////////
class EuclideDistanceFunc(T=int,Z=long) : DistanceFunc!(T,Z)
{
    this()
    {
        super();
        _distanceType = DistanceType.distanceEuclide;
    }
protected:
    override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
    {
        real s = super.perform_compute(data1, data2, irow1, irow2);
        return sqrt(s);
    }// perform_compute
}// class EuclideDistanceFunc
/////////////////////////////////////////////
class KhiDeuxDistanceFunc(T=int,Z=double) : DistanceFunc!(T,Z){
	static assert((Z.stringof == "float")||(Z.stringof == "double")||(Z.stringof == "real"));
	private:
		real[] _rowsum;
		real[] _colsum;
	public:
		this(in size_t nRows, in size_t nCols, in T[] data)
		in {
			assert(nRows > 0);
			assert(nCols > 0);
			assert(!(data is null));
			assert(data.length >= cast(size_t)(nRows * nCols));
		}
		body {
			this._valid = true;
			this._distanceType = DistanceType.distanceKhiDeux;
			_rowsum = [];
			_colsum = [];
			_rowsum.length = nRows;
			_colsum.length = nCols;
			real total = 0;
			for (size_t icol = 0; icol < nCols; ++icol){
				_colsum[icol] = 0;
				for (size_t irow = 0; irow < nRows; ++irow){
					_colsum[icol] += data[irow * nCols + icol];
				}// irow
				if (_colsum[icol] <= 0){
					this._valid = false;
				}
				total += _colsum[icol];
			}// icol
			for (size_t icol = 0; icol < nCols; ++icol){
				_colsum[icol] /= total;
			}
			for (size_t irow = 0; irow < nRows; ++irow){
				_rowsum[irow] = 0;
				for (size_t icol = 0; icol < nCols; ++icol){
					_rowsum[irow] += data[irow * nCols + icol];
				}// irow
				if (_rowsum[irow] <= 0){
					this._valid = false;
				}
			}// icol
		}// body
		protected:
 			 override real compute_one_step(in T v1, in T v2, in size_t icol, in size_t irow1, in size_t irow2) const
    			{
    				assert(this.is_valid);
    				assert(icol < _colsum.length);
    				assert(irow1 < _rowsum.length);
    				assert(irow2 < _rowsum.length);
        			real r = (v1/_rowsum[irow1]) - (v2/ _rowsum[irow2]);
        			return (r * r) * _colsum[icol];
    			}// compute_one_step
			override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
				{
					real s = 0;
					immutable size_t n = data1.length;
					for (size_t i = 0; i < n; ++i){
						s  += compute_one_step(data1[i],data2[i],i, irow1, irow2);
					}// i
					return s;
		}// perform_compute
}// KhiDeuxDistanceFunc(T=int,Z=long)
//////////////////////////////////////
class VarianceDistanceFunc(T=int,Z=double) : DistanceFunc!(T,Z){
	private:
		real[] weights;
	public:
		this(W)(in size_t nRows, in size_t nCols, in W[] gdata)
		in {
			assert(nRows > 0);
			assert(nCols > 0);
			assert(!(gdata is null));
			assert(gdata.length >= cast(size_t)(nRows * nCols));
		}
		body {
			this._valid = true;
			this._distanceType = DistanceType.distanceVariance;
			weights  = [];
			weights.length = nCols;
			for (size_t icol = 0; icol < nCols; ++icol){
				real s1 = 0;
				real s2 = 0;
				for (size_t i = 0; i < nRows; ++i){
					real x = cast(real)gdata[i * nCols + icol];
					s1  += x;
					s2 += x * x;
					}// i
				const real moy = s1 / nRows;
				s2 = (s2/nRows) - (moy * moy);
				assert(s2 > 0);
				if (s2 <= 0){
					this._valid = false;
				} else{
				 weights[icol] = 1.0/s2;	
				}
			}// icol
		}// body
		protected:
 			 override real compute_one_step(in T v1, in T v2, in size_t icol, in size_t irow1, in size_t irow2) const
    			{
    				assert(this.is_valid);
    				assert(icol < weights.length);
        			real r =  v1 - v2;
        			return (r * r) * weights[icol];
    			}// compute_one_step
			override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
				{
					real s = 0;
					immutable size_t n = data1.length;
					for (size_t i = 0; i < n; ++i){
						s  += compute_one_step(data1[i],data2[i],i, irow1, irow2);
					}// i
					return s;
		}// perform_compute
}// VarianceDistanceFunc(T=int,Z=long)
/////////////////////////////////////
class DivergenceDistanceFunc(T=int,Z=double) : DistanceFunc!(T,Z){
	static assert((Z.stringof == "float")||(Z.stringof == "double")||(Z.stringof == "real"));
	public:
		this()
		{
			super();
			this._distanceType = DistanceType.distanceDivergence;
		}// body
		protected:
 			 override real compute_one_step(in T v1, in T v2, in size_t icol, in size_t irow1, in size_t irow2) const
    			{
    				real res = 0;
    				real den = v1 + v2;
    				if (den != 0){
    					res = abs((v1 - v2)/den);
    				}
    				return res;
    			}// compute_one_step
			override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
				{
					real s = 0;
					immutable size_t n = data1.length;
					for (size_t i = 0; i < n; ++i){
						s  += compute_one_step(data1[i],data2[i],i, irow1, irow2);
					}// i
					return s;
		}// perform_compute
}// DivergenceDistanceFunc(T=int,Z=long)
///////////////////////////////////////////
class MaxDistanceFunc(T=int,Z=double) : DistanceFunc!(T,Z){
	public:
		this()
		{
			super();
			this._distanceType = DistanceType.distanceMax;
		}// body
		protected:
 			 override real compute_one_step(in T v1, in T v2, in size_t icol, in size_t irow1, in size_t irow2) const
    			{
    				return abs(v1 - v2);
    			}// compute_one_step
			override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
				{
					real s = 0;
					immutable size_t n = data1.length;
					for (size_t i = 0; i < n; ++i){
						real v = compute_one_step(data1[i],data2[i],i, irow1, irow2);
						if (i == 0){
							s = v;
						} else if (v > s){
							s = v;
						}
						return s;
					}// i
					return s;
		}// perform_compute
}// MaxDistanceFunc(T=int,Z=long)
/////////////////////////////////////////////
class WeightedDistanceFunc(T=int,Z=long) : DistanceFunc!(T,Z)
{
public:
    const DistanceFunc!(T,Z) distance_function;
    real[] weights;
    //
    this(W)(in W[] pw, in DistanceFunc!(T,Z) fn)
    in
    {
        assert(!(fn is null));
        assert(!(pw is null));
        assert(pw.length > 0);
    }
    body
    {
    	this._valid = true;
    	this._distanceType = DistanceType.distanceWeighted;
        distance_function = fn;
        weights = [];
        const size_t n = pw.length;
        weights.length = n;
        for (size_t i = 0; i < n; ++i){
        	real v = cast(real)pw[i];
        	if (v < 0){
        		this._valid = false;
        	}
        	weights[i] = v;
        }// i
    }
protected:
    override real perform_compute(in T[] data1, in T[] data2, in size_t irow1, in size_t irow2) const
    {
        real s = 0;
        immutable size_t n = data1.length;
        assert(weights.length >= n);
        for (size_t i = 0; i < n; ++i)
        {
            real r = distance_function.compute_one_step(data1[i],data2[i],i, irow1, irow2);
            real c = weights[i];
            assert(c >= 0);
            s  +=  c * r;
        }// i
        return s;
    }// perform_compute
} // class WeightedDistanceFunc
////////////////////////////////
unittest
{
	//import std.stdio;
	//import std.format;
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
	distsf ~= new VarianceDistanceFunc!(int,float)(nRows, nCols,gdata);
	distsf ~= new DivergenceDistanceFunc!(int,float);
	distsf ~= new KhiDeuxDistanceFunc!(int,float)(nRows, nCols,gdata);
	/////////////////////////////
	foreach (f ; dists){
		assert(f.is_valid);
	}// f
	foreach (f ; distsf){
		assert(f.is_valid);
	}// f
    /////////////////////////////
    for (size_t i = 0; i < nRows; ++i){
    	string s1 = names[i];
    	const int[] data1 = gdata[i*nCols..(i+1)*nCols];
    	for (size_t j = 0; j < i; ++j){
    		string s2 = names[j];
    	    const int[] data2 = gdata[j*nCols..(j+1)*nCols];
    	    long[] res = [];
    	    float[] resf = []; 
    	    foreach (f ; dists){
				res ~= f(data1, data2, i, j);
				}// f
			foreach (f ; distsf){
				resf ~= f(data1, data2, i, j);
				}// f
		//	string sout = format("%s --> %s\t%s\t%s",s1,s2, res,resf);
		//	writeln(sout);	
    	} // j
    }// i
    ////////////////////////////////
}// unittest
