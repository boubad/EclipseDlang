/*
 * distancetest.cpp
 *
 *  Created on: 11 avr. 2016
 *      Author: boubad
 */
#include <catch/catch.hpp>
///////////////////////////////
#include <distance.h>
/////////////////////////////
#include "infotestdata.h"
///////////////////////////////
using namespace info;
///////////////////////////////
typedef int DataType;
typedef double DistanceType;
typedef std::vector<DataType> DataTypeVector;
typedef std::vector<StringType> StringTypeVector;
typedef DistanceFunc<double> DistanceFuncType;
typedef std::shared_ptr<DistanceFunc<double> > DistanceFuncTypePtr;
typedef std::vector<DistanceFuncTypePtr> DistanceFuncTypePtrVector;
typedef std::vector<double> DistanceTypeVector;
///////////////////////////////
TEST_CASE("DistanceFuncTest","[distance]")
{
	size_t nCols=0, nRows = 0;
	DataTypeVector gdata;
	StringTypeVector names;
	DistanceFuncTypePtrVector distancefuncs;
	InfoTestData::get_conso(nRows, nCols, gdata, &names);
	REQUIRE(nRows > 1);
	REQUIRE(nCols > 1);
	REQUIRE(gdata.size() == (size_t )(nRows * nCols));
	REQUIRE(names.size()== nRows);
	//
	std::vector<double> weights(nCols);
	for (size_t i = 0; i < nCols; ++i){
		weights[i] = i;
	}// i
	//
	DistanceFunc<double> fBase;
	ManhattanDistanceFunc<double> f1;
	EuclideDistanceFunc<double> f2;
	MaxDistanceFunc<double> f3;
	DivergenceDistanceFunc<double> f4(nRows,nCols,gdata);
	Khi2DistanceFunc<double> f5(nRows,nCols,gdata);
	WeightedDistanceFunc<double> f6(weights);
	//
	std::vector<DistanceFunc<double> *> fdists;
	fdists.push_back(&fBase);
	fdists.push_back(&f1);
	fdists.push_back(&f2);
	fdists.push_back(&f3);
	fdists.push_back(&f4);
	fdists.push_back(&f5);
	fdists.push_back(&f6);
	//
	for (size_t i = 0; i < nRows; ++i) {
			const StringType sId = names[i];
			DataTypeVector vi(nCols);
			for (size_t k = 0; k < nCols; ++k) {
				vi[k] = gdata[i * nCols + k];
			} // k
			for (size_t j = 0; j < i; ++j){
				DataTypeVector vj(nCols);
				for (size_t k = 0; k < nCols; ++k) {
					vj[k] = gdata[j * nCols + k];
				} // k
				StringStreamType os;
				os << names[i] << STRING_COMMA << names[j];
				for (auto it = fdists.begin(); it != fdists.end(); ++ it) {
					double res = 0;
					DistanceFuncType *pf = (*it);
					REQUIRE(pf != nullptr);
					info_distance(vi, vj,res,pf,i,j);
					REQUIRE(res > 0);
					os << STRING_COMMA << res;
				}// it
				StringType ss = os.str();
				INFO(ss);
			}// j
		}// i
}//DistanceFuncTest

