include_directories(.)

add_library(
  snark-aggregation

  STATIC

  snark-aggregation.cpp
)

target_link_libraries(
  snark-aggregation

  snark
)

target_link_libraries(
  snark-aggregation

  ff
)

target_include_directories(
  snark-aggregation

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)

set_target_properties(
  snark-aggregation PROPERTIES

  PUBLIC_HEADER snark-aggregation.h
)

add_executable(
  test-aggregation

  test-aggregation.cpp
)
target_link_libraries(
  test-aggregation

  snark
)
target_include_directories(
  test-aggregation

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)