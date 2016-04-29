# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

#SUBDIRS =

AM_CXXFLAGS = $(PROJECT_WXXFLAGS)

# TODO: Why do we still need to specify this for each executable that we build?
COMMON_CXXFLAGS = $(PROJECT_CFLAGS) $(PROJECT_WXXFLAGS)

bin_PROGRAMS = \
  murrayc_find_objects_in_image_with_disjoint_set \
  murrayc_greedy_make_change \
  murrayc_dp_make_change \
  murrayc_bellman_ford \
  murrayc_dijkstra \
  murrayc_floyd_warshall \
  murrayc_kruskals \
  murrayc_prims \
  murrayc_dependency_resolution \
  murrayc_quickselect \
  murrayc_subset_iteration \
  murrayc_dp_string_edit_distance

#List of source files needed to build the executable:
murrayc_find_objects_in_image_with_disjoint_set_SOURCES = \
	src/find_objects_in_image_with_disjoint_set/murrayc_find_objects_in_image_with_disjoint_set.cc
murrayc_find_objects_in_image_with_disjoint_set_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_find_objects_in_image_with_disjoint_set_LDADD = \
	$(PROJECT_LIBS)

murrayc_greedy_make_change_SOURCES = \
	src/greedy_make_change/murrayc_greedy_make_change.cc
murrayc_greedy_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_greedy_make_change_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_make_change_SOURCES = \
	src/dynamic_programming/dp_make_change/murrayc_dp_make_change.cc
murrayc_dp_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_make_change_LDADD = \
	$(PROJECT_LIBS)


graphs_utils_sources =  \
	src/graphs/utils/edge.h \
	src/graphs/utils/vertex.h \
	src/graphs/utils/shortest_path.h \
	src/graphs/utils/example_graphs.h

graph_utils_cxxflags = -I$(top_srcdir)/src/graphs

murrayc_bellman_ford_SOURCES = \
	src/graphs/bellman_ford/murrayc_bellman_ford.cc \
	$(graphs_utils_sources)
murrayc_bellman_ford_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_bellman_ford_LDADD = \
	$(PROJECT_LIBS)

murrayc_dijkstra_SOURCES = \
	src/graphs/dijkstra/murrayc_dijkstra.cc \
	$(graphs_utils_sources)
murrayc_dijkstra_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_dijkstra_LDADD = \
	$(PROJECT_LIBS)

murrayc_floyd_warshall_SOURCES = \
	src/graphs/floyd_warshall/murrayc_floyd_warshall.cc \
	$(graphs_utils_sources)
murrayc_floyd_warshall_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_floyd_warshall_LDADD = \
	$(PROJECT_LIBS)

murrayc_kruskals_SOURCES = \
	src/graphs/kruskals/murrayc_kruskals.cc \
	$(graphs_utils_sources)
murrayc_kruskals_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_kruskals_LDADD = \
	$(PROJECT_LIBS)

murrayc_prims_SOURCES = \
	src/graphs/prims/murrayc_prims.cc \
	$(graphs_utils_sources)
murrayc_prims_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_prims_LDADD = \
	$(PROJECT_LIBS)

murrayc_dependency_resolution_SOURCES = \
	src/graphs/dependency_resolution/murrayc_dependency_resolution.cc
murrayc_dependency_resolution_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dependency_resolution_LDADD = \
	$(PROJECT_LIBS)

murrayc_quickselect_SOURCES = \
	src/quickselect/murrayc_quickselect.cc
murrayc_quickselect_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_quickselect_LDADD = \
	$(PROJECT_LIBS)

murrayc_subset_iteration_SOURCES = \
	src/subset_iteration/murrayc_subset_iteration.cc \
	src/subset_iteration/subset_iterator.h
murrayc_subset_iteration_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_subset_iteration_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_string_edit_distance_SOURCES = \
	src/dynamic_programming/dp_string_edit_distance/murrayc_dp_string_edit_distance.cc
murrayc_dp_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)
