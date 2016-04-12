#include <catch/catch.hpp>
///////////////////////////////
#include <distance.h>
#include <distancemap.h>
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
TEST_CASE("DistanceMap tests", "[distancemap]")
{
	size_t nCols = 0, nRows = 0;
	DataTypeVector gdata;
	StringTypeVector names;
	StringTypeVector colNames;
	InfoTestData::get_conso(nRows, nCols, gdata, &names,&colNames);
	REQUIRE(nRows > 1);
	REQUIRE(nCols > 1);
	REQUIRE(gdata.size() == (size_t)(nRows * nCols));
	REQUIRE(names.size() == nRows);
	//
	DistanceFunc<double> fBase;
	const double zero = 0;
	std::vector<double> dists(nRows * nRows, zero);
	//
	SECTION("Default constructor") {
		DistanceMap<size_t, double> oMap;
		DistanceMap<size_t, double> xMap(oMap);
		DistanceMap<size_t, double> yMap = oMap;
		oMap.clear();
		for (size_t i = 0; i < nRows; ++i) {
			for (size_t j = 0; j < i; ++j) {
				double res = oMap(i, j);
				REQUIRE(res == 0);
			}// j
		}// i
	}//Default constructor
	SECTION("Normal operation") {
		DistanceMap<size_t, double> oMap;
		for (size_t i = 0; i < nRows; ++i) {
			DataTypeVector vi(nCols);
			for (size_t k = 0; k < nCols; ++k) {
				vi[k] = gdata[i * nCols + k];
			} // k
			for (size_t j = 0; j < i; ++j) {
				DataTypeVector vj(nCols);
				for (size_t k = 0; k < nCols; ++k) {
					vj[k] = gdata[j * nCols + k];
				} // k
				double res = 0;
				info_distance(vi, vj, res, &fBase, i, j);
				REQUIRE(res > 0);
				dists[i*nRows + j] = res;
				dists[j*nRows + i] = res;
				oMap.add(i, j, res);
			}// j
		}// i
		for (size_t i = 0; i < nRows; ++i) {
			for (size_t j = 0; j < i; ++j) {
				double expected = dists[i*nRows + j];
				double actual = oMap(i, j);
				REQUIRE(actual == Approx(expected));
			}// j
		}// i
	}//Normal operation
	//
}//DistanceFuncTest
