#pragma once
#ifndef __MATCLUSTER_H__
#define __MATCLUSTER_H__
//////////////////////////////
#include "cluster.h"
///////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_CLUSTERS ("cluters")
#else
#define STRING_CLUSTERS (L"cluters")
#endif // INFO_STRING_TYPE
//////////////////////////////////
namespace info {
	/////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long,
		class ALLOCT = std::allocator<T> ,
		class ALLOCDOUBLE = std::allocator<double>,
		class ALLOCINDIV = std::allocator<std::shared_ptr<Indiv<T, U, ALLOCT> > >,
		class ALLOCCLUSTER = std::allocator<std::shared_ptr<Cluster<T, U, ALLOCT, ALLOCINDIV, ALLOCDOUBLE> > >
	   > class MatCluster: public BaseInfo {
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
			typedef MatCluster<T, U, Z, ALLOCT,ALLOCDOUBLE, ALLOCINDIV, ALLOCCLUSTER> MatClusterType;
		private:
			IIndivProviderType *_provider;
			DistanceFuncType *_pfunc;
			ClusterTypePtrVector _vec;
			LinkMode _link;
		public:
			MatCluster(IIndivProviderType *pProvider = nullptr, 
				DistanceFuncType *pfunc = nullptr,
				LinkMode mode = LinkMode::linkCenter) :
				_provider(pProvider), _pfunc(pfunc),_link(mode) {
			}// MatArrange
			MatCluster(const MatClusterType &other) :BaseInfo(other),_provider(other._provider),
				_pfunc(other._pfunc), _vec(other._vec),_link(other._link) {}
			MatClusterType & operator=(const MatClusterType &other) {
				if (this != &other) {
					BaseInfo::operator=(other);
					this->_provider = other._provider;
					this->_pfunc = other._pfunc;
					this->_vec = other._vec;
					this->_link = other._link;
				}
				return (*this);
			}
			virtual ~MatCluster() {}
		public:
			IIndivProviderType *provider(void) const {
				return (this->provider);
			}
			void provider(IIndivProviderType *p) {
				this->_provider = p;
			}
			DistanceFuncType *distance_function(void) const {
				return (this->_pfunc);
			}
			void distance_function(DistanceFuncType *pf) {
				this->_pfunc = pf;
			}
			const ClusterTypePtrVector & clusters(void) const {
				return (this->_vec);
			}
			size_t clusters_count(void) const {
				return (this->clusters().size());
			}
			const ClusterType *cluster_at(const size_t i) const {
				assert(i < this->_vec.size());
				const ClusterTypePtr &c = this->_vec[i];
				const ClusterType *p = c.get();
				assert(p != nullptr);
				return (p);
			}
			LinkMode linkMode(void) const {
				return (this->_link);
			}
			void linkMode(const LinkMode m) {
				if (m != LinkMode::noLink) {
					this->_link = m;
				}
			}
			void reset_data_provider(void) {
				if (this->_provider != nullptr) {
					this->_provider->reset();
				}
			}
			bool next_indiv(IndivTypePtr &oInd) {
				bool bRet = false;
				if (this->_provider != nullptr) {
					bRet = this->_provider->next(oInd);
				}
				return (bRet);
			}
		protected:
			ClusterTypePtrVector & clusters(void) {
				return (this->_vec);
			}
			void clusters(const ClusterTypePtrVector &v) {
				this->_vec = v;
			}
			virtual bool reset(void) {
				return (this->is_valid());
			}
		public:
			virtual bool is_valid(void) const {
				return ((this->_provider != nullptr) && (this->_link != LinkMode::noLink));
			}
		public:
			DistanceType distance(size_t ipos, size_t jpos, DistanceMode &mode) const {
				const ClusterTypePtrVector &oVec = this->_vec;
				const size_t n = oVec.size();
				assert(ipos < n);
				assert(jpos < n);
				assert(ipos != jpos);
				//
				const ClusterType *p1 = (oVec[ipos]).get();
				assert(p1 != nullptr);
				const ClusterType *p2 = (oVec[jpos]).get();
				assert(p2 != nullptr);
				//
				DistanceType dummy;
				DistanceType dist = p1->find_min_distance(*p2, mode, dummy, const_cast<const DistanceFuncType *>(this->_pfunc));
				return (dist);
			}// distance
			DistanceType distance(const size_t ipos, const size_t jpos) const {
				const ClusterTypePtrVector &oVec = this->_vec;
				const size_t n = oVec.size();
				assert(ipos < n);
				assert(jpos < n);
				assert(ipos != jpos);
				//
				const ClusterType *p1 = (oVec[ipos]).get();
				assert(p1 != nullptr);
				const ClusterType *p2 = (oVec[jpos]).get();
				assert(p2 != nullptr);
				//
				return p1->compute_distance(*p2, this->linkMode(), this->distance_function());
				//
			}// distance
			virtual OStreamType & write_to(OStreamType &os) const {
				os << std::endl << START_OBJECT;
				os << std::endl;
				os << STRING_QUOTE << STRING_CLUSTERS << STRING_QUOTE_END << START_ARRAY;
				const ClusterTypePtrVector &inds = this->clusters();
				auto iStart = inds.begin();
				auto iEnd = inds.end();
				for (auto it = iStart; it != iEnd; ++it) {
					os << std::endl;
					if (it != iStart) {
						os << STRING_COMMA;
					}
					ClusterTypePtr oInd = *it;
					const ClusterType *p2 = oInd.get();
					assert(p2 != nullptr);
					os << *p2;
				}// it
				os << END_ARRAY << std::endl;
				os << END_OBJECT;
				//
				return (os);
			}// write_to
			void toString(StringType &s) const {
				StringStreamType os;
				this->write_to(os);
				s = os.str();
			}// toString
	}; // class  MatCluster<T, U, Z, ALLOCT,ALLOCDOUBLE, ALLOCINDIV, ALLOCCLUSTER>
	/////////////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__MATCLUSTER_H__
