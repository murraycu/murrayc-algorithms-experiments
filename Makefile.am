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

check_PROGRAMS = \
  murrayc_find_objects_in_image_with_disjoint_set \
  murrayc_greedy_make_change \
  murrayc_dp_make_change \
  murrayc_bellman_ford \
  murrayc_breadth_first_search \
  murrayc_detect_cycle \
  murrayc_output_dot_file \
  murrayc_dijkstra \
  murrayc_dinic \
  murrayc_floyd_warshall \
  murrayc_ford_fulkerson \
  murrayc_kruskals \
  murrayc_johnsons \
  murrayc_prims \
  murrayc_push_relabel \
  murrayc_dependency_resolution \
  murrayc_fibonacci_by_matrix_multiplication \
  murrayc_quickselect \
  murrayc_subset_iteration \
  murrayc_dp_string_edit_distance \
  murrayc_wang_tiles \
  murrayc_substring_search_z_algorithm

TESTS = $(check_PROGRAMS)

#List of source files needed to build the executable:
murrayc_find_objects_in_image_with_disjoint_set_SOURCES = \
	src/find_objects_in_image_with_disjoint_set/murrayc_find_objects_in_image_with_disjoint_set.cc \
	src/find_objects_in_image_with_disjoint_set/disjoint_set.hpp
murrayc_find_objects_in_image_with_disjoint_set_CXXFLAGS = \
	$(COMMON_CXXFLAGS) $(PROJECT_GDKMM_CFLAGS)
murrayc_find_objects_in_image_with_disjoint_set_LDADD = \
	$(PROJECT_LIBS)	$(PROJECT_GDKMM_LIBS)

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
	src/graphs/utils/source_and_edge.h \
	src/graphs/utils/example_graphs.h

graph_utils_cxxflags = -I$(top_srcdir)/src/graphs

murrayc_bellman_ford_SOURCES = \
	src/graphs/shortest_path/bellman_ford/main.cc \
	src/graphs/shortest_path/bellman_ford/bellman_ford.h \
	src/graphs/shortest_path/bellman_ford/bellman_ford_with_queue.h \
	$(graphs_utils_sources)
murrayc_bellman_ford_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_bellman_ford_LDADD = \
	$(PROJECT_LIBS)

murrayc_breadth_first_search_SOURCES = \
	src/graphs/shortest_path/breadth_first_search/main.cc \
	src/graphs/shortest_path/breadth_first_search/breadth_first_search.h \
	$(graphs_utils_sources)
murrayc_breadth_first_search_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_breadth_first_search_LDADD = \
	$(PROJECT_LIBS)

murrayc_detect_cycle_SOURCES = \
	src/graphs/detect_cycle/main.cc \
	src/graphs/detect_cycle/detect_cycle.h \
	$(graphs_utils_sources)
murrayc_detect_cycle_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_detect_cycle_LDADD = \
	$(PROJECT_LIBS)

murrayc_output_dot_file_SOURCES = \
	src/graphs/output_dot_file/main.cc
	$(graphs_utils_sources)
murrayc_output_dot_file_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_output_dot_file_LDADD = \
	$(PROJECT_LIBS)

murrayc_dijkstra_SOURCES = \
	src/graphs/shortest_path/dijkstra/main.cc \
	src/graphs/shortest_path/dijkstra/dijkstra.h \
	$(graphs_utils_sources)
murrayc_dijkstra_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_dijkstra_LDADD = \
	$(PROJECT_LIBS)

murrayc_floyd_warshall_SOURCES = \
	src/graphs/shortest_path/floyd_warshall/floyd_warshall.h \
	src/graphs/shortest_path/floyd_warshall/main.cc \
	$(graphs_utils_sources)
murrayc_floyd_warshall_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_floyd_warshall_LDADD = \
	$(PROJECT_LIBS)

murrayc_johnsons_SOURCES = \
	src/graphs/shortest_path/johnsons/johnsons.h \
	src/graphs/shortest_path/johnsons/main.cc \
	src/graphs/shortest_path/bellman_ford/bellman_ford.h \
	src/graphs/shortest_path/dijkstra/dijkstra.h \
	$(graphs_utils_sources)
murrayc_johnsons_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_johnsons_LDADD = \
	$(PROJECT_LIBS)

murrayc_dinic_SOURCES = \
	src/graphs/max_flow/dinic/dinic.h \
	src/graphs/max_flow/dinic/main.cc \
	$(graphs_utils_sources)
murrayc_dinic_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_dinic_LDADD = \
	$(PROJECT_LIBS)

murrayc_push_relabel_SOURCES = \
	src/graphs/max_flow/push_relabel/push_relabel.h \
	src/graphs/max_flow/push_relabel/main.cc \
	$(graphs_utils_sources)
murrayc_push_relabel_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_push_relabel_LDADD = \
	$(PROJECT_LIBS)

murrayc_ford_fulkerson_SOURCES = \
	src/graphs/max_flow/ford_fulkerson/ford_fulkerson.h \
	src/graphs/max_flow/ford_fulkerson/main.cc \
	$(graphs_utils_sources)
murrayc_ford_fulkerson_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_ford_fulkerson_LDADD = \
	$(PROJECT_LIBS)

murrayc_kruskals_SOURCES = \
	src/graphs/minimum_spanning_tree/kruskals/kruskals.h \
	src/graphs/minimum_spanning_tree/kruskals/main.cc \
	$(graphs_utils_sources)
murrayc_kruskals_CXXFLAGS = \
	$(COMMON_CXXFLAGS) \
	$(graph_utils_cxxflags)
murrayc_kruskals_LDADD = \
	$(PROJECT_LIBS)

murrayc_prims_SOURCES = \
	src/graphs/minimum_spanning_tree/prims/prims.h \
	src/graphs/minimum_spanning_tree/prims/main.cc \
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

murrayc_fibonacci_by_matrix_multiplication_SOURCES = \
	src/fibonacci_by_matrix_multiplication/murrayc_fibonacci_by_matrix_multiplication.cc
murrayc_fibonacci_by_matrix_multiplication_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_fibonacci_by_matrix_multiplication_LDADD = \
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

murrayc_wang_tiles_SOURCES = \
	src/combinatorics/wang_tiles/murrayc_wang_tiles.cc
murrayc_wang_tiles_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_wang_tiles_LDADD = \
	$(PROJECT_LIBS)

murrayc_substring_search_z_algorithm_SOURCES = \
	src/substring_search/z_algorithm/main.cc
murrayc_substring_search_z_algorithm_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_substring_search_z_algorithm_LDADD = \
	$(PROJECT_LIBS)
