#pragma once
#ifndef __WRITE_OUT_H__
#define __WRITE_OUT_H__
//////////////////////////////////
#include "gendefs.h"
//////////////////////////////////////
namespace info {
	/////////////////////////////////

	template <class T>
	OStreamType & info_write_json_value(OStreamType &os, const StringType &name, const T &data) {
		os << STRING_QUOTE << name << STRING_QUOTE_END << data;
		return (os);
	}// info_write_json_value
	template <class T, class Z>
	OStreamType & info_write_json_pair(OStreamType &os, const std::pair<T, Z> &oPair) {
		os << START_OBJECT;
		info_write_json_value(os, STRING_FIRST, oPair.first);
		os << STRING_COMMA;
		info_write_json_value(os, STRING_FIRST, oPair.second);
		os << END_OBJECT;
		return (os);
	}//info_write_pair
	template <class ALLOCS>
	OStreamType & info_write_json_array(OStreamType &os, const std::vector<StringType, ALLOCS> &data) {
		os << START_ARRAY;
		auto iStart = data.begin();
		for (auto it = iStart; it != data.end(); ++it) {
			if (it != iStart) {
				os << STRING_COMMA;
			}
			os << STRING_QUOTE << *it << STRING_QUOTE;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_json_array
	template <class CONTAINER>
	OStreamType & info_write_json_array(OStreamType &os, const CONTAINER &data) {
		os << START_ARRAY;
		auto iStart = data.begin();
		for (auto it = iStart; it != data.end(); ++it) {
			if (it != iStart) {
				os << STRING_COMMA;
			}
			os << *it;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_json_array
	 //////////////////////////////////////
	template <class T, class Z>
	OStreamType & info_write_pair(OStreamType &os, const std::pair<T, Z> &oPair) {
		os << LEFT_PAR << oPair.first << STRING_COMMA << oPair.second << RIGHT_PAR;
		return (os);
	}//info_write_pair
	template <class T>
	OStreamType & info_write_array(OStreamType &os, const std::valarray<T> &oAr) {
		os << START_ARRAY;
		const size_t n = oAr.size();
		for (size_t i = 0; i < n; ++i) {
			if (i != 0) {
				os << STRING_COMMA;
			}
			os << oAr[i];
		}
		os << END_ARRAY;
		return (os);
	}// info_write_array
	template <class T, class Alloc>
	OStreamType & info_write_array(OStreamType &os, const std::list<T, Alloc> &oAr) {
		os << START_ARRAY;
		for (auto it = oAr.begin(); it != oAr.end(); ++it) {
			if (it != oAr.begin()) {
				os << STRING_COMMA;
			}
			os << *it;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_array
	template <class T, class Alloc>
	OStreamType & info_write_array(OStreamType &os, const std::deque<T, Alloc> &oAr) {
		os << START_ARRAY;
		for (auto it = oAr.begin(); it != oAr.end(); ++it) {
			if (it != oAr.begin()) {
				os << STRING_COMMA;
			}
			os << *it;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_array
	template <class T, class Alloc>
	OStreamType & info_write_array(OStreamType &os, const std::vector<T, Alloc> &oAr) {
		os << START_ARRAY;
		for (auto it = oAr.begin(); it != oAr.end(); ++it) {
			if (it != oAr.begin()) {
				os << STRING_COMMA;
			}
			os << *it;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_array
	template <class T, class Compare, class Alloc>
	OStreamType & info_write_array(OStreamType &os, const std::set<T, Compare, Alloc> &oAr) {
		os << START_ARRAY;
		for (auto it = oAr.begin(); it != oAr.end(); ++it) {
			if (it != oAr.begin()) {
				os << STRING_COMMA;
			}
			os << *it;
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_array
	template <class Key, class T, class Compare, class Alloc>
	OStreamType & info_write_array(OStreamType &os, const std::map<Key, T, Compare, Alloc> &oAr) {
		os << START_ARRAY;
		for (auto it = oAr.begin(); it != oAr.end(); ++it) {
			if (it != oAr.begin()) {
				os << STRING_COMMA;
			}
			info_write_pair(os, *it);
		}// it
		os << END_ARRAY;
		return (os);
	}// info_write_array
	/////////////////////////////////////
	template <class T, class ALLOCT>
		OStreamType & info_write_json_value(OStreamType &os, const StringType &name, const std::vector<T, ALLOCT> &data) {
			os << STRING_QUOTE << name << STRING_QUOTE_END;
			info_write_json_array(os, data);
			return (os);
		}// info_write_json_value
	///////////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__WRITE_OUT_H__
