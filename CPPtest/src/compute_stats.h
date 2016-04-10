#pragma once
#ifndef __COMPUTE_STATS_H__
#define __COMPUTE_STATS_H__
///////////////////////////////
#include "utils.h"
////////////////////////////////
namespace info {
	//////////////////////////////
	template<typename T, typename X, class ALLOCX, class ALLOCT>
	static bool info_compute_stats(const size_t nr, const size_t nv,
		const std::vector<X, ALLOCX> &oSrc, std::vector<T, ALLOCT> &oMeans,
		std::vector<T, ALLOCT> &oStds, std::vector<T, ALLOCT> &oCorr) {
		//
		assert(nv > 1);
		assert(nr > nv);
		//
		const size_t nnv = nv * nv;
		const size_t nnr = nr * nv;
		//
		assert(oSrc.size() >= nnr);
		//
		oMeans = std::vector<T, ALLOCT >(nv);
		oStds = std::vector<T, ALLOCT >(nv);
		oCorr = std::vector<T, ALLOCT >(nnv);
		//
		std::vector<double> oTemp(nnr);
		std::vector<double> xCorr(nnv);
		std::vector<double> xStds(nv);
		bool bRet = true;
		double dnr = (double)nr;
		for (size_t ivar = 0; ivar < nv; ++ivar) {
			xCorr[ivar * nv + ivar] = 1.0;
			oCorr[ivar * nv + ivar] = (T) 1.0;
			double s = 0.0;
			for (size_t i = 0; i < nr; ++i) {
				const size_t k = i * nv + ivar;
				double x = (double)oSrc[k];
				oTemp[k] = x;
				s += x;
			} // i
			double moy = s / dnr;
			oMeans[ivar] = (T)moy;
			s = 0.0;
			for (size_t i = 0; i < nr; ++i) {
				const size_t k = i * nv + ivar;
				double x = oTemp[k] - moy;
				oTemp[k] = x;
				s += x * x;
			} // i
			double dvar = s / dnr;
			if (dvar > 0.0) {
				double ec1 = std::sqrt(dvar);
				xStds[ivar] = ec1;
				oStds[ivar] = (T)ec1;
				for (size_t ivar1 = 0; ivar1 < ivar; ++ivar1) {
					double s = 0.0;
					for (size_t i = 0; i < nr; ++i) {
						double x = oTemp[i * nv + ivar1]
							* oTemp[i * nv + ivar];
						s += x;
					}
					s /= dnr;
					double r = s / (ec1 * xStds[ivar1]);
					T rt = (T)r;
					xCorr[ivar * nv + ivar1] = r;
					oCorr[ivar * nv + ivar1] = rt;
					xCorr[ivar1 * nv + ivar] = r;
					oCorr[ivar1 * nv + ivar] = rt;
				}
			}
			else {
				bRet = false;
			}
		}
		return (bRet);
	}// compute_stats
	///////////////////////////////
}// namespace info
///////////////////////////////
#endif // !__COMPUTE_STATS_H__
