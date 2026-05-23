#include <iostream>
#include <map>
#include <cmath>

#include "TRestRun.h"
#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

using namespace std;

void CsI_analyze_2(const std::string& fileName,
                         double peakWindow = 5.0,
                         double crystalThickness_cm = 5.0) {


    TRestRun* run = new TRestRun(fileName);

    TRestGeant4Metadata* G4Metadata =
        static_cast<TRestGeant4Metadata*>(
            run->GetMetadataClass("TRestGeant4Metadata"));

    const auto sensitiveVolumeName =
        G4Metadata->GetSensitiveVolume();

    const auto sensitiveVolID =
        G4Metadata->GetGeant4GeometryInfo()
            .GetIDFromVolume(sensitiveVolumeName);

    TRestGeant4Event* g4Event =
        static_cast<TRestGeant4Event*>(
            run->GetInputEvent());


    int totalEvents = 0;

    int eventsWithPhoto = 0;
    int eventsWithCompt = 0;


    int purePhotoPeak = 0;
    int multiComptonPeak = 0;


    for (int i = 0; i < run->GetEntries(); i++) {

        run->GetEntry(i);

        totalEvents++;

        double depositedEnergy =
            g4Event->GetEnergyInVolume(
                sensitiveVolumeName.Data());


        map<string, int> processCounter;

        const auto nTracks =
            g4Event->GetNumberOfTracks();

        for (unsigned int trackIndex = 0;
             trackIndex < nTracks;
             trackIndex++) {

            const auto track =
                g4Event->GetTrackPointer(trackIndex);

            const auto hits = track->GetHits();

            for (size_t hitIndex = 0;
                 hitIndex < hits.GetNumberOfHits();
                 hitIndex++) {

                // Solo volumen sensible
                if (hits.GetVolumeId(hitIndex)
                    != sensitiveVolID)
                    continue;

                string process =
                    hits.GetProcessName(hitIndex).Data();

                // Ignorar transporte
                if (process == "Init" ||
                    process == "Transportation")
                    continue;

                processCounter[process]++;
            }
        }


        bool hasPhoto = false;
        bool hasCompt = false;

        int nPhotEvent = 0;
        int nComptEvent = 0;

        for (const auto& [process, n] : processCounter) {


            if (process == "phot") {
                hasPhoto = true;
                nPhotEvent += n;
            }

            if (process == "compt") {
                hasCompt = true;
                nComptEvent += n;
            }
        }

        if (hasPhoto)
            eventsWithPhoto++;

        if (hasCompt)
            eventsWithCompt++;


        if (fabs(depositedEnergy - 662.0)
            < peakWindow) {

            if (nPhotEvent >= 1 &&
                nComptEvent == 0) {

                purePhotoPeak++;
            }


            if (nComptEvent >= 1) {

                multiComptonPeak++;
            }
        }
    }



    double rho = 4.51; // g/cm3 para CsI

    double Pphoto =
        (double)eventsWithPhoto / totalEvents;

    double Pcompt =
        (double)eventsWithCompt / totalEvents;

    double muPhoto =
        -log(1.0 - Pphoto)
        / crystalThickness_cm;

    double muCompt =
        -log(1.0 - Pcompt)
        / crystalThickness_cm;

    double muRhoPhoto =
        muPhoto / rho;

    double muRhoCompt =
        muCompt / rho;



    double totalPeak =
        purePhotoPeak + multiComptonPeak;

    double probPhotoPeak = 0;
    double probMultiPeak = 0;

    if (totalPeak > 0) {

        probPhotoPeak =
            purePhotoPeak / totalPeak;

        probMultiPeak =
            multiComptonPeak / totalPeak;
    }



    cout << endl;
    cout << "Numero total de eventos: "
         << totalEvents << endl;

    cout << endl;
    cout << "Eventos con fotoelectrico: "
         << eventsWithPhoto << endl;

    cout << "Eventos con Compton: "
         << eventsWithCompt << endl;

    cout << endl;
    cout << "COEFICIENTES DE ATENUACION MASICOS" << endl;
 

    cout << endl;
    cout << "mu/rho fotoelectrico = "
         << muRhoPhoto
         << " cm2/g" << endl;

    cout << "mu/rho Compton = "
         << muRhoCompt
         << " cm2/g" << endl;

    cout << endl;
    cout << "FOTOPICO 662 keV" << endl;
 

    cout << endl;
    cout << "Eventos fotoelectricos puros = "
         << purePhotoPeak << endl;

    cout << "Eventos multi-Compton = "
         << multiComptonPeak << endl;

    cout << endl;
    cout << "Probabilidad fotoelectrico puro = "
         << probPhotoPeak << endl;

    cout << "Probabilidad multi-Compton = "
         << probMultiPeak << endl;
}