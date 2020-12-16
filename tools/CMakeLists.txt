add_executable(make_header)
target_compile_features(make_header PUBLIC cxx_std_17)
target_sources(make_header PRIVATE make_header.cc)
