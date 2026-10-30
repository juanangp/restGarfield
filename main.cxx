/// restGarfield: runs with Garfield++ the simulations of a TRestDetectorTransportManager configuration
///
///    restGarfield config.yaml
///
///    transportManager:
///      class: "TRestDetectorTransportManager"
///      medium:
///        class: "TRestDetectorMedium"
///        name: "ArIso"
///        state: "gas"
///        components: {Ar: 99, iC4H10: 1}
///      gasTable:
///        class: "TRestDetectorTransport"
///        pressures: [1 bar]
///        fieldRange: [10 V/cm, 1000 V/cm]
///
/// The metadata classes are in the detector library (they do not depend on Garfield++); this package only
/// runs them, in declaration order.

#include <exception>
#include <iostream>
#include <stdexcept>

#include "TRestDetectorTransportManager.h"
#include "TRestDetectorTransport.h"
#include "TRestTools.h"
#include "TransportSimulation.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: restGarfield <config.yaml>" << std::endl;
        return 1;
    }

    try {
        const YAML::Node config = TRestTools::ResolveAllRefs(YAML::LoadFile(argv[1]));
        const auto section = TRestTools::GetMetadataClass(config, "TRestDetectorTransportManager");
        if (!section.second) {
            throw std::runtime_error("no section of class TRestDetectorTransportManager in " +
                                     std::string(argv[1]));
        }

        TRestDetectorTransportManager manager(section.first, section.second);
        manager.CheckConfig();
        manager.PrintMetadata();
        const bool verbose = manager.GetVerboseLevel() >= TRestLogManager::REST_Verbose_Level::REST_Info;
        for (const auto& metadata : manager.fSimulations) {
            if (auto* simulation = dynamic_cast<TRestDetectorTransport*>(metadata.get())) {
                RunTransportSimulation(*manager.fMedium, *simulation, verbose);
            } else {
                throw std::runtime_error("restGarfield cannot run " + metadata->GetClassName() + " '" +
                                         metadata->GetName() + "'");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "restGarfield: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}
