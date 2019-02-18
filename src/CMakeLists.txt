include_directories(.)

add_library(
  libboojum

  STATIC

  libboojum.cpp
)

target_link_libraries(
  libboojum

  snark
)

target_link_libraries(
  libboojum

  ff
)

target_include_directories(
  boojum

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)

set_target_properties(
  boojum PROPERTIES

  PUBLIC_HEADER libboojum.h
)

# add_executable(
#   test-aggregation

#   test-aggregation.cpp
# )
# target_link_libraries(
#   test-aggregation

#   snark
# )
# target_include_directories(
#   test-aggregation

#   PUBLIC
#   ${DEPENDS_DIR}/libsnark
#   ${DEPENDS_DIR}/libsnark/depends/libfqfft
#   ${DEPENDS_DIR}/libsnark/depends/libff
# )

add_executable(
  test-curve-init

  test_curve_initialization.cpp
)
target_link_libraries(
  test-curve-init

  snark
)
target_include_directories(
  test-curve-init

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)