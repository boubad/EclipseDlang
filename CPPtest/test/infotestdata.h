#pragma once
///////////////////////////////
#ifndef __INFOTESTDATA_H__
#define __INFOTESTDATA_H__
/////////////////////////////////
#include <gendefs.h>
//////////////////////////////
namespace info {
///////////////////////////
class InfoTestData {
public:
	InfoTestData();
	~InfoTestData();
public:
	template<typename T, class ALLOCS>
	static void get_conso(size_t &nRows, size_t &nCols, std::valarray<T> &data,
			std::vector<StringType, ALLOCS> *pIndsNames = nullptr,
			std::vector<StringType, ALLOCS> *pVarsNames = nullptr) {
		const size_t nc = InfoTestData::st_conso_cols;
		assert(nc > 2);
		const size_t nr = InfoTestData::st_conso_rows;
		assert(nr > 3);
		const int *pData = &(InfoTestData::st_conso_data[0]);
		assert(pData != nullptr);
		data.resize(nr * nc);
		nRows = nr;
		nCols = nc;
		for (size_t i = 0; i < nr; ++i) {
			for (size_t j = 0; j < nc; ++j) {
				const size_t pos = (size_t) (i * nc + j);
				data[pos] = (T) pData[pos];
			} // j
		} // i
		if (pIndsNames != nullptr) {
			pIndsNames->resize(nr);
			for (size_t i = 0; i < nr; ++i) {
				(*pIndsNames)[i] = st_conso_inds[i];
			} // i
		}
		if (pVarsNames != nullptr) {
			pVarsNames->resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				(*pVarsNames)[i] = st_conso_vars[i];
			} // i
		}
	} //get_conso
	template<typename T, class ALLOCT, class ALLOCS>
	static void get_conso(size_t &nRows, size_t &nCols,
			std::vector<T, ALLOCT> &data,
			std::vector<StringType, ALLOCS> *pIndsNames = nullptr,
			std::vector<StringType, ALLOCS> *pVarsNames = nullptr) {
		const size_t nc = InfoTestData::st_conso_cols;
		assert(nc > 2);
		const size_t nr = InfoTestData::st_conso_rows;
		assert(nr > 3);
		const int *pData = &(InfoTestData::st_conso_data[0]);
		assert(pData != nullptr);
		data.resize(nr * nc);
		nRows = nr;
		nCols = nc;
		for (size_t i = 0; i < nr; ++i) {
			for (size_t j = 0; j < nc; ++j) {
				const size_t pos = (size_t) (i * nc + j);
				data[pos] = (T) pData[pos];
			} // j
		} // i
		if (pIndsNames != nullptr) {
			pIndsNames->resize(nr);
			for (size_t i = 0; i < nr; ++i) {
				(*pIndsNames)[i] = st_conso_inds[i];
			} // i
		}
		if (pVarsNames != nullptr) {
			pVarsNames->resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				(*pVarsNames)[i] = st_conso_vars[i];
			} // i
		}
	} //get_conso
	template<typename T, class ALLOCS>
	static void get_mortal(size_t &nRows, size_t &nCols, std::valarray<T> &data,
			std::vector<StringType, ALLOCS> *pIndsNames = nullptr,
			std::vector<StringType, ALLOCS> *pVarsNames = nullptr) {
		const size_t nc = InfoTestData::st_socmortal_cols;
		assert(nc > 2);
		const size_t nr = InfoTestData::st_socmortal_rows;
		assert(nr > 3);
		const int *pData = &(InfoTestData::st_socmortal_data[0]);
		assert(pData != nullptr);
		data.resize(nr * nc);
		nRows = nr;
		nCols = nc;
		for (size_t i = 0; i < nr; ++i) {
			for (size_t j = 0; j < nc; ++j) {
				const size_t pos = (size_t) (i * nc + j);
				data[pos] = (T) pData[pos];
			} // j
		} // i
		if (pIndsNames != nullptr) {
			pIndsNames->resize(nr);
			for (size_t i = 0; i < nr; ++i) {
				(*pIndsNames)[i] = st_socmortal_inds[i];
			} // i
		}
		if (pVarsNames != nullptr) {
			pVarsNames->resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				(*pVarsNames)[i] = st_socmortal_vars[i];
			} // i
		}
	} //get_mortal
	template<typename T, class ALLOCT, class ALLOCS>
	static void get_mortal(size_t &nRows, size_t &nCols,
			std::vector<T, ALLOCT> &data,
			std::vector<StringType, ALLOCS> *pIndsNames = nullptr,
			std::vector<StringType, ALLOCS> *pVarsNames = nullptr) {
		const size_t nc = InfoTestData::st_socmortal_cols;
		assert(nc > 2);
		const size_t nr = InfoTestData::st_socmortal_rows;
		assert(nr > 3);
		const int *pData = &(InfoTestData::st_socmortal_data[0]);
		assert(pData != nullptr);
		data.resize(nr * nc);
		nRows = nr;
		nCols = nc;
		for (size_t i = 0; i < nr; ++i) {
			for (size_t j = 0; j < nc; ++j) {
				const size_t pos = (size_t) (i * nc + j);
				data[pos] = (T) pData[pos];
			} // j
		} // i
		if (pIndsNames != nullptr) {
			pIndsNames->resize(nr);
			for (size_t i = 0; i < nr; ++i) {
				(*pIndsNames)[i] = st_socmortal_inds[i];
			} // i
		}
		if (pVarsNames != nullptr) {
			pVarsNames->resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				(*pVarsNames)[i] = st_socmortal_vars[i];
			} // i
		}
	} //get_mortal
private:
	static size_t st_socmortal_cols;
	static size_t st_socmortal_rows;
	static int st_socmortal_data[];

	static size_t st_conso_cols;
	static size_t st_conso_rows;
	static int st_conso_data[];
#if defined(INFO_STRING_TYPE)
	static const char *st_socmortal_vars[];
	static const char *st_socmortal_inds[];
	static const char *st_conso_vars[];
	static const char *st_conso_inds[];
#else
	static const wchar_t *st_socmortal_vars[];
	static const wchar_t *st_socmortal_inds[];
	static const wchar_t *st_conso_vars[];
	static const wchar_t *st_conso_inds[];
#endif
};
}// namespace info
///////////////////////////////
#endif //__INFOTESTDATA_H__

