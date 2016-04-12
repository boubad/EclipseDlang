#pragma once
#ifndef __INDIVPROVIDER_H__
#define __INDIVPROVIDER_H__
///////////////////////////////////
#include "indiv.h"
/////////////////////////////////
namespace info {
	//////////////////////////////
	template <typename T = int, typename U = int, class ALLOCT = std::allocator<T> > class IIndivProvider {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
	public:
		typedef T DataType;
		typedef U IndexType;
		//
		typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef IIndivProvider<DataType, IndexType, ALLOCT> IIndivProviderType;
	public:
		IIndivProvider() {}
		IIndivProvider(const IIndivProviderType & /*other*/) {}
		IIndivProviderType & operator=(const IIndivProviderType & /*other*/) { return (*this); }
		virtual ~IIndivProvider() {}
	public:
		virtual void reset(void) = 0;
		virtual bool next(IndivTypePtr &oInd) = 0;
	};// class IndivProvider<T,U,ALLOCT>
	////////////////////////////////
}// namespace info
//////////////////////////////
#endif // !__INDIVPROVIDER_H__
