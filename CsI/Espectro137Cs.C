void Espectro137Cs(){

    std::ifstream infile("CsI_spectrum.txt");
    if (!infile.is_open()) {
        std::cout << "No se pudo abrir el archivo\n";
        return;
    }

    auto h = new TH1F("espectro","Espectro Cs-137",2048,0.,2047.);

    double value;
    int bin = 1;
    while (infile >> value) {
        h->SetBinContent(bin, value);
        bin++;
    }
    infile.close();

    // Ajuste de los picos
    auto fitFunc = new TF1("fitFunc","gaus(0)",65.,90.);
    h->Fit(fitFunc, "R");
    double canal1 = fitFunc->GetParameter(1);

    auto fitFunc2 = new TF1("fitFunc2","gaus(0)",1690,1700.);
    h->Fit(fitFunc2, "R+");
    double canal2 = fitFunc2->GetParameter(1);
    
    // Crear TGraph para calibración
    double canal[] = {canal1, canal2};
    double energia[] = {32.0, 662.0};

    TGraph *g = new TGraph(2, canal, energia);
    g->SetMarkerStyle(20);

    auto fitRecta = new TF1("fitRecta","[0]+[1]*x",0,2048);
    g->Fit(fitRecta);

    double offset = fitRecta->GetParameter(0);
    double pendiente = fitRecta->GetParameter(1);

    std::cout << "offset: " << offset << std::endl;
    std::cout << "pendiente: " << pendiente << std::endl;

    // Dibujar histograma original sin rellenar
    h->SetLineColor(kBlack);
    h->Draw("HIST");

    // Cambiar eje X a energía usando la recta
    h->GetXaxis()->SetTitle("Energia (keV)");
    h->GetYaxis()->SetTitle("Cuentas");
    h->GetXaxis()->SetLimits(offset + pendiente*1, offset + pendiente*h->GetNbinsX());

}
