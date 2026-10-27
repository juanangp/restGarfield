#ifndef REST_GARFIELD_DRAWTRANSPORT_C
#define REST_GARFIELD_DRAWTRANSPORT_C

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TRestDetectorTransport.h"

/// Draws the transport tables of a medium file (restGarfield gasTable, see TRestDetectorTransport):
/// drift velocity, longitudinal and transversal diffusion, Townsend and attachment coefficients versus the
/// electric field, one color per table (pressure, temperature and field range) at the pressure of the table
/// and one line style per carrier.
///
///    restRoot
///    REST_Garfield_DrawTransport("~/.rest/media/ArIso.root")
///
/// medium is the name of the medium in the file, by default the file name.
TCanvas* REST_Garfield_DrawTransport(const std::string& fileName, std::string medium = "") {
    std::string path = fileName;
    const char* home = std::getenv("HOME");
    if (home && !path.empty() && path[0] == '~') path = home + path.substr(1);
    if (medium.empty()) medium = std::filesystem::path(path).stem().string();

    std::unique_ptr<TFile> file(TFile::Open(path.c_str(), "READ"));
    if (!file || file->IsZombie()) {
        std::cerr << "REST_Garfield_DrawTransport: cannot open " << path << std::endl;
        return nullptr;
    }

    const char* titles[5] = {";E (V/cm);Drift velocity (mm/#mus)",
                             ";E (V/cm);Longitudinal diffusion (#sqrt{cm})",
                             ";E (V/cm);Transversal diffusion (#sqrt{cm})", ";E (V/cm);Townsend (1/cm)",
                             ";E (V/cm);Attachment (1/cm)"};
    TMultiGraph* graphs[5];
    for (int q = 0; q < 5; q++) graphs[q] = new TMultiGraph("", titles[q]);
    auto* legend = new TLegend(0.1, 0.1, 0.9, 0.9);

    int color = 1;
    for (const auto& entry : TRestDetectorTransport::GetTables(file.get(), medium)) {
        const std::string key = entry["key"].as<std::string>();
        TRestDetectorTransport table;
        try {
            table = TRestDetectorTransport::Read(file.get(), medium, key);
        } catch (const std::exception& error) {
            std::cerr << error.what() << std::endl;
            continue;
        }
        // One color per table, one line style per carrier (electron solid, hole dashed, ion dotted)
        for (const auto& [carrier, carrierValues] : table.fCarriers) {
            const std::vector<double>* values[5] = {
                &carrierValues.fDriftVelocity, &carrierValues.fLongitudinalDiffusion,
                &carrierValues.fTransversalDiffusion, &carrierValues.fTownsend, &carrierValues.fAttachment};
            const int style = carrier == "electron" ? 1 : carrier == "hole" ? 2 : 3;
            for (int q = 0; q < 5; q++) {
                if (values[q]->size() != table.fFields.size()) continue;
                auto* graph = new TGraph(table.fFields.size());
                for (size_t i = 0; i < table.fFields.size(); i++) {
                    graph->SetPoint(i, table.fFields[i] * 10, (*values[q])[i]);  // V/mm -> V/cm
                }
                graph->SetLineColor(color);
                graph->SetMarkerColor(color);
                graph->SetLineStyle(style);
                graph->SetLineWidth(2);
                graphs[q]->Add(graph, "LP");
                if (q == 0) legend->AddEntry(graph, (key + " " + carrier).c_str(), "l");
            }
        }
        color = color == 4 ? 6 : color + 1;  // skip yellow
    }
    if (legend->GetNRows() == 0) {
        std::cerr << "REST_Garfield_DrawTransport: no tables of medium '" << medium << "' in " << path
                  << std::endl;
        return nullptr;
    }

    auto* canvas = new TCanvas("REST_Garfield_DrawTransport", medium.c_str(), 1500, 900);
    canvas->Divide(3, 2);
    for (int q = 0; q < 5; q++) {
        canvas->cd(q + 1);
        gPad->SetLogx();
        gPad->SetGrid();
        graphs[q]->Draw("A");
    }
    canvas->cd(6);
    legend->SetHeader(medium.c_str());
    legend->Draw();
    canvas->cd();
    return canvas;
}

#endif
