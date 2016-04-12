#pragma once
#ifndef __MYUTILS_H__
#define __MYUTILS_H__
/////////////////////////////////
#include "write_out.h"
////////////////////////////////////////
namespace info {
	//////////////////////////////////////////////
	template <typename X, class ALLOCX, class ALLOCINT, class ALLOCPAIR>
	void info_get_ranks(const std::vector<X, ALLOCX> &data,
		std::vector<size_t, ALLOCINT> &oRanks, std::vector<std::pair<X, size_t>, ALLOCPAIR> &oPairs) {
		const size_t n = data.size();
		assert(n > 0);
		oRanks.resize(n);
		oPairs.clear();
		for (size_t i = 0; i < n; ++i) {
			const X v = data[i];
			std::pair<X, size_t> o(v, i);
			oPairs.push_back(o);
		}// i
		std::sort(oPairs.begin(), oPairs.end(), [](const std::pair<X, size_t> &p1, const std::pair<X, size_t> &p2)->bool {
			return (p1.first < p2.first);
		});
		size_t iCurIndex = 0;
		size_t iCurRank = 1;
		size_t iNextRank = 1;
		X curVal = (oPairs[0]).first;
		while (iCurIndex < n) {
			const std::pair<X, size_t> &p = oPairs[iCurIndex];
			const X v = p.first;
			const size_t ipos = p.second;
			if (v == curVal) {
				oRanks[ipos] = iCurRank;
			}
			else {
				iCurRank = iNextRank;
				oRanks[ipos] = iCurRank;
				curVal = v;
			}
			++iCurIndex;
			++iNextRank;
		}// iCurIndex
	}// info_get_ranks
	template <typename X, class ALLOCX, class ALLOCINT>
	void info_get_ranks(const std::vector<X, ALLOCX> &data,
		std::vector<size_t, ALLOCINT> &oRanks) {
		std::vector<std::pair<X, size_t> > oPairs;
		info_get_ranks(data, oRanks, oPairs);
	}// info_get_ranks
	/////////////////////////////////////////////////
	template <typename X, typename F, class ALLOCX, class ALLOCF>
	bool info_compute_variances(const size_t nRows, const size_t nCols,
		const std::vector<X, ALLOCX> &oData, std::vector<F, ALLOCF> &oVars) {
		assert(nRows > 0);
		assert(nCols > 0);
		assert(oData.size() >= (size_t)(nRows * nCols));
		bool bRet = true;
		oVars.resize(nCols);
		assert(oVars.size() == nCols);
		for (size_t icol = 0; icol < nCols; ++icol) {
			double s1 = 0;
			double s2 = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = (double)oData[i * nCols + icol];
				s1 += x;
				s2 += x * x;
			}// i
			s1 /= nRows;
			double v = (s2 / nRows) - (s1 * s1);
			if (v <= 0) {
				bRet = false;
			}
			oVars[icol] = v;
		}// icol
		return (bRet);
	}//info_compute_variances
	//////////////////////////////////////////
	template <typename X, class ALLOCX, typename Z, class ALLOCZ>
	bool info_recode_data(std::vector<X, ALLOCX> &vData,
		std::vector<Z, ALLOCZ> &vRet,
		const Z zMax = (Z)1000, const Z zMin = (Z)0) {
		assert(zMin < zMax);
		const size_t n = vData.size();
		assert(n > 1);
		X vmin = 0, vmax = 0;
		for (size_t i = 0; i < n; ++i) {
			const X x = vData[i];
			if (i == 0) {
				vmin = x;
				vmax = x;
			}
			else if (x < vmin) {
				vmin = x;
			}
			else if (x > vmax) {
				vmax = x;
			}
		}// i
		if (vmin >= vmax) {
			return (false);
		}
		double dvmin = (double)vmin;
		double dzmin = (double)zMin;
		double delta = ((double)zMax - dzmin) / (double)(vmax - dvmin);
		vRet.resize(n);
		for (size_t i = 0; i < n; ++i) {
			double x = ((double)vData[i] - dvmin) * delta + dzmin;
			vRet[i] = (Z)x;
		}// i
		return (true);
	}// info_recode_data
	template <typename X, class ALLOCX, typename Z, class ALLOCZ, class ALLOCDOUBLE>
	bool info_normalize_data(const size_t nRows, const size_t nCols,
		std::vector<X, ALLOCX> &vData, std::vector<Z, ALLOCZ> &vRet,
		std::vector<double, ALLOCDOUBLE> &oVars) {
		static_assert(std::is_floating_point<Z>::value, "Z typename must be floating point type");
		assert(nRows > 2);
		assert(nCols > 0);
		const size_t nTotal = (size_t)(nCols * nRows);
		assert(vData.size() >= nTotal);
		vRet.resize(nTotal);
		oVars.resize(nCols);
		for (size_t icol = 0; icol < nCols; ++icol) {
			std::vector<double, ALLOCDOUBLE> dv(nRows);
			double somme = 0.0;
			for (size_t i = 0; i < nRows; ++i) {
				double s = vData[i * nCols + icol];
				dv[i] = s;
				somme += s;
			}
			const double moy = somme / nRows;
			double s2 = 0;
			for (size_t i = 0; i < nRows; ++i) {
				double x = dv[i] - moy;
				s2 += x * x;
				dv[i] = x;
			}
			double xvar = s2 / nRows;
			if (xvar <= 0.0) {
				return (false);
			}
			oVars[icol] = xvar;
			const double ecart = std::sqrt(xvar);
			assert(ecart >= 0.0);
			for (size_t i = 0; i < nRows; ++i) {
				vRet[i * nCols + icol] = (Z)(dv[i] / ecart);
			}// i
		}// icol
		return (true);
	}//info_normalize_data
	///////////////////////////////////////

}// namespace info
 /////////////////////////////////////
#endif // !__UTILS_H__


