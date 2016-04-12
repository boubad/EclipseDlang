#pragma once
#ifndef __DATASET_H__
#define __DATASET_H__
////////////////////////////////
#include "indiv.h"
#include "indivprovider.h"
#include "matdata.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////////////
	template <typename T = int, typename U = size_t,
		class ALLOCT = std::allocator<T>,
		class ALLOCS = std::allocator<StringType>,
		class ALLOCPTR = std::allocator<std::shared_ptr<Indiv<T,U,ALLOCT> > >
	>
		class Dataset : public IIndivProvider<T, U, ALLOCT> {
		public:
			typedef T DataType;
			typedef U IndexType;
			//
			typedef MatData<T, ALLOCT, ALLOCS> MatDataType;
			typedef IIndivProvider<DataType, IndexType, ALLOCT> IIndivProviderType;
			typedef typename IIndivProviderType::IndivType IndivType;
			typedef typename IIndivProviderType::IndivTypePtr IndivTypePtr;
			typedef std::vector<IndivTypePtr, ALLOCPTR> IndivTypePtrVector;
			typedef typename MatDataType::DataTypeVector DataTypeVector;
			//
			typedef Dataset<DataType, IndexType, ALLOCT, ALLOCS,ALLOCPTR> DatasetType;
			
		private:
			size_t _curindex;
			const MatDataType *_pdata;
			IndivTypePtrVector _vec;
		public:
			Dataset(const MatDataType *pMatData):_curindex(0),
				_pdata(pMatData){
				assert(this->_pdata != nullptr);
				assert(this->_pdata->is_valid());
				const size_t n = this->_pdata->rows();
				assert(n > 0);
				this->_vec.resize(n);
			}// Dataset
			Dataset(const DatasetType &other) :_curnindex(other._curindex),_pdata(other._pdata),
			_vec(other._vec){}// Dataset
			DatasetType & operator=(const DatasetType &other) {
				if (this != &other) {
					this->_curindex = other._curindex;
					this->_pdata = other._pdata;
					this->_vec = other._vec;
				}
				return (*this);
			}// operator=
			virtual ~Dataset() {}
		public:
			virtual bool is_valid(void) const {
				return ((this->_pdata != nullptr) && this->_pdata->is_valid());
			}
			virtual void reset(void) {
				this->_curindex = 0;
			}// reset
			virtual bool next(IndivTypePtr &oInd) {
				if (!this->is_valid()) {
					oInd.reset();
					return (false);
				}
				const MatDataType *pData = this->_pdata;
				assert(pData != nullptr);
				const size_t nr = pData->rows();
				assert(nr > 0);
				const size_t irow = this->_curindex;
				if (irow >= nr) {
					oInd.reset();
					return (false);
				}
				assert(irow < this->_vec.size());
				IndivTypePtr o = this->_vec[irow];
				if (o.get() == nullptr) {
					DataTypeVector data;
					StringType sId;
					if (!pData->row_at(irow, sId, data)) {
						oInd.reset();
						return (false);
					}
					IndexType aIndex = (IndexType)(irow + 1);
					o = std::make_shared<IndivType>(aIndex, sId, data);
					assert(o.get() != nullptr);
					this->_vec[irow] = o;
				}
				oInd = o;
				this->_curindex++;
				return (true);
			}// next
	};// class Dataset<T,U>
}// namespace info
///////////////////////////////////
#endif // !__Dataset_H__
