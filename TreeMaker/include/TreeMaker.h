#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "TFile.h"
#include "TTree.h"

#include "Object.h"


class TreeMaker{

	public :

		TreeMaker()  = default;
		~TreeMaker() = default;

		void InitializeInput(std::string InitialPath, std::string FinalPath, int NEvent);
		void InitializeOutput();

		bool FileEnd(){ return FinalFile.eof(); }
		bool MakeTree(int NEventBreak);

		int GetEventRead() { return NEventRead;  }
		int GetEventSaved(){ return NEventSaved; }

	private :

		// Variables : Base

		std::fstream FinalFile;

		Event 	 Evt;		
		Particle Trk;

		TTree *EventTree 		= 0;
		TTree *ParticleTree = 0;

		int NEventRead 	= 0;
		int NEventSaved = 0;

		// Variables : Cache

		TFile *InitialFile  = 0;
		TTree *InitialTree  = 0;
		int    NEventSample = 0;

};