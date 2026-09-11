#!/usr/bin/python3

import os, sys

import h5py
import uproot
import numpy as np
import pyopencl as cl

from config import read_config, write_config
from visc import CLVisc

from subprocess import call
from time import time

from logo import print_logo


def initialize(cfg, gpuid, dens, idx = 0) :
  hydro = CLVisc(cfg, gpu_id = gpuid)

  # Energy Density
  edens  = np.transpose(dens[:], (2, 1, 0)).flatten()
  gridpt = len(edens)

  hydro.h_ev1[:, 0] = cfg.KFACTOR*edens.astype(np.float32)
  hydro.h_ev1[:, 1] = np.zeros(gridpt, dtype = np.float32) # Velocity ?
  hydro.h_ev1[:, 2] = np.zeros(gridpt, dtype = np.float32)
  hydro.h_ev1[:, 3] = np.zeros(gridpt, dtype = np.float32)
  cl.enqueue_copy(hydro.queue, hydro.d_ev[1], hydro.h_ev1).wait()

  # Baryon Density
  # if cfg.baryon_on :
  #   nbdens = np.transpose(dens, (2, 1, 0)).flatten()
  #   hydro.h_nb[:] = nbdens
  #   cl.enqueue_copy(hydro.queue, hydro.d_nb[1], hydro.h_nb).wait()

  return hydro


def freezeout(cfg, gpuid, idx = 0) :
  # Create 2 files : Hydro output (resonances decayed), Cascade input (with resonances)

  # Sample
  call(['./sampler', cfg.fPathOut, "0", cfg.switch_eos, str(cfg.nsample), "0", "SMASH"])
  call("mkdir -p before && mv mc_particle_list0 before", shell = True)

  # Force Decay
  call(['./sampler', cfg.fPathOut, "1", cfg.switch_eos, str(cfg.nsample), "1", "SMASH"])

  # Collect Output
  call("mv mc_particle_list0 ParticleHydro-%d" % (idx), shell = True)
  call("mv before/mc_particle_list0 .", shell = True)
  
  # Clean
  call("rm -r before *.dat*", shell = True)


def afterburner(cfg, idx = 0) :
  if cfg.run_afterbuner :
    # Run
    call("./smash -f -i input.smash -o %s" % (os.getcwd()), shell = True)

    # Collect Output
    call("mv particle_lists.oscar ParticleCascade-%d" % (idx), shell = True)

    # Clean
    call("rm mc_particle_list0", shell = True)


if __name__ == '__main__' :

  if len(sys.argv) < 3:
    print("Usage : ./ebe.py path/to/config gpuid")
    sys.exit(1)

  cfg   = read_config(sys.argv[1])
  gpuid = int(sys.argv[2])

  write_config(cfg)
  print_logo()

  # Event Loop

  ti = time()
  print("[Initialized]\n")
  
  evttyp = {'b': np.float64, 'PsiRP' : np.float64, 'NPart' : np.int32, 'Mult' : np.int32}
  evtarr = { 'b' : [], 'PsiRP' : [], 'NPart' : [], 'Mult': [] }

  with h5py.File(cfg.Initial_profile) as init :
    nevt = len(list(init.keys()))

    for idx in range(nevt) :
      print("Event :", idx + 1, end = "\n")

      dens = init["event_%d" % (idx)]

      evtarr['b'].append(dens.attrs['b'])
      evtarr['PsiRP'].append(0.)
      evtarr['NPart'].append(dens.attrs['npart'])
      evtarr['Mult'].append(int(dens.attrs['mult']))

      hydro = initialize(cfg, gpuid, dens, idx)
      hydro.evolve(max_loops = 2000)

      freezeout(cfg, gpuid, idx)
      afterburner(cfg, idx)

      print("\n")

  tf = time()
  print('[Time: {dtime}'.format(dtime = tf - ti), "]\n")


  with uproot.recreate("Initial.root") as init :
    init.mktree("Event", evttyp)

    for branch in evtarr :
      evtarr[branch] = np.array(evtarr[branch], evttyp[branch])

    init["Event"].extend(evtarr)
