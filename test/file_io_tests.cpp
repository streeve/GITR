#define CATCH_CONFIG_MAIN
//#include "file_io.hpp"
#include <iostream>
#include <libconfig.h++>
#include <stdio.h>
#include "utils.h"
#include "catch.hpp"

TEST_CASE("Factorials are computed", "[factorial]") {
  std::cout << "starting test " << std::endl;
  typedef double P;
  libconfig::Config cfg;
  std::string file = "../test/file.cfg";
  std::cout << "about to importLibConfig " << std::endl;
  importLibConfig(cfg, file);
  std::cout << "imported cfg " << std::endl;
  const std::string var1 = "stuff.thing";
  int this_thing=0;
  getVariable(cfg, var1,this_thing);
  std::cout << "this thing values " << this_thing << std::endl;
  REQUIRE(this_thing == 1);
}
