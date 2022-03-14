#pragma once

#include <cstdio>
#include <fmt/core.h>
namespace pl
{
    
class FilePtr{
    FILE* fp_{nullptr};
    public:
        FilePtr() = default;
        FilePtr(const char* fname){
            fp_ = fopen(fname, "r");
            if(!fp_)
                throw std::runtime_error(fmt::format("Could not open: {}", fname));
        }
        FilePtr(const FilePtr&) = delete;               //we don't want a copy
        FilePtr& operator=(const FilePtr&) = delete;    //since we handle a resource
        
        FilePtr(FilePtr&& other){
            std::swap(fp_, other.fp_);
        }
        
        FilePtr& operator=(FilePtr&& other){
            std::swap(fp_, other.fp_);
            return *this;
        }

        FILE* get(){
            return fp_;
        }
        ~FilePtr(){
            if(fp_)
                fclose(fp_); //check?
        }
};

}