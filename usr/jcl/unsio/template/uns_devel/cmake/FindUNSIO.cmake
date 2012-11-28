# ============================================================================
# Copyright Jean-Charles LAMBERT - 2012
# e-mail:   Jean-Charles.Lambert@oamp.fr
# address:  Dynamique des galaxies
#           Centre de donneeS Astrophysique de Marseille (CeSAM)
#           Laboratoire d'Astrophysique de Marseille
#           Pole de l'Etoile, site de Chateau-Gombert
#           38, rue Frederic Joliot-Curie
#           13388 Marseille cedex 13 France
#           CNRS U.M.R 6110
# ============================================================================
# CMake module to detect UNSIO library
# ============================================================================

SET(UNSIO_FOUND FALSE)

set (UNSIOLIB UNSIOLIB-NOTFOUND)

if ( UNSIOPATH ) # user configure cmake with variable -DUNSIO_INSTALLPATH="/unsio/path"

  find_library(UNSIOLIB NAMES unsio PATHS ${UNSIOPATH}/lib)
  MESSAGE (STATUS "UNSIOLIB = " ${UNSIOLIB})
  if (NOT  ${UNSIOLIB} STREQUAL  UNSIOLIB-NOTFOUND)
     MESSAGE(STATUS "Found UNSIOLIB =" ${UNSIOLIB})
     SET(UNSIO_FOUND TRUE)
  else ()

  endif()
endif ()

if (NOT UNSIO_FOUND) # try NEMO
  find_library(UNSIOLIB NAMES unsio PATHS $ENV{NEMO}/lib)
  MESSAGE (STATUS "UNSIOLIB = " ${UNSIOLIB})
  if ( NOT  ${UNSIOLIB} STREQUAL  UNSIOLIB-NOTFOUND)
     MESSAGE(STATUS "Found UNSIOLIB in NEMO =" ${UNSIOLIB})
     SET(UNSIO_FOUND TRUE)
     SET(UNSIOPATH $ENV{NEMO})
  else ()

  endif()
endif()

if (NOT UNSIO_FOUND) # try $HOME/local
  find_library(UNSIOLIB NAMES unsio PATHS $ENV{HOME}/local/unsio/lib)
  MESSAGE (STATUS "UNSIOLIB = " ${UNSIOLIB} " -- "  $ENV{HOME})
  if (NOT  ${UNSIOLIB} STREQUAL  UNSIOLIB-NOTFOUND)
     MESSAGE(STATUS "Found UNSIOLIB in $ENV{HOME}/local/unsio/ =" ${UNSIOLIB})
     SET(UNSIO_FOUND TRUE)
     SET(UNSIOPATH $ENV{HOME}/local/unsio)
  else ()

  endif()
endif()

if (NOT UNSIO_FOUND) #  ABORT
  message(SEND_ERROR "UNSIO not found - skipping building tests")
else()

endif()

