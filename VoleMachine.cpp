#include "VoleMachine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;
VoleMachine::VoleMachine():_outputScreen(),_CPU(_outputScreen),_memory(){}
void VoleMachine::Start(){
int choice;
do{
cout<<"\n1. Load Program\n";
cout<<"2. Run Program\n";
cout<<"3. Reset\n";
cout<<"4. Exit\n";
cout<<"Enter your choice: ";
cin>>choice;
switch(choice){
case 1:{
string fileName,line;
cout<<"Enter file name: ";
cin>>fileName;
ifstream file(fileName);
vector<uint16_t> instructions;
uint16_t instruction;
while(getline(file,line)){
stringstream ss(line.substr(0,line.find(';')));
if(ss>>hex>>instruction)
instructions.push_back(instruction);
}
file.close();
LoadProgram(instructions);
break;
}
case 2:
_CPU.Start();
while(!_CPU.IsHalted())
Step();
break;
case 3:
Reset();
break;
case 4:
cout<<"Exiting...\n";
break;
}
}while(choice!=4);
}
void VoleMachine::LoadProgram(vector<uint16_t> instructions){
int loaded=0;
for(uint16_t instruction:instructions){
int address=_CPU.GetPC();
if(address+1>=_memory.GetSize())break;
_memory.SetCell(address,instruction>>8);
_memory.SetCell(address+1,instruction&0xFF);
_CPU.SetPC(address+2);
loaded++;
}
_CPU.SetPC(_CPU.GetPC()-loaded*2);
}
void VoleMachine::Step(){
_CPU.Step(_memory);
cout<<"\n========== MACHINE STATE ==========\n";
cout<<"\nPC: "<<hex<<uppercase<<setfill('0')<<setw(2)<<(int)_CPU.GetPC();
cout<<"\nIR: "<<setw(4)<<_CPU.GetIR();
cout<<"\n\nREGISTERS:\n";
for(int i=0;i<16;i++){
cout<<"R"<<hex<<i<<": "<<setw(2)<<(int)_CPU.GetRegister(i)<<"  ";
if(i%4==3)cout<<"\n";
}
cout<<"\nMEMORY:\n";
for(int i=0;i<256;i++){
if(i%16==0)cout<<"\n"<<setw(2)<<i<<": ";
cout<<setw(2)<<(int)_memory.GetCell(i)<<" ";
}
cout<<"\n\nSCREEN OUTPUT:\n";
cout<<_outputScreen.GetOutput();
cout<<"\n\nSCREEN OUTPUT (HEX):\n";
for(unsigned char c:_outputScreen.GetOutput())
cout<<hex<<uppercase<<setw(2)<<setfill('0')<<(int)c<<" ";
cout<<"\n===================================\n";
cout<<dec;
}
void VoleMachine::Reset(){
_memory.Reset();
_CPU.Reset();
}