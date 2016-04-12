#pragma once
#ifndef __EIGEN_IMPL_H__
#define __EIGEN_IMPL_H__
////////////////////////////////
#include "gendefs.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////////
	extern bool info_compute_eigen_impl(const int nr, const double *pData,
		double *pVals, double *pVecs, int *pNbFacts /*= nullptr */);
	//////////////////////////////////
	template <class T = double, typename Z = double> class EigenSolver {
		static_assert(std::is_floating_point<Z>::value, "Z must be floating point type");
	public:
		typedef T DataType;
		typedef Z FloatType;
		typedef std::shared_ptr<DataType> DataTypePtr;
		typedef std::shared_ptr<FloatType> FloatTypePtr;
		typedef std::tuple<int, FloatTypePtr, FloatTypePtr> EigenResultType;
	private:
		typedef std::unique_ptr<double> DoublePtr;
	public:
		static EigenResultType compute(const int n, const DataType  *pData) {
			assert(n > 0);
			assert(pData != nullptr);
			const int nn = n * n;
			DoublePtr oSrc(new double[nn]);
			double *pDest = oSrc.get();
			assert(pDest != nullptr);
			for (int i = 0; i < nn; ++i) {
				pDest[i] = (double)pData[i];
			}// i
			DoublePtr v(new double[n]);
			DoublePtr vv(new double[nn]);
			double *pValSrc = v.get();
			assert(pValSrc != nullptr);
			double *pVecSrc = vv.get();
			assert(pVecSrc != nullptr);
			int nFacts = 0;
			FloatTypePtr oVals, oVecs;
			if (!info_compute_eigen_impl(n, pDest, pValSrc, pVecSrc, &nFacts)) {
				return std::make_tuple(nFacts, oVals, oVecs);
			}
			assert(nFacts <= n);
			oVals.reset(new FloatType[nFacts]);
			FloatType *pValDest = oVals.get();
			assert(pValDest != nullptr);
			oVecs.reset(new FloatType[nn]);
			FloatType *pVecDest = oVecs.get();
			assert(pVecDest != nullptr);
			for (int i = 0; i < n; ++i) {
				pValDest[i] = (FloatType)pValSrc[i];
				for (int j = 0; j < n; ++j) {
					const size_t pos = (size_t)(i * n + j);
					pVecDest[pos] = (FloatType)pVecSrc[pos];
				}// j
			}// i
			return std::make_tuple(nFacts, oVals, oVecs);
		}// compute
		static EigenResultType compute(const int n, const DataTypePtr &oData) {
			return compute(n, oData.get());
		}// compute
		static EigenResultType compute(const int n, const std::vector<T> &oVec) {
			assert(n > 0);
			const size_t nn = (size_t)(n * n);
			assert(oVec.size() >= nn);
			std::unique_ptr<DataType> o(new DataType[nn]);
			DataType *pData = o.get();
			assert(pData != nullptr);
			for (size_t i = 0; i < nn; ++i) {
				pData[i] = oVec[i];
			}// i
			return compute(n, pData);
		}// compute
	};// class EigenSolver
	//////////////////////////////////
}// namespace info
/////////////////////////////////
#endif // !__EIGEN_IMPL_H__
