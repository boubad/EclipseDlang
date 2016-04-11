#pragma once
#ifndef __DISTANCE_H__
#define __DISTANCE_H__
/////////////////////////
#include "infobase.h"
/////////////////////////////
namespace info {
////////////////////////////////////////
enum class DistanceType {
	distanceInvalid,
	distanceSquare,
	distanceManhattan,
	distanceEuclide,
	distanceMax,
	distanceDivergence,
	distanceKhiDeux,
	distanceVariance,
	distanceEigen,
	distanceWeighted
};
// DistanceType
//////////////////////////////////////
template<typename Z = long> class DistanceFunc: public BaseInfo {
public:
	typedef DistanceFunc<Z> DistanceFuncType;
protected:
	bool _valid;
	DistanceType _type;
public:
	DistanceFunc() :
			_valid(true), _type(DistanceType::distanceSquare) {
	}
	DistanceFunc(const DistanceFuncType &other) :
			BaseInfo(other), _valid(other._valid), _type(other._type) {
	}
	DistanceFuncType & operator=(const DistanceFuncType &other) {
		if (this != &other) {
			BaseInfo::operator=(other);
			this->_valid = other._valid;
			this->_type = other._type;
		}
		return (*this);
	}
	virtual ~DistanceFunc() {
	}
	static DistanceFuncType DEFAULT_DISTANCE_FUNCTION;
public:
	DistanceType type(void) const {
		return (this->_type);
	}
	virtual bool is_valid(void) const {
		return ((this->_type != DistanceType::distanceInvalid) && this->_valid);
	}
	template<class CONTAINER1, class CONTAINER2>
	Z operator()(const CONTAINER1 &v1, const CONTAINER2 &v2,
			const size_t irow1 = 0, const size_t irow2 = 0) const {
		assert(this->is_valid());
		return ((Z) (this->perform_compute(v1, v2, irow1, irow2)));
	}	// operator()
protected:
	virtual double perform_one_step(const double v1, const double v2,
			const size_t /*icol*/, const size_t /*irow1*/,
			const size_t /*irow2*/) const {
		const double t = v1 - v2;
		return (t * t);
	}	//compute_one_step
	virtual double perform_last(const double dval, const size_t n) const {
		assert(dval >= 0);
		assert(n >= 0);
		double dRet = dval;
		if (n > 1) {
			dRet /= n;
		}
		return (dRet);
	}	// perform_last
	template<class CONTAINER1, class CONTAINER2>
	double perform_compute(const CONTAINER1 & v1, const CONTAINER2 & v2,
			const size_t irow1, const size_t irow2) const {
		assert(this->is_valid());
		const size_t n = v1.size();
		assert(v2.size() >= n);
		double res = 0;
		for (size_t i = 0; i < n; ++i) {
			res += perform_one_step((double) v1[i], (double) v2[i], i, irow1,
					irow2);
		}	// i
		return perform_last(res, n);
	}	// perform_compute
};
// class DistanceFunc
template<typename Z>
DistanceFunc<Z> DistanceFunc<Z>::DEFAULT_DISTANCE_FUNCTION;
/////////////////////////////////////////////
template<typename Z = long> class ManhattanDistanceFunc: public DistanceFunc<Z> {
public:
	typedef DistanceFunc<Z> DistanceFuncType;
	typedef ManhattanDistanceFunc<DistanceType> ManhattanDistanceFuncType;
public:
	ManhattanDistanceFunc() {
		this->_type = DistanceType::distanceManhattan;
	}
	ManhattanDistanceFunc(const ManhattanDistanceFuncType &other) :
			DistanceFuncType(other) {
	}
	ManhattanDistanceFuncType & operator=(
			const ManhattanDistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~ManhattanDistanceFunc() {
	}
protected:
	virtual double perform_one_step(const double v1, const double v2,
			const size_t /*icol*/, const size_t /*irow1*/,
			const size_t /* irow2 */) const {
		const double t = v1 - v2;
		return (std::abs(t));
	}	//compute_one_step
};
// class ManhattanDistanceFunc
///////////////////////////////////////////
template<typename Z = long> class MaxDistanceFunc: public DistanceFunc<Z> {
public:
	typedef DistanceFunc<Z> DistanceFuncType;
	typedef MaxDistanceFunc<DistanceType> MaxDistanceFuncType;
public:
	MaxDistanceFunc() {
		this->_type = DistanceType::distanceMax;
	}
	MaxDistanceFunc(const MaxDistanceFuncType &other) :
			DistanceFuncType(other) {
	}
	MaxDistanceFuncType & operator=(const MaxDistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~MaxDistanceFunc() {
	}
protected:
	virtual double perform_one_step(const double v1, const double v2,
			const size_t /*icol*/, const size_t /*irow1*/,
			const size_t /* irow2 */) const {
		const double t = v1 - v2;
		return (std::abs(t));
	}	//compute_one_step
	template<class CONTAINER1, class CONTAINER2>
	double perform_compute(const CONTAINER1 & v1, const CONTAINER2 & v2,
			const size_t irow1, const size_t irow2) const {
		assert(this->is_valid());
		const size_t n = v1.size();
		assert(v2.size() >= n);
		double res = 0;
		for (size_t i = 0; i < n; ++i) {
			double d = perform_one_step((double) v1[i], (double) v2[i], i,
					irow1, irow2);
			if (i == 0) {
				res = d;
			} else if (d > res) {
				res = d;
			}
		}	// i
		return (res);
	}	// perform_compute
	virtual double perform_last(const double dval, const size_t /*n*/) const {
		assert(dval >= 0);
		return (dval);
	}	// perform_last
};
// class ManhattanDistanceFunc
//////////////////////////////////////////
template<typename Z = long> class EuclideDistanceFunc: public DistanceFunc<Z> {
public:
	typedef DistanceFunc<Z> DistanceFuncType;
	typedef EuclideDistanceFunc<Z> EuclideDistanceFuncType;
public:
	EuclideDistanceFunc() {
		this->_type = DistanceType::distanceEuclide;
	}
	EuclideDistanceFunc(const EuclideDistanceFuncType &other) :
			DistanceFuncType(other) {
	}
	EuclideDistanceFuncType & operator=(const EuclideDistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~EuclideDistanceFunc() {
	}
protected:
	virtual double perform_last(const double dval, const size_t n) const {
		assert(dval >= 0);
		assert(n >= 0);
		double dRet = dval;
		if (n > 1) {
			dRet /= n;
		}
		return (std::sqrt(dRet));
	}	// perform_last
};
// class EuclideDistanceFunc
///////////////////////////////////////////
template<typename Z = double, class ALLOCDOUBLE = std::allocator<double> > class Khi2DistanceFunc: public DistanceFunc<Z> {
#if defined(_MSC_VER)
	static_assert(std::is_floating_point<Z>::value, "Z must be floating point type");
#endif // _MSC_VER
public:
	typedef std::vector<double,ALLOCDOUBLE> DoubleTypeVector;
	//
	typedef DistanceFunc<Z> DistanceFuncType;
	typedef Khi2DistanceFunc<Z> Khi2DistanceFuncType;
private:
	DoubleTypeVector _colsum;
	DoubleTypeVector _rowsum;
public:
	template<class CONTAINER>
	Khi2DistanceFunc(const CONTAINER &srows, const CONTAINER &scols) {
		const size_t nRows = srows.size();
		const size_t nCols = scols.size();
		assert(nRows > 1);
		assert(nCols > 1);
		DoubleTypeVector &cols = this->_colsum;
		cols.resize(nCols);
		DoubleTypeVector &rows = this->_rowsum;
		rows.resize(nRows);
		this->_valid = true;
		this->_type = DistanceType::distanceKhiDeux;
		double total = 0;
		for (size_t i = 0; i < nCols; ++i) {
			double x = (double) scols[i];
			if (x < 0) {
				x = 0;
			}
			total += x;
			cols[i] = x;
		}	// i
		if (total <= 0) {
			this->_valid = false;
		}
		total = 0;
		for (size_t i = 0; i < nRows; ++i) {
			double x = (double) srows[i];
			if (x < 0) {
				x = 0;
			}
			total += x;
			rows[i] = x;
		}	// i
		if (total <= 0) {
			this->_valid = false;
		}
	}
	template<class CONTAINER>
	Khi2DistanceFunc(const size_t nRows, const size_t nCols,
			const CONTAINER &data) {
		assert(nRows > 1);
		assert(nCols > 1);
		assert(data.size() >= (size_t )(nRows * nCols));
		this->_valid = true;
		this->_type = DistanceType::distanceKhiDeux;
		DoubleTypeVector &cols = this->_colsum;
		cols.resize(nCols);
		DoubleTypeVector &rows = this->_rowsum;
		rows.resize(nRows);
		double total = 0;
		for (size_t icol = 0; icol < nCols; ++icol) {
			cols[icol] = 0;
			for (size_t irow = 0; irow < nRows; ++irow) {
				cols[icol] += (double) data[irow * nCols + icol];
			}	// irow
			if (cols[icol] <= 0) {
				this->_valid = false;
				cols[icol] = 0;
			}
			total += cols[icol];
		}	// icol
		if (total > 0) {
			for (size_t icol = 0; icol < nCols; ++icol) {
				cols[icol] /= total;
			}
		} else {
			this->_valid = false;
		}
		total = 0;
		for (size_t irow = 0; irow < nRows; ++irow) {
			rows[irow] = 0;
			for (size_t icol = 0; icol < nCols; ++icol) {
				rows[irow] += (double) data[irow * nCols + icol];
			}	// irow
			if (rows[irow] <= 0) {
				this->_valid = false;
				rows[irow] = 0;
			}
			total += rows[irow];
		}	// irow
		if (total <= 0) {
			this->_valid = false;
		} else {
			for (size_t irow = 0; irow < nRows; ++irow) {
				rows[irow] /= total;
			}
		}
	}	//Khi2DistanceFunc

	Khi2DistanceFunc(const Khi2DistanceFuncType &other) :
			DistanceFuncType(other), _colsum(other._colsum), _rowsum(
					other._rowsum) {
	}
	Khi2DistanceFuncType & operator=(const Khi2DistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
			this->_colsum = other._colsum;
			this->_rowsum = other._rowsum;
		}
		return (*this);
	}
	virtual ~Khi2DistanceFunc() {
	}
protected:
	virtual double perform_one_step(const double v1, const double v2,
			const size_t icol, const size_t irow1, const size_t irow2) const {
		const DoubleTypeVector &vr = this->_rowsum;
		const DoubleTypeVector &vc = this->_colsum;
		assert(irow1 < vr.size());
		assert(irow2 < vr.size());
		assert(icol < vc.size());
		const double d1 = vr[irow1];
		assert(d1 > 0);
		const double d2 = vr[irow2];
		assert(d2 > 0);
		const double dc = vc[icol];
		assert(dc > 0);
		const double t = (d1 * v1) - (d2 * v2);
		return (t * t * dc);
	}	//compute_one_step
	virtual double perform_last(const double dval, const size_t /*n*/) const {
		assert(dval >= 0);
		return (dval);
	}	// perform_last
};
// class Khi2DistanceFunc<Z,ALLOCDOUBLZ>
//////////////////////////////////////////
template<class Z = double> class DivergenceDistanceFunc: public DistanceFunc<Z> {
#if defined(_MSC_VER)
	static_assert(std::is_floating_point<Z>::value, "Z must be floating point type");
#endif
public:
	typedef DistanceFunc<Z> DistanceFuncType;
	typedef DivergenceDistanceFunc<Z> DivergenceDistanceFuncType;
public:
	template<class CONTAINER>
	DivergenceDistanceFunc(const size_t nRows, const size_t nCols,
			const CONTAINER &data) {
		assert(nRows > 0);
		assert(nCols > 0);
		assert(data.size() >= (size_t )(nRows * nCols));
		this->_valid = true;
		this->_type = DistanceType::distanceDivergence;
		for (size_t i = 0; i < nRows; ++i) {
			if (!this->_valid) {
				break;
			}
			for (size_t j = 0; j < i; ++j) {
				if (!this->_valid) {
					break;
				}
				for (size_t k = 0; k < nCols; ++k) {
					double v1 = data[i * nCols + k];
					double v2 = data[j * nCols + k];
					if ((v1 + v2) <= 0) {
						this->_valid = false;
						break;
					}
				}	// k
			}	// j
		}	//i
	}	//DivergenceDistanceFunc
	DivergenceDistanceFunc(const DivergenceDistanceFuncType &other) :
			DistanceFuncType(other) {
	}
	DivergenceDistanceFuncType & operator=(
			const DivergenceDistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~DivergenceDistanceFunc() {
	}
protected:
	virtual double perform_one_step(const double v1, const double v2,
			const size_t /*icol */, const size_t /*irow1*/,
			const size_t /*irow2*/) const {
		const double d = v1 + v2;
		assert(d > 0);
		return (std::abs((v1 - v2) / d));
	}	//compute_one_step
};
// class DivergenceDistanceFunc<Z,ALLOCDOUBLZ>
////////////////////////////////////////
template<typename Z = long, class ALLOCDOUBLE = std::allocator<double> > class WeightedDistanceFunc: public DistanceFunc<
		Z> {
public:
	typedef std::vector<double, ALLOCDOUBLE> DoubleTypeVector;
	//
	typedef DistanceFunc<DistanceType> DistanceFuncType;
	typedef WeightedDistanceFunc<DistanceType, ALLOCDOUBLE> WeightedDistanceFuncType;
protected:
	DoubleTypeVector _weights;
private:
	const DistanceFuncType *_pfunc;
public:
	template<class CONTAINER>
	WeightedDistanceFunc(const DistanceFuncType &func, const CONTAINER &ww) :
			_weights(ww.size()), _pfunc(&func) {
		const size_t n = ww.size();
		assert(n > 0);
		DoubleTypeVector &oAr = this->_weights;
		assert(oAr.size() == n);
		double somme = 0;
		for (size_t i = 0; i < n; ++i) {
			double v = (double) ww[i];
			if (v < 0.0) {
				v = 0;
			}
			oAr[i] = v;
			somme += v;
		}	  // i
		this->_valid = (somme > 0.0);
		assert(somme > 0.0);
		for (size_t i = 0; i < n; ++i) {
			oAr[i] /= somme;
		}	  // i
		this->_type = DistanceType::distanceWeighted;
	}
	template<class CONTAINER>
	WeightedDistanceFunc(const CONTAINER &ww) :
			_weights(ww.size()), _pfunc(
					&DistanceFuncType::DEFAULT_DISTANCE_FUNCTION) {
		const size_t n = ww.size();
		assert(n > 0);
		DoubleTypeVector &oAr = this->_weights;
		assert(oAr.size() == n);
		double somme = 0;
		for (size_t i = 0; i < n; ++i) {
			double v = (double) ww[i];
			if (v < 0.0) {
				v = 0;
			}
			oAr[i] = v;
			somme += v;
		}	  // i
		this->_valid = (somme > 0.0);
		assert(somme > 0.0);
		for (size_t i = 0; i < n; ++i) {
			oAr[i] /= somme;
		}	  // i
		this->_type = DistanceType::distanceWeighted;
	}
	template<class CONTAINER>
	WeightedDistanceFunc(const size_t nRows, const size_t nCols,
			const CONTAINER &oData) :
			_weights(nCols), _pfunc(
					&DistanceFuncType::DEFAULT_DISTANCE_FUNCTION) {
		assert(nRows > 0);
		assert(nCols > 0);
		assert(oData.size() >= (size_t )(nRows * nCols));
		DoubleTypeVector &oAr = this->_weights;
		oAr.resize(nCols);
		assert(oAr.size() == nCols);
		this->_valid = true;
		this->_type = DistanceType::distanceVariance;
		double somme = 0;
		for (size_t icol = 0; icol < nCols; ++icol) {
			double s1 = 0;
			double s2 = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = (double) oData[i * nCols + icol];
				s1 += x;
				s2 += x * x;
			}	  // i
			s1 /= nRows;
			double v = (s2 / nRows) - (s1 * s1);
			if (v <= 0) {
				this->_valid = false;
				v = 0;
			} else {
				v = 1.0 / v;
			}
			oAr[icol] = v;
			somme += v;
		}	  // icol
		if (somme > 0) {
			for (size_t i = 0; i < nCols; ++i) {
				oAr[i] /= nCols;
			}
		}
	}
	WeightedDistanceFunc(const WeightedDistanceFuncType &other) :
			DistanceFuncType(other), _weights(other._weights), _pfunc(
					other._pfunc) {
	}
	WeightedDistanceFuncType & operator=(
			const WeightedDistanceFuncType &other) {
		if (this != &other) {
			DistanceFuncType::operator=(other);
			this->_weights = other._weights;
			this->_pfunc = other._pfunc;
		}
		return (*this);
	}
	virtual ~WeightedDistanceFunc() {
	}
public:
	const DistanceFuncType & base_function(void) const {
		assert(this->_pfunc != nullptr);
		return (this->_pfunc);
	}
	const DoubleTypeVector &weights(void) const {
		return (this->_weights);
	}
	template<class CONTAINER>
	void weights(const CONTAINER &ww) {
		const size_t n = ww.size();
		assert(n > 0);
		DoubleTypeVector &oAr = this->_weights;
		oAr.resize(n);
		assert(oAr.size() == n);
		double somme = 0;
		for (size_t i = 0; i < n; ++i) {
			double v = (double) ww[i];
			if (v < 0) {
				v = 0;
			}
			somme += v;
		}	  // i
		if (somme <= 0.0) {
			this->_valid = false;
		} else {
			for (size_t i = 0; i < n; ++i) {
				double v = (double) ww[i];
				if (v < 0) {
					v = 0;
				}
				oAr[i] = v / somme;
			}	  // i
		}
	}
protected:
	template<class CONTAINER1, class CONTAINER2>
	double perform_compute(const CONTAINER1 & v1, const CONTAINER2 & v2,
			const size_t irow1, const size_t irow2) const {
		assert(this->is_valid());
		const size_t n = v1.size();
		assert(v2.size() >= n);
		const DistanceFuncType *pfunc = this->_pfunc;
		assert(pfunc != nullptr);
		const DoubleTypeVector &oAr = this->_weights;
		assert(oAr.size() >= n);
		double res = 0;
		for (size_t i = 0; i < n; ++i) {
			double d = pfunc->perform_one_step((double) v1[i], (double) v2[i],
					i, irow1, irow2);
			res += d * oAr[i];
		}	  // i
		return (res);
	}	  // perform_compute
};
// class WeightedDistanceFunc<Z,ALLOCDOUBLZ>
////////////////////////////////////////
template<typename Z = long, class ALLOCDOUBLE = std::allocator<double> > class VarianceDistanceFunc: public WeightedDistanceFunc<
		Z, ALLOCDOUBLE> {
public:
	typedef WeightedDistanceFunc<Z, ALLOCDOUBLE> WeightedDistanceFuncType;
	typedef typename WeightedDistanceFuncType::DoubleTypeVector DoubleTypeVector;
	typedef typename WeightedDistanceFuncType::DistanceFuncType DistanceFuncType;
	//
	typedef VarianceDistanceFunc<DistanceType, ALLOCDOUBLE> VarianceDistanceFuncType;
public:
	template<class CONTAINER>
	VarianceDistanceFunc(const CONTAINER &ww) :
			WeightedDistanceFuncType(ww) {
		this->_type = DistanceType::distanceVariance;
	}
	template<class CONTAINER>
	VarianceDistanceFunc(const size_t nRows, const size_t nCols,
			const CONTAINER &oData) :
			WeightedDistanceFuncType(nRows, nCols, oData) {
		this->_type = DistanceType::distanceVariance;
	}
	VarianceDistanceFunc(const VarianceDistanceFuncType &other) :
			WeightedDistanceFuncType(other) {
	}
	VarianceDistanceFunc & operator=(const VarianceDistanceFunc &other) {
		if (this != &other) {
			WeightedDistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~VarianceDistanceFunc() {
	}
};
//class VarianceDistanceFunc<Z,ALLOCDOUBLE>
//////////////////////////////////////////////////////////
template<typename Z = long, class ALLOCDOUBLE = std::allocator<double> > class EigenDistanceFunc: public WeightedDistanceFunc<
		Z, ALLOCDOUBLE> {
public:
	typedef WeightedDistanceFunc<Z, ALLOCDOUBLE> WeightedDistanceFuncType;
	typedef typename WeightedDistanceFuncType::DistanceType DistanceType;
	typedef typename WeightedDistanceFuncType::DoubleTypeVector DoubleTypeVector;
	typedef typename WeightedDistanceFuncType::DistanceFuncType DistanceFuncType;
	//
	typedef EigenDistanceFunc<DistanceType, ALLOCDOUBLE> EigenDistanceFuncType;
public:
	template<class CONTAINER>
	EigenDistanceFunc(const CONTAINER &ww) :
			WeightedDistanceFuncType(ww) {
		this->_type = DistanceType::distanceEigen;
	}
	EigenDistanceFunc(const EigenDistanceFuncType &other) :
			WeightedDistanceFuncType(other) {
	}
	EigenDistanceFunc & operator=(const EigenDistanceFunc &other) {
		if (this != &other) {
			WeightedDistanceFuncType::operator=(other);
		}
		return (*this);
	}
	virtual ~EigenDistanceFunc() {
	}
};
//class EigenDistanceFunc<Z,ALLOCDOUBLE>
////////////////////////////////////////////////////////
template<typename Z, class CONTAINER1, class CONTAINER2>
void info_distance(const CONTAINER1 &vv1, const CONTAINER2 &vv2, Z &result,
		const DistanceFunc<Z> *pFunc = nullptr, const size_t irow1 = 0,
		const size_t irow2 = 0) {
	if (pFunc == nullptr) {
		result = DistanceFunc<Z>::DEFAULT_DISTANCE_FUNCTION(vv1, vv2, irow1,
				irow2);
	} else {
		result = (*pFunc)(vv1, vv2, irow1, irow2);
	}
}	// info_distance
////////////////////////////////////////
}	 // namespace info
//////////////////////////
#endif // __DISTANCE_H__
