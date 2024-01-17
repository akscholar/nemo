.. _galpy:

REBOUND
-------

.. note::
   See https://rebound.readthedocs.io


REBOUND is an N-body integrator, i.e. a software package that can
integrate the motion of particles under the influence of gravity. The
particles can represent stars, planets, moons, ring or dust
particles. REBOUND is very flexible and can be customized to
accurately and efficiently solve many problems in astrophysics.

REBOUND is primarely a library, with a mature API, but tools will need
to be written (typically in C/C++) to create science applications. A
python interface is also available.

NEMO has current 4 example programs that are linked with the REBOUND library:

.. list-table:: **Table of REBOUND programs in NEMO**
   :header-rows: 1
   :widths: 15,45

   * - program
     - description

   * - reb2s
     - convert rebound to NEMO

   * - s2reb
     - convert NEMO to rebound

   * - reb_integrate
     - integrate a system and produce a SimulationArchive

   * - reb_viewer
     - view a SimulationArchive


Installation can be done via ``mknemo rebound``, but the manual steps are as follows:     
     
.. code-block::

     cd $NEMO/usr/hannorein
     make install
     cd nemo
     make install


This will copy the library and header file into the $NEMO tree for each of compile path management.