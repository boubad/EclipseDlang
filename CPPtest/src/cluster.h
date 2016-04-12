#pragma once
#ifndef __CLUSTER_H__
#define __CLUSTER_H__
/////////////////////////////////////////
#include "utils.h"
#include "indiv.h"
#include "indivprovider.h"
#include "indivstore.h"
///////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_NORM ("norm")
#define STRING_INERTIA ("inertia")
#define STRING_VARIANCE ("variance")
#define STRING_CENTER ("center")
#define STRING_MEMBERS ("members")
#else
#define STRING_NORM (L"norm")
#define STRING_INERTIA (L"inertia")
#define STRING_VARIANCE (L"variance")
#define STRING_CENTER (L"center")
#define STRING_MEMBERS (L"members")
#endif // INFO_STRING_TYPE
//////////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	template <typename T = int, typename U = int, class ALLOCT = std::allocator<T>,
		class ALLOCPTR = std::allocator<std::shared_ptr<Indiv<T, U, ALLOCT> > >,
		class ALLOCDOUBLE = std::allocator<double>
	>
		class Cluster : public Indiv<T, U, ALLOCT> {
		public:
			typedef Indiv<T, U, ALLOCT> IndivType;
			typedef typename IndivType::DataType DataType;
			typedef typename IndivType::IndexType IndexType;
			typedef typename IndivType::DataTypeVector DataTypeVector;
			typedef std::shared_ptr<IndivType> IndivTypePtr;
			typedef std::deque<IndivTypePtr, ALLOCPTR> IndivTypePtrVector;
			typedef std::vector<double, ALLOCDOUBLE> DoubleTypeVector;
			typedef Cluster<DataType, IndexType, ALLOCT, ALLOCPTR, ALLOCDOUBLE> ClusterType;
			typedef std::shared_ptr<ClusterType> ClusterTypePtr;
		private:
			bool	_dirty;
			IndivTypePtrVector _indivs;
		public:
			Cluster(const IndexType aIndex = 0,
				const StringType &sId = StringType(),
				const DataTypeVector &data = DataTypeVector()) :IndivType(aIndex, sId, data), _dirty(false) {}
			Cluster(const IndivTypePtr &oInd) :_dirty(false) {
				this->add(oInd, true);
			}// Cluster
			Cluster(const ClusterType &other) :IndivType(other), _indivs(other._indivs) {}
			ClusterType & operator=(const ClusterType &other) {
				if (this != &other) {
					IndivType::operator=(other);
					this->_dirty = other._dirty;
					this->_indivs = other._indivs;
				}
				return (*this);
			}
			virtual ~Cluster() {}
		public:
			virtual bool is_valid(void) const {
				return (IndivType::is_valid() && (!this->_indivs.empty()));
			}
			bool is_dirty(void) const {
				return (this->_dirty);
			}
			bool empty(void) const {
				return (this->_indivs.empty());
			}
			void clear_indivs(void) {
				this->_indivs.clear();
				this->_dirty = false;
			}// clear_indivs
			ClusterType & operator+=(const IndivTypePtr &oInd) {
				this->add(oInd, false);
				return (*this);
			}// operator +=
			size_t indivs_count(void) const {
				return (this->_indivs.size());
			}
			const IndivType *indiv_at(const size_t i) const {
				assert(i < this->_indivs.size());
				return ((this->_indivs[i]).get());
			}
			const IndivType *front(void) const {
				const IndivTypePtrVector &vv = this->_indivs;
				assert(!vv.empty());
				IndivTypePtr c = vv.front();
				const IndivType *p = c.get();
				assert(p != nullptr);
				return (p);
			}// front
			const IndivType *back(void) const {
				const IndivTypePtrVector &vv = this->_indivs;
				assert(!vv.empty());
				IndivTypePtr c = vv.back();
				const IndivType *p = c.get();
				assert(p != nullptr);
				return (p);
			}// back
			virtual int compare_to(const IndivType &o) const {
				const ClusterType &other = dynamic_cast<const ClusterType &>(o);
				double n1, n2;
				this->inertia(n1);
				other.inertia(n2);
				if (n1 < n2) {
					return (-1);
				}
				else if (n1 > n2) {
					return (1);
				}
				double d1, d2;
				this->variance(d1);
				other.variance(d2);
				if (d1 < d2) {
					return (-1);
				}
				else if (d1 > d2) {
					return (1);
				}
				if (this->norm() < other.norm()) {
					return (-1);
				}
				else if (this->norm() > other.norm()) {
					return (1);
				}
				else {
					return (0);
				}
			}// compare_to
			bool operator<(const ClusterType &other) const {
				return (this->compare_to(other) < 0);
			}// operator<
			bool operator==(const ClusterType &other) const {
				return (this->compare_to(other) == 0);
			}// operator<
			bool contains_indiv_index(const IndexType aIndex) const {
				const IndivTypePtrVector &inds = this->_indivs;
				auto iend = inds.end();
				for (auto it = inds.begin(); it != iend; ++it) {
					IndivTypePtr o = *it;
					const IndivType *p = o.get();
					assert(p != nullptr);
					if (p->index() == aIndex) {
						return (true);
					}
				}// it
				return (false);
			}// contains_indiv_index
			bool contains_indiv_id(const StringType &aId) const {
				const IndivTypePtrVector &inds = this->_indivs;
				auto iend = inds.end();
				for (auto it = inds.begin(); it != iend; ++it) {
					IndivTypePtr o = *it;
					const IndivType *p = o.get();
					assert(p != nullptr);
					if (p->id() == aId) {
						return (true);
					}
				}// it
				return (false);
			}// contains_indiv_id
		public:
			bool add(const IndivTypePtr &oInd, const bool bUpdate = false) {
				const IndivType *pInd = oInd.get();
				assert(pInd != nullptr);
				const DataTypeVector &v = pInd->value();
				const size_t n = v.size();
				assert(n > 0);
				IndivTypePtrVector &inds = this->_indivs;
				if (inds.empty()) {
					this->_dirty = true;
					IndivTypePtr o(oInd);
					inds.push_back(o);
					if (bUpdate) {
						this->update_center();
					}
					return	(true);
				}
				const IndexType aIndex = pInd->index();
				if (this->contains_indiv_index(aIndex)) {
					return (false);
				}
				auto iend = inds.end();
				const size_t nt = inds.size();
				if (nt < 2) {
					IndivTypePtr o(oInd);
					this->_dirty = true;
					inds.push_back(o);
				}
				else {
					IndivTypePtr o(oInd);
					IndivType *p1 = (inds.front()).get();
					assert(p1 != nullptr);
					IndivType *p2 = (inds.back()).get();
					assert(p2 != nullptr);
					double d1 = 0, d2 = 0;
					p1->distance(*pInd, d1);
					p2->distance(*pInd, d2);
					if (d1 < d2) {
						this->_dirty = true;
						inds.push_front(o);
					}
					else {
						this->_dirty = true;
						inds.push_back(o);
					}
				}
				if (bUpdate) {
					this->update_center();
				}
				return	(true);
			}// add
			template <typename Z>
			Z compute_distance(const ClusterTypePtr &o,
				const DistanceMode mode = DistanceMode::modeCenter,
				const DistanceFunc<Z> *pFunc = nullptr) const {
				const ClusterType *p = o.get();
				assert(p != nullptr);
				return this->compute_distance(*p, mode, pFunc);
			}// compute_distance
			template <typename Z>
			Z compute_distance(const ClusterType &other,
				const DistanceMode mode = DistanceMode::modeCenter,
				const DistanceFunc<Z> *pFunc = nullptr) const {
				Z res = 0;
				assert(mode != DistanceMode::modeInvalid);
				const IndivTypePtrVector &inds1 = this->_indivs;
				const IndivTypePtrVector &inds2 = other._indivs;
				switch (mode) {
				case DistanceMode::modeCenter:
				{
					assert(this->is_valid());
					assert(other.is_valid());
					info_distance(this->value(), other.value(), res, pFunc);
				}
				break;
				case DistanceMode::modeUpUp:
				{
					const IndivType *p1 = this->front();
					const IndivType *p2 = other.front();
					info_distance(p1->value(), p2->value(), res, pFunc);
				}
				break;
				case DistanceMode::modeUpDown:
				{
					const IndivType *p1 = this->front();
					const IndivType *p2 = other.back();
					info_distance(p1->value(), p2->value(), res, pFunc);
				}
				break;
				case DistanceMode::modeDownUp:
				{
					const IndivType *p1 = this->back();
					const IndivType *p2 = other.front();
					info_distance(p1->value(), p2->value(), res, pFunc);
				}
				break;
				case DistanceMode::modeDownDown:
				{
					const IndivType *p1 = this->back();
					const IndivType *p2 = other.back();
					info_distance(p1->value(), p2->value(), res, pFunc);
				}
				break;
				default:
					// should not fall here!
					assert(false);
					break;
				}// mode
				return (res);
			}// compute_distance
			void merge(const ClusterType &other, 
				const DistanceMode mode = DistanceMode::modeUpUp,
				bool bUpdate = false) {
				assert(!(this->empty()));
				assert(!(other.empty()));
				IndivTypePtrVector &dest = this->_indivs;
				const IndivTypePtrVector &src = other._indivs;
				const size_t n = src.size();
				switch (mode) {
				case DistanceMode::modeUpUp:
				{
					for (size_t i = 0; i < n; ++i) {
						dest.push_front(src[i]);
					}// i
				}
				break;
				case DistanceMode::modeUpDown:
				{
					for (size_t i = (size_t)(n - 1); i >= 0; i--) {
						dest.push_front(src[i]);
						if (i == 0) {
							break;
						}
					}//i
				}
				break;
				case DistanceMode::modeDownUp:
				{
					for (size_t i = 0; i < n; ++i) {
						dest.push_back(src[i]);
					}// i
				}
				break;
				case DistanceMode::modeDownDown:
				{
					for (size_t i = (size_t)(n - 1); i >= 0; i--) {
						dest.push_back(src[i]);
						if (i == 0) {
							break;
						}
					}// i
				}
				break;
				default:
					// should not fall here!
					assert(false);
					break;
				}// mode
				this->_dirty = true;
				if (bUpdate) {
					this->update_center();
				}
			}// merge
			template <typename Z>
			Z find_min_distance(const ClusterType &other, DistanceMode &mode, Z & /*dummy*/,
				const DistanceFunc<Z> *pFunc = nullptr) const {
				//
				assert(!(this->empty()));
				assert(!(other.empty()));
				//
				mode = DistanceMode::modeUpUp;
				Z d = 0;
				Z dMin = this->compute_distance(other, DistanceMode::modeUpUp, pFunc);
				const size_t n1 = this->_indivs.size();
				const size_t n2 = other._indivs.size();
				if ((n1 < 2) && (n2 < 2)) {
					return (dMin);
				}
				else if ((n1 < 2) && (n2 > 1)) {
					d = this->compute_distance(other, DistanceMode::modeUpDown, pFunc);
					if (d < dMin) {
						mode = DistanceMode::modeUpDown;
						return (d);
					}
				}
				else  if ((n1 > 1) && (n2 < 2)) {
					d = this->compute_distance(other, DistanceMode::modeDownUp, pFunc);
					if (d < dMin) {
						mode = DistanceMode::modeDownUp;
						return (d);
					}
				}
				d = this->compute_distance(other, DistanceMode::modeUpDown, pFunc);
				if (d < dMin) {
					dMin = d;
					mode = DistanceMode::modeUpDown;
				}
				d = this->compute_distance(other, DistanceMode::modeDownUp, pFunc);
				if (d < dMin) {
					dMin = d;
					mode = DistanceMode::modeDownUp;
				}
				d = this->compute_distance(other, DistanceMode::modeDownDown, pFunc);
				if (d < dMin) {
					dMin = d;
					mode = DistanceMode::modeDownDown;
				}
				return (dMin);
			}// find_min_distance
			template <typename Z>
			void inertia(Z &dist) const {
				DistanceFunc<double> ofunc;
				const IndivTypePtrVector &v1 = this->_indivs;
				const size_t n = v1.size();
				const DataTypeVector &oAr0 = this->value();
				double ss = 0.0;
				for (size_t i = 0; i < n; ++i) {
					const IndivTypePtr &oInd1 = v1[i];
					const IndivType *p1 = oInd1.get();
					assert(p1 != nullptr);
					const DataTypeVector &oAr1 = p1->value();
					double dd = 0;
					info_distance(oAr0, oAr1, dd, &ofunc);
					ss += dd;
				}// i
				if (n > 0) {
					ss = (ss / n);
				}
				dist = (Z)ss;
			}// inertia
			template <typename Z>
			void variance(Z &dist) const {
				DistanceFunc<double> ofunc;
				const IndivTypePtrVector &v1 = this->_indivs;
				const size_t n = v1.size();
				size_t count = 0;
				double ss = 0.0;
				for (size_t i = 0; i < n; ++i) {
					IndivTypePtr oInd1 = v1[i];
					const IndivType *p1 = oInd1.get();
					assert(p1 != nullptr);
					const DataTypeVector &oAr1 = p1->value();
					for (size_t j = i + 1; j < n; ++j) {
						IndivTypePtr oInd2 = v1[j];
						const IndivType *p2 = oInd2.get();
						assert(p2 != nullptr);
						const DataTypeVector &oAr2 = p2->value();
						double dd = 0;
						info_distance(oAr1, oAr2, dd, &ofunc);
						ss += dd;
						++count;
					}// j
				}// i
				if (count > 1) {
					ss = ss / count;
				}
				dist = (Z)ss;
			}// intra_variance
			OStreamType & operator<<(OStreamType &os) const {
				return (this->write_to(os));
			}
			virtual OStreamType & write_to(OStreamType &os) const {
				double var = 0;
				double tx = 0;
				this->inertia(tx);
				this->variance(var);
				//
				os << std::endl << START_OBJECT;
				info_write_json_value(os, STRING_INDEX, this->index()) << STRING_COMMA;
				info_write_json_value(os, STRING_ID, this->id()) << STRING_COMMA;
				info_write_json_value(os, STRING_CENTER, this->value()) << STRING_COMMA;
				info_write_json_value(os, STRING_INERTIA, tx) << STRING_COMMA;
				info_write_json_value(os, STRING_VARIANCE, var) << STRING_COMMA;
				info_write_json_value(os, STRING_NORM, this->norm()) << STRING_COMMA;
				os << std::endl;
				os << STRING_QUOTE << STRING_MEMBERS << STRING_QUOTE_END << START_ARRAY;
				const IndivTypePtrVector &inds = this->_indivs;
				auto iStart = inds.begin();
				auto iEnd = inds.end();
				for (auto it = iStart; it != iEnd; ++it) {
					os << std::endl;
					if (it != iStart) {
						os << STRING_COMMA;
					}
					IndivTypePtr oInd = *it;
					const IndivType *p2 = oInd.get();
					assert(p2 != nullptr);
					os << *p2;
				}// it
				os << END_ARRAY << std::endl;
				os << END_OBJECT;
				//
				return (os);
			}// write_to
			void update_center(void) {
				IndivTypePtrVector &inds = this->_indivs;
				const size_t nt = inds.size();
				this->_dirty = false;
				if (nt < 1) {
					return;
				}
				IndivTypePtr oInd = inds[0];
				IndivType *p = oInd.get();
				assert(p != nullptr);
				const size_t nc = (p->value()).size();
				assert(nc > 0);
				double zero = 0;
				DoubleTypeVector somme(nc, zero);
				for (size_t i = 0; i < nt; ++i) {
					IndivTypePtr o = inds[i];
					const IndivType *px = o.get();
					assert(px != nullptr);
					const DataTypeVector &v = p->value();
					assert(v.size() == nc);
					for (size_t j = 0; j < nc; ++j) {
						somme[j] += (double)v[j];
					}// j
				}// 
				DataTypeVector vRes(nc);
				for (size_t i = 0; i < nc; ++i) {
					vRes[i] = (DataType)(somme[i] / nt);
				}// i
				this->value(vRes);
			}// update_center
	};// class Cluster<T,U,Z,ALLOCT>
	////////////////////////////////////////////////
}// namespace info
//////////////////////////////////////////////
#endif // !__CLUSTER_H__
