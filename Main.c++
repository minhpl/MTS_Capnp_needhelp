/*
 * File:   MapTest.c++
 * Author: tanmn2
 *
 * Created on July 13, 2015, 11:40 AM
 */

#include "message.capnp.h"
#include <kj/debug.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <iostream>
#include <capnp/serialize.h>
#include <map>
using ::capnp::List;
using namespace std;
using namespace vmtsmap;





#include "message.capnp.h"
#include <kj/debug.h>
#include <kj/string.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <iostream>
#include <chrono>
#include <string>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <fstream>
#include <tbb/parallel_for.h>
#include "tbb/parallel_reduce.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/version.hpp>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

//#include "message.pb.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/geometry/index/rtree.hpp>

#include <cmath>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <string>
#include <atomic>
#include <mutex>
#include <random>
#include <algorithm>
#include <list>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

using namespace std;
using namespace boost::archive;

typedef bg::model::point<double, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef bg::model::polygon<point, false, false> polygon; // ccw, open polygon
typedef std::pair<box, string> value;
typedef bgi::rtree< value, bgi::rstar<32, 4> > RTREE;

#include "tbb/tbb.h"

using namespace tbb;

struct BoxTypeC {
    double topLat;
    double bottomLat;
    double topLng;
    double bottomLng;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & topLat;
        ar & bottomLat;
        ar & topLng;
        ar & bottomLng;
    }

    kj::String toString() {
        return kj::str("(", topLat, ",", topLng, ") , (", bottomLat, ",", bottomLng, ")");
    }
};

struct PointFullC {
    double lat;
    double lon;

    int64_t datetime;
    int32_t device;
    int32_t id;
    int32_t cellID;
    int32_t rfcn; // new
    int16_t rscpRssi;
    int8_t ecno;
    int8_t c_over_i_min; // new
    int8_t c_over_i_max; // new
    int8_t c_over_i_avg; // new
    int8_t rxqual;
    int8_t rxlev1;
    int8_t rxlev2;
    int8_t rxlev3;
    int8_t rxlev4;
    int8_t rxlev5;
    int8_t rxlev6;
    int8_t type;
    int8_t ueposition;
    bool indoor;
    uint8_t eventID;
    //    string data3g2g;
    //    string nbExtra;
    //



    kj::String toString() {
        return kj::str("(", lat, ",", lon, ",", id, ")" );
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & lat;
        ar & lon;
        ar & datetime;
        ar & device;
        ar & id;
        ar & cellID;
        ar & rfcn; // new
        ar & rscpRssi;
        ar & ecno;
        ar & c_over_i_min; // new
        ar & c_over_i_max; // new
        ar & c_over_i_avg; // new
        ar & rxqual;
        ar & rxlev1;
        ar & rxlev2;
        ar & rxlev3;
        ar & rxlev4;
        ar & rxlev5;
        ar & rxlev6;
        ar & type;
        ar & ueposition;
        ar & indoor;
        ar & eventID;
        //        ar & data3g2g;
        //        ar & nbExtra;
    }
};

struct TileDataC {
    vector<PointFullC> data2G;
    vector<PointFullC> data3G;
    BoxTypeC bb;
    int32_t numPoints;
    int32_t maxId;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & data2G;
        ar & data3G;
        ar & bb;
        ar & numPoints;
        ar & maxId;
    }

    void reset() {
        data2G.clear();
        data3G.clear();
        numPoints = 0;
        maxId = 0;
        bb.bottomLat = 0;
        bb.bottomLng = 0;
        bb.topLat = 0;
        bb.topLng = 0;
    }

    void clear() {
        data2G.clear();
        data3G.clear();

        //Basically, we free the underlying memory of data2G and data3G
        data2G.shrink_to_fit();
        data3G.shrink_to_fit();
        numPoints = 0;
        maxId = 0;
        bb.bottomLat = 0;
        bb.bottomLng = 0;
        bb.topLat = 0;
        bb.topLng = 0;
    }

    kj::String toString() {
        return kj::str("(", bb.topLat, ",", bb.topLng, ") , (", bb.bottomLat, ",", bb.bottomLng, ")  <-- ", data2G.size(), "  + ", data3G.size() );
    }
};

struct MetaDataEntryC {
    double topLat;
    double bottomLat;
    double topLng;
    double bottomLng;
    size_t numPoints;
    string fileName;

    kj::String toString() {
        return kj::str("(", topLat, "," , topLng, ") , (", bottomLat, "," , bottomLng, ") , ", numPoints , " , ", fileName);
    }
};

struct MetaDataC {
    BoxTypeC bb;
    vector<MetaDataEntryC> data;

    void reset() {
        bb.bottomLat = 0;
        bb.bottomLng = 0;
        bb.topLat = 0;
        bb.topLng = 0;
        data.clear();
    }

    kj::String toString() {
        kj::String s;
        s = kj::str(bb, " <-- :\n", kj::strArray(data, " \n  "));
        return s;
    }
};

int LoadCapnp(const char* nameFile, TileDataC& tileDataC) {
    //    auto start = std::chrono::high_resolution_clock::now();

    int fd = open(nameFile, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file\n");
        return 1;
    }

    capnp::ReaderOptions options;
    options.traversalLimitInWords = 1024 * 1024 * 1024;
    capnp::StreamFdMessageReader message(fd, options);

    vmtsmap::TileData::Reader reader = message.getRoot<vmtsmap::TileData>();

    auto data2G = reader.getData2G();
    auto sizeData2G = data2G.size();
    //    cout << "Size of Data2G in map.cap: " << sizeData2G << endl;
    tileDataC.data2G.resize(sizeData2G);
    for (int i = 0; i < sizeData2G; i++) {
        vmtsmap::PointFull::Reader item = data2G[i];
        auto& it = tileDataC.data2G[i];
        it.lat = item.getLat();
        it.lon = item.getLon();
        it.datetime = item.getDatetime();
        it.device = item.getDevice();
        it.id = item.getId();
        it.type = item.getType();
        it.cellID = item.getCellID();
        it.rscpRssi = item.getRscpRssi();
        it.ecno = item.getEcno();
        it.c_over_i_avg = item.getCOverIavg();
        it.c_over_i_max = item.getCOverImax();
        it.c_over_i_min = item.getCOverImin();
        it.eventID = item.getEventID();
        it.rxqual = item.getRxqual();
        it.rxlev1 = item.getRxlev1();
        it.rxlev2 = item.getRxlev2();
        it.rxlev3 = item.getRxlev3();
        it.rxlev4 = item.getRxlev4();
        it.rxlev5 = item.getRxlev5();
        it.rxlev6 = item.getRxlev6();
        it.indoor = item.getIndoor();
        it.rfcn = item.getRfcn();
        it.ueposition = item.getUeposition();
    }

    auto data3G = reader.getData3G();
    auto sizeData3G = data3G.size();
    //    cout << "Size of Data3G in map.cap: " << sizeData3G << endl << endl;
    tileDataC.data3G.resize(sizeData3G);
    for (int i = 0; i < sizeData3G; i++) {
        vmtsmap::PointFull::Reader item = data3G[i];
        auto& it = tileDataC.data3G[i];
        it.lat = item.getLat();
        it.lon = item.getLon();
        it.datetime = item.getDatetime();
        it.device = item.getDevice();
        it.id = item.getId();
        it.type = item.getType();
        it.cellID = item.getCellID();
        it.rscpRssi = item.getRscpRssi();
        it.ecno = item.getEcno();
        it.c_over_i_avg = item.getCOverIavg();
        it.c_over_i_max = item.getCOverImax();
        it.c_over_i_min = item.getCOverImin();
        it.eventID = item.getEventID();
        it.rxqual = item.getRxqual();
        it.rxlev1 = item.getRxlev1();
        it.rxlev2 = item.getRxlev2();
        it.rxlev3 = item.getRxlev3();
        it.rxlev4 = item.getRxlev4();
        it.rxlev5 = item.getRxlev5();
        it.rxlev6 = item.getRxlev6();
        it.indoor = item.getIndoor();
        it.rfcn = item.getRfcn();
        it.ueposition = item.getUeposition();
    }

    auto bb = reader.getBb();
    tileDataC.bb.bottomLat = bb.getBottomLat();
    tileDataC.bb.bottomLng = bb.getBottomLng();
    tileDataC.bb.topLat = bb.getTopLat();
    tileDataC.bb.topLng = bb.getTopLng();

    //    cout << "Top Lat in map.cap: " << tileDataC.bb.topLat << endl;
    //    cout << "Bottom Lat in map.cap: " << tileDataC.bb.bottomLat << endl;
    //    cout << "Top Lon in map.cap: " << tileDataC.bb.topLng << endl;
    //    cout << "Bottom Lon in map.cap: " << tileDataC.bb.bottomLng << endl << endl;

    tileDataC.numPoints = reader.getNumPoints();
    tileDataC.maxId = reader.getMaxId();

    close(fd);

    //    auto finish = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    //    std::cout << "Capnp: Time to load from file = " << duration << " milliseconds" << std::endl;
}

int SaveCapnp(const char* nameFile, TileDataC& tileDataC) {
    int fd = open(nameFile, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd < 0) {
        printf("Error opening file\n");
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    //    std::memset(buffer.begin(),0,buffer.size());

    capnp::MallocMessageBuilder message;


    //How to copy contents of the map into Capnp object and write it to file?
    vmtsmap::TileData::Builder builder = message.initRoot<vmtsmap::TileData>();

    auto sizeData2G = tileDataC.data2G.size();
    //    cout << "Size of Data2G in tile: " << sizeData2G << endl;
    auto data2G = builder.initData2G(sizeData2G);
    for (int i = 0; i < sizeData2G; i++) {
        vmtsmap::PointFull::Builder item = data2G[i];
        auto& it = tileDataC.data2G[i];
        item.setLat(it.lat);
        item.setLon(it.lon);
        item.setCOverIavg(it.c_over_i_avg);
        item.setCOverImax(it.c_over_i_max);
        item.setCOverImin(it.c_over_i_min);
        item.setCellID(it.cellID);
        item.setDatetime(it.datetime);
        item.setDevice(it.device);
        item.setEcno(it.ecno);
        item.setEventID(it.eventID);
        item.setId(it.id);
        item.setIndoor(it.indoor);
        item.setRfcn(it.rfcn);
        item.setRscpRssi(it.rscpRssi);
        item.setRxlev1(it.rxlev1);
        item.setRxlev2(it.rxlev2);
        item.setRxlev3(it.rxlev3);
        item.setRxlev4(it.rxlev4);
        item.setRxlev5(it.rxlev5);
        item.setRxlev6(it.rxlev6);
        item.setRxqual(it.rxqual);
        item.setType(it.type);
        item.setUeposition(it.ueposition);
    }

    auto sizeData3G = tileDataC.data3G.size();
    //    cout << "Size of Data3G in tile: " << sizeData3G << endl;
    auto data3G = builder.initData3G(sizeData3G);
    for (int i = 0; i < sizeData3G; i++) {
        vmtsmap::PointFull::Builder item = data3G[i];
        auto& it = tileDataC.data3G[i];
        item.setCOverIavg(it.c_over_i_avg);
        item.setCOverImax(it.c_over_i_max);
        item.setCOverImin(it.c_over_i_min);
        item.setCellID(it.cellID);
        item.setDatetime(it.datetime);
        //        item.setData3g2g(it.data3g2g.c_str());
        item.setDevice(it.device);
        item.setEcno(it.ecno);
        item.setEventID(it.eventID);
        item.setId(it.id);
        item.setIndoor(it.indoor);
        item.setLat(it.lat);
        item.setLon(it.lon);
        //        item.setNbExtra(it.nbExtra.c_str());
        //        cout << "Test nbEXtra: " <<  it.nbExtra << endl;
        item.setRfcn(it.rfcn);
        item.setRscpRssi(it.rscpRssi);
        item.setRxlev1(it.rxlev1);
        item.setRxlev2(it.rxlev2);
        item.setRxlev3(it.rxlev3);
        item.setRxlev4(it.rxlev4);
        item.setRxlev5(it.rxlev5);
        item.setRxlev6(it.rxlev6);
        item.setRxqual(it.rxqual);
        item.setType(it.type);
        item.setUeposition(it.ueposition);
    }

    auto bb = builder.getBb();
    bb.setBottomLat(tileDataC.bb.bottomLat);
    bb.setBottomLng(tileDataC.bb.bottomLng);
    bb.setTopLat(tileDataC.bb.topLat);
    bb.setTopLng(tileDataC.bb.topLng);

    builder.setMaxId(tileDataC.maxId);
    builder.setNumPoints(tileDataC.numPoints);


    capnp::writeMessageToFd(fd, message);
    close(fd);

    //    auto segments = message.getSegmentsForOutput();
    //    size_t sum = 0;
    //    for (int i = 0; i < segments.size(); ++i) {
    //        sum += segments[i].size();
    //    }
    //    std::cout << "Final size " << sum << endl;
    //    auto finish = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    //    cout << "Capnp: Time to save file = " << duration << " milliseconds" << endl << endl;
    //    cout << "Test....."<<builder.getMaxId() << endl;
}

//void SaveBoost(string nameBoost) {
//
//    std::ofstream file{nameBoost};
//    text_oarchive oa{file};
//
//    auto start = std::chrono::high_resolution_clock::now();
//    oa << tileDataC;
//    auto finish = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
//    std::cout << "Boost: Time to save file = " << duration << " milliseconds" << std::endl;
//}
//
//void LoadBoost(string fileName) {
//    auto start = std::chrono::high_resolution_clock::now();
//    std::ifstream file{fileName};
//    text_iarchive ia{file};
//
//    ia >> tileDataC;
//
//    auto finish = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
//    cout << "Boost: Time to load from file = " << duration << " milliseconds" << endl << endl;
//
//}
//
//

inline BoxTypeC boundingBox(TileDataC& tile) {
    double minLat = 99999;
    double maxLat = -99999;
    double minLng = 99999;
    double maxLng = -99999;
    for (int tp = 2; tp < 4; ++tp) {
        auto& data = (tp == 2) ? tile.data2G : tile.data3G;
        for (int j = 0; j < data.size(); ++j) {
            auto& pts = data[j];
            if (pts.lat < minLat) {
                minLat = pts.lat;
            }
            if (pts.lat > maxLat) {
                maxLat = pts.lat;
            }
            if (pts.lon < minLng) {
                minLng = pts.lon;
            }
            if (pts.lon > maxLng) {
                maxLng = pts.lon;
            }
        }
    };

    //        cout<<"--------------------"<<endl;
    //        cout << endl << "topLat: " << maxLat << endl;
    //        cout << "bottomLat: " << minLat << endl;
    //        cout << "topLon: " << maxLng << endl;
    //        cout << "bottomLon: " << minLng << endl;
    BoxTypeC bb;
    bb.topLat = maxLat;
    bb.bottomLat = minLat;
    bb.topLng = maxLng;
    bb.bottomLng = minLng;
    return bb;
}

inline BoxTypeC boundingBox(vector<PointFullC>& data) {
    //        printf("i = %d \n", i);
    double minLat = 99999;
    double maxLat = -99999;
    double minLng = 99999;
    double maxLng = -99999;
    for (int j = 0; j < data.size(); ++j) {
        auto& pts = data[j];
        if (pts.lat < minLat) {
            minLat = pts.lat;
        }
        if (pts.lat > maxLat) {
            maxLat = pts.lat;
        }
        if (pts.lon < minLng) {
            minLng = pts.lon;
        }
        if (pts.lon > maxLng) {
            maxLng = pts.lon;
        }
    }
    BoxTypeC bb;
    bb.topLat = maxLat;
    bb.bottomLat = minLat;
    bb.topLng = maxLng;
    bb.bottomLng = minLng;
    return bb;
}


MetaDataC metaDataC;

int LoadMetaDataCapnp(const char* nameFile) {
    //    auto start = std::chrono::high_resolution_clock::now();

    int fd = open(nameFile, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file\n");
        return 1;
    }

    capnp::ReaderOptions options;
    options.traversalLimitInWords = 1024 * 1024 * 1024;
    capnp::StreamFdMessageReader message(fd, options);

    vmtsmap::MetaData::Reader reader = message.getRoot<vmtsmap::MetaData>();
    auto bb = reader.getBox();
    metaDataC.bb.bottomLat = bb.getBottomLat();
    metaDataC.bb.bottomLng = bb.getBottomLng();
    metaDataC.bb.topLat = bb.getTopLat();
    metaDataC.bb.topLng = bb.getTopLng();

    auto data = reader.getData();
    auto sizeData = data.size();
    cout << "Size of Data in MetaData.cap: " << sizeData << endl;
    metaDataC.data.resize(sizeData);
    for (int i = 0; i < sizeData; i++) {
        vmtsmap::MetaDataEntry::Reader item = data[i];
        auto& it = metaDataC.data[i];
        it.numPoints = item.getNumPoints();
        it.topLat = item.getTopLat();
        it.bottomLat = item.getBottomLat();
        it.topLng = item.getTopLng();
        it.bottomLng = item.getBottomLng();

        it.fileName = item.getFilename();
        //        cout << "TEST fileName: " << it.fileName << endl;
        //        cout << "TEST metadataEntry.topLat: " << it.topLat << endl;
    }

    //    cout << "Check Metadata.bb.bottomLat: " << metaDataC.bb.bottomLat << endl;

    close(fd);
    //
    //    auto finish = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    //    std::cout << "Capnp: Time to load Metadata from file = " << duration << " milliseconds" << std::endl;
}

int SaveMetaDataCapnp(const char* nameFile, BoxTypeC boxTypeC) {
    int fd = open(nameFile, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd < 0) {
        printf("Error opening file\n");
        return 1;
    }

    //    auto start = std::chrono::high_resolution_clock::now();
    //    std::memset(buffer.begin(),0,buffer.size());

    capnp::MallocMessageBuilder message(16000013);

    //How to copy contents of the map into Capnp object and write it to file?
    vmtsmap::MetaData::Builder builder = message.initRoot<vmtsmap::MetaData>();


    auto bb = builder.getBox();
    bb.setBottomLat(boxTypeC.bottomLat);
    bb.setBottomLng(boxTypeC.bottomLng);
    bb.setTopLat(boxTypeC.topLat);
    bb.setTopLng(boxTypeC.topLng);

    auto size = metaDataC.data.size();
    auto data = builder.initData(size);

    for (int i = 0; i < size; i++) {
        vmtsmap::MetaDataEntry::Builder item = data[i];
        auto& it = metaDataC.data[i];
        item.setNumPoints(it.numPoints);
        item.setTopLat(it.topLat);
        item.setBottomLat(it.bottomLat);
        item.setTopLng(it.topLng);
        item.setBottomLng(it.bottomLng);
        item.setFilename(it.fileName);
    }

    capnp::writeMessageToFd(fd, message);
    close(fd);

    auto segments = message.getSegmentsForOutput();
    size_t sum = 0;
    for (int i = 0; i < segments.size(); ++i) {
        sum += segments[i].size();
    }
    //    std::cout << "Final size " << sum << endl;
    //    auto finish = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    //    cout << "Capnp: Time to save file Metadata = " << duration << " milliseconds" << endl << endl;
}

#define X 70
#define Y 70

size_t clusterSize = 0;

void cluster(TileDataC& tileDataC, TileDataC& output) {
    PointFullC map2G[X * Y];
    PointFullC map3G[X * Y];
    for (int i = 0; i < X * Y; i++) {
        map2G[i].id = -1;
        map3G[i].id = -1;
    }

    size_t size = tileDataC.data2G.size() + tileDataC.data3G.size();

    double maxLat = tileDataC.bb.topLat;
    double minLat = tileDataC.bb.bottomLat;
    double maxLon = tileDataC.bb.topLng;
    double minLon = tileDataC.bb.bottomLng;

    double dx = (maxLat - minLat) / X;
    double dy = (maxLon - minLon) / Y;

    for (int type = 2; type < 4; ++type) {
        auto &data = (type == 2) ? tileDataC.data2G : tileDataC.data3G;
        auto &map = (type == 2) ? map2G : map3G;
        auto sz = data.size();
        for (int i = 0; i < sz; i++) {
            auto &pts = data[i];
            double x = pts.lat - minLat;
            double y = pts.lon - minLon;
            int xx = floor(x / dx);
            int yy = floor(y / dy);

            int id = yy * X + xx;
            if (id >= X * Y) continue;
            auto& item = map[id];

            if (item.id == -1) {
                item = pts;
            } else {
                if (pts.id < item.id) {
                    item = pts;
                }
            }
        };

        auto& outputData = (type == 2) ? output.data2G : output.data3G;
        outputData.clear();
        for (int i = 0; i < X * Y; i++) {
            auto& item = map[i];
            if (item.id != -1) {
                outputData.emplace_back(item);
            }

        }
    }

    output.bb.topLat = maxLat;
    output.bb.bottomLat = minLat;
    output.bb.topLng = maxLon;
    output.bb.bottomLng = minLon;

    output.maxId = tileDataC.maxId;
    output.numPoints = output.data2G.size() + output.data3G.size();
    //    cout << "Size of Tile after clustering: " << output.numPoints << endl;

    // for (int type = 2; type < 4; ++type) {
    //     auto& outputData = (type == 2) ? output.data2G : output.data3G;
    //     for (int i = 0; i < outputData.size(); i++) {
    //         double lat = outputData[i].lat;
    //         double lon = outputData[i].lon;
    //         //            cout << "lat of point " << i << ": " << lat << endl;
    //         //            cout << "lon of point " << i << " : " << lon << endl;
    //         if (lat > maxLat || lon > maxLon || lat < minLat || lon < minLon) {
    //             cout << "FAIL" << endl;
    //         }
    //     }

    // }
}

double fRand(double fMin, double fMax) {
    double f = (double) rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

#define N 500
#define M 50
#define FIX 100

BoxTypeC generate() {
    TileDataC tileDataC;
    LoadCapnp("map.cap", tileDataC);
    TileDataC tileDataC2;
    TileDataC output;

    double num[FIX];
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        num[i] = fRand(-0.000001, 0.000001);
    }


    //    cout << endl << "topLat: " << tileDataC.bb.topLat << endl;
    //    cout << "bottomLat: " << tileDataC.bb.bottomLat << endl;
    //    cout << "topLon: " << tileDataC.bb.topLng << endl;
    //    cout << "bottomLon: " << tileDataC.bb.bottomLng << endl << endl;

    double minLatAll = 99999;
    double maxLatAll = -99999;
    double minLonAll = 99999;
    double maxLonAll = -99999;

    cout << "Total points: " << M * N * 2 << endl;

    //    size_t sz = N*2*(sizeof(PointFullC)+32)+sizeof(TileDataC)+1024;
    //    printf("Size %zu\n",sz/1024/1024);
    //    kj::ArrayPtr<capnp::word> buffer = kj::heapArray<capnp::word>(16000013);
    //    printf("Buffer = %zu\n",buffer.size());

    metaDataC.data.resize(M);
    for (int i = 0; i < M; ++i) {
        //        printf("i = %d \n", i);
        tileDataC2.reset();
        auto& bb = tileDataC2.bb;
        double minLat = 99999;
        double maxLat = -99999;
        double minLng = 99999;
        double maxLng = -99999;

        for (int tp = 2; tp < 4; ++tp) {
            auto& data = (tp == 2) ? tileDataC2.data2G : tileDataC2.data3G;
            auto& cpdata = (tp == 2) ? tileDataC.data2G : tileDataC.data3G;
            size_t cpSize = cpdata.size();
            data.resize(N);
            for (int j = 0; j < N; ++j) {
                auto pts = cpdata[ (i * j) % cpSize ];
                pts.lat += num[i % FIX] * 0.00001;
                pts.lon += num[(j + i) % FIX] * 0.00001;
                data[j] = pts;
                if (pts.lat < minLat) {
                    minLat = pts.lat;
                }
                if (pts.lat > maxLat) {
                    maxLat = pts.lat;
                }
                if (pts.lon < minLng) {
                    minLng = pts.lon;
                }
                if (pts.lon > maxLng) {
                    maxLng = pts.lon;
                }
            }
        };

        //        cout<<"--------------------"<<endl;
        //        cout << endl << "topLat: " << maxLat << endl;
        //        cout << "bottomLat: " << minLat << endl;
        //        cout << "topLon: " << maxLng << endl;
        //        cout << "bottomLon: " << minLng << endl;
        bb.topLat = maxLat;
        bb.bottomLat = minLat;
        bb.topLng = maxLng;
        bb.bottomLng = minLng;

        if (maxLat > maxLatAll) {
            maxLatAll = maxLat;
        }

        if (minLat < minLatAll) {
            minLatAll = minLat;
        }

        if (minLng < minLonAll) {
            minLonAll = minLng;
        }

        if (maxLng > maxLonAll) {
            maxLonAll = maxLng;
        }

        tileDataC2.numPoints = N * 2;
        tileDataC2.maxId = tileDataC.maxId;

        //        printf("i = %d \n",i);
        auto s = kj::str(i, ".cap");
        SaveCapnp((char*) s.cStr(), tileDataC2);

        auto s2 = kj::str(i, ".cap.cluster");
        cluster(tileDataC2, output);
        clusterSize += output.numPoints;
        SaveCapnp((char*) s2.cStr(), output);

        //        metaDataC.data.resize(M);
        auto& metaDataEntry = metaDataC.data[i];
        metaDataEntry.numPoints = tileDataC2.numPoints;
        metaDataEntry.topLat = maxLat;
        metaDataEntry.bottomLng = minLng;
        metaDataEntry.bottomLat = minLat;
        metaDataEntry.topLng = maxLng;

        //        cout << "metadataEntry.topLat " << maxLat << endl;

        metaDataEntry.fileName = (char*) s.cStr();

    }
    //    cout << "Size data2G: " << tileDataC2.data2G.size() << endl;
    //    cout << "Size data3G: " << tileDataC2.data3G.size() << endl << endl;
    //
    //    cout << endl << "topLat: " << maxLatAll << endl;
    //    cout << "bottomLat: " << minLatAll << endl;
    //    cout << "topLon: " << maxLonAll << endl;
    //    cout << "bottomLon: " << minLonAll << endl;

    BoxTypeC bbC;
    bbC.topLat = maxLatAll;
    bbC.bottomLat = minLatAll;
    bbC.topLng = maxLonAll;
    bbC.bottomLng = minLonAll;

    //    cout << "TEST bottomLat: " << bbC.bottomLat << endl;

    return bbC;
}

RTREE makeRtree(MetaDataC& metaDataC) {
    std::vector<MetaDataEntryC> data = metaDataC.data;

    std::vector<value> values;

    for (int i = 0; i < data.size(); i++) {
        MetaDataEntryC metaDataEntryC = data[i];

        point maxCorner(metaDataEntryC.topLat, metaDataEntryC.topLng);
        point minCorner(metaDataEntryC.bottomLat, metaDataEntryC.bottomLng);

        box b(minCorner, maxCorner);

        string filename = metaDataEntryC.fileName;

        value v = std::make_pair(b, filename);
        values.push_back(v);
    }

    RTREE rtree(values.begin(), values.end());
    return rtree;
}

void query(BoxTypeC& btc, RTREE& rtree, std::vector<value>& intersect) {
    std::vector<value> result_val;

    point maxCorner(btc.topLat, btc.topLng);
    point minCorner(btc.bottomLat, btc.bottomLng);

    box b(minCorner, maxCorner);

    // KJ_DBG(maxCorner.get<0>(), maxCorner.get<1>());
    // KJ_DBG(minCorner.get<0>(), minCorner.get<1>());

    rtree.query(bgi::intersects(b), std::back_inserter(result_val));
    intersect.clear();


    for (auto value : result_val) {
        intersect.push_back(value);
    }
}

bool inSizeBox(const BoxTypeC& queryBox, const PointFullC& point) {
    double lat = point.lat;
    double lng = point.lon;

    double minLat = queryBox.bottomLat;
    double minLng = queryBox.bottomLng;
    double maxLat = queryBox.topLat;
    double maxLng = queryBox.topLng;

    //    std::cout << "inSizeBox: " <<queryBox.topLat << " " << queryBox.topLng << " " << queryBox.bottomLat << " " << queryBox.bottomLng << std::endl;
    //
    //    std::cout << "inSizeBox2: " <<minLat << " " << minLng << " " << maxLat << " " << maxLng << std::endl;
    //    KJ_DBG(lat,lng);

    return lat >= minLat && lat <= maxLat && lng >= minLng && lng <= maxLng;
}

void queryBox(BoxTypeC& queryBox, std::vector<PointFullC>& points) {

}

inline size_t count(vector<PointFullC>& points, BoxTypeC & bb) {
    size_t size = 0;

    for (auto & pts : points) {

        if (inSizeBox(bb, pts)) {
            size++;
        }
    }
    return size;
};

inline bool insideBox(box b1, BoxTypeC b2) {
    point minCorner2(b2.bottomLat, b2.bottomLng);
    point maxCorner2(b2.topLat, b2.topLng);
    box b(minCorner2, maxCorner2);

    point minCorner1 = b1.min_corner();
    point maxCorner1 = b1.max_corner();

    //    KJ_DBG(b1.min_corner().get<0>(),b1.min_corner().get<1>(),b1.max_corner().get<0>(),b1.max_corner().get<1>() );
    //    KJ_DBG(b.min_corner().get<0>(),b.min_corner().get<1>(),b.max_corner().get<0>(),b.max_corner().get<1>() );

    return bg::covered_by(minCorner1, b) && bg::covered_by(maxCorner1, b);
}


typedef std::pair<point, PointFullC*> value2;
typedef bgi::rtree< value2, bgi::rstar<32, 4> > RTREE_PTS;

struct Cache {
    vector<PointFullC> data2G;
    vector<PointFullC> data3G;
    RTREE_PTS rtree2G;
    RTREE_PTS rtree3G;
    bool loaded;


    Cache() : loaded(false) {
        //        loaded = false;
    }

    void clear() {
        loaded = false;
        {
            vector<PointFullC> tmp2G(0);
            data2G.swap(tmp2G);
        }

        {
            vector<PointFullC> tmp3G(0);
            data3G.swap(tmp3G);
        }

        {
            RTREE_PTS rt;
            rtree2G.swap(rt);
        }

        {
            RTREE_PTS rt;
            rtree3G.swap(rt);
        }
    }

    void makeRTREE(vector<PointFullC>& data, RTREE_PTS& rtree) {
        vector<value2> values;
        values.reserve(data.size());

        for (auto& pts : data) {
            point p(pts.lat, pts.lon);
            values.emplace_back(p, &pts);
        }

        RTREE_PTS rt(values.begin(), values.end());
        rtree.swap(rt);
    }

    void makeRTREE(TileDataC& data) {

        data2G = data.data2G;
        data3G = data.data3G;

        makeRTREE(data2G, rtree2G);
        makeRTREE(data3G, rtree3G);

        loaded = true;
    }

    void queryBox(BoxTypeC& queryBox, vector<PointFullC>& output2G, vector<PointFullC>& output3G) {
        if (!loaded) return;

        point minCorner(queryBox.bottomLat, queryBox.bottomLng);
        point maxCorner(queryBox.topLat, queryBox.topLng);
        box b(minCorner, maxCorner);
//        KJ_DBG(minCorner.get<0>(),minCorner.get<1>());
//        KJ_DBG(maxCorner.get<0>(),maxCorner.get<1>());



        tbb::parallel_invoke([&] {
            vector<value2> results1;
            //            rtree.query(bgi::intersects(b), std::back_inserter(result_val));
//            KJ_DBG(rtree2G.size());
            // printf("********************\n");
            // std::cout<<bg::wkt(b)<<endl;
            // std::cout<<bg::wkt( rtree2G.bounds() )<<endl;

            rtree2G.query(bgi::covered_by(b), std::back_inserter(results1));

            // rtree2G.query(bgi::covered_by([&](value2 const & v){
            //     std::cout<<bg::wkt(v.first)<<endl;
            //     if (bg::covered_by(v.first,b) ){
            //         return true;
            //     }
            //     return false;

            // }), std::back_inserter(results1));
            // KJ_DBG(results1.size());
//            output2G.clear();
//            output2G.reserve(rtree2G.size());
            for (auto& item : results1) {
                output2G.push_back(*(item.second) );
            };
        },
        [&] {
            vector<value2> results2;

            rtree3G.query(bgi::covered_by(b), std::back_inserter(results2));
//            KJ_DBG(results2.size());
//            output3G.clear();
//            output3G.reserve(rtree3G.size());
            for (auto& item : results2) {
                output3G.push_back( *(item.second) );
            };
        });

    }

};

struct LruCache : public Cache {
    typedef std::list<LruCache*> List;
    List::iterator li;

};

std::vector<Cache> cache(M);


/********* A LRU cache that removes items when the mem used in Mb exceeds LruCacheSize, default to 1024 Mb = 1Gb********/
/*************** This cache is used to store unclustered tiles, so we have to be very careful not to use up all memory ****************/
class LRU {
private:
    std::vector<LruCache> rawCache;
    size_t memInMb;
    // typedef std::pair<int, Cache*> ListEntry;
    typedef std::list<LruCache*> List;
    List li;
    size_t LruCacheSize = 1024; //Max
public:

    LRU(size_t maxSizeInMb = 1024) : rawCache(M), memInMb(0), LruCacheSize(maxSizeInMb) {};

    inline LruCache& operator[](int i) {
        return rawCache[i];
    }

    void insert(LruCache& item) {

        size_t sizeOfItemInMb = (sizeof(item) + (item.data2G.size() + item.data3G.size()) * sizeof(PointFullC) + (item.rtree2G.size() + item.rtree3G.size()) * sizeof(RTREE_PTS::value_type)) >> 20 ;
        size_t needed = LruCacheSize - sizeOfItemInMb;
        // KJ_DBG( needed >> 20 , (LruCacheSize >> 20) , LruCacheSize < sizeOfItemInMb);

        if (memInMb > needed) { //Now we need to remove some item from cache
            while (!li.empty() && memInMb > needed) {
                auto toDel(li.back());
                size_t sz = ((toDel->data2G.size() + toDel->data3G.size()) * sizeof(PointFullC) + (toDel->rtree2G.size() + toDel->rtree3G.size()) * sizeof(RTREE_PTS::value_type)) >> 20;
                toDel->clear();
                li.pop_back();

                memInMb -= sz;
                // KJ_DBG("Remove 1 item from mem ",sz,memInMb >> 20,li.size(),needed);
            }
        }
        memInMb += sizeOfItemInMb;
        li.push_front(&item);
        item.li = li.begin();
    }

    inline void update(LruCache& item) {
        li.erase(item.li);
        li.push_front(&item);
        item.li = li.begin();
    }

    inline size_t size() {
        return li.size();
    }

    inline size_t sizeInMb() {
        return memInMb;
    }
};

LRU bigCache(1024);//A cache of 1 Gb

//rawCache is a LRU cache,


std::mutex mtx_loadfile;

//@index : Get an item from cache at a specific index if present
//@filename : If the item is not yet cached in memory, load it from the given file
//big_cache : the item is taken from clustered cache (big_cache = false) or raw cache (original data).
inline Cache& getFromCache(int index, string filename, bool big_cache = false) {
    static __thread TileDataC tileDataC;
    auto& c = (!big_cache) ? cache[index] : bigCache[index];
    if (!c.loaded) {
        tileDataC.reset();
        mtx_loadfile.lock();
        LoadCapnp((char*)filename.c_str(), tileDataC);
        mtx_loadfile.unlock();
        c.makeRTREE(tileDataC);

        if (big_cache) {
            LruCache& item = bigCache[index];
            bigCache.insert(item);
        }
    } else if (big_cache) { //Push c to front of list
        LruCache& item = bigCache[index];
        bigCache.update(item);
    }


    // KJ_DBG("Insert item ",li.size(),memInMb);

    return c;
}



class SumFoo {
    vector<value>* overlaps;
    BoxTypeC querybox;
    MetaDataC* metaDtc;
public:
    size_t my_sum;

    void operator()(const blocked_range<size_t>& r) {
        //            float *a = my_a;
        for (size_t i = r.begin(); i != r.end(); ++i) {
            // tbb::parallel_for(r.begin(),r.end(),(size_t)1,[&](size_t i){
            auto& item = (*overlaps)[i];
            auto& box1 = item.first;
            string filename = item.second;


            if (insideBox(box1, querybox)) {
                for (MetaDataEntryC entry : metaDtc->data) {
                    if (entry.fileName == filename) {
                        my_sum += entry.numPoints;
                    }
                }
                // KJ_DBG(my_sum);
            } else {

                std::string::size_type sz;   // alias of size_t

                int i_dec = std::stoi (filename, &sz);
                auto& c = getFromCache(i_dec, filename, true);//Read from big_cache

                size_t count2G, count3G;
                point minCorner(querybox.bottomLat, querybox.bottomLng);
                point maxCorner(querybox.topLat, querybox.topLng);
                box b(minCorner, maxCorner);
                count2G = 0;
                count3G = 0;

                tbb::parallel_invoke([&]
                {
                    for ( RTREE_PTS::const_query_iterator it = c.rtree2G.qbegin(bgi::covered_by(b)) ;
                    it != c.rtree2G.qend() ; ++it )
                    {
                        count2G++;
                    }

                }
                ,
                [&]
                {
                    for ( RTREE_PTS::const_query_iterator it = c.rtree3G.qbegin(bgi::covered_by(b)) ;
                    it != c.rtree3G.qend() ; ++it )
                    {
                        count3G++;
                    }
                }
                                    );

                // tileDataC.reset();

                // //                mtx_loadfile.lock();
                // LoadCapnp((char *) filename.c_str(), tileDataC);
                // //                mtx_loadfile.unlock();

                // auto& data2G = tileDataC.data2G;
                // auto& data3G = tileDataC.data3G;

                // //        points.insert(points.end(),data2G.begin(),data2G.end());
                // //        points.insert(points.end(),data3G.begin(),data3G.end());

                // size_t count2G = count(data2G, querybox);
                // size_t count3G = count(data3G, querybox);

                //
                //        KJ_DBG(count2G,count3G);
                my_sum += count2G + count3G;
                // KJ_DBG(my_sum);
            }
        }
        // });
    }

    SumFoo(SumFoo& x, split) : my_sum(0), overlaps(x.overlaps), querybox(x.querybox), metaDtc(x.metaDtc) {
    }

    void join(const SumFoo& y) {
        my_sum += y.my_sum;
    }

    SumFoo(std::vector<value>* _overlaps, BoxTypeC _querybox, MetaDataC* _metaDtc) :
        overlaps(_overlaps), querybox(_querybox), metaDtc(_metaDtc), my_sum(0) {
    }


};

class PointFoo {
private:
    vector<value>* overlaps;
    BoxTypeC querybox;
    MetaDataC* metaDtc;


public:
    bool clustered = true;
    vector<PointFullC> points2G;
    vector<PointFullC> points3G;


    void operator()(const blocked_range<size_t>& r) {
        //            float *a = my_a;

        for (size_t i = r.begin(); i != r.end(); ++i) {
            auto& item = (*overlaps)[i];
            auto& box = item.first;
            std::string filename = (clustered) ? kj::str(item.second, ".cluster").cStr() : item.second;

            std::string::size_type sz;   // alias of size_t

            int i_dec = std::stoi (filename, &sz);
//            KJ_DBG(i_dec);
            bool big_cache = (clustered) ? false : true;
            auto& c = getFromCache(i_dec, filename, big_cache);

//             if (!c.loaded) {
//                 // printf("Loading tile %d   %s\n",i_dec,(char*)filename.c_str());
//                 tileDataC.reset();
//                 LoadCapnp((char*)filename.c_str(), tileDataC);
//                 c.makeRTREE(tileDataC);
// //                KJ_DBG(c.loaded,c.data2G.size(),c.data3G.size());
//             }


            if (insideBox(box, querybox)) {
                points2G.insert(points2G.end(), c.data2G.begin(), c.data2G.end());
                points3G.insert(points3G.end(), c.data3G.begin(), c.data3G.end());
                //                for (MetaDataEntryC entry : metaDtc->data) {
                //                    if (entry.fileName == filename) {
                //                        my_sum += entry.numPoints;
                //                    }
                //                }

            } else {
                c.queryBox(querybox, points2G, points3G);
            }
        }
    }

//     ,points2G(x.points2G),points3G(x.points3G)
    PointFoo(PointFoo& x, split) : overlaps(x.overlaps), querybox(x.querybox), metaDtc(x.metaDtc), clustered(true) {
    }

    void join(const PointFoo& y) {
        points2G.insert(points2G.end(), y.points2G.begin(), y.points2G.end());
        points3G.insert(points3G.end(), y.points3G.begin(), y.points3G.end());
//        my_sum += y.my_sum;
    }

    PointFoo(std::vector<value>* _overlaps, BoxTypeC _querybox, MetaDataC* _metaDtc, bool _clustered = true) :
        overlaps(_overlaps), querybox(_querybox), metaDtc(_metaDtc), clustered(_clustered) {
//        points.reserve(1000000);
    }
};

bool loadedMetaData = false;
RTREE rtree;
// MetaDataC metaDtC;

void queryPts(BoxTypeC& queryBox, TileDataC& output, bool clustered = true) {
    if (!loadedMetaData) {
        LoadMetaDataCapnp("metadata.cap");
        // metaDtC = metaDataC;
        // KJ_DBG(metaDataC);

        RTREE rt = makeRtree(metaDataC);
        rtree.swap(rt);
        loadedMetaData = true;
    }

    std::vector<value> overlaps;
    // KJ_DBG(queryBox);
    query(queryBox, rtree, overlaps);
    // KJ_DBG(overlaps.size());
    //    __thread TileDataC tileDataC;

    //    SumFoo sf(&overlaps, queryBox, &metaDtC);
    //    parallel_reduce( blocked_range<size_t>(0,overlaps.size()), sf );

    PointFoo pf(&overlaps, queryBox, &metaDataC, clustered);
    parallel_reduce( blocked_range<size_t>(0, overlaps.size()), pf );

    // KJ_DBG(pf.points2G.size(), pf.points3G.size());
    if (clustered) {
        // if (pf.points2G.size() + pf.points3G.size() ==0){
        //     pf.clustered = false;
        //     pf.points2G.clear();
        //     pf.points3G.clear();
        //     parallel_reduce( blocked_range<size_t>(0, overlaps.size()), pf );
        // }

        TileDataC data;
        data.data2G.swap(pf.points2G);
        data.data3G.swap(pf.points3G);
        data.numPoints = data.data2G.size() + data.data3G.size();
        data.bb = boundingBox(data);
        data.maxId = -1;

        // TileDataC output;
        cluster(data, output);
    } else {
        output.data2G.swap(pf.points2G);
        output.data3G.swap(pf.points3G);
        output.numPoints = output.data2G.size() + output.data3G.size();
        output.bb = boundingBox(output);
        output.maxId = -1;
    }
}

size_t countPts(BoxTypeC& queryBox) {
    if (!loadedMetaData) {
        LoadMetaDataCapnp("metadata.cap");
        // metaDtC = metaDataC;
        // KJ_DBG(metaDataC);

        RTREE rt = makeRtree(metaDataC);
        rtree.swap(rt);
        loadedMetaData = true;
    }

    std::vector<value> overlaps;
    query(queryBox, rtree, overlaps);
    SumFoo sf(&overlaps, queryBox, &metaDataC);
    parallel_reduce( blocked_range<size_t>(0, overlaps.size()), sf );
    return sf.my_sum;
}

inline double RandomDouble(double min, double max)
{
    double r = (double)rand() / (double)RAND_MAX;
    return min + r * (max - min);
}


void genRadomBox(BoxTypeC& box, BoxTypeC& output, double minSz = 0) {
    double mlat = RandomDouble(box.bottomLat, box.topLat);
    double mlng = RandomDouble(box.bottomLng, box.topLng);
    double d = RandomDouble(minSz, 1);
    double dlat = box.topLat - box.bottomLat;
    double dlng = box.topLng - box.bottomLng;
    output.bottomLat = mlat;
    output.bottomLng = mlng;
    output.topLat = mlat + d * dlat;
    output.topLng = mlng + d * dlng;
}

#define NUM_QUERIES 1000

void testQuery() {
    LoadMetaDataCapnp("metadata.cap");
    std::vector<BoxTypeC> boxes(NUM_QUERIES);
    for (int i = 0; i < boxes.size(); ++i) {
        genRadomBox(metaDataC.bb, boxes[i], 0.2);
    }

    /************** CAN BE REMOVED ************/
    /****** Preload some tiles into memory ********/
    for (int i = 0; i < metaDataC.data.size(); ++i) {
        auto& item = metaDataC.data[i];
        std::string filename = kj::str(item.fileName, ".cluster").cStr();

        std::string::size_type sz;   // alias of size_t

        int i_dec = std::stoi (filename, &sz);
        getFromCache(i_dec, filename, false);//Read from clustered_cache
    }

    // queryBox.topLat = (queryBox.topLat + queryBox.bottomLat)*0.5;
    // queryBox.topLng = (queryBox.topLng + queryBox.bottomLng)*0.5;
    TileDataC output;
    double totals = 0;
    bool isClustered = true;
    for (int i = 0; i < boxes.size(); ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        auto& queryBox = boxes[i];
        output.reset();

        queryPts(queryBox, output, isClustered);
        // size_t count = countPts(queryBox);

        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        KJ_DBG(i, queryBox, " --> ", output.numPoints);
        // KJ_DBG(i, queryBox, " --> ", count);
        totals += duration;
        std::cout << "Query took  " << duration << " milliseconds" << std::endl;
        // break;
    }

    std::cout << "It took  " << totals / boxes.size() << " milliseconds per query" << std::endl;


    //    std::cout << "early: " << queryBox.topLat << " " << queryBox.topLng << " " << queryBox.bottomLat << " " << queryBox.bottomLng << std::endl;

    //
    //

    //    KJ_DBG(sf.my_sum);


    //    //for (int i = 0; i < overlaps.size(); i++) {
    //    tbb::parallel_for((size_t) 0, overlaps.size(), (size_t) 1, [&](size_t i) {
    //        //        TileDataC tileDataC;
    //        auto& item = overlaps[i];
    //        auto& box = item.first;
    //                string filename = item.second;
    //
    //        if (insideBox(box, queryBox)) {
    //            for (MetaDataEntryC entry : metaDtC.data) {
    //                if (entry.fileName == filename) {
    //                    totals += entry.numPoints;
    //                }
    //            }
    //        } else {
    //            tileDataC.reset();
    //                    LoadCapnp((char *) filename.c_str(), tileDataC);
    //
    //                    vector<PointFullC> data2G = tileDataC.data2G;
    //                    vector<PointFullC> data3G = tileDataC.data3G;
    //
    //                    //        points.insert(points.end(),data2G.begin(),data2G.end());
    //                    //        points.insert(points.end(),data3G.begin(),data3G.end());
    //
    //                    auto count2G = count(data2G, queryBox);
    //                    auto count3G = count(data3G, queryBox);
    //                    //        KJ_DBG(count2G,count3G);
    //                    totals += count2G + count3G;
    //        }
    //    }
    //    );

    // auto finish = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    // std::cout << "Capnp: Time to load from file = " << duration << " milliseconds" << std::endl;
    //    totals = sf.my_sum;
    // std::cout << "total num point: " << totals << std::endl;

}

// int main(int argc, char** argv) {
//     auto start = std::chrono::high_resolution_clock::now();
    
//     // SaveMetaDataCapnp("metadata.cap", generate());
    
//     testQuery();
//     auto finish = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
//     std::cout << "Total execution time = " << duration << " milliseconds" << std::endl;
//     return 0;
// }



class MapStructImpl final: public MapStruct::Server {

  // Implementation of the Hash Cap'n Proto interface.
  kj::Promise<void> generate(GenerateContext context) override {
    std::cout << "Client request generate " << endl;

    auto response = context.getResults();
    capnp::MallocMessageBuilder messageBuilder;
    auto box = messageBuilder.getRoot<BoxType>();


    LoadMetaDataCapnp("metadata.cap");
    auto bb = metaDataC.bb;

    box.setTopLat(bb.topLat);
    box.setTopLng(bb.topLng);
    box.setBottomLat(bb.bottomLat);
    box.setBottomLng(bb.bottomLng);
    response.setResult(box);

    for (int i = 0; i < metaDataC.data.size(); ++i) {
        auto& item = metaDataC.data[i];
        std::string filename = kj::str(item.fileName, ".cluster").cStr();

        std::string::size_type sz;   // alias of size_t

        int i_dec = std::stoi (filename, &sz);
        getFromCache(i_dec, filename, false);//Read from clustered_cache
    }

    return kj::READY_NOW;

  }

  kj::Promise<void> getBox(GetBoxContext context) override {
    std::cout << "----- Client request getBox----- " << endl;

    auto box = context.getParams().getBox();

    BoxTypeC querybox;
    querybox.topLat = box.getTopLat();
    querybox.bottomLat = box.getBottomLat();
    querybox.topLng = box.getTopLng();
    querybox.bottomLng = box.getBottomLng();

    TileDataC output;
    KJ_DBG(querybox);
    queryPts(querybox, output, true);


    auto response = context.getResults();
    // KJ_DBG(response.getTopLat());

    // capnp::MallocMessageBuilder messageBuilder;
    // auto pointFulls = messageBuilder.getRoot<List<PointFull>>();
    int numPoint2G = output.data2G.size();    
    int numPoint3G = output.data3G.size();
    auto pointFulls = response.initResult(numPoint2G + numPoint3G);
    // //pointFulls.initPointFull(1);
    // // pointFulls.setSize(1);

    std::vector<PointFullC>& data2G = output.data2G;
    std::vector<PointFullC>& data3G = output.data3G;

    // KJ_DBG(data2G[0]);
    KJ_DBG(numPoint2G,numPoint3G);

    int j = 0;
    for (size_t i = 0; i < numPoint2G; i++)
    {
      PointFullC& point2G = data2G[i];      
      auto item = pointFulls[j++];
      item.setLat(point2G.lat);
      item.setLon(point2G.lon);      
    }

    for (size_t i = 0; i < numPoint3G; i++)
    {
      PointFullC& point3G = data3G[i];      
      auto item = pointFulls[j++];
      item.setLat(point3G.lat);
      item.setLon(point3G.lon);      
    }
    // std::cout << "sending reply... " << endl;
    // response.setResult(pointFulls);

    KJ_DBG((numPoint3G+numPoint2G) * sizeof(TileDataC) );

    return kj::READY_NOW;
  }
};

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " ADDRESS[:PORT]\n"
              "Runs the server bound to the given address/port.\n"
              "ADDRESS may be '*' to bind to all local addresses.\n"
              ":PORT may be omitted to choose a port automatically." << std::endl;
    return 1;
  }

  // Set up a server.
  capnp::EzRpcServer server(kj::heap<MapStructImpl>(), argv[1]);

  // Write the port number to stdout, in case it was chosen automatically.
  auto& waitScope = server.getWaitScope();
  uint port = server.getPort().wait(waitScope);
  if (port == 0) {
    // The address format "unix:/path/to/socket" opens a unix domain socket,
    // in which case the port will be zero.
    std::cout << "Listening on Unix socket..." << std::endl;
  } else {
    std::cout << "Listening on port " << port << "..." << std::endl;
  }

  // Run forever, accepting connections and handling requests.
  kj::NEVER_DONE.wait(waitScope);
}
