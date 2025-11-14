"""
ROOT -> HEPMC converter
--------------------------------------------------------------------------------
Since I already have a GEANT4-HEPMC interface it makes sense to conver the 
gfaser ROOT Ntuples from GENIE into HEPMC files
Will split the ROOT file into multiple smaller files, depending on args
This script converts the gfaser files to hepmc, it takes three arguments:

1) input - the input gfaser file to convert
2) -n (--nevents) - the number of events to write per file
3) -o (--output) - a directory to write the output to 
"""

import uproot
import pyhepmc
import argparse
import os
from tqdm import tqdm
import cProfile
import gc

def main(input_file, nevents_per_file=None, outputdir=None):

    # Open the ROOT file and get the tree
    data = uproot.open(f"{input_file}:gFaser")
    
    events = data.arrays(data.keys())
        
    nentries = data.num_entries

    # Work out number of files to make
    print(f"Processing {nentries} events")
    if nevents_per_file is None:
        nevents_per_file = nentries
    
    # Make output directory
    output_filedir = os.getcwd()
    if outputdir is not None:
        output_filedir = outputdir
        os.makedirs(output_filedir,exist_ok=True)
    
    
    # Name output file
    file_num = 0
    output_file = os.path.basename(input_file).replace(".root", f".part.{file_num}.hepmc")
    output_file = os.path.join(output_filedir, output_file)
    
    # Create a HepMC writer
    writer = pyhepmc.io.WriterAscii(output_file)

    # Iterate over the entries in the tree
    for event_num, data in tqdm(enumerate(events), total=nentries):
        
        # if event_num < 160000:
        #     if event_num % nevents_per_file == 0 and event_num != 0:
        #         file_num += 1
        #     continue
        
        # data = {}
        # for key, value in data_dict.items():   # Just reformat the arrays slightly since they're nested 
        #     data[key] = value[0]
         
        event = pyhepmc.GenEvent(pyhepmc.Units.GEV, pyhepmc.Units.MM)
        
        # impose axial timing constraint - time is expected in units of length, so just use z position
        pos = pyhepmc.FourVector(data["vx"]*1000, data["vy"]*1000, data["vz"]*1000, data["vz"]*1000)   #! Got to rescale from meters -> millimeters!
        vertex = pyhepmc.GenVertex(pos)

        # Make lists of data
        nParticles = data["n"]
        pdgc = list(data["pdgc"])
        status = list(data["status"])
        px = list(data["px"])
        py = list(data["py"])
        pz = list(data["pz"])
        E = list(data["E"])
        M = list(data["M"])
        first_mother = list(data["firstMother"])    
        last_mother = list(data["lastMother"])    
        first_daughter = list(data["firstDaughter"])    
        last_daughter = list(data["lastDaughter"])    

        # Make list of particles
        list_of_particles = []
        nccl = 0
        for i in range(len(px)):
            
            genie_status = status[i]
            if (genie_status == 0):   # initial particle
                hepmc_status = 4
            elif (genie_status == 1): # stable final particle
                hepmc_status = 1
                
                if abs(pdgc[first_mother[i]]) in [12, 14, 16] and abs(pdgc[i]) in [11, 13, 15]:
                    # print(f"In event {event_num} - Mother pdgc - {pdgc[first_mother[i]]} lepton pdgc = {pdgc[i]}  setting hepmc status to 5!")
                    hepmc_status = 5 #! My special hack to flag the charged lepton from nuCC events
                    nccl += 1
            elif (genie_status == 3): # decayed particle
                hepmc_status = 2
            else:                     # catch-all informational particle
                hepmc_status = 3
            
            
            
            mom = pyhepmc.FourVector(px[i], py[i], pz[i], E[i])      
            particle = pyhepmc.GenParticle(mom, int(pdgc[i]), hepmc_status)   
            particle.generated_mass = M[i]

            list_of_particles.append(particle)
        
        if nccl > 1:
            print("WARING: Neutrino produced more than one charged lepton??")
        
        # Add particles to vertex
        for i, particle in enumerate(list_of_particles):
            if (particle.status == 4):
                vertex.add_particle_in(particle)
            else:
                #! Work out mother/daughter assignment  - CAN'T DO!
                # particle.set_parent(list_of_particles[first_mother[i]]) 
                vertex.add_particle_out(particle)    
                

        event.add_vertex(vertex)
        writer.write_event(event)
        gc.collect() #? Will this fix the massive slow down in code after 16,000 events ---> NOPE
        
        # Check if we need a new file
        # print(nevents_per_file, event_num, event_num % nevents_per_file )
        if event_num % nevents_per_file == 0 and event_num != 0:
            writer.close()
            file_num += 1
            output_file = os.path.basename(input_file).replace(".root", f".part.{file_num}.hepmc")
            print(f"Done {event_num} events - switching to {output_file}")
            output_file = os.path.join(output_filedir, output_file)
            writer = pyhepmc.io.WriterAscii(output_file)
        
    writer.close()
    
    
def wrapper():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help='input file', type=str)
    parser.add_argument("-n",  "--nevents", help='number events per file', type=int, default=None)
    parser.add_argument("-o",  "--outputdir", help='output file location', type=str, default=None)
    args = parser.parse_args()
    
    main(args.input, args.nevents, args.outputdir)
    
    
if __name__ == "__main__":
    
    # cProfile.run('wrapper()')
    wrapper()
    