/*
 * indivtest.cpp
 *
 *  Created on: 11 avr. 2016
 *      Author: boubad
 */
#include <catch/catch.hpp>
///////////////////////////////
#include <indiv.h>
/////////////////////////////
#include "infotestdata.h"
///////////////////////////////
using namespace info;
///////////////////////////////
typedef int DataType;
typedef int IndexType;
typedef double DistanceType;
typedef Indiv<int,int> IndivType;
typedef std::vector<DataType> DataTypeVector;
typedef std::vector<StringType> StringTypeVector;
typedef DistanceFunc<double> DistanceFuncType;
typedef std::shared_ptr<DistanceFunc<double> > DistanceFuncTypePtr;
typedef std::vector<DistanceFuncTypePtr> DistanceFuncTypePtrVector;
typedef std::vector<double> DistanceTypeVector;
//////////////////////////////////
TEST_CASE("Indiv test", "[indiv]")
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
		std::vector<IndivType> oinds;
			for (size_t i = 0; i < nRows; ++i) {
				IndexType aIndex = (IndexType) (i + 1);
				const StringType sId = names[i];
				DataTypeVector v(nCols);
				for (size_t j = 0; j < nCols; ++j) {
					v[j] = gdata[i * nCols + j];
				} // j
				IndivType oInd(aIndex, sId, v);
				REQUIRE(oInd.is_valid());
				REQUIRE(aIndex == oInd.index());
				REQUIRE(sId == oInd.id());
				REQUIRE(nCols == oInd.size());
				const DataTypeVector &vdata = oInd.value();
				REQUIRE(nCols == vdata.size());
				for (size_t j = 0; j < nCols; ++j){
					REQUIRE(v[j]== vdata[j]);
				}// j
				oinds.push_back(oInd);
				IndivType ind1(oInd);
				IndivType ind2 = ind1;
				REQUIRE(ind2 == ind1);
				REQUIRE_FALSE(ind2 < ind1);
				double nx = oInd.norm();
				REQUIRE(nx > 0);
				EuclideDistanceFunc<double> func;
				const size_t n = oinds.size();
				for (size_t i = 0; i < n; ++i){
					const IndivType &ind1 = oinds[i];
					for (size_t j = 0; j < i; ++j){
						const IndivType &ind2 = oinds[j];
						double res = 0;
						ind1.distance(ind2,res,&func);
						REQUIRE(res > 0);
					}// j
				}// i
			} // i
}// Indiv test
///////////////////////////////






