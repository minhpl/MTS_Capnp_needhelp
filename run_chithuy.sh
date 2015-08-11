#!/bin/bash
c++ -std=gnu++1y -O3 -w -Wall serverdemo.c++ message.capnp.c++ \
     $(pkg-config --cflags --libs capnp-rpc) -ltbb -o serverdemo 
     
c++ -std=gnu++1y -w -O3 -Wall clientdemo.c++ message.capnp.c++ \
    $(pkg-config --cflags --libs capnp-rpc) -o clientdemo 

c++ -std=gnu++1y -w -O3 -Wall Main.c++ message.capnp.c++ \
    $(pkg-config --cflags --libs capnp-rpc) -o Main -ltbb 
