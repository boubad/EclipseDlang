#pragma once
#ifndef __TREEELEM_H__
#define __TREEELEM_H__
////////////////////////////////
#include "cluster.h"
#include "elemcrit.h"
////////////////////////////////////
namespace info {
	///////////////////////////////////////
#if defined(INFO_STRING_TYPE)
#define DOT_STRING_1  ("digraph G { ")
#define DOT_STRING_2  ("size = \"4,4\";")
#define DOT_STRING_3  (" [")
#define DOT_STRING_4 ( "] ")
#define DOT_STRING_5 (" ")
#define DOT_STRING_6 ( " [shape = box,style=filled,color=yellow, label=\"")
#define DOT_STRING_7 ("\"];")
#define DOT_STRING_8 ( "}")
#define DOT_STRING_9 (" [label=\"S")
#define DOT_STRING_10 ("\\nVar = ")
#define DOT_STRING_11 ( "\"];")
#define DOT_STRING_12 ( " -> ")
#define DOT_STRING_13 ( ";")
#define DOT_STRING_14 (" [label=\"ROOT\"];")
#else
#define DOT_STRING_1  (L"digraph G { ")
#define DOT_STRING_2  (L"size = \"4,4\";")
#define DOT_STRING_3  (L" [")
#define DOT_STRING_4 ( L"] ")
#define DOT_STRING_5 (L" ")
#define DOT_STRING_6 ( L" [shape = box,style=filled,color=yellow, label=\"")
#define DOT_STRING_7 (L"\"];")
#define DOT_STRING_8 ( L"}")
#define DOT_STRING_9 (L" [label=\"S")
#define DOT_STRING_10 (L"\\nVar = ")
#define DOT_STRING_11 ( L"\"];")
#define DOT_STRING_12 ( L" -> ")
#define DOT_STRING_13 ( L";")
#define DOT_STRING_14 (L" [label=\"ROOT\"];")
#endif // INFO_STRING_TYPE
	//////////////////////////////////////
	template <typename T, typename U, typename Z,
		class ALLOCT, class ALLOCDOUBLE>
	    class TreeElem : BaseInfo {
		public:
			typedef T DataType;
			typedef U IndexType;
			typedef Z DistanceType;
			//
			typedef DistanceFunc<DistanceType> DistanceFuncType;
			typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
			typedef typename IndivType::DataTypeVector DataTypeVector;
			typedef std::shared_ptr<IndivType> IndivTypePtr;
			typedef std::vector<double, ALLOCDOUBLE> DoubleTypeVector;
			//
			typedef TreeElem<T, U, Z, ALLOCT,ALLOCDOUBLE> TreeElemType;
			typedef std::shared_ptr<TreeElemType> TreeElemTypePtr;
			typedef std::deque<TreeElemTypePtr> TreeElemTypePtrVector;
		private:
			IndexType _index;
			size_t _order;
			DistanceType _dist;
			StringType _id;
			IndivTypePtr _center;
			TreeElemTypePtrVector _children;
		public:
			TreeElem(const IndexType aIndex = 0, const size_t aOrder = 0,
				const DistanceType dist = 0,
				const StringType &sId = StringType(),
				IndivTypePtr oCenter = IndivTypePtr()
			) :_index(aIndex), _order(aOrder), _dist(dist), _id(sId), _center(oCenter)
			{
			}// TreeElem
			TreeElem(const TreeElemType &other) :BaseInfo(other), _index(other._index), _order(other._order),
				_dist(other._dist), _id(other._id), _center(other._center), _children(other._children) {}
			TreeElemType & operator=(const TreeElemType &other) {
				if (this != &other) {
					BaseInfo::operator=(other);
					this->_index = other._index;
					this->_order = other._order;
					this->_dist = other._dist;
					this->_id = other._id;
					this->_center = other._center;
					this->_children = other._children;
				}
				return (*this);
			}
			virtual ~TreeElem() {}
		public:
			OStreamType & operator<<(OStreamType &os) const {
				return (this->write_to(os));
			}
			OStreamType & write_to(OStreamType &os) const {
				os << START_OBJECT << this->index() << STRING_COMMA;
				os << this->id() << STRING_COMMA;
				os << this->order() << STRING_COMMA;
				os << this->linkDistance() << STRING_COMMA;
				if (this->center().get() != nullptr) {
					const IndivType *p = this->center().get();
					assert(p != nullptr);
					os << *p << STRING_COMMA;
				}
				const TreeElemTypePtrVector &vv = this->children();
				const size_t n = vv.size();
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					p->write_to(os);
				}// i
				os << END_OBJECT;
				return (os);
			}// write_to
			
			void sort(void) {
				TreeElemTypePtrVector &vv = this->_children;
				const size_t n = vv.size();
				if (n < 2) {
					return;
				}
				std::sort(vv.begin(), vv.end(), [](const TreeElemTypePtr &o1, const TreeElemTypePtr &o2)->bool {
					const TreeElemType *p1 = o1.get();
					assert(p1 != nullptr);
					const TreeElemType *p2 = o2.get();
					assert(p2 != nullptr);
					if (p1->order() < p2->order()) {
						return (true);
					}
					else if (p1->order() > p2->order()) {
						return (false);
					}
					return (p1->linkDistance() < p2->linkDistance());
				});
			}// sort
			OStreamType & writeDot(OStreamType &out) const
			{
				out << std::endl;
				out << DOT_STRING_1 << std::endl;
				out << DOT_STRING_2 << std::endl;
				const TreeElemTypePtrVector &vv = this->children();
				const size_t n = vv.size();
				if (n == 0)
				{
					IndexType nId = this->index();
					std::wstringstream os;
					os << DOT_STRING_3 << nId << DOT_STRING_4;
					const StringType &ss = this->id();
					if (!ss.empty())
					{
						os << ss << DOT_STRING_5;
					}
					std::wstring slabel = os.str();
					out << nId << DOT_STRING_6;
					<< slabel << DOT_STRING_7 << std::endl;
				}
				else
				{
					for (size_t i = 0; i < n; ++i)
					{
						const TreeElemTypePtr &pp = vv[i];
						const TreeElemType *p = pp.get();
						p->internalWriteTo(out);
					} // i
				}
				out << DOT_STRING_8 << std::endl;
				return (out);
			} // writeDot
		public:
			DistanceType linkDistance(void) const {
				return (this->_dist);
			}
			void linkDistance(DistanceType d) {
				this->_dist = d;
			}
			const TreeElemTypePtrVector &children(void) const {
				return (this->_children);
			}
			void children(const TreeElemTypePtrVector &vv) {
				this->_children = vv;
			}
			bool is_leaf(void) const {
				return (this->_children.empty());
			}
			const StringType &id(void) const {
				return (this->_id);
			}
			void id(const StringType &s) {
				this->_id = s;
			}
			IndexType index(void) const {
				return (this->_index);
			}
			size_t order(void) const {
				return (this->_order);
			}
			const IndivTypePtr &center(void) const {
				return (this->_center);
			}
			IndivTypePtr &center(void) {
				return (this->_center);
			}
			void center(const  IndivTypePtr &c) {
				this->_center = c;
			}
			const IndivType *get_center(void) const {
				const IndivType *p = this->_center.get();
				assert(p != nullptr);
				return (p);
			}
			DistanceType distance(const TreeElemType &other, const LinkMode link, DistanceFuncType *pfunc = nullptr) const {
				assert(link != LinkMode::noLink);
				DistanceFuncType *pf = pfunc;
				DistanceFunc<DistanceType> oDef;
				if (pf == nullptr) {
					pf = &oDef;
				}
				const IndivType *pInd1 = this->get_center();
				const TreeElemTypePtrVector &vv1 = this->children();
				const TreeElemTypePtrVector &vv2 = other.children();
				const size_t n1 = vv1.size();
				const size_t n2 = vv2.size();
				DistanceType result = 0;
				//
				if ((n1 > 1) && (n2 < 1)) {
					return (other.distance(*this, link, pf));
				}
				if (((n1 < 1) && (n2 < 1)) || (link == LinkMode::linkCenter)) {
					const IndivType *pInd2 = other.get_center();
					pInd1->distance(*pInd2, result, pf);
					return (result);
				}
				if ((n1 < 1) && (n2 > 0)) {
					switch (link) {
					case LinkMode::linkMin:
					{
						const TreeElemTypePtr &t0 = vv2[0];
						const TreeElemType *p0 = t0.get();
						assert(p0 != nullptr);
						const IndivType *pIndx = p0->get_center();
						assert(pIndx != nullptr);
						pInd1->distance(*pIndx, result, pf);
						for (size_t i = 1; i < n2; ++i) {
							const TreeElemTypePtr &t = vv2[i];
							const TreeElemType *p = t.get();
							assert(p != nullptr);
							const IndivType *pInd = p->get_center();
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
							if (rr < result) {
								result = rr;
							}
						}// i
					}
					break;
					case LinkMode::linkMax: {
						const TreeElemTypePtr &t0 = vv2[0];
						const TreeElemType *p0 = t0.get();
						assert(p0 != nullptr);
						const IndivType *pIndx = p0->get_center();
						assert(pIndx != nullptr);
						pInd1->distance(*pIndx, result, pf);
						for (size_t i = 1; i < n2; ++i) {
							const TreeElemTypePtr &t = vv2[i];
							const TreeElemType *p = t.get();
							assert(p != nullptr);
							const IndivType *pInd = p->get_center();
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
							if (rr > result) {
								result = rr;
							}
						}// i
					}
											break;
					case LinkMode::linkMean:
					{
						for (size_t i = 1; i < n2; ++i) {
							const TreeElemTypePtr &t = vv2[i];
							const TreeElemType *p = t.get();
							assert(p != nullptr);
							const IndivType *pInd = p->get_center();
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
							result = (DistanceType)(result + rr);
						}// i
						result = (DistanceType)(result / n2);
					}
					default:
						break;
					}// link
				}
				else if ((n1 > 1) && (n2 > 1)) {
					switch (link) {
					case LinkMode::linkMin:
					{
						for (size_t i = 0; i < n1; ++i) {
							const TreeElemTypePtr &t1 = vv1[i];
							const TreeElemType *p1 = t1.get();
							assert(p1 != nullptr);
							const IndivType *px = p1->get_center();
							for (size_t j = 0; j < n2; ++j) {
								const TreeElemTypePtr &t2 = vv2[j];
								const TreeElemType *p2 = t2.get();
								assert(p2 != nullptr);
								const IndivType *pInd = p2->get_center();
								DistanceType rr = 0;
								px->distance(*pInd, rr, pf);
								if ((i == 0) && (j == 0)) {
									result = rr;
								}
								else if (rr < result) {
									result = rr;
								}
							}// j
						}// i
					}
					break;
					case LinkMode::linkMax:
					{
						for (size_t i = 0; i < n1; ++i) {
							const TreeElemTypePtr &t1 = vv1[i];
							const TreeElemType *p1 = t1.get();
							assert(p1 != nullptr);
							const IndivType *px = p1->get_center();
							for (size_t j = 0; j < n2; ++j) {
								const TreeElemTypePtr &t2 = vv2[j];
								const TreeElemType *p2 = t2.get();
								assert(p2 != nullptr);
								const IndivType *pInd = p2->get_center();
								DistanceType rr = 0;
								px->distance(*pInd, rr, pf);
								if ((i == 0) && (j == 0)) {
									result = rr;
								}
								else if (rr > result) {
									result = rr;
								}
							}// j
						}// i
					}
					break;
					case LinkMode::linkMean:
					{
						size_t count = 0;
						for (size_t i = 0; i < n1; ++i) {
							const TreeElemTypePtr &t1 = vv1[i];
							const TreeElemType *p1 = t1.get();
							assert(p1 != nullptr);
							const IndivType *px = p1->get_center();
							for (size_t j = 0; j < n2; ++j) {
								const TreeElemTypePtr &t2 = vv2[j];
								const TreeElemType *p2 = t2.get();
								assert(p2 != nullptr);
								const IndivType *pInd = p2->get_center();
								assert(pInd != nullptr);
								DistanceType rr = 0;
								px->distance(*pInd, rr, pf);
								result = (DistanceType)(result + rr);
								++count;
							}// j
						}// i
						result = (Z)(result / count);
					}
					break;
					default:
						break;
					}// link
				}
				return (result);
			}// distance
			void update_center(void) {
				const TreeElemTypePtrVector &vv = const_cast<TreeElemTypePtrVector &>(this->_children);
				const size_t n = vv.size();
				if (n < 1) {
					return;
				}
				const TreeElemTypePtr &t = vv[0];
				const TreeElemType *p = t.get();
				assert(p != nullptr);
				const IndivType *pInd = p->get_center();
				assert(pInd != nullptr);
				const DataTypeVector &dd = pInd->value();
				const size_t nCols = dd.size();
				assert(nCols > 0);
				double zero = 0.0;
				DoubleTypeVector sum(nCols, zero);
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &t = vv[i];
					const TreeElemType *p = t.get();
					assert(p != nullptr);
					const IndivType *pInd = p->get_center();
					assert(pInd != nullptr);
					const DataTypeVector &d = pInd->value();
					for (size_t j = 0; j < nCols; ++j) {
						sum[j] += (double)d[j];
					}// j
				}// i
				DataTypeVector val(nCols);
				for (size_t i = 0; i < nCols; ++i) {
					val[i] = (DataType)(sum[i] / nCols);
				}
				IndivTypePtr oCenter = this->_center;
				IndivType *pCenter = oCenter.get();
				if (pCenter == nullptr) {
					StringType sx;
					this->_center = std::make_shared<IndivType>(this->index(), sx, val);
				}
				else {
					pCenter->value(val);
				}
			}// update_center
			template <class ALLOCX>
			void get_indivs(std::vector<IndivTypePtr, ALLOCX>  &oVec) const {
				const TreeElemTypePtrVector &vv = this->_children;
				const size_t n = vv.size();
				if (n < 1) {
					if (this->_center.get() != nullptr) {
						oVec.push_back(this->_center);
					}
					return;
				}
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					p->get_indivs(oVec);
				}// i
			}//get_indivs
			template <class ALLOCX>
			void get_indivs_indexes(std::vector<IndexType, ALLOCX> &oVec) const {
				const TreeElemTypePtrVector &vv = this->_children;
				const size_t n = vv.size();
				if (n < 1) {
					if (this->_center.get() != nullptr) {
						oVec.push_back(this->index());
					}
					return;
				}
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					p->get_indivs_indexes(oVec);
				}// i
			}//get_indivs_indexes
			template <class ALLOCX>
			void get_indivs_ids(std::vector<StringType, ALLOCX> &oVec) const {
				const TreeElemTypePtrVector &vv = this->_children;
				const size_t n = vv.size();
				if (n < 1) {
					if (this->_center.get() != nullptr) {
						oVec.push_back(this->id());
					}
					return;
				}
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					p->get_indivs_ids(oVec);
				}// i
			}//get_indivs_ids
		public:
			OStreamType & internalWriteTo(OStreamType &out) const
			{
				const TreeElemTypePtrVector &vv = this->children();
				const size_t n = vv.size();
				const size_t nId = (size_t) this->index();
				if (n == 0)
				{
					std::wstringstream os;
					os << DOT_STRING_3 << nId << DOT_STRING_4;
					const StringType & ss = this->id();
					if (!ss.empty())
					{
						os << ss << DOT_STRING_5;
					}
					std::wstring slabel = os.str();
					out << nId << DOT_STRING_6;
					out << slabel << DOT_STRING_7 << std::endl;
				}
				else
				{
					out << nId << DOT_STRING_9 << nId << DOT_STRING_10;
					out << this->linkDistance() << DOT_STRING_11 << std::endl;
					for (size_t i = 0; i < n; ++i)
					{
						const TreeElemTypePtr &pp = vv[i];
						const TreeElemType *p = pp.get();
						assert(p != nullptr);
						p->internalWriteTo(out);
						out << nId << DOT_STRING_12 << p->index() << DOT_STRING_13 << std::endl;
					} // i
				}
				return (out);
			} // internalWriteTo
	};// class TreeElem<T,U,Z,ALLOCT,ALLOCPTR,ALLOCDOUBLE>
	  /////////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long,
		class Compare = std::less<U>,
		class ALLOCT = std::allocator<T>, 
		class ALLOCU = std::allocator<U>,
		class ALLOCZ = std::allocator<Z>,
	    class ALLOCDOUBLE=std::allocator<double>,
		class ALLOCSET = std::allocator<std::pair<U,std::set<U,Compare,ALLOCU>>>,
		class ALLOCPTR = std::allocator<std::shared_ptr<TreeElem<T, U, Z, ALLOCT,ALLOCDOUBLE >> >
	>
		class Tree : BaseInfo {
		public:
			typedef T DataType;
			typedef U IndexType;
			typedef Z DistanceType;
			//
			typedef std::set<U, Compare, ALLOCU> IndexTypeSet;
			typedef std::map<U, IndexTypeSet, Compare, ALLOCSET> IndexTypeSetMap;
			//
			typedef IIndivProvider<DataType, IndexType, ALLOCT> IIndivProviderType;
			typedef typename IIndivProviderType::IndivType IndivType;
			typedef typename IIndivProviderType::IndivTypePtr IndivTypePtr;
			//
			typedef TreeElem<DataType, IndexType, DistanceType, ALLOCT,ALLOCDOUBLE> TreeElemType;
			typedef DistanceFunc<DistanceType> DistanceFuncType;
			typedef typename TreeElemType::TreeElemTypePtr TreeElemTypePtr;
			typedef typename TreeElemType::TreeElemTypePtrVector TreeElemTypePtrVector;
			typedef typename TreeElemType::DoubleTypeVector DoubleTypeVector;
			typedef std::vector<DistanceType, ALLOCZ> DistanceTypeVector;
			typedef typename TreeElemType::DataTypeVector DataTypeVector;
			//
			typedef Tree<T, U, Z,Compare, ALLOCT, ALLOCU,ALLOCZ,ALLOCDOUBLE,ALLOCSET,ALLOCPTR> TreeType;
		private:
			IIndivProviderType *_provider;
			LinkMode _link;
			DistanceFuncType *_pfunc;
			TreeElemTypePtrVector _elems;
			DistanceTypeVector _distances;
		public:
			Tree(IIndivProviderType *pProvider,
				const LinkMode m = LinkMode::linkMean,
				DistanceFuncType *fdist = nullptr) :_provider(pProvider), _link(m), _pfunc(fdist) {
			}
			Tree(const TreeType &other) :BaseInfo(other),_provider(other._privider), _link(other._link),
				_pfunc(other._pfunc), _elems(other._elems),
				_distances(other._distances) {}
			TreeType & operator=(const TreeType &other) {
				if (this != &other) {
					BaseInfo::operator=(other);
					this->_provider = other._provider;
					this->_link = other._link;
					this->_pfunc = other._pfunc;
					this->_elems = other._elems;
					this->_distances = other._distances;
				}
				return (*this);
			}
			virtual ~Tree() {}
		public:
			bool is_valid(void) const {
				return ((this->_provider != nullptr) && (this->_link != LinkMode::noLink));
			}
			OStreamType & write_to(OStreamType &os) const {
				os << START_OBJECT;
				const size_t n = this->_elems.size();
				os << n;
				for (size_t i = 0; i < n; ++i) {
					const TreeElemType *p = ((this->_elems)[i]).get();
					assert(p != nullptr);
					os << STRING_COMMA << *p << std::endl;
				}// i
				os << END_OBJECT << std::endl;
				return (os);
			}// write_to
			void toString(StringType &s) const {
				StringStreamType os;
				this->write_to(os);
				s = os.str();
			}// toString
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
			const size_t size(void) const {
				return (this->_elems.size());
			}
			const size_t elements_count(void) const {
				return (this->_elems.size());
			}
			const TreeElemType *element_at(const size_t i) const {
				assert(i < this->_elems.size());
				const TreeElemTypePtr &c = this->_elems[i];
				const TreeElemType *p = c.get();
				assert(p != nullptr);
				return (p);
			}// element_at
			bool reset(void) {
				if (!this->is_valid()) {
					return (false);
				}
				this->_elems.clear();
				this->_distances.clear();
				IIndivProviderType *pProvider = this->_provider;
				assert(pProvider != nullptr);
				pProvider->reset();
				//
				TreeElemTypePtrVector &vv = this->_elems;
				const size_t order = 0;
				const DistanceType dist = 0;
				IndivTypePtr oCenter;
				IndexType aIndex = 0;
				while (pProvider->next(oCenter)) {
					const IndivType *pCenter = oCenter.get();
					assert(pCenter != nullptr);
					aIndex++;
					StringType sId = pCenter->id();
					if (sId.empty()) {
						StringStreamType os;
						os << STRING_LEAF << aIndex;
						sId = os.str();
					}
					TreeElemTypePtr t = std::make_shared<TreeElemType>(aIndex, order, dist, sId, oCenter);
					assert(t.get() != nullptr);
					vv.push_back(t);
				}// while
				return (this->_elems.size() > 0);
			}// initialize
			bool aggregate(const size_t nClasses = 1) {
				assert(nClasses > 0);
				if (!this->reset()) {
					return (false);
				}
				if (this->_elems.size() == nClasses) {
					return (true);
				}
				if (nClasses > this->_elems.size()) {
					return (false);
				}
				if (!this->compute_distances()) {
					return (false);
				}
				size_t nOrder = 0;
				while (this->_elems.size() > nClasses) {
					if (!this->aggreg_one_step(nOrder)) {
						break;
					}
				}// while size
				this->sort();
				return (this->_elems.size() == nClasses);
			}// aggregate
			OStreamType & writeDot(OStreamType &out) const
			{
				out << std::endl;
				out << DOT_STRING_1 << std::endl;
				out << DOT_STRING_2 << std::endl;
				const TreeElemTypePtrVector &vv = this->_elems;
				const size_t n = vv.size();
				if (n < 1)
				{
					return (out);
				}
				size_t nId = 0;
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &pp = vv[i];
					const TreeElemType *p = pp.get();
					assert(p != nullptr);
					const size_t nx = (size_t)p->index();
					if (i == 0) {
						nId = nx;
					}
					else if (nx > nId) {
						nId = nx;
					}
				}// i
				++nId;
				out << nId << DOT_STRING_14 << std::endl;
				for (size_t i = 0; i < n; ++i)
				{
					const TreeElemTypePtr &pp = vv[i];
					const TreeElemType *p = pp.get();
					assert(p != nullptr);
					p->internalWriteTo(out);
					out << nId << DOT_STRING_12 << p->index() << DOT_STRING_13 << std::endl;
				} // i
				out << DOT_STRING_8 << std::endl;
				return (out);
			} // writeDot
		private:
			void sort(void) {
				TreeElemTypePtrVector &vv = this->_elems;
				const size_t n = vv.size();
				if (n < 2) {
					return;
				}
				std::sort(vv.begin(), vv.end(), [](const TreeElemTypePtr &o1, const TreeElemTypePtr &o2)->bool {
					const TreeElemType *p1 = o1.get();
					assert(p1 != nullptr);
					const TreeElemType *p2 = o2.get();
					assert(p2 != nullptr);
					if (p1->order() < p2->order()) {
						return (true);
					}
					else if (p1->order() > p2->order()) {
						return (false);
					}
					return (p1->linkDistance() < p2->linkDistance());
				});
			}// sort
			bool aggreg_one_step(size_t &nOrder) {
				IndexTypeSetMap oMap;
				DistanceType dMin = 0;
				TreeElemTypePtrVector &vv = this->_elems;
				const size_t n = vv.size();
				if (n < 2) {
					return false;
				}
				const IndexType nm1 = (IndexType)(n - 1);
				for (IndexType i = 0; i < nm1; ++i) {
					for (IndexType j = (IndexType)(i + 1); j < (IndexType)n; ++j) {
						const DistanceType d = this->elems_distances(i, j);
						if ((i == 0) && (j == 1)) {
							dMin = d;
							IndexTypeSet o;
							o.insert(j);
							oMap[i] = o;
						}
						else if (d < dMin) {
							oMap.clear();
							dMin = d;
							IndexTypeSet o;
							o.insert(j);
							oMap[i] = o;
						}
						else if (d == dMin) {
							if (oMap.find(i) != oMap.end()) {
								(oMap[i]).insert(j);
							}
							else {
								bool bFound = false;
								auto iend = oMap.end();
								for (auto it = oMap.begin(); it != iend; ++it) {
									IndexTypeSet &xSet = (*it).second;
									if (xSet.find(j) != xSet.end()) {
										bFound = true;
										break;
									}
								}// it
								if (!bFound) {
									IndexTypeSet o;
									o.insert(j);
									oMap[i] = o;
								}
							}
						}
					}// j
				}// i
				if (oMap.empty()) {
					return (false);
				}
				IndexType indexMax = 0;
				for (size_t i = 0; i < n; ++i) {
					TreeElemTypePtr t = vv[i];
					TreeElemType *p = t.get();
					assert(p != nullptr);
					IndexType aIndex = p->index();
					if (i == 0) {
						indexMax = aIndex;
					}
					else if (aIndex > indexMax) {
						indexMax = aIndex;
					}
				}// i
				++nOrder;
				TreeElemTypePtrVector vres;
				IndexTypeSet zSet;
				auto xxEnd = oMap.end();
				for (auto it = oMap.begin(); it != xxEnd; ++it) {
					TreeElemTypePtrVector vx;
					size_t i = (*it).first;
					TreeElemTypePtr ti = vv[i];
					TreeElemType *pi = ti.get();
					assert(pi != nullptr);
					vx.push_back(ti);
					zSet.insert(pi->index());
					IndexTypeSet &xSet = (*it).second;
					auto yyEnd = xSet.end();
					for (auto jt = xSet.begin(); jt != yyEnd; ++jt) {
						size_t j = *jt;
						TreeElemTypePtr tj = vv[j];
						TreeElemType *pj = tj.get();
						assert(pj != nullptr);
						vx.push_back(tj);
						zSet.insert(pj->index());;
					}// jt
					TreeElemTypePtr t = std::make_shared<TreeElemType>(++indexMax, nOrder, dMin);
					TreeElemType *p = t.get();
					assert(p != nullptr);
					StringStreamType os;
					os << STRING_NODE << indexMax;
					StringType sId = os.str();
					p->id(sId);
					p->children(vx);
					p->update_center();
					p->sort();
					vres.push_back(t);
				}// it
				auto zzEnd = zSet.end();
				for (size_t i = 0; i < n; ++i) {
					TreeElemTypePtr t = vv[i];
					TreeElemType *p = t.get();
					assert(p != nullptr);
					IndexType aIndex = p->index();
					if (zSet.find(aIndex) == zzEnd) {
						vres.push_back(t);
					}
				}// i
				this->_elems = vres;
				return (this->compute_distances());
			}// aggreg_one_step
			DistanceType elems_distances(const size_t i1, const size_t i2) const {
				const TreeElemTypePtrVector &vv = this->_elems;
				const size_t n = vv.size();
				assert(i1 < n);
				assert(i2 < n);
				if (i1 == i2) {
					return (0);
				}
				const size_t pos = (size_t)(i1 * n + i2);
				assert(pos < this->_distances.size());
				return ((this->_distances)[pos]);
			}//elems_distance
			bool compute_distances(void) {
				TreeElemTypePtrVector &vv = this->_elems;
				const size_t n = vv.size();
				if (n < 1) {
					return (false);
				}
				DistanceFuncType *pFunc = this->_pfunc;
				const LinkMode link = this->_link;
				DistanceTypeVector &vec = this->_distances;
				vec.resize(n * n);
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &t1 = vv[i];
					const TreeElemType *p1 = t1.get();
					assert(p1 != nullptr);
					vec[i * n + i] = 0;
					for (size_t j = (size_t)(i + 1); j < n; ++j) {
						const TreeElemTypePtr &t2 = vv[j];
						const TreeElemType *p2 = t2.get();
						assert(p2 != nullptr);
						const DistanceType d = p1->distance(*p2, link, pFunc);
						vec[i * n + j] = d;
						vec[j * n + i] = d;
					}// j
				}//i
				return (true);
			}// compute_distances
	};// class Tree<T, U, Z,Compare, ALLOCT, ALLOCU,ALLOCZ,ALLOCDOUBLE,ALLOCSET,ALLOCPTR>
	  /////////////////////////////////////////////
}// namespace info
 //////////////////////////////////////
template <typename T, typename U, typename Z, class ALLOCT, class ALLOCDOUBLE>
info::OStreamType & operator<<(info::OStreamType &os, const info::TreeElem<T, U, Z, ALLOCT, ALLOCDOUBLE> &d) {
	return d.write_to(os);
}
////////////////////////////////////
template <typename T, typename U, typename Z, class Compare, class ALLOCT,class ALLOCU, class ALLOCZ, class ALLOCDOUBLE,class ALLOCSET, class ALLOCPTR>
info::OStreamType & operator<<(info::OStreamType &os, const info::Tree<T, U, Z, Compare, ALLOCT, ALLOCU, ALLOCZ, ALLOCDOUBLE, ALLOCSET, ALLOCPTR> &d) {
	return d.write_to(os);
}
/////////////////////////////////
#endif // !__TREEELEM_H__
/////////////////////
//eof: treeelem.h
