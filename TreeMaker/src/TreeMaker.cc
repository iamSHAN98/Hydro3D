#include <map>
#include <cstdlib>
#include <iostream>

#include "TreeMaker.h"


void TreeMaker :: InitializeInput(std::string InitialPath, std::string FinalPath, int NEvent){
	InitialFile = TFile::Open(InitialPath.data());	
	InitialTree = InitialFile->Get<TTree>("Event");

	InitialTree->SetBranchAddress("b", &Evt.b);
	InitialTree->SetBranchAddress("PsiRP", &Evt.PsiRP);
	InitialTree->SetBranchAddress("NPart", &Evt.NPart);
	Evt.Mult = 0;

	NEventSample = NEvent;

	FinalFile = std::fstream(FinalPath.data(), std::fstream::in);
}

void TreeMaker :: InitializeOutput(){
	EventTree = new TTree("Events", "Event Info");
	EventTree->Branch("Event", "Event", &Evt);
	EventTree->SetAutoSave(10737418240);

	ParticleTree = new TTree("Particles", "Particle Info");
	ParticleTree->Branch("Particle", "Particle", &Trk);
	ParticleTree->SetAutoSave(10737418240);
}

bool TreeMaker :: MakeTree(int NEventBreak){
	std::string Line;
	std::stringstream LineStream;

	int DummyInt;

	// Load
	while(!FileEnd()){
		Evt.Mult = 0;

		while(std::getline(FinalFile, Line) && Line.size() > 0){
			if(Line[0] == '#') break;

			LineStream.clear();
			LineStream.str(Line);

			// Particle Info
			LineStream >> Trk.Position.t >> Trk.Position.x >> Trk.Position.y >> Trk.Position.z;
			LineStream >> Trk.Mass;
			LineStream >> Trk.Momentum.t >> Trk.Momentum.x >> Trk.Momentum.y >> Trk.Momentum.z;
			LineStream >> Trk.ID;
			LineStream >> DummyInt >> DummyInt;

			ParticleTree->Fill();
			Evt.Mult++;
		}

		if(Evt.Mult){
			if(NEventSaved % NEventSample == 0) InitialTree->GetEntry(NEventSaved/NEventSample);

			EventTree->Fill();

			if(++NEventRead  % NEventBreak == 0) std::cout << NEventRead << " events read\n";
			if(++NEventSaved % NEventBreak == 0) return true;
		}
	}

	if(InitialFile) InitialFile->Close();

	return false;
}
