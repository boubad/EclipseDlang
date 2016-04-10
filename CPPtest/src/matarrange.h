#pragma once
#ifndef __MATARRANGE_H__
#define __MATARRANGE_H__
////////////////////////////
#include "crits.h"
#include "matcluster.h"
/////////////////////////////////////
namespace info {
	////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long,
		class ALLOCT = std::allocator<T>,
		class ALLOCDOUBLE = std::allocator<double>,
		class ALLOCINDIV = std::allocator<std::shared_ptr<Indiv<T, U, ALLOCT> > >,
		class ALLOCCLUSTER = std::allocator<std::shared_ptr<Cluster<T, U, ALLOCT, ALLOCINDIV, ALLOCDOUBLE> > >
	>
		class MatArrange : public MatCluster<T, U, Z, ALLOCT, ALLOCDOUBLE, ALLOCINDIV, ALLOCCLUSTER> {
		public:
			typedef T DataType;
			typedef U IndexType;
			typedef Z DistanceType;
			//
			typedef DistanceFunc<Z> DistanceFuncType;
			typedef IIndivProvider<T, U, ALLOCT> IIndivProviderType;
			typedef typename IIndivProviderType::IndivType IndivType;
			typedef typename IIndivProviderType::IndivTypePtr IndivTypePtr;
			typedef Cluster<T, U, ALLOCT, ALLOCINDIV, ALLOCDOUBLE> ClusterType;
			typedef std::shared_ptr<ClusterType> ClusterTypePtr;
			typedef std::vector<ClusterTypePtr, ALLOCCLUSTER> ClusterTypePtrVector;
			//
			typedef MatCluster<T, U, Z, ALLOCT, ALLOCDOUBLE, ALLOCINDIV, ALLOCCLUSTER> MatClusterType;
			typedef MatArrange<T,U,Z,ALLOCT,ALLOCDOUBLE,ALLOCINDIV,ALLOCCLUSTER> MatArrangeType;
		public:
			MatArrange(IIndivProviderType *pProvider = nullptr,
				DistanceFuncType *pfunc = nullptr,
				LinkMode mode = LinkMode::linkCenter) : MatClusterType(pProvider, pfunc, mode) {
			}// MatArrange
			MatArrange(const MatArrangeType &other) : MatClusterType(other) {}
			MatArrangeType & operator=(const MatArrangeType &other) {
				if (this != &other) {
					MatClusterType::operator=(other);
				}
				return (*this);
			}
			virtual ~MatArrange() {}
		public:
			bool arrange(const size_t nClasses = 1) {
				assert(nClasses > 0);
				if (!this->reset()) {
					return (false);
				}
				if (nClasses > this->clusters_count()) {
					return (false);
				}
				while (this->clusters_count() > nClasses) {
					if (!this->update_candidates()) {
						break;
					}
				}// while
				ClusterTypePtrVector &vv = this->clusters();
				for (auto it = vv.begin(); it != vv.end(); ++it) {
					ClusterTypePtr o = *it;
					ClusterType *px = o.get();
					assert(px != nullptr);
					if (px->is_dirty()) {
						px->update_center();
					}
				}// it
				const size_t n = vv.size();
				if (n > 1) {
					std::sort(vv.begin(), vv.end(), [](const ClusterTypePtr &o1, const ClusterTypePtr &o2)->bool {
						const ClusterType *p1 = o1.get();
						assert(p1 != nullptr);
						const ClusterType *p2 = o2.get();
						assert(p2 != nullptr);
						return (p1->compare_to(*p2) < 0);
					});
				}// sort
				for (size_t i = 0; i < n; ++i) {
					ClusterTypePtr o = vv[i];
					ClusterType *px = o.get();
					assert(px != nullptr);
					IndexType aIndex = (IndexType)(i + 1);
					StringStreamType os;
					os << STRING_CLUSTER << aIndex;
					StringType sId = os.str();
					px->index(aIndex);
					px->id(sId);
				}//i
				return (true);
			}// arrange
		protected:
			virtual bool reset(void) {
				if (!this->is_valid()) {
					return (false);
				}
				ClusterTypePtrVector &vv = this->clusters();
				vv.clear();
				this->reset_data_provider();
				IndivTypePtr oInd;
				while (this->next_indiv(oInd)) {
					const IndivType *p = oInd.get();
					assert(p != nullptr);
					ClusterTypePtr c = std::make_shared<ClusterType>(oInd);
					assert(c.get() != nullptr);
					vv.push_back(c);
				}// oInd
				return (true);
			}// reset
			bool update_candidates(void) {
				ClusterTypePtrVector &oVec = this->clusters();
				const size_t n = oVec.size();
				if (n < 2) {
					return (false);
				}
				Crits<Z,U> oCrits;
				typedef typename Crits<Z, U>::IndexTypeSet IndexTypeSet;
				this->find_best_step(oCrits);
				if (oCrits.empty()) {
					return (false);
				}
				IndexTypeSet oSet;
				IndexTypeSet oKeys;
				oCrits.keys(oKeys);
				for (auto it = oKeys.begin(); it != oKeys.end(); ++it) {
					const U i = *it;
					assert(i >= 0);
					assert(i < (U) n);
					U j;
					DistanceMode m = DistanceMode::modeInvalid;
					bool bRet = oCrits.entry_value(i, j, m);
					assert(bRet);
					assert(j >= 0);
					assert(j < (U)n);
					assert(m != DistanceMode::modeInvalid);
					ClusterTypePtr o = oVec[i];
					ClusterType *p = o.get();
					assert(p != nullptr);
					ClusterTypePtr ox = oVec[j];
					const ClusterType *px = ox.get();
					assert(px != nullptr);
					p->merge(*px, m);
					oSet.insert(j);
				}// *it
				ClusterTypePtrVector xtemp;
				for (size_t i = 0; i < n; ++i) {
					if (oSet.find((U)i) == oSet.end()) {
						xtemp.push_back(oVec[i]);
					}
				}//i
				this->clusters(xtemp);
				return (true);
			}//update_candidates
			template <class CompareU,class CompareEntry,class ALLOCU, class ALLOCC, class ALLOCP>
			void  find_best_step(Crits<Z, U, CompareU, CompareEntry, ALLOCU, ALLOCC, ALLOCP>  &oCrits) const {
				oCrits.clear();
				const size_t n = this->clusters_count();
				for (size_t i = 0; i < n; ++i) {
					for (size_t j = (size_t)(i + 1); j < n; ++j) {
						DistanceMode mode;
						Z crit = this->distance(i, j, mode);
						oCrits.add(crit, i, j, mode);
					}// j
				}// i
			}// st_find_best_step
	};// class MatArrange<T,U,Z>
	/////////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__MATARRANGE_H__
