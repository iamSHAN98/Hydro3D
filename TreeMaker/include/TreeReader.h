#pragma once

#include <memory>
#include "TFile.h"
#include "TTree.h"
#include "Object.h"


class TreeReader {

  public :

    TreeReader()          = default;
    virtual ~TreeReader() = default;

    virtual void Load(const std::string& FilePath){
      if(File) File->Close();

      File         = TFile::Open(FilePath.data());
      EventTree    = File->Get<TTree>("Events");
      ParticleTree = File->Get<TTree>("Particles");

      Initialize();
    }

    virtual void Initialize(){
      EventTree->SetBranchAddress("Event", &EventEntry);
      ParticleTree->SetBranchAddress("Particle", &ParticleEntry);
    
      EventEntry    = 0;
      ParticleEntry = 0;
      NParticle     = 0;
    }

    Long64_t         GetNEvent(){ return EventTree->GetEntriesFast(); }
    virtual Long64_t GetNParticle(Long64_t Index){ return EventEntry->Mult; }

    virtual Event GetEvent(Long64_t Index){
      if(EventEntry) NParticle += EventEntry->Mult;
      EventTree->GetEntry(Index);
      return *EventEntry;
    }

    virtual Particle GetParticle(Long64_t Index){
      ParticleTree->GetEntry(NParticle + Index);
      return *ParticleEntry;
    }

  protected :

    // Containers
    TFile *File = 0;
    TTree *EventTree, *ParticleTree;

  private :

    // Variables
    Event    *EventEntry;
    Particle *ParticleEntry;
    Long64_t  NParticle;

};