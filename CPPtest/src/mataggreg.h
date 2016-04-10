#pragma once
#ifndef __MATAGGREG_H__
#define __MATAGGREG_H__
////////////////////////////////////////
#include "elemcrit.h"
#include "matcluster.h"
/////////////////////////////////////
namespace info {
	////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long,
		class ALLOCT = std::allocator<T>,
		class ALLOCP = std::allocator<std::shared_ptr<Indiv<T, U, ALLOCT> > >
	>
		class MatAggreg : public MatCluster<T, U, Z, ALLOCT> {
		public:
			typedef MatCluster<T, U, Z, ALLOCT> MatClusterType;
			typedef typename MatClusterType::DataType DataType;
			typedef typename MatClusterType::IndexType IndexType;
			typedef typename MatClusterType::DistanceType DistanceType;
			//
			typedef ElemCrit<Z,U> ElemCritType;
			typedef ElemCrits<DistanceType, size_t> ElemCritsType;
			typedef typename MatClusterType::DistanceFuncType DistanceFuncType;
			typedef typename MatClusterType::IndivType IndivType;
			typedef typename MatClusterType::IndivTypePtr IndivTypePtr;
			typedef typename MatClusterType::IIndivProviderType IIndivProviderType;
			//
			typedef typename MatClusterType::ClusterType ClusterType;
			typedef typename MatClusterType::ClusterTypePtr ClusterTypePtr;
			typedef typename MatClusterType::ClusterTypePtrVector ClusterTypePtrVector;
			//
			typedef std::vector<IndivTypePtr, ALLOCP> IndivTypePtrVector;
			typedef MatAggreg<T, U, Z, ALLOCT, ALLOCP> MatAggregType;
			typedef std::map<size_t, size_t> SizeTMap;
		private:
			IndivTypePtrVector _inds;
			static const size_t NB_DEFAULT_CLUSTERS;
			static const size_t NB_MAX_ITERS;
		public:
			MatAggreg(IIndivProviderType *pProvider, DistanceFuncType *pFunc = nullptr,
				LinkMode mode = LinkMode::linkCenter) : MatClusterType(pProvider, pFunc, mode) {
			}
			MatAggreg(const MatAggregType &other) :MatClusterType(other), _inds(other._inds) {}
			MatAggregType & operator=(const MatAggregType &other) {
				if (this != &other) {
					MatClusterType::operator=(other);
					this->_inds = other._inds;
				}
				return (*this);
			}
			virtual ~MatAggreg() {}
		public:
			bool retrieve_data(void) {
				return (this->reset());
			}// retrieve_data
			size_t aggreg(const size_t nbClusters = NB_DEFAULT_CLUSTERS,
				const size_t nbIters = NB_MAX_ITERS) {
				assert(nbClusters > 0);
				assert(nbIters > 0);
				size_t icount = 0;
				if (this->_inds.size() <= nbClusters) {
					if (!this->initialize(nbClusters)) {
						return (icount);
					}
				}
				SizeTMap oldMap;
				this->perform_one_step(oldMap);
				for (size_t i = 0; i < nbIters; ++i) {
					SizeTMap curMap;
					this->perform_one_step(curMap);
					++icount;
					bool bChange = false;
					for (auto it = curMap.begin(); it != curMap.end(); ++it) {
						const size_t key = (*it).first;
						const size_t val = (*it).second;
						assert(oldMap.find(key) != oldMap.end());
						const size_t x = oldMap[key];
						if (x != val) {
							bChange = true;
							break;
						}
					}// it
					if (!bChange) {
						break;
					}
					oldMap = curMap;
				}// i
				ClusterTypePtrVector &vc = this->clusters();
				ClusterTypePtrVector vr;
				size_t n = vc.size();
				for (size_t i = 0; i < n; ++i) {
					ClusterTypePtr c = vc[i];
					ClusterType *pc = c.get();
					assert(pc != nullptr);
					if (!pc->empty()) {
						pc->update_center();
						vr.push_back(c);
					}
				}// i
				this->clusters(vr);
				vc = this->clusters();
			   n = vc.size();
				if (n > 1) {
					std::sort(vc.begin(), vc.end(), [](const ClusterTypePtr &o1, const ClusterTypePtr &o2)->bool {
						const ClusterType *p1 = o1.get();
						assert(p1 != nullptr);
						const ClusterType *p2 = o2.get();
						assert(p2 != nullptr);
						return (p1->compare_to(*p2) < 0);
					});
				}// sort
				for (size_t i = 0; i < n; ++i) {
					ClusterTypePtr o = vc[i];
					ClusterType *px = o.get();
					assert(px != nullptr);
					IndexType aIndex = (IndexType)(i + 1);
					StringStreamType os;
					os << STRING_CLUSTER << aIndex;
					StringType sId = os.str();
					px->index(aIndex);
					px->id(sId);
				}//i
				return (icount);
			}// aggreg
		protected:
			virtual bool reset(void) {
				if (!this->is_valid()) {
					return (false);
				}
				IndivTypePtrVector &vv = this->_inds;
				vv.clear();
				this->reset_data_provider();
				IndivTypePtr oInd;
				while (this->next_indiv(oInd)) {
					const IndivType *p = oInd.get();
					assert(p != nullptr);
					IndivTypePtr o(oInd);
					vv.push_back(o);
				}// oInd
				return (vv.size() > 0);
			}// reset
		protected:
			bool initialize(const size_t nbClusters) {
				assert(nbClusters > 0);
				IndivTypePtrVector &vv = this->_inds;
				size_t n = vv.size();
				if (n < 1) {
					if (!this->reset()) {
						return (false);
					}
				vv = this->_inds;
				n = vv.size();
				}
				assert(n >= nbClusters);
				if (n == nbClusters) {
					return (true);
				}
				std::vector<size_t> oIndex(n);
				for (size_t i = 0; i < n; ++i) {
					oIndex[i] = i;
				}
				std::random_shuffle(oIndex.begin(), oIndex.end());
				ClusterTypePtrVector &vc = this->clusters();
				vc.clear();
				for (size_t i = 0; i < nbClusters; ++i) {
					size_t icur = oIndex[i];
					assert(icur < n);
					IndivTypePtr oInd = vv[icur];
					IndivType *p = oInd.get();
					assert(p != nullptr);
					ClusterTypePtr c = std::make_shared<ClusterType>(oInd);
					ClusterType *pc = c.get();
					assert(pc != nullptr);
					vc.push_back(c);
				}// i
				return (true);
			}// initialize
			size_t find_cluster_index(const IndivTypePtr &oInd) const {
				const ClusterTypePtrVector &vc = this->clusters();
				const size_t n = vc.size();
				size_t iRet = 0;
				DistanceType dMin = 0;
				const IndivType  *pInd = oInd.get();
				assert(pInd != nullptr);
				auto oSrc = pInd->value();
				for (size_t i = 0; i < n; ++i) {
					ClusterTypePtr c = vc[i];
					const ClusterType *pc = c.get();
					assert(pc != nullptr);
					DistanceType dist = 0;
					info_distance(oSrc, pc->value(), dist, this->distance_function());
					if (i == 0) {
						iRet = i;
						dMin = dist;
					}
					else if (dist < dMin) {
						iRet = i;
						dMin = dist;
					}
				}// i
				return (iRet);
			}// find_cluster_index
			void perform_one_step(SizeTMap &oMap) {
				const ClusterTypePtrVector &vc = this->clusters();
				const size_t n = vc.size();
				for (size_t i = 0; i < n; ++i) {
					ClusterTypePtr c = vc[i];
					ClusterType *pc = c.get();
					assert(pc != nullptr);
					pc->update_center();
					pc->clear_indivs();
				}// i
				oMap.clear();
				const IndivTypePtrVector &vv = this->_inds;
				const size_t nn = vv.size();
				for (size_t i = 0; i < nn; ++i) {
					IndivTypePtr oInd = vv[i];
					size_t ipos = this->find_cluster_index(oInd);
					assert(ipos < n);
					oMap[i] = ipos;
					ClusterTypePtr c = vc[ipos];
					ClusterType *pc = c.get();
					assert(pc != nullptr);
					pc->add(oInd, false);
				}// i
			}// perform_one_step
	};// class MatAggreg<T,U,Z>
	//////////////////////////////////////////////
	template <typename T, typename U, typename Z, class ALLOCT, class ALLOCP>
	const size_t MatAggreg<T, U, Z, ALLOCT, ALLOCP>::NB_DEFAULT_CLUSTERS = 5;
	template <typename T, typename U, typename Z, class ALLOCT, class ALLOCP>
	const size_t MatAggreg<T, U, Z, ALLOCT, ALLOCP>::NB_MAX_ITERS = 50;
	/////////////////////////////////////
}// namespace info
//////////////////////////////////////////
#endif // !__MATAGGREG_H__
