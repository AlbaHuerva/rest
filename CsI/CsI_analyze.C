#include "TRestRun.h"
#include "TRestGeant4Event.h"
#include "TH1F.h"
#include <iostream>

void CsI_analyze(const std::string &fileName)
{

    TRestRun run(fileName);
    TRestGeant4Event *evt = (TRestGeant4Event *)run.GetInputEvent();

    TH1F *hE = new TH1F("hE", "Spectrum;keV;counts", 2048, 0, 800.);

    int nEvents = run.GetEntries();

    for (int i = 0; i < nEvents; i++) {
        run.GetEntry(i);

        double E = evt->GetSensitiveVolumeEnergy(); 

        hE->Fill(E);
    }

    std::ofstream out("CsI_spectrum.txt");

    int nBins = hE->GetNbinsX();

    for (int i = 1; i <= nBins; i++) {

        double y = hE->GetBinContent(i);

        out << y << "\n";
    }

    out.close();
    
    std::cout << "Espectro guardado en CsI_spectrum.txt (Emplear Espectro137Cs.C para la calibracion) " << std::endl;
    
    std::cout << "El espectro se ve con buena resolucion porque Geant4 no tiene en cuenta la resolucion del detector (la energia depositada es exacta, no hay ruido electronico, fluctuaciones estadisticas...)" << std::endl;
    
    std::cout << "Se podría emular esta resolucion con una funcion tipo gaussian smearing, que dada la resolucion en el fotopico (por ejemplo, 7% FWHM a 662 keV), aplique la resolucion adecuada para cada energia" << std::endl;

}
