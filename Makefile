all:
	mkdir -p build
	mkdir -p objects
	rm -rf ./build/*
	rm -rf ./objects/*
	cd build && cmake .. && make libboojum

clean-build:
	mkdir -p build
	rm -rf ./build/*

build-compile:
	mkdir -p build
	cd build && cmake .. && make libboojum; notify-send "Done building"
