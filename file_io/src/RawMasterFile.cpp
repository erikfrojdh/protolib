 
 #include "protolib/RawMasterFile.hpp"
#include "fmt/core.h"
namespace pl{

 RawMasterFile::RawMasterFile(const std::filesystem::path& fname){
     base_path = fname.parent_path();
     base_name = fname.stem();

 }

 std::filesystem::path RawMasterFile::path(){
     return base_path;
 }

 std::filesystem::path RawMasterFile::raw_file(int mod_id, int file_id){
     return base_path/fmt::format("{}", base_name);
 }

}