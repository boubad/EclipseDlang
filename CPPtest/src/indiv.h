#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////
#include "infobase.h"
#include "distance.h"
/////////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_INDEX ("index")
#define STRING_ID ("id")
#define STRING_DATA ("data")
#define STRING_CURRENTINDEX ("currentindex")
#define STRING_KEYS ("keys")
#define STRING_ALL_INDIVS ("indivs")
#else
#define STRING_INDEX (L"index")
#define STRING_ID (L"id")
#define STRING_DATA (L"data")
#define STRING_CURRENTINDEX (L"currentindex")
#define STRING_KEYS (L"keys")
#define STRING_ALL_INDIVS (L"indivs")
#endif // INFO_STRING_TYPE
////////////////////////////////
namespace info {
		enum class LinkMode { noLink, linkMin, linkMax, linkMean, linkCenter };
	//////////////////////////////////////////
	template <typename T = int, typename U = int, class ALLOCT = std::allocator<T>
	> class Indiv : public BaseInfo {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
	public:
		typedef T DataType;
		typedef U IndexType;
		//
		typedef std::vector<DataType, ALLOCT> DataTypeVector;
		typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
	private:
		IndexType _index;
		StringType _id;
		DataTypeVector _data;
	public:
		Indiv() :_index(0) {}
		template <class CONTAINER>
		Indiv(const IndexType aIndex,
			const StringType &sid,
			const CONTAINER &v) :
			_index(aIndex), _id(sid), _data(v.size()) {
			const size_t n = v.size();
			assert(this->_data.size() == n);
			for (size_t i = 0; i < n; ++i) {
				this->_data[i] = (DataType)v[i];
			}// i
		}
		Indiv(const IndivType &other) :BaseInfo(other), _index(other._index), _id(other._id), _data(other._data) {}
		IndivType & operator=(const IndivType &other) {
			if (this != &other) {
				BaseInfo::operator=(other);
				this->_index = other._index;
				this->_id = other._id;
				this->_data = other._data;
			}
			return (*this);
		}
		virtual ~Indiv() {}
	public:
		virtual int compare_to(const IndivType &other) const {
			if (this->index() < other.index()) {
				return (-1);
			}
			else if (this->index() > other.index()) {
				return (1);
			}
			else {
				return (0);
			}
		}// compare_to
		bool operator==(const IndivType &other) const {
			return (this->compare_to(other) == 0);
		}
		bool operator<(const IndivType &other) const {
			return (this->compare_to(other) < 0);
		}
	public:
		virtual bool is_valid(void) const {
			return ((this->index() >= 0) && (this->size() > 0));
		}
		virtual const DataTypeVector & value(void) const {
			return (this->_data);
		}
		template <class CONTAINER>
		void value(const CONTAINER &v) {
			const size_t n = v.size();
			this->_data.resize(n);
			for (size_t i = 0; i < n; ++i) {
				this->_data[i] = (DataType)v[i];
			}// i
		}
		virtual IndexType index(void) const {
			return (this->_index);
		}
		virtual void index(const IndexType aIndex) {
			this->_index = aIndex;
		}
		virtual const StringType & id(void) const {
			return (this->_id);
		}
		virtual void id(const StringType &s) {
			this->_id = s;
		}
		//
		size_t size(void) const {
			return (this->value().size());
		}
		double norm(void) const {
			const DataTypeVector &t = this->value();
			double res = 0;
			const size_t n = t.size();
			for (size_t i = 0; i < n; ++i) {
				double tx = t[i];
				res += tx * tx;
			}
			if (n > 0) {
				res /= n;
			}
			return (std::sqrt(res));
		}// trace
		template <typename Z, typename TX, typename UX, class ALLOCX>
		void distance(const Indiv<TX, UX, ALLOCX> &other,
			Z &result,
			const DistanceFunc<Z> *pFunc = nullptr,
			const size_t irow1 = 0,
			const size_t irow2 = 0) const {
			info_distance(this->value(), other.value(), result, pFunc, irow1, irow2);
		}// distance
		template <typename Z, class CONTAINER>
		void distance(const CONTAINER &vv1, Z &result,
			const DistanceFunc<Z> *pFunc = nullptr,
			const size_t irow1 = 0,
			const size_t irow2 = 0) const {
			info_distance(this->value(), vv1, result, pFunc, irow1, irow2);
		}// distance
	public:
		OStreamType & operator<<(OStreamType &os) const {
			return (this->write_to(os));
		}
		virtual OStreamType & write_to(OStreamType &os) const {
			os << START_OBJECT;
			info_write_json_value(os, STRING_INDEX, this->index()) << STRING_COMMA;
			info_write_json_value(os, STRING_ID, this->id()) << STRING_COMMA;
			info_write_json_value(os, STRING_DATA, this->value());
			os << END_OBJECT;
			return (os);
		}// write_to
	}; // class Indiv<T,U,ALLOCT>
	//////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__INDIV_H__
///////////////////////
//eof: indiv.h
