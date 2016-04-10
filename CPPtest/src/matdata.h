#pragma once
#ifndef __MATDATA_H__
#define __MATDATA_H__
/////////////////////////////////
#include "infobase.h"
/////////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_ROWS ("rows")
#define STRING_COLS ("cols")
#define STRING_DATA_ARRAY ("data_array")
#define STRING_NAMES ("names")
#else
#define STRING_ROWS (L"rows")
#define STRING_COLS (L"cols")
#define STRING_DATA_ARRAY (L"data_array")
#define STRING_NAMES (L"names")
#endif // INFO_STRING_TYPE
///////////////////////////////////
namespace info {
	//////////////////////////////////////////
	template <typename T = int, 
		class ALLOCT = std::allocator<T>,
		class ALLOCS = std::allocator<StringType> >
		class MatData : public BaseInfo {
		public:
			typedef T DataType;
			//
			typedef std::vector<DataType, ALLOCT> DataTypeVector;
			typedef std::vector<StringType, ALLOCS> StringTypeVector;
			typedef MatData<DataType, ALLOCT, ALLOCS> MatDataType;
		private:
			size_t _rows;
			size_t _cols;
			DataTypeVector _data;
			StringTypeVector _rowids;
		public:
			MatData() :_rows(0), _cols(0) {}
			template <class CONTAINER1, class CONTAINER2>
			MatData(const size_t r, const size_t c, const CONTAINER1 &oData,
				const CONTAINER2 *pRowNames = nullptr) :
				_rows(r), _cols(c){
				assert(this->_rows > 0);
				assert(this->_cols > 0);
				const size_t n = (size_t)(r * c);
				this->_data.resize(n);
				this->_rowids.resize(r);
				assert(this->_data.size() >= n);
				assert(this->_rowids.size() >= this->_rows);
				for (size_t i = 0; i < n; ++i) {
					this->_data[i] = (DataType)oData[i];
				}
				if (pRowNames != nullptr) {
					assert(pRowNames->size() >= r);
					for (size_t i = 0; i < r; ++i) {
						this->_rowids[i] = (*pRowNames)[i];
					}// i
				}
			}// MatData
			MatData(const MatDataType &other) :BaseInfo(other),_rows(other._rows), _cols(other._cols), _data(other._data),
				_rowids(other._rowids) {}// MatData
			MatDataType & operator=(const MatDataType &other) {
				if (this != &other) {
					BaseInfo::operator=(other);
					this->_rows = other._rows;
					this->_cols = other._cols;
					this->_data = other._data;
					this->_rowids = other._rowids;
				}
				return (*this);
			}// operator=
			virtual ~MatData() {}
		public:
			virtual bool is_valid(void) const {
				return ((this->_rows > 0) && (this->_cols > 0) &&
					(this->_data.size() >= (size_t)(this->_rows * this->_cols)) &&
					(this->_rowids.size() >= this->_rows));
			}// is_valid
			size_t rows(void) const {
				return (this->_rows);
			}
			size_t cols(void) const {
				return (this->_cols);
			}
			const DataTypeVector & data(void) const {
				return (this->_data);
			}
			const StringTypeVector & names(void) const {
				return (this->_rowids);
			}
			template <class CONTAINER>
			bool row_at(const size_t irow, StringType &sId, CONTAINER &data) const {
				if (!this->is_valid()) {
					return (false);
				}
				if (irow >= this->rows()) {
					return (false);
				}
				const size_t n = this->cols();
				data.resize(n);
				const StringTypeVector &vs = this->names();
				sId = vs[irow];
				const size_t offset = (size_t)(irow * n);
				const DataTypeVector &vv = this->data();
				for (size_t i = 0; i < n; ++i) {
					data[i] = vv[offset + i];
				}// i
				return (true);
			}// row_at
			virtual OStreamType & write_to(OStreamType &os) const {
				//
				os << std::endl << START_OBJECT;
				info_write_json_value(os, STRING_ROWS, this->rows()) << STRING_COMMA << std::endl;
				info_write_json_value(os, STRING_COLS, this->cols()) << STRING_COMMA << std::endl;
				info_write_json_value(os, STRING_NAMES, this->names()) << STRING_COMMA << std::endl;
				info_write_json_value(os, STRING_DATA_ARRAY, this->data()) << std::endl;
				os << END_OBJECT;
				//
				return (os);
			}// write_to
	};// class MatData<T,U>
}// namespace info
 ///////////////////////////////////
template <typename T,class ALLOCT, class ALLOCS>
info::OStreamType & operator<<(info::OStreamType &os, const info::MatData<T,ALLOCT, ALLOCS> &ind) {
	return(ind.write_to(os));
}
/////////////////////////////////////
#endif // !__MATDATA_H__
