#ifndef RESTGARFIELD_TRANSPORTSIMULATION_H
#define RESTGARFIELD_TRANSPORTSIMULATION_H

class TRestDetectorMedium;
class TRestDetectorTransport;

/// \brief Runs a TRestDetectorTransport with Garfield++: for each condition, creates the Garfield
/// medium of the type of the medium definition (MediumMagboltz, which computes the gas table with
/// Magboltz, MediumSilicon, ...), evaluates the transport parameters on the field grid and adds the table
/// to the medium file (TRestDetectorTransport).
void RunTransportSimulation(const TRestDetectorMedium& medium,
                            TRestDetectorTransport& simulation, bool verbose);

#endif
