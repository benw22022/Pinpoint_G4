import uproot
import awkward as ak
import numpy as np
import matplotlib.pyplot as plt
import os
from tqdm import tqdm
from particle import Particle, InvalidParticle, ParticleNotFound

def _get_name_from_pdgc(pdgc):
    try:
        particle = Particle.from_pdgid(pdgc)
        
        particle_name = particle.name
        
        if pdgc < 0 and "+" not in particle_name and "-" not in particle_name:
            particle_name = f"anti-{particle_name}"
        
        return particle_name
        
    except InvalidParticle:
        return f"Unknown PDG Code: {pdgc}"
    except ParticleNotFound:
        return f"Particle not found: {pdgc}"

def _get_charge_from_pdgc(pdgc):
    try:
        particle = Particle.from_pdgid(pdgc)
        
        particle_charge = particle.charge
        
        return particle_charge
        
    except InvalidParticle:
        return f"Unknown PDG Code: {pdgc}"
    except ParticleNotFound:
        return f"Particle not found: {pdgc}"
    
    
def get_name_from_pdgc(pdgc):
    result = []
    try:
        len(pdgc)
        for p in pdgc:
            result.append(_get_name_from_pdgc(p))
    except TypeError:
        return _get_name_from_pdgc(pdgc)

    return np.array(result)


def get_charge_from_pdgc(pdgc):
    result = []
    try:
        len(pdgc)
        for p in pdgc:
            result.append(_get_charge_from_pdgc(p))
    except TypeError:
        return _get_name_from_pdgc(pdgc)
    
    return np.array(result)


def scatter_on_ax(ax, event_data, x_var, y_var, label, color,  alpha=1, marker_size=1):
    
    if len(event_data[x_var]) == 0:
        return None
    
    ax.scatter(event_data[x_var], event_data[y_var], color=color, marker=",", label=label, alpha=alpha, s=marker_size)

   
def main():
    data = uproot.open("/home/bewilson/FASERvSi_G4/GeantOutput/FASERvSi.1000.HITS.root")

    output_dir = "eventPlots/FASERvSi.1000.HITS-2/"
    os.makedirs(output_dir, exist_ok=True)

    hits = [data[treename] for treename in data.keys()]

    event_numbers = hits[0].arrays("fEvent", library="np")["fEvent"]

    for i in tqdm(event_numbers):
        
        fig, ax = plt.subplots(ncols=2, figsize=(18, 6))
        
        for s, station in enumerate(hits):
            
            kinematics = station.arrays(station.keys(), library="ak", cut=f"(fEvent == {i}) & (z > 0)")
            
            
            marker_size = 1
            alpha = 1
            
            electrons = kinematics[np.where(np.abs(kinematics['pdgc']) == 11)]
            muons = kinematics[np.where(np.abs(kinematics['pdgc']) == 13)]
            taus = kinematics[np.where(np.abs(kinematics['pdgc']) == 15)]
            
            gluons = kinematics[np.where(np.abs(kinematics['pdgc']) == 21)]
            photons = kinematics[np.where(np.abs(kinematics['pdgc']) == 22)]
            charged_hadrons = kinematics[np.where(np.abs(kinematics['pdgc']) > 37)]
            neutral_hadrons = kinematics[np.where(np.abs(kinematics['pdgc']) > 37)]
            charged_hadrons = kinematics[np.where(get_charge_from_pdgc(kinematics['pdgc']) != 0)]
            neutral_hadrons = kinematics[np.where(get_charge_from_pdgc(kinematics['pdgc']) == 0)]
            
            # scatter_on_ax(ax[0], neutral_hadrons, "z", "y", "neutral hadrons", 'grey', alpha=0.5, marker_size=marker_size)
            scatter_on_ax(ax[0], charged_hadrons, "z", "y", "charged hadrons", 'forestgreen', alpha=alpha, marker_size=marker_size)
            # scatter_on_ax(ax[0], photons, "z", "y", r"$\gamma$", 'yellow', alpha=0.5, marker_size=marker_size)
            # scatter_on_ax(ax[0], gluons, "z", "y", r"$g$", 'orange', alpha=0.5, marker_size=marker_size)
            scatter_on_ax(ax[0], electrons, "z", "y", r"$e^\pm$", 'lightblue', alpha=alpha, marker_size=marker_size)
            scatter_on_ax(ax[0], muons, "z", "y", r"$\mu^\pm$", 'tomato', alpha=alpha, marker_size=marker_size)
            scatter_on_ax(ax[0], taus, "z", "y", r"$\tau^\pm$", 'purple', alpha=alpha, marker_size=marker_size)
            
            # scatter_on_ax(ax[0], neutral_hadrons, "x", "y", "neutral hadrons", 'grey', alpha=0.5, marker_size=marker_size)
            scatter_on_ax(ax[1], charged_hadrons, "x", "y", "charged hadrons", 'forestgreen', alpha=alpha, marker_size=marker_size)
            # scatter_on_ax(ax[0], photons, "x", "y", r"$\gamma$", 'yellow', alpha=0.5, marker_size=marker_size)
            # scatter_on_ax(ax[0], gluons, "x", "y", r"$g$", 'orange', alpha=0.5, marker_size=marker_size)
            scatter_on_ax(ax[1], electrons, "x", "y", r"$e^\pm$", 'lightblue', alpha=alpha, marker_size=marker_size)
            scatter_on_ax(ax[1], muons, "x", "y", r"$\mu^\pm$", 'tomato', alpha=alpha, marker_size=marker_size)
            scatter_on_ax(ax[1], taus, "x", "y", r"$\tau^\pm$", 'purple', alpha=alpha, marker_size=marker_size)
            
        handles, labels = plt.gca().get_legend_handles_labels()
        by_label = dict(zip(labels, handles))
        ax[0].legend(by_label.values(), by_label.keys())# , ncols=2) #, loc='center right', bbox_to_anchor=(1.55, 0.75))
        ax[1].legend(by_label.values(), by_label.keys())# , ncols=2) #, loc='center right', bbox_to_anchor=(1.55, 0.75)) 
        
        ax[0].set_xlabel("z position (mm)")
        ax[0].set_ylabel("y position (mm)")
        ax[0].set_xlim((550, 1600))
        # ax[0].set_ylim((-150, 150))
        
        ax[1].set_xlabel("x position (mm)")
        ax[1].set_ylabel("y position (mm)")
        ax[1].set_xlim((-125, 125))
        # ax[1].set_ylim((-150, 150))
        
        ax[0].set_title(f"{is_cc_label} {neutrino_pdgc_to_label_dict[neutrino_pdgc]} + {get_name_from_pdgc(target_pdgc)}", loc="left")
        
        plt.savefig(f"{output_dir}/event_{i}_particle-dist-x-y-z-y.png", dpi=300, bbox_inches='tight')
            # plt.show()
            # break

if __name__ == "__main__":
    main()