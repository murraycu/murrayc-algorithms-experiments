# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

#SUBDIRS =

AM_CXXFLAGS = $(PROJECT_WXXFLAGS)

bin_PROGRAMS = \
  murrayc_find_objects_in_image_with_disjoint_set \
  murrayc_greedy_make_change \
  murrayc_dp_make_change

#List of source files needed to build the executable:
murrayc_find_objects_in_image_with_disjoint_set_SOURCES = \
	src/find_objects_in_image_with_disjoint_set/murrayc_find_objects_in_image_with_disjoint_set.cc
murrayc_find_objects_in_image_with_disjoint_set_CXXFLAGS = \
	$(PROJECT_CFLAGS)
murrayc_find_objects_in_image_with_disjoint_set_LDADD = \
	$(PROJECT_LIBS)

murrayc_greedy_make_change_SOURCES = \
	src/greedy_make_change/murrayc_greedy_make_change.cc
murrayc_greedy_make_change_CXXFLAGS = \
	$(PROJECT_CFLAGS)
murrayc_greedy_make_change_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_make_change_SOURCES = \
	src/dp_make_change/murrayc_dp_make_change.cc
murrayc_dp_make_change_CXXFLAGS = \
	$(PROJECT_CFLAGS)
murrayc_dp_make_change_LDADD = \
	$(PROJECT_LIBS)

