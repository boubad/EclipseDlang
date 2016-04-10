#pragma once
#ifndef __INFOBASE_H__
#define __INFOBASE_H__
/////////////////////////////
#include "utils.h"
//////////////////////////////////
namespace info {
	/////////////////////////////////
	class BaseInfo {
	protected:
		BaseInfo() {}
		BaseInfo(const BaseInfo & /*other*/) {}
		BaseInfo & operator=(const BaseInfo & /* other*/) {
			return (*this);
		}
	public:
		virtual ~BaseInfo() {}
		virtual bool is_valid(void) const {
			return (true);
		}
		virtual OStreamType & write_to(OStreamType &os)const {
			return (os);
		}
		void toString(StringType &s) const {
			StringStreamType os;
			this->write_to(os);
			s = os.str();
		}// toString
	};// class BaseInfo
	/////////////////////////////////////
}// namespace info
///////////////////////////////////////
inline info::OStreamType & operator<<(info::OStreamType &os, const info::BaseInfo &ind) {
	return (ind.write_to(os));
}
///////////////////////////////
#endif // !__INFOBASE_H__
