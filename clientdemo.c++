#include "message.capnp.h"
#include <kj/debug.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <iostream>
#include <capnp/serialize.h>
#include <map>
#include <unistd.h>
#include <chrono>
#include <vector>
using ::capnp::List;
using namespace std;
using namespace vmtsmap;

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

int main(int argc, const char* argv[]) {
  if (argc != 2) {
      std::cerr << "usage: " << argv[0] << " HOST:PORT\n"
          "Connects to the Hash server at the given address and "
          "does some RPCs." << std::endl;
      return 1;
    }
  capnp::EzRpcClient client(argv[1]);
  MapStruct::Client mapstruct = client.getMain<MapStruct>();

  auto& waitScope = client.getWaitScope();
  auto request = mapstruct.generateRequest();
  vmtsmap::BoxType::Reader box =request.send().wait(waitScope).getResult();     
  cout<<"Generation done"<<endl;
    

  capnp::MallocMessageBuilder messageBuilder;
  BoxTypeC bb;
  bb.topLat=box.getTopLat();
  bb.topLng=box.getTopLng();
  bb.bottomLat=box.getBottomLat();
  bb.bottomLng=box.getBottomLng();
  srand(time(NULL));
    std::vector<BoxTypeC> boxes(1000);
    for (int i = 0; i < boxes.size(); ++i) {
        genRadomBox(bb, boxes[i], 0.2);
    }
    for (int i = 0; i < boxes.size(); ++i) {
      
//      KJ_DBG((rangeLng));
      // KJ_DBG(((double)rand()/ (double) RAND_MAX));
      // double lng2=btLg + ((double)rand()/ (double) RAND_MAX) * (rLg);
      // double lng1=btLg + ((double)rand()/ (double) RAND_MAX) * (rLg);
      // double lat1=btL + ((double)rand()/ (double) RAND_MAX) * (rL);
      // double lat2=btL + ((double)rand()/ (double) RAND_MAX) * (rL);
      // box1.setTopLat(lat1>=lat2?(++lat1):lat2);
      // box1.setTopLng(lng1>=lng2?(++lng1):lng2);
      // box1.setBottomLat(lat1>=lat2?lat2:lat1);
      // box1.setBottomLng(lng1>=lng2?lng2:lng1);
      // KJ_DBG(box1);
      auto request1=mapstruct.getBoxRequest();
      auto boxBuilder = request1.initBox();
      
      boxBuilder.setTopLat(boxes[i].topLat);
      boxBuilder.setTopLng(boxes[i].topLng);
      boxBuilder.setBottomLat(boxes[i].bottomLat);
      boxBuilder.setBottomLng(boxes[i].bottomLng);
      auto start = std::chrono::high_resolution_clock::now();
      auto listPoint=request1.send().wait(waitScope).getResult();
  


      auto finish = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
      cout << "Time = " << duration << " milliseconds" << std::endl;

      KJ_DBG(listPoint.size());
      cout<<endl;
      sleep(5);
    }        
    kj::NEVER_DONE.wait(waitScope);

}