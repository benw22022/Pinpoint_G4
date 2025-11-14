"""
Process HEPMC files through GEANT4
----------------------------------------------------------------------------
Simple script to process a HEPMC through the GEANT4 model
Can process multiple files in parallel using multiprocessing
Use `GenieOutput/convert_gfaser_to_hepmc.py` to split your large HEPMCs into 
small ones for parallel processing
Script takes three arguements:
1) g4exe - path to the GEANT4 app executable file
2) input - path to the hepmc file you want to process or path to directory conatining 
           all hepmc files you want to process (will glob all files ending in .hepmc)
3) output - path to a directory to write the output NTuples to. Will create directory if
            it doesn't already exist
"""

import glob
import os
import argparse
import sys
import pyhepmc
import multiprocessing
from multiprocessing import Pool, Queue, Manager


def get_number_of_events_in_hepmc(hepmc_file: str) -> int:
    """
    Simple script to count the number of events in a HEPMC file

    Args:
        hepmc_file (str): path to file

    Returns:
        int: number of events in file
    """
    
    # Open the HepMC3 file
    reader = pyhepmc.io.ReaderAscii(hepmc_file)
    event_count = 0

    # Read and count events
    while not reader.failed():
        event = pyhepmc.GenEvent()
        reader.read_event(event)
        if not reader.failed():
            event_count += 1

    reader.close()

    return event_count


def write_macro(hepmcfile: str, outputfile: str, nevents: int=1000, macro_name: str="proc_hepmc.in", gui: str=False) -> None:
    """
    Function to write geant4 steering macro
    
    Args:
        hepmcfile (str): filepath to hepmc to process
        outputfile (str): filepath to where NTuple output will be saved
        nevents (int, optional): Number of HEPMC evets to run over. Defaults to 1000.
        macro_name (str, optional): file name of macro. Defaults to "proc_hepmc.in".
        gui (str, optional): if True, will create GUI (probably unhelpful but left in just in case). Defaults to False.
    Returns:
        None
    """
    file_contents = f"\
{'/control/execute vis.mac' if gui else ''}\n\
/generator/select hepmcAscii\n\
/generator/hepmcAscii/open {hepmcfile}\n\
/ntuple/output {outputfile}\n\
/run/beamOn {nevents}"
    
    with open(macro_name, 'w') as macro:
        macro.write(file_contents)


def process_one_file(g4exe: str, input_file: str, output_dir: str, nevents: int, output_queue: multiprocessing.Queue) -> None:
    """
    Processes one hepmc file through geant4 and checks whether the task succeeed by checking for the creation of the 
    output NTuple file. Will place a tuple of (<input-file>, <True/False>) in output queue for later retrival.

    Args:
        g4exe (str): filepath to geant4 app executable file
        input_file (str): filepath to hepmc to process
        output_dir (str): filepath to where NTuple output will be saved
        nevents (int): Number of HEPMC evets to run over
        output_queue (multiprocessing.Queue): multiprocessing queue to place result
    Returns:
        None
    """
    macro_name = os.path.basename(input_file).replace(".hepmc", ".in")
    new_output_filepath = os.path.join(output_dir, os.path.basename(input_file).replace(".hepmc", ".root"))
    logfile_name = os.path.join("logs", os.path.basename(input_file).replace(".hepmc", ".log"))
    
    write_macro(input_file, new_output_filepath, nevents=nevents, macro_name=macro_name)
    print(f"Processesing {input_file}...")
    
    # Check if output file exists and is a valid ROOT file
    skip_file = False
    if os.path.exists(new_output_filepath):
        print(f"Output file {new_output_filepath} already exists")
        skip_file = True
        file_size = os.path.getsize(new_output_filepath)
        file_size_kb = file_size / 1024
        if file_size_kb < 100:
            print(f"Existing output file {new_output_filepath} < 100 KB. This is suspiciously small - will rerun")
            skip_file = False
    
    if skip_file:
        return
    
    os.system(f"./{os.path.basename(g4exe)} {macro_name} > {logfile_name} 2>&1")

    if os.path.exists(new_output_filepath):
        output_queue.put((input_file, True))
    else:
        output_queue.put((input_file, False))
        
def worker(hepmc, g4exe, output_dir, output_queue): 
    nevents_in_file = get_number_of_events_in_hepmc(hepmc) 
    process_one_file(g4exe, hepmc, output_dir, nevents_in_file, output_queue)

def main(g4exe: str, input_dir: str, output_dir: str) -> None:
    """
    Main code body

    Args:
        g4exe (str): filepath to geant4 app executable file
        input_file (str): filepath to hepmc to process
        output_dir (str): filepath to where NTuple output will be saved
    Returns:
        None
    """
    
    if input_dir.endswith(".hepmc"):
        hepmc_files = [input_dir]
        if not os.path.exists(input_dir):
            raise ValueError(f"File {input_dir} not found!")
    else:
        hepmc_files = glob.glob(os.path.join(input_dir, "*.hepmc"))
        
        if len(hepmc_files) == 0:
            raise ValueError("No hepmc files found on path!")
    
    g4build_dir = os.path.dirname(g4exe)
    
    os.makedirs(output_dir, exist_ok=True)
    os.chdir(g4build_dir)
    os.makedirs("logs", exist_ok=True)
    
    with Manager() as manager: 
        output_queue = manager.Queue() 
        num_cores = 8
        with Pool(processes=num_cores) as pool: 
            jobs = [] 
            for hepmc in hepmc_files: 
                jobs.append(pool.apply_async(worker, (hepmc, g4exe, output_dir, output_queue))) # Wait for all jobs to finish 
            
            for job in jobs: 
                job.get()
    print("All done")

    # Check the results in the queue and warn if there was a failure
    results = [output_queue.get() for _ in range(len(hepmc_files))]
    was_success = True
    for (in_file, is_complete) in results:
        if not is_complete:
            print(f"Failed to generate output file for {in_file}")
            was_success = False

    #TODO: Make number of CPU cores used configurable
    # jobs = []
    # output_queue = multiprocessing.Queue() 
    # for hepmc in hepmc_files:
        
    #     nevents_in_file = get_number_of_events_in_hepmc(hepmc)
        
    #     p = multiprocessing.Process(target=process_one_file, args=(g4exe, hepmc, output_dir, nevents_in_file, output_queue))
    #     jobs.append(p)
    #     p.start()
    
    # for job in jobs:
    #     job.join()
    
    # print("All done")
    
    # # Check the results in the queue and warn if there was a failure
    # results = [output_queue.get() for _ in range(len(jobs))]
    # was_success = True
    # for (in_file, is_complete) in results:
    #     if not is_complete:
    #         print(f"Failed to generate output file for {in_file}")
    #         was_success = False
    
    # if was_success:
    #     print("All jobs succeeded!")
            
        
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("g4exe", help='path to geant4 app executable')
    parser.add_argument("input", help='input files')
    parser.add_argument("output", help='output directory')
    
    args = parser.parse_args()
    
    main(os.path.abspath(args.g4exe), os.path.abspath(args.input), os.path.abspath(args.output))
    