#! /usr/bin/env python
from __future__ import print_function

from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
import hashlib

from tools import check_cmd, thread_count, last_thread_num, first_thread_num
from helper import clean_outs, agent_time_series, which_outfile, diff_dbs

prey = "Prey"
pred = "Predator"

DIR = os.path.dirname(__file__)

def test_predator_only(thread_count):
    """Tests simulations with Predators only.

    The population is expected to die off after a timestep.
    """
    if thread_count == first_thread_num:
        clean_outs()

    # A reference simulation input for Lotka-Volterra simulation
    sim_input = os.path.join(DIR, "input", "predator.xml")

    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile(thread_count)
    cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]

    print("Confirming valid Cyclus execution.")
    assert rtn ==  0

    series = agent_time_series([prey, pred], outfile)
    print("Prey:", series[prey], "Predators:", series[pred])

    prey_exp = [0 for n in range(10)]
    pred_exp = [1, 1] + [0 for n in range(8)]

    assert series[prey] ==  prey_exp
    assert series[pred] ==  pred_exp

    if thread_count == last_thread_num:
        diff_dbs()
        clean_outs()

def test_prey_only(thread_count):
    """Tests simulations with Preys only.

    The population is expected to grow exponentially.
    """
    if thread_count == first_thread_num:
        clean_outs()
    sim_input = os.path.join(DIR, "input", "prey.xml")
    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile(thread_count)

    cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]

    print("Confirming valid Cyclus execution.")
    assert rtn ==  0

    series = agent_time_series([prey, pred], outfile)
    print("Prey:", series[prey], "Predators:", series[pred])

    prey_exp = [2**n for n in range(10)]
    pred_exp = [0 for n in range(10)]

    assert series[prey] ==  prey_exp
    assert series[pred] ==  pred_exp

    if thread_count == last_thread_num:
        diff_dbs()
        clean_outs()

def test_lotka_volterra(thread_count):
    """Tests simulations with Preys and Predators

    Preys offer a resource representing itself. Predators acquire the resources
    of the preys. When the prey's resource gets accepted, it decommissions
    itself. Preys and predators have a fixed life expectancy. However, if
    a predator does not get any resource for several time steps,
    it must decommission itself as well.
    After certain time steps, predators and preys reproduce and deploy new
    predators and preys respectively.

    A single oscillation is expected and the peak of the predator population is
    expected to occur after the peak of the prey population.
    """
    if thread_count == first_thread_num:
        clean_outs()
    sim_input = os.path.join(DIR, "input", "lotka_volterra_determ.xml")
    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile(thread_count)

    cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]

    print("Confirming valid Cyclus execution.")
    assert rtn ==  0

    series = agent_time_series([prey, pred], outfile)
    print("Prey:", series[prey], "Predators:", series[pred])

    prey_max = series[prey].index(max(series[prey]))
    pred_max = series[pred].index(max(series[pred]))
    print("t_prey_max:", prey_max, "t_pred_max:", pred_max)

    assert(prey_max < pred_max)

    if thread_count == last_thread_num:
        diff_dbs()
        # clean_outs()

