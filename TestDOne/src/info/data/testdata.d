//testdata.d
module info.data.testdata;
///////////////////
struct TestData {
	static immutable size_t conso_cols = 7;
	static immutable size_t conso_rows = 12;
	static immutable int[] conso_data = [
		332,428,354,1437,526,247,437,
		293,559,388,1527,567,239,258,
		372,767,562,1948,927,235,433,
		406,563,341,1507,544,324,407,
		386,608,396,1501,558,319,363,
		438,843,689,2345,1148,243,341,
		534,660,367,1620,638,414,407,
		460,699,484,1856,762,400,416,
		385,789,621,2366,1149,304,282,
		655,776,423,1848,759,495,486,
		584,995,548,2056,893,518,319,
		515,1097,887,2630,1187,561,284];
	static string[] conso_vars = [ "PAIN","LEGUMES","FRUITS",
	"VIANDES","VOLAILLES","LAIT","VIN" ];
	static  string[] conso_inds = [ "MA2","EM2","CA2","MA3",
	"EM3","CA3","MA4","EM4","CA4","MA5","EM5","CA5" ];
	///////////////////////////////////
	static immutable size_t socmortal_cols = 6;
	static immutable size_t socmortal_rows = 19;
	static immutable int[] socmortal_data = [ 241,16,330,43,363,325,
											156, 9, 225,10,535,328,
											85,19,349,7,281,345,
											210,12,230,21,298,269,
											156,10,260,13,367,144,
											251, 26,180,29,387,55,
											194,11,151,13,384,122,
											225, 9, 195,26,276,128,
											54,11,219,19,224,319,
											40,136,215,18,320,43,
											241,6,168,11,230,107,
											101,5,179,23,380,9,
											82,15,155,18,342,59,
											40,4,136,17,237,225,
											104,6,138,22,346,41,
											38,7,182,32,314,37,
											89,7,169,10,218,47,
											79,10,130,14,203,36,
											121,102,220,26,273,158];
	static  string[] socmortal_vars = [ "SUICI","HOMIC","AROUT",
	"AINDU","AAUTR","CIRFO" ];
	static string[] socmortal_inds = [ "AUTRICHE","FRANCE","PORTUGAL",
	"ALLFEDER","BELGIQUE","FINLANDE","SUEDE","SUISSE","ITALIE",
	"IRLNORD","DANNEMARK","ISLANDE","ECOSSE","ESPAGNE","NORVEGE",
	"IRLSUD","PABAS","ANGGALLE","USA" ];
  //////////////////
}//struct TestData
