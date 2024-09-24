.. _using:

Using NEMO
==========

.. note::
   The ``nemo_start.(c)sh`` file needs to be sourced to set up your shell environment
   for using NEMO.

In order to use NEMO, you will need to modify your
shell environment, for example in the ``bash`` (or ``zsh``) shell
this could be

.. code-block:: bash

	source /opt/nemo/nemo_start.sh

assuming NEMO was installed in ``/opt/nemo/``, and

.. code-block:: bash

	source /opt/nemo/nemo_start.csh

for users of a ``csh`` like shell. Normally this
line would be added to your ``~/.bashrc`` or ``/.cshrc`` file.

If NEMO was installed with the modules environment, this command would also load
NEMO, independent of the shell:

.. code-block:: bash

	module load nemo


After this the following commands should work for you

.. code-block:: bash

	tsf --help
	man tsf

Updating NEMO
-------------

Although a full update is out of scope for the discussion here, a common case is
when a program is not available, or a collaborator had made a new or updated program
available via github.  The following procedure generally works, assuming you have
write permission in the ``$NEMO`` directory tree:

.. code-block:: bash

   mknemo -u tsf
   mknemo -h
   man mknemo
   tsf --help
		
and an updated version should now be available. Check the value of the ``VERSION=``
value in the output of ``--help`` of the recompiled program, or use the
``help=h`` for a bit more help even.

Writing NEMO program programs is covered in :ref:`progr`, or see
also :ref:`install`.

	
