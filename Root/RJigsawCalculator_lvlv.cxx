#include "RestFrames/LabGenFrame.hh"
#include "RestFrames/ResonanceGenFrame.hh"
#include "RestFrames/ResonanceGenFrame.hh"
#include "RestFrames/ResonanceGenFrame.hh"
#include "RestFrames/VisibleGenFrame.hh"
#include "RestFrames/InvisibleGenFrame.hh"
#include "RestFrames/VisibleGenFrame.hh"
#include "RestFrames/InvisibleGenFrame.hh"

#include "RestFrames/LabRecoFrame.hh"
#include "RestFrames/DecayRecoFrame.hh"
#include "RestFrames/DecayRecoFrame.hh"
#include "RestFrames/DecayRecoFrame.hh"
#include "RestFrames/VisibleRecoFrame.hh"
#include "RestFrames/InvisibleRecoFrame.hh"
#include "RestFrames/VisibleRecoFrame.hh"
#include "RestFrames/InvisibleRecoFrame.hh"

#include "RestFrames/InvisibleGroup.hh"
#include "RestFrames/SetMassInvJigsaw.hh"
#include "RestFrames/SetRapidityInvJigsaw.hh"
#include "RestFrames/ContraBoostInvJigsaw.hh"

#include "RJigsawTools/RJigsawCalculator_lvlv.h"
#include "EventLoop/StatusCode.h"

// this is needed to distribute the algorithm to the workers
ClassImp(RJigsawCalculator_lvlv)

RJigsawCalculator_lvlv :: RJigsawCalculator_lvlv() :
  //todo fix
  m_mH(0),
  m_mHw(0),
  m_mW(0),
  m_mWw(0),
  m_mL(0),
  m_mN(0),

  LAB_G(nullptr),
  H_G(nullptr),
  Wa_G(nullptr),
  Wb_G(nullptr),
  La_G(nullptr),
  Na_G(nullptr),
  Lb_G(nullptr),
  Nb_G(nullptr),
  LAB_R(nullptr),
  H_R(nullptr),
  Wa_R(nullptr),
  Wb_R(nullptr),
  La_R(nullptr),
  Na_R(nullptr),
  Lb_R(nullptr),
  Nb_R(nullptr),
  INV_R(nullptr),
  MinMassJigsaw_R(nullptr),
  RapidityJigsaw_R(nullptr),
  ContraBoostJigsaw_R(nullptr)
{}

RJigsawCalculator_lvlv :: ~RJigsawCalculator_lvlv() {
  delete LAB_G;
  delete H_G;
  delete Wa_G;
  delete Wb_G;
  delete La_G;
  delete Na_G;
  delete Lb_G;
  delete Nb_G;
  delete LAB_R;
  delete H_R;
  delete Wa_R;
  delete Wb_R;
  delete La_R;
  delete Na_R;
  delete Lb_R;
  delete Nb_R;
  delete INV_R;
  delete MinMassJigsaw_R;
  delete RapidityJigsaw_R;
  delete ContraBoostJigsaw_R;
}

EL::StatusCode RJigsawCalculator_lvlv::doClearEvent() {
  if(! LAB_G->ClearEvent()){return EL::StatusCode::FAILURE;}
  if(! LAB_R->ClearEvent()){return EL::StatusCode::FAILURE;}

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode RJigsawCalculator_lvlv::doInitialize() {
  using namespace RestFrames;

  m_mH  = 125.;
  m_mHw = 1. ;

  m_mW  = 80.;
  m_mWw = 3  ;

  m_mL = .105;
  m_mN = 0.;


  // Set up toy generation tree (not needed for reconstruction)
  LAB_G = new LabGenFrame     ("LAB_G","LAB");
  H_G   = new ResonanceGenFrame("H_G","H");
  Wa_G  = new ResonanceGenFrame ("Wa_G","W_{a}");
  Wb_G  = new ResonanceGenFrame("Wb_G","W_{b}");
  La_G  = new VisibleGenFrame  ("La_G","#it{l}_{a}");
  Na_G  = new InvisibleGenFrame ("Na_G","#nu_{a}");
  Lb_G  = new VisibleGenFrame   ("Lb_G","#it{l}_{b}");
  Nb_G  = new InvisibleGenFrame ("Nb_G","#nu_{b}");

  LAB_G->SetChildFrame(*H_G);
  H_G->AddChildFrame(*Wa_G);
  H_G->AddChildFrame(*Wb_G);
  Wa_G->AddChildFrame(*La_G);
  Wa_G->AddChildFrame(*Na_G);
  Wb_G->AddChildFrame(*Lb_G);
  Wb_G->AddChildFrame(*Nb_G);

  if( ! LAB_G->InitializeTree()){ return EL::StatusCode::FAILURE;}
  // set Higgs masses

  H_G->SetMass (m_mH  );
  H_G->SetWidth(m_mHw);
  // set W masses
  Wa_G->SetMass(m_mW);
  Wb_G->SetMass(m_mW);
  Wa_G->SetWidth(m_mWw);
  Wb_G->SetWidth(m_mWw);
  // set lepton masses
  La_G->SetMass(m_mL);
  Lb_G->SetMass(m_mL);
  // set neutrino masses
  Na_G->SetMass(m_mN);
  Nb_G->SetMass(m_mN);

  if( ! LAB_G->InitializeAnalysis()){ return EL::StatusCode::FAILURE;}

  // Set up reco analysis tree
  LAB_R = new   LabRecoFrame     ("LAB_R","LAB");
  H_R   = new     DecayRecoFrame   ("H_R","H");
  Wa_R  = new	DecayRecoFrame   ("Wa_R","W_{a}");
  Wb_R  = new	DecayRecoFrame   ("Wb_R","W_{b}");
  La_R  = new    VisibleRecoFrame ("La_R","#it{l}_{a}");
  Na_R  = new  InvisibleRecoFrame ("Na_R","#nu_{a}");
  Lb_R  = new    VisibleRecoFrame ("Lb_R","#it{l}_{b}");
  Nb_R  = new  InvisibleRecoFrame ("Nb_R","#nu_{b}");

  LAB_R->SetChildFrame(*H_R);
  H_R ->AddChildFrame(*Wa_R);
  H_R ->AddChildFrame(*Wb_R);
  Wa_R->AddChildFrame(*La_R);
  Wa_R->AddChildFrame(*Na_R);
  Wb_R->AddChildFrame(*Lb_R);
  Wb_R->AddChildFrame(*Nb_R);

  if( ! LAB_R->InitializeTree()){ return EL::StatusCode::FAILURE; }

  // define groups for the reconstruction trees
  INV_R = new InvisibleGroup("INV_R","WIMP Jigsaws");
  INV_R->AddFrame(*Na_R);
  INV_R->AddFrame(*Nb_R);

  // define jigsaws for the reconstruction tree
  MinMassJigsaw_R = new SetMassInvJigsaw ("MINMASS_R", "Invisible system mass Jigsaw");
  INV_R->AddJigsaw(*MinMassJigsaw_R);

  RapidityJigsaw_R = new SetRapidityInvJigsaw ("RAPIDITY_R", "Invisible system rapidity Jigsaw");
  INV_R->AddJigsaw(*RapidityJigsaw_R);
  RapidityJigsaw_R->AddVisibleFrames((LAB_R->GetListVisibleFrames()));

  ContraBoostJigsaw_R = new ContraBoostInvJigsaw("CONTRA_R","Contraboost invariant Jigsaw");
  INV_R->AddJigsaw(*ContraBoostJigsaw_R);

  ContraBoostJigsaw_R->AddVisibleFrames((Wa_R->GetListVisibleFrames()), 0);
  ContraBoostJigsaw_R->AddVisibleFrames((Wb_R->GetListVisibleFrames()), 1);
  ContraBoostJigsaw_R->AddInvisibleFrames((Wa_R->GetListInvisibleFrames()), 0);
  ContraBoostJigsaw_R->AddInvisibleFrames((Wb_R->GetListInvisibleFrames()), 1);

  // check reconstruction trees
  if( ! LAB_R->InitializeAnalysis()) { return EL::StatusCode::FAILURE;}

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode RJigsawCalculator_lvlv::doCalculate(std::unordered_map<std::string, double>& RJVars,
						   xAOD::IParticleContainer& particles,
						   xAOD::MissingET& met
						   ){
  //todo check the initialization

  // generate event
  double PTH = m_mH*gRandom->Rndm();
  LAB_G->SetTransverseMomenta(PTH);               // give the Higgs some Pt
  double PzH = m_mH*(2.*gRandom->Rndm()-1.);
  LAB_G->SetLongitudinalMomenta(PzH);             // give the Higgs some Pz
  LAB_G->AnalyzeEvent();                          // generate a new event

  // analyze event
  TVector3 MET = LAB_G->GetInvisibleMomentum();    // Get the MET from gen tree
  MET.SetZ(0.);

  // give the signal-like tree the event info and analyze
  INV_R->SetLabFrameThreeVector(MET);               // Set the MET in reco tree
  La_R->SetLabFrameFourVector(La_G->GetFourVector());
  Lb_R->SetLabFrameFourVector(Lb_G->GetFourVector());
  LAB_R->AnalyzeEvent();                            // analyze the event

  //////////////////////////////////////
  // Observable Calculations
  //////////////////////////////////////

  //
  // signal tree observables
  //

  //*** Higgs mass
  double MH = H_R->GetMass();
  double MW = Wa_R->GetMass();

  RJVars["MH_over_HGMass"] = MH/H_G->GetMass();
  RJVars["HGMass"] = H_G->GetMass();
  RJVars["MW_over_mW"] = MW/m_mW;

  // h_MH->Fill(MH/H_G->GetMass());
  // h_mH->Fill(H_G->GetMass());
  // h_MW->Fill(MW/mW);
  // h_MH_v_MW->Fill(MH/H_G->GetMass(),MW/mW);
  // h_mW_v_mW->Fill(Wa_G->GetMass(),Wb_G->GetMass());


  return EL::StatusCode::SUCCESS;
}
