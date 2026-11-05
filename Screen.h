#pragma once
#include <string>
#include <iostream>
#include <cstdint>
using namespace std;
class Screen {
    private:
      string _memoryMappedOutput;
    public:
      void DisplayChar( uint8_t character){
        _memoryMappedOutput+= static_cast<char>(character );
    }
    string GetOutput() const {
     return _memoryMappedOutput;
    }
     void Clear(){
      _memoryMappedOutput.clear();
    }
    void Print() const {
        cout << _memoryMappedOutput << endl;
    }
};
