#pragma once
#ifndef __MATDATA_H__
#define __MATDATA_H__
/////////////////////////////////
#include "infobase.h"
#include "distance.h"
#include "utils.h"
#include "distancemap.h"
/////////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_ROWS ("rows")
#define STRING_COLS ("cols")
#define STRING_DATA_ARRAY ("data_array")
#define STRING_NAMES ("names")
#define STRING_COL_NAMES ("colNames")
#else
#define STRING_ROWS (L"rows")
#define STRING_COLS (L"cols")
#define STRING_DATA_ARRAY (L"data_array")
#define STRING_NAMES (L"names")
#define STRING_COL_NAMES (L"colNames")
#endif // INFO_STRING_TYPE
///////////////////////////////////
namespace info {
//////////////////////////////////////////
template<typename T = int, class ALLOCT = std::allocator<T>,
		class ALLOCS = std::allocator<StringType> >
class MatData: public BaseInfo {
public:
	typedef T DataType;
	//
	typedef std::vector<DataType, ALLOCT> DataTypeVector;
	typedef std::vector<StringType, ALLOCS> StringTypeVector;
	typedef MatData<DataType, ALLOCT, ALLOCS> MatDataType;
public:
	size_t _rows;
	size_t _cols;
	DataTypeVector _data;
	StringTypeVector _rowids;
	StringTypeVector _colids;
public:
	MatData() :
			_rows(0), _cols(0) {
	}
	template<class CONTAINER1, class CONTAINER2>
	MatData(const size_t r, const size_t c, const CONTAINER1 &oData,
			const CONTAINER2 *pRowNames = nullptr, const CONTAINER2 *pColNames =
					nullptr) :
			_rows(r), _cols(c) {
		assert(this->_rows > 0);
		assert(this->_cols > 0);
		const size_t n = (size_t) (r * c);
		this->_data.resize(n);
		this->_rowids.resize(r);
		this->_colids.resize(c);
		assert(this->_data.size() >= n);
		assert(this->_rowids.size() >= this->_rows);
		for (size_t i = 0; i < n; ++i) {
			this->_data[i] = (DataType) oData[i];
		}
		if (pRowNames != nullptr) {
			const size_t nx = pRowNames->size();
			for (size_t i = 0; i < nx; ++i) {
				if (i >= r) {
					break;
				}
				this->_rowids[i] = (*pRowNames)[i];
			} // i
		}
		if (pColNames != nullptr) {
			const size_t nx = pColNames->size();
			for (size_t i = 0; i < nx; ++i) {
				if (i >= c) {
					break;
				}
				this->_colids[i] = (*pColNames)[i];
			} // i
		}
	} // MatData
	MatData(const MatDataType &other) :
			BaseInfo(other), _rows(other._rows), _cols(other._cols), _data(
					other._data), _rowids(other._rowids), _colids(other._colids) {
	} // MatData
	MatDataType & operator=(const MatDataType &other) {
		if (this != &other) {
			BaseInfo::operator=(other);
			this->_rows = other._rows;
			this->_cols = other._cols;
			this->_data = other._data;
			this->_rowids = other._rowids;
			this->_colids = other._colids;
		}
		return (*this);
	} // operator=
	virtual ~MatData() {
	}
public:
	virtual bool is_valid(void) const {
		return ((this->_rows > 0) && (this->_cols > 0)
				&& (this->_data.size() >= (size_t) (this->_rows * this->_cols))
				&& (this->_rowids.size() >= this->_rows)
				&& (this->_colids.size() >= this->_cols));
	} // is_valid
	size_t rows(void) const {
		return (this->_rows);
	}
	size_t cols(void) const {
		return (this->_cols);
	}
	const DataTypeVector & data(void) const {
		return (this->_data);
	}
	const StringTypeVector & names(void) const {
		return (this->_rowids);
	}
	const StringTypeVector & colNames(void) const {
		return (this->_colids);
	}
	const StringType &row_name_at(const size_t irow) const {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nRows = this->rows();
		if (irow >= nRows) {
			throw std::invalid_argument("invalid argument(s)");
		}
		return (this->_rowids[irow]);
	}
	void row_name_at(const size_t irow, const StringType &s) {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nRows = this->rows();
		if (irow >= nRows) {
			throw std::invalid_argument("invalid argument(s)");
		}
		StringType ss(s);
		this->_rowids[irow] = ss;
	}
	const StringType &col_name_at(const size_t icol) const {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nCols = this->cols();
		if (icol >= nCols) {
			throw std::invalid_argument("invalid argument(s)");
		}
		return (this->_colids[icol]);
	}
	void col_name_at(const size_t icol, const StringType &s) {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nCols = this->cols();
		if (icol >= nCols) {
			throw std::invalid_argument("invalid argument(s)");
		}
		StringType ss(s);
		this->_colids[icol] = ss;
	}
	template<class CONTAINER>
	bool row_at(const size_t irow, StringType &sId, CONTAINER &data) const {
		if (!this->is_valid()) {
			return (false);
		}
		if (irow >= this->rows()) {
			return (false);
		}
		const size_t n = this->cols();
		data.resize(n);
		const StringTypeVector &vs = this->names();
		sId = vs[irow];
		const size_t offset = (size_t) (irow * n);
		const DataTypeVector &vv = this->data();
		for (size_t i = 0; i < n; ++i) {
			data[i] = vv[offset + i];
		} // i
		return (true);
	} // row_at
	template<class CONTAINER>
	bool col_at(const size_t icol, StringType &sId, CONTAINER &data) const {
		if (!this->is_valid()) {
			return (false);
		}
		if (icol >= this->cols()) {
			return (false);
		}
		const size_t n = this->rows();
		const size_t nCols = this->cols();
		data.resize(n);
		const StringTypeVector &vs = this->colNames();
		sId = vs[icol];
		const DataTypeVector &vv = this->data();
		size_t offset = 0;
		for (size_t i = 0; i < n; ++i) {
			data[i] = vv[offset + icol];
			offset += nCols;
		} // i
		return (true);
	} // col_at
	DataType value_at(const size_t irow, const size_t icol) const {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nCols = this->cols();
		if ((icol >= nCols) || (irow >= this->rows())) {
			throw std::invalid_argument("invalid argument(s)");
		}
		return (this->_data[irow * nCols + icol]);
	} // value_at
	DataType & value_at(const size_t irow, const size_t icol) {
		if (!this->is_valid()) {
			throw std::out_of_range("invalid matdata");
		}
		const size_t nCols = this->cols();
		if ((icol >= nCols) || (irow >= this->rows())) {
			throw std::invalid_argument("invalid argument(s)");
		}
		return (this->_data[irow * nCols + icol]);
	} // value_at
	DataType operator()(const size_t irow, const size_t icol) const {
		return this->value_at(irow, icol);
	}
	DataType & operator()(const size_t irow, const size_t icol) {
		return this->value_at(irow, icol);
	}
	OStreamType & operator<<(OStreamType &os) const {
		return (this->write_to(os));
	} // operator<<
	virtual OStreamType & write_to(OStreamType &os) const {
		//
		os << std::endl << START_OBJECT;
		info_write_json_value(os, STRING_ROWS, this->rows()) << STRING_COMMA
				<< std::endl;
		info_write_json_value(os, STRING_COLS, this->cols()) << STRING_COMMA
				<< std::endl;
		info_write_json_value(os, STRING_NAMES, this->names()) << STRING_COMMA
				<< std::endl;
		info_write_json_value(os, STRING_COL_NAMES, this->colNames())
				<< STRING_COMMA << std::endl;
		info_write_json_value(os, STRING_DATA_ARRAY, this->data()) << std::endl;
		os << END_OBJECT;
		//
		return (os);
	} // write_to
public:
	template<class ALLOCDOUBLE>
	bool compute_weights(std::vector<double, ALLOCDOUBLE> &weights) const {
		if (!this->is_valid()) {
			return (false);
		}
		const size_t nRows = this->rows();
		const size_t nCols = this->cols();
		if ((nCols < 2) || (nRows < 2)) {
			return (false);
		}
		weights.clear();
		weights.resize(nCols);
		const DataTypeVector &dd = this->_data;
		double somme = 0;
		for (size_t icol = 0; icol < nCols; ++icol) {
			double s1 = 0, s2 = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = (double) dd[i * nCols + icol];
				s1 += x;
				s2 += x * x;
			} // i
			const double mx = s1 / nRows;
			double ec = (s2 / nRows) - (mx * mx);
			if (ec <= 0) {
				return (false);
			} // ec
			ec = 1.0 / ec;
			somme += ec;
			weights[icol] = ec;
		} // icol
		for (size_t i = 0; i < nCols; ++i) {
			weights[i] /= somme;
		} // i
		return (true);
	} // compute_weights
	template<class ALLOCZ, class ALLOCSS>
	bool to_ranks(MatData<size_t, ALLOCZ, ALLOCSS> &oRes) const {
		if (!this->is_valid()) {
			return (false);
		}
		const size_t nRows = this->rows();
		const size_t nCols = this->cols();
		oRes._colids.resize(nCols);
		oRes._cols = nCols;
		oRes._data.resize(nCols * nRows);
		oRes._rowids.resize(nRows);
		oRes._rows = nRows;
		for (size_t irow = 0; irow < nRows; ++irow) {
			oRes._rowids[irow] = this->_rowids[irow];
		} // irow
		for (size_t icol = 0; icol < nCols; ++icol) {
			oRes._colids[icol] = this->_colids[icol];
			DataTypeVector data(nRows);
			for (size_t i = 0; i < nRows; ++i) {
				data[i] = this->_data[i * nCols + icol];
			} // i
			std::vector<size_t, ALLOCZ> oRanks;
			info_get_ranks(data, oRanks);
			for (size_t i = 0; i < nRows; ++i) {
				oRes._data[i * nCols + icol] = oRanks[i];
			} // i
		} // icol
		return (true);
	} //to_ranks
	template<typename X, class ALLOCX, class ALLOCSS>
	bool to_normalized(MatData<X, ALLOCX, ALLOCSS> &oRes) const {
#if defined(_MSC_VER)
		static_assert(std::is_floating_point<X>::value, "X must be floating point type");
#endif // _MSC_VER
		if (!this->is_valid()) {
			return (false);
		}
		const size_t nRows = this->rows();
		const size_t nCols = this->cols();
		oRes._colids.resize(nCols);
		oRes._cols = nCols;
		oRes._data.resize(nCols * nRows);
		oRes._rowids.resize(nRows);
		oRes._rows = nRows;
		for (size_t irow = 0; irow < nRows; ++irow) {
			oRes._rowids[irow] = this->_rowids[irow];
		} // irow
		const DataTypeVector &dd = this->_data;
		for (size_t icol = 0; icol < nCols; ++icol) {
			oRes._colids[icol] = this->_colids[icol];
			double s1 = 0, s2 = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = (double) dd[i * nCols + icol];
				s1 += x;
				s2 += x * x;
			} // i
			const double mx = s1 / nRows;
			double ec = (s2 / nRows) - (mx * mx);
			if (ec <= 0) {
				return (false);
			} // ec
			const double vstd = std::sqrt(ec);
			if (vstd <= 0.0) {
				return (false);
			}
			for (size_t i = 0; i < nRows; ++i) {
				const size_t pos = i * nCols + icol;
				oRes._data[pos] = (X) (((double) dd[pos] - mx) / vstd);
			} // i
		} // icol
		return (true);
	} //to_normalized
	template<typename X, class ALLOCX, class ALLOCSS>
	bool to_recoded(MatData<X, ALLOCX, ALLOCSS> &oRes, const X xmax = 1000,
			const X xmin = 0) const {
		if (xmax <= xmin) {
			return (false);
		}
		if (!this->is_valid()) {
			return (false);
		}
		const size_t nRows = this->rows();
		const size_t nCols = this->cols();
		oRes._colids.resize(nCols);
		oRes._cols = nCols;
		oRes._data.resize(nCols * nRows);
		oRes._rowids.resize(nRows);
		oRes._rows = nRows;
		for (size_t irow = 0; irow < nRows; ++irow) {
			oRes._rowids[irow] = this->_rowids[irow];
		} // irow
		const DataTypeVector &dd = this->_data;
		for (size_t icol = 0; icol < nCols; ++icol) {
			oRes._colids[icol] = this->_colids[icol];
			double smin = 0, smax = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = (double) dd[i * nCols + icol];
				if (i == 0) {
					smin = x;
					smax = x;
				} else if (x < smin) {
					smin = x;
				} else if (x > smax) {
					smax = x;
				}
			} // i
			if (smin >= smax) {
				return (false);
			}
			const double delta = (xmax - xmin) / (smax - smin);
			for (size_t i = 0; i < nRows; ++i) {
				const size_t pos = i * nCols + icol;
				double xx = (dd[pos] - smin) * delta + (double) xmin;
				oRes._data[pos] = (X) xx;
			} // i
		} // icol
		return (true);
	} //to_recoded
	template<typename U, typename Z, class COMPAREU, class ALLOCPM1,
			class ALLOCPM2>
	void compute_distances(
			DistanceMap<U, Z, COMPAREU, ALLOCPM1, ALLOCPM2> &oMap,
			const DistanceFunc<Z> *pFunc = nullptr) const {
		const size_t nRows = this->rows();
		const size_t nCols = this->cols();
		const DataTypeVector &gdata = this->_data;
		oMap.clear();
		for (size_t i = 0; i < nRows; ++i) {
			DataTypeVector vi(nCols);
			for (size_t k = 0; k < nCols; ++k) {
				vi[k] = gdata[i * nCols + k];
			} //k
			for (size_t j = 0; j < i; ++j) {
				DataTypeVector vj(nCols);
				for (size_t k = 0; k < nCols; ++k) {
					vj[k] = gdata[j * nCols + k];
				} //k
				Z res = 0;
				info::info_distance(vi, vj, res, pFunc, i, j);
				if (res > 0) {
					oMap.add((U) i, (U) j, res);
				}
			} // j
		} // i
	} // compute_distance
	template<typename U, typename Z, class COMPAREU, class ALLOCPM1,
			class ALLOCPM2>
	void compute_distances(
			DistanceMap<U, Z, COMPAREU, ALLOCPM1, ALLOCPM2> &oRes,
			DistanceType mode = DistanceType::distanceSquare) const {
		switch (mode) {
		case DistanceType::distanceDivergence: {
			DivergenceDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceEuclide: {
			EuclideDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceManhattan: {
			ManhattanDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceMax: {
			MaxDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceSquare: {
			DistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceKhiDeux: {
			const size_t nRows = this->rows();
			const size_t nCols = this->cols();
			const DataTypeVector &gdata = this->_data;
			Khi2DistanceFunc<Z> fDist(nRows, nCols, gdata);
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceVariance: {
			const size_t nRows = this->rows();
			const size_t nCols = this->cols();
			const DataTypeVector &gdata = this->_data;
			VarianceDistanceFunc<Z> fDist(nRows, nCols, gdata);
			this->compute_distances(oRes, &fDist);
		}
			break;
		default:
			// should not fall here
			assert(false);
			break;
		} // mode
	} // compute_distance
};
// class MatData<T,U>
}// namespace info
///////////////////////////////////
#endif // !__MATDATA_H__
