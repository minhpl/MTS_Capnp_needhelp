@0x881a44bf3eccf134;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("vmtsmap");

struct Map {
	campaingID @0 : UInt32; 
	provinceCode @1 : Text;
	startTime @2 : Int64;
	stopTime @3 : Int64;
	deviceList @4 : List(Int32);
	type @5: UInt8;
	indoor @6: Int8;
	ueposition @7: Int8;
}

struct PtLatLon {
	x @0: Int64;
	y @1: Int64;
}

struct PointType {
	x @0: Float64;
	y @1: Float64;
}

struct BoxType {
	topLat @0 : Float64;
	bottomLat @1: Float64;
	topLng @2 : Float64;
	bottomLng @3: Float64;	
}
	
struct PointDataNew {
	lat @0 : Float64;
	lon @1 : Float64;
	datetime @2: Int64;
	id @3 : UInt32;
	device @4: Int32;		
	cellID @5 : UInt32;
	rfcn @6 : Int32;	
	rscpRssi @7 :Int16;
	ecno @8: Int8;	
	cOverImin @9 : Int8;
	cOverImax @10 : Int8;	
	cOverIavg @11 : Int8;			
	indoor @12 : Bool;
	eventID @13 : UInt8;	
	rxqual @14 : Int8;	
	type @15 : UInt8;
	ueposition @16 : Int8;	
}

struct PointDataVector{
	data @0 : List(PointDataNew);	
	numPoints @1 : Int32;
	campaignID @2 : UInt32; 
	provinceCode @3 : Text;
	startTime @4 : Int64;
	stopTime @5 : Int64;
	deviceList @6 : List(Int32);
	type @7: UInt8;
	maxID @8 : Int32;
	totalPoints @9 : Int32;
}

struct Rect {
	topLat @0 : Int64;
	bottomLat @1: Int64;
	topLng @2 : Int64;
	bottomLng @3: Int64;	
}

struct ClusterParam{
	filterType @0 : Int32;
	zoomLevel @1 : Int32;
	viewPercentW @2 : Float32;
	viewPercentH @3 : Float32;
	mapWidthPx @4 : Int32;
	mapHeightPx @5 : Int32;
	pointLimit @6 : Int32;
}

struct Point {
	id @0 : UInt32;
	type @1 : UInt8;
	lat @2 : Int64;
	lon @3 : Int64;	
	rscpRssi @4 :Int16;
	ecno @5: Int16;	
	cOverImin @6 : Int16;
	cOverImax @7 : Int16;	
	datetime @8: Int64;
	device @9: Int32;	
	indoor @10 : Bool;
	eventID @11 : UInt8;
	cellID @12 : Int32;
	rxqual @13 : Int16;
	#rxlev1 @13 : Int16;
	#rxlev2 @14 : Int16;
	#rxlev3 @15 : Int16;
	#rxlev4 @16 : Int16;
	#rxlev5 @17 : Int16;
	#rxlev6 @18 : Int16;
	cOverIavg @14 : Int16;	
	rfcn @15 : Int32;
	ueposition @16 : Int8;
}

struct PointFull {	
	lat @0 : Float64;
	lon @1 : Float64;
	datetime @2: Int64;
	id @3 : UInt32;
	device @4: Int32;	
	cellID @5 : Int32;
	rfcn @6 : Int32;
	type @7 : UInt8;
	rscpRssi @8 : Int16;
	ecno @9: Int8;	
	cOverImin @10 : Int8;
	cOverImax @11 : Int8;
	cOverIavg @12 : Int8;		
	eventID @13 : UInt8;	
	rxqual @14 : Int8;
	rxlev1 @15 : Int8;
	rxlev2 @16 : Int8;
	rxlev3 @17 : Int8;
	rxlev4 @18 : Int8;
	rxlev5 @19 : Int8;
	rxlev6 @20 : Int8;		
	ueposition @21 : Int8;
	indoor @22 : Bool;	
}


struct TileData{
	data2G  @0 : List(PointFull);
	data3G  @1 : List(PointFull);
	bb @2 : BoxType;
	numPoints @3 : Int32;
	maxId	  @4 : Int32;
}


struct MapDetailData{	
	bb @0 : BoxType;
	numPoints @1 : Int32;
	maxId	  @2 : Int32;
	numTiles  @3 : Int8;
	lx 	  @4 : Float64;
        ly        @5 : Float64;
        nROWS     @6 : Int8;
        nCOLS     @7 : Int8;
}

struct Results{
	data @0 : List(Point);
	remains @1 : List(List(PtLatLon));
	loaded @2 : Bool;
	numPoints @3 : Int32;
}


struct AnalysisOptions{
	eps @0 : Float32;
    minP @1: Float32;  
    dbmThreshold @2: Int16;
    rssiThreshold @3: Int16;
    ecnoThreshold @4: Int16;
    lx @5: Int64;
    ly @6: Int64;
    analysisType @7: Int8;
    ov2G @8 : Int32;
    ov3G @9 : Int32;    
    dataType @10 : UInt8;
    minPts @11 : Int32;
    ovExtra @12 : UInt8;    
    rxqualThreshold @13: Int16;
    viewExtra @14 : Bool;
}

struct ResultType{
	area @0 : Float64;
    numPoints @1: Int32;
    centroidx @2: Float64;
    centroidy @3: Float64;
    radius @4 : Float64;
    totalPoints @5 : Int32;
    data @6: List(List(PtLatLon));
    type @7 : UInt8;
    dataType @8 : UInt8;
    maxID @9 : Int32;
	mapNumPoints @10 : Int32;
}

struct NeighboursData{	
	cellCode @0 : Text;
	count @1 : Int32;
	totals @2 :Int32;
	percent @3 : Float32; 
	lat @4 : Float64;
	lon @5 : Float64;
	azimuth @6 : Int16;
}

struct MissingNeighbours{
	serving @0: Text;	
	missing @1 : List(NeighboursData);
	lat @2 : Float64;
	lon @3 : Float64;
	azimuth @4 : Int16;
	type @5 : Int8;
	maxID @6 : Int32;
	mapNumPoints @7 : Int32;
}

struct OverlapReport{
	numCells @0 : Int32;
	percent @1 : Float32;		
}

struct OverlapFinalReport{
	report2G @0 : List(List(OverlapReport));
	report3G @1 : List(List(OverlapReport));
	campaignID @2 : Int32;
	provinceCode @3 : Text;
}

struct MissingNeighbourParam{
	rSearch @0: Float32;
	minOF @1 : Float32;
}

struct FeederParams{
	minStrength2G @0: Int16;
	minStrength3G @1: Int16;
	minPts @2 : Int16;
	minR @3 : Float32;
	minP @4 : Float32;	
}
 
struct CellInfo{
	neighbors @0 : List(Text);
	cellCode @1 : Text;
	lat @2 : Float64;
	lng @3 : Float64;
	percent @4 : Float32;
	tilt @5 : Float32;
	antennaHeightGround @6 : Float32;
	total @7: Int32;
	azimuth @8 : Int16;
	ci @9 : Int32;
	lac @10: Int32;
	bcch @11 : Int32;
	pscBsic @12 : Int32;
	type @13 : UInt8;
	voiceTraffic @14 : Float64;
	dataTraffic @15 : Float64;
}

struct CellDataInfo{
	relations @0 : List(Text);
	cellName @1 : Text;
	lat @2 : Float64;
	lng @3 : Float64;
	ci @4 : Int32;
	azimuth @5 : Int16;
	type @6 : UInt8;
	cellId @7: Int32;
	stationHouseId @8: Int32;
	stationCode @9 : Text;
	voiceTraffic @10 : Float64;
	dataTraffic @11 : Float64;
}

struct CellDataExt
{
	myRelations @0 : List(Text);
	stationCode @1 : Text;
	lat @2 : Float64;
	lng @3 : Float64;
	ci @4 : Int32;	
	cellId @5: Int32;
	stationHouseId @6: Int32;	
	antennaHeightGround @7 : Float32;
	tilt @8 : Float32;
	bcch @9 : Int32;
	lac @10: Int32;
	pscBsic @11 : Int32;	
	azimuth @12 : Int16;	
	type @13 : UInt8;   
	code @14: Text;
}

struct AddSignalMsg2{
	campaignID @0 : UInt32; 
	provinceCode @1 : Text;
	startId @2 : UInt32;
	rows    @3 : List(PointFull);
}

struct DeleteSignalMsg2{
	poly @0: List(PointType);	
	vDeleted @1 : List(UInt32);
}

struct SignalCellExtra{
	bsic   @0    : Int32;
	lac    @1    : Int32;
	arfcn  @2   : Int32;
	cirnc   @3   : Int32;		
}

struct SignalCell{
	id   @0    : Int32; 
    lat  @1    : Float64; 
    lng  @2    : Float64; 
    sc   @3    : Int32;
	lac  @4    : Int32;
	cirnc @5   : Int32;
	rfcn  @6   : Int32;
	nbData @7  : List( SignalCellExtra );                                                                  
    type   @8  : Int8;
}

struct SignalCells{
	numPoints @0 : Int32;
	maxId     @1 : Int32;
	data      @2 : List(SignalCell);
}

struct MetaDataEntry{
	topLat @0 : Float64;
	bottomLat @1: Float64;
	topLng @2 : Float64;
	bottomLng @3: Float64;
    numPoints @4 : Int32;
	filename @5 : Text;
}

struct MetaData{
        box @0 : BoxType;
        data @1 : List(MetaDataEntry);
}

interface MapStruct {	
	insertToMap @0 (map: Map, rect: Rect, data: List(Point)) -> (result: Void);	
	getPoints @1 (map: Map, rect: Rect, extra: ClusterParam) -> (result: Results);	
	getPoints2 @12 (map: Map, rect: Rect, extra: ClusterParam) -> (result: List(Point) );
	insertPointsInPolygon @2 (map: Map, polygons: List(List(PtLatLon)), data: List(Point) ,strictWithin: Bool ) -> (result: Int32);
	analyze @3 (map:Map, rect: Rect, options: AnalysisOptions) -> (result: List(ResultType) );
	report @4 (map: Map, options: AnalysisOptions ) -> (result: OverlapFinalReport);
	reportDone @5 (map: Map) -> (result: Void);
	queryPolygon @6 (map: Map, poly: List(PtLatLon)) -> (result: List(Point));
	missingNeibours @7 (map: Map, rect: Rect,options: AnalysisOptions,param: MissingNeighbourParam) -> (result: List(MissingNeighbours) );
	findMissingNeighbours @8 (map:Map,options: AnalysisOptions,param: MissingNeighbourParam, area: ResultType) -> (result: List(MissingNeighbours) );
	findSwappedFeeders @9 (map:Map,options: AnalysisOptions,param: FeederParams, area: ResultType) -> (result: List(CellInfo));	
	notify @10 (text: Text) -> (result : Text);
	cacheMap @11 (map: Map) -> (result: Void);
	queryPolygon2 @13 (map: Map, poly: List(PtLatLon)) -> (result: List(Point));
	getClusteredPoints @14 (map: Map, box: BoxType, extra: ClusterParam) -> (result: PointDataVector);	
	analyze2 @15 (map:Map,options: AnalysisOptions) -> (result: List(ResultType) );
	report2 @16 (map: Map, options: AnalysisOptions ) -> (result: OverlapFinalReport);
	notify2 @17 (text: Text) -> (result : Text);
	cacheMap2 @18 (map: Map) -> (result: Void);
	reportPoly @19 (map: Map, poly: List(PtLatLon) , options: AnalysisOptions ) -> (result: OverlapFinalReport);
	reportPoly2 @20 (map: Map, poly: List(PointType) , options: AnalysisOptions ) -> (result: OverlapFinalReport);
	findMissingNeighbours2 @21 (map:Map,options: AnalysisOptions,param: MissingNeighbourParam, area: ResultType) -> (result: List(MissingNeighbours) );
	findSwappedFeeders2 @22 (map:Map,options: AnalysisOptions,param: FeederParams, area: ResultType) -> (result: List(CellInfo));	
	removeFromCache2 @23 (map:Map) -> (result: Void);
	reloadCells @24 () -> (result: Void);
	queryCells @25 (box: BoxType) -> (result: List(CellDataInfo));
	insertSignal @26 (msg : AddSignalMsg2) ->  (result : Void);
	removeSignal @27 (map: Map, msg : DeleteSignalMsg2) ->  (result : Void);
	onCellChange @28 (insertIds : List(Int32), updateIds: List(Int32) ) ->  (result : Void);
	updateCell @29 (inserted : List(CellDataExt), updated: List(CellDataExt) ) ->  (result : Void);
	finishUpdateCell @30 () -> (result: Void); 
	pingRPC @31 () -> (result: Void);
	toCSV @32 (map: Map, filename: Text) -> (result: Void);
	exportCellCSV @33 (map: Map, filename: Text) -> (result: Void);

	generate @34 () -> (result: BoxType);
	getBox @35 (box: BoxType) -> (result:List(PointFull));
	
}
