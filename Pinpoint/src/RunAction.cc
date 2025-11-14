#include "RunAction.hh"

#include "AnalysisManager.hh"

RunAction::RunAction() :
  G4UserRunAction() 
{
  //* This will ensure that the AnalysisManager singleton is created at the start of the run action
  //* We need to do this so that we can pass macro commands to it before the run starts
  AnalysisManager* analysis = AnalysisManager::GetInstance();
}

void RunAction::BeginOfRunAction(const G4Run*) {
  AnalysisManager* analysis = AnalysisManager::GetInstance();
  analysis->BeginOfRun();
}

void RunAction::EndOfRunAction(const G4Run* run) {
  AnalysisManager* analysis = AnalysisManager::GetInstance();
  analysis->EndOfRun();

  // retrieve the number of events produced in the run
  G4int nofEvents = run->GetNumberOfEvent();

  // do nothing, if no events were processed
  if (nofEvents == 0) return;
}

RunAction::~RunAction() {;}
