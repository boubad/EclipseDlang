#include <catch/catch.hpp>
///////////////////////////////
#include <matdata.h>
/////////////////////////////
#include "infotestdata.h"
///////////////////////////////
using namespace info;
///////////////////////////////
typedef int DataType;
typedef std::vector<DataType> DataTypeVector;
typedef std::vector<StringType> StringTypeVector;
typedef MatData<DataType> MatDataType;
typedef DistanceMap<size_t, double> DistanceMapType;
///////////////////////////////
TEST_CASE("Matadata Tests", "[matdata]") {
	size_t nCols = 0;
	size_t nRows = 0;
	DataTypeVector gdata;
	StringTypeVector names;
	StringTypeVector colNames;
	//
	InfoTestData::get_mortal(nRows, nCols, gdata, &names, &colNames);
	REQUIRE(nRows > 1);
	REQUIRE(nCols > 1);
	REQUIRE(gdata.size() >= (size_t )(nCols * nRows));
	REQUIRE(names.size() >= nRows);
	REQUIRE(colNames.size() >= nCols);
	//
	SECTION("Default constructor"){
	MatDataType oMat;
	REQUIRE_FALSE(oMat.is_valid());
	REQUIRE((size_t)0 == oMat.cols());
	REQUIRE((size_t)0 == oMat.rows());
	REQUIRE(oMat.data().empty());
	REQUIRE(oMat.names().empty());
	REQUIRE(oMat.colNames().empty());
	size_t irow = nRows + 10;
	size_t icol = nCols + 89;
	StringType sId;
	DataTypeVector data;
	REQUIRE_FALSE(oMat.row_at(irow, sId, data));
	REQUIRE_FALSE(oMat.col_at(icol, sId, data));
	auto fn1 = [oMat, irow, icol] {return oMat.value_at(irow, icol);};
	REQUIRE_THROWS(fn1());
	auto fn2 = [oMat, irow, icol] {return oMat(irow, icol);};
	REQUIRE_THROWS(fn2());
	auto fn3 = [oMat, irow] {return oMat.row_name_at(irow);};
	REQUIRE_THROWS(fn3());
	auto fn4 = [&oMat, irow, sId] {oMat.row_name_at(irow, sId);};
	REQUIRE_THROWS(fn4());
	auto fn5 = [oMat, icol] {return oMat.col_name_at(icol);};
	REQUIRE_THROWS(fn5());
	auto fn6 = [&oMat, icol, sId] {oMat.row_name_at(icol, sId);};
	REQUIRE_THROWS(fn6());
	StringType ss;
	oMat.toString(ss);
	INFO(ss);
} // section default constructor
	SECTION("Normal constructor"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	//
	REQUIRE(oMat.is_valid());
	REQUIRE(nCols == oMat.cols());
	REQUIRE(nRows == oMat.rows());
	REQUIRE(oMat.data().size() == (size_t)(nCols * nRows));
	REQUIRE(oMat.names().size() == nRows);
	REQUIRE(oMat.colNames().size() == nCols);
	for (size_t icol = 0; icol < nCols; ++icol) {
		StringType sId;
		DataTypeVector data;
		bool b = oMat.col_at(icol, sId, data);
		REQUIRE(b);
		REQUIRE(sId == colNames[icol]);
		REQUIRE(nRows == data.size());
		StringType sx = oMat.col_name_at(icol);
		REQUIRE(sx == sId);
		for (size_t i = 0; i < nRows; ++i) {
			REQUIRE(gdata[i * nCols + icol] == data[i]);
		} // i
	} // icol
	StringType ss;
	oMat.toString(ss);
	INFO(ss);
	oMat.toString(ss);
	INFO(ss);
	for (size_t irow = 0; irow < nRows; ++irow) {
		StringType sId;
		DataTypeVector data;
		bool b = oMat.row_at(irow, sId, data);
		REQUIRE(b);
		REQUIRE(sId == names[irow]);
		REQUIRE(nCols == data.size());
		StringType sx = oMat.row_name_at(irow);
		REQUIRE(sx == sId);
		for (size_t i = 0; i < nCols; ++i) {
			DataType v = gdata[irow * nCols + i];
			REQUIRE(v == data[i]);
			REQUIRE(v == oMat(irow, i));
			oMat(irow, i) = (irow + 1)*(i + 1);
		} // i
	} // irow
	for (size_t i = 0; i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			DataType expected = (DataType)((i + 1) * (j + 1));
			DataType v = oMat(i, j);
			REQUIRE(expected == v);
		} // j
	} // i
	oMat.toString(ss);
	INFO(ss);
} // section normal constructor
	SECTION("Matdata to Ranks"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	MatData<size_t> oRes;
	bool b = oMat.to_ranks(oRes);
	REQUIRE(b);
	//
	REQUIRE(oRes.is_valid());
	REQUIRE(nCols == oRes.cols());
	REQUIRE(nRows == oRes.rows());
	REQUIRE(oRes.data().size() == (size_t)(nCols * nRows));
	REQUIRE(oRes.names().size() == nRows);
	REQUIRE(oRes.colNames().size() == nCols);
	for (size_t i = 0; i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			size_t v = oRes(i, j);
			REQUIRE(v > 0);
			REQUIRE(v <= nRows);
		} // j
	} // i
	StringType ss;
	oRes.toString(ss);
	INFO(ss);
} //section Matdata to rank
	SECTION("Compute Weights"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	std::vector<double> weights;
	bool b = oMat.compute_weights(weights);
	REQUIRE(b);
	REQUIRE(weights.size() == nCols);
	double somme = 0.0;
	for (size_t i = 0; i < nCols; ++i) {
		double x = weights[i];
		REQUIRE(x > 0.0);
		REQUIRE(x < 1.0);
		somme += x;
	} // i
	REQUIRE(somme == Approx(1.0));
} //section Compute Weights
	SECTION("Matdata to Normalized"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	MatData<double> oRes;
	bool b = oMat.to_normalized(oRes);
	REQUIRE(b);
	//
	REQUIRE(oRes.is_valid());
	REQUIRE(nCols == oRes.cols());
	REQUIRE(nRows == oRes.rows());
	REQUIRE(oRes.data().size() == (size_t)(nCols * nRows));
	REQUIRE(oRes.names().size() == nRows);
	REQUIRE(oRes.colNames().size() == nCols);
	for (size_t i = 0; i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			double v = oRes(i, j);
			REQUIRE(v > -6.0);
			REQUIRE(v < 6.0);
		} // j
	} // i
	StringType ss;
	oRes.toString(ss);
	INFO(ss);
} //section Matdata to Normalized
	SECTION("Matdata to Recoded"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	int xmin = 100;
	int xmax = 2000;
	MatData<int> oRes;
	bool b = oMat.to_recoded(oRes, xmax, xmin);
	REQUIRE(b);
	//
	REQUIRE(oRes.is_valid());
	REQUIRE(nCols == oRes.cols());
	REQUIRE(nRows == oRes.rows());
	REQUIRE(oRes.data().size() == (size_t)(nCols * nRows));
	REQUIRE(oRes.names().size() == nRows);
	REQUIRE(oRes.colNames().size() == nCols);
	for (size_t i = 0; i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			int v = oRes(i, j);
			REQUIRE(v >= xmin);
			REQUIRE(v <= xmax);
		} // j
	} // i
	StringType ss;
	oRes.toString(ss);
	INFO(ss);
} //section Matdata to Recoded
	SECTION("Compute distance maps"){
	MatDataType oMat(nRows, nCols, gdata, &names, &colNames);
	DistanceMapType oMap;
	DistanceType type = DistanceType::distanceVariance;
	oMat.compute_distances(oMap,type);
	for (size_t i = 0; i < nRows; ++i) {
		for (size_t j = 0; j < i; ++j) {
			double actual = oMap(i, j);
			REQUIRE(actual > 0);
		} // j
	} // i
} // secttion distance maps
} //Matadata
