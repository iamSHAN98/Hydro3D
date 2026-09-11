#include <fstream>
#include <stdexcept>
#include <chrono>
#include <iostream>

#include "TreeMaker.h"


int main(int argc, char **argv){

	if(argc < 4) throw std::runtime_error("Usage : ./maketree path/to/final path/to/initial nevent-sample");

	auto FinalPath 	  = argv[1];
	auto InitialPath  = argv[2];
	auto NEventSample = std::stoi(argv[3]);
	auto NEventBreak  = (argc > 4) ? std::stoi(argv[4]) : NEventSample;


	TreeMaker Obj;
	Obj.InitializeInput(InitialPath, FinalPath, NEventSample);

	std::cout << "TreeMaker initialized \n";
  auto Start = std::chrono::system_clock::now();

	int 	 FileIdx = 1;
	TFile *OutFile = 0;

	while(!Obj.FileEnd()){
		OutFile = new TFile(TString::Format("Final-%d.root", FileIdx), "recreate");
		OutFile->SetCompressionLevel(5);

		Obj.InitializeOutput();

		if(Obj.MakeTree(NEventBreak)){
	  	OutFile->Write();
			OutFile->Close();
			FileIdx++;
	  }
	  else{
	  	OutFile->Write();
			OutFile->Close();
	  	break;
	  }
  }

  auto End = std::chrono::system_clock::now();
  double Time = std::chrono::duration_cast<std::chrono::duration<double>>(End - Start).count();
  std::cout << "Total events read : " << Obj.GetEventRead() << "\n";
  std::cout << "Total events (saved) : " << Obj.GetEventSaved() << "\n";
  std::cout << "Time : " << Time << "\n";

  return 0;

}
