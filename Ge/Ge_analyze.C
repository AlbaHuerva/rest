#include "TRestRun.h"
#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

#include "TH1F.h"
#include <iostream>

void Ge_analyze(const std::string &fileName)
{
    TRestRun run(fileName);
    TRestGeant4Event *evt = (TRestGeant4Event *)run.GetInputEvent();

    TH1F *hE = new TH1F("hE", "Spectrum;keV;counts", 2000, 0, 2000);

    int nEvents = run.GetEntries();

    for (int i = 0; i < nEvents; i++) {
        run.GetEntry(i);

        double E = evt->GetSensitiveVolumeEnergy(); 

        hE->Fill(E);
    }

    double E0 = 1460.8;
    double window = 20;

    int b1 = hE->FindBin(E0 - window);
    int b2 = hE->FindBin(E0 + window);

    double counts = hE->Integral(b1, b2);
    double eff = counts / nEvents;

    cout << "Counts peak: " << counts << endl;
    
    double BR=0.1034;
    double Activity=1000; // Bq/kg
    
    cout << "Rythm: " << Activity*eff*BR << " counts/s for a sample with A="<<Activity<<" Bq/kg"<< endl;
    
    cout << "Detection efficiency for 1460 keV gammas: " << eff << endl;
}
