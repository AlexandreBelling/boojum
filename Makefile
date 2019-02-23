all:
	mkdir -p build
	mkdir -p objects
	#//mkdir -p setup
	rm -rf ./build/*
	rm -rf ./objects/*
	#// rm -rf ./setup/*
	cd build && cmake .. && make libboojum
	#find . -name \*.a -exec cp {} ./objects \;
	#find . -name \*.o -exec cp {} ./objects \;
	#find . -name \*.so -exec cp {} ./objects \;
	#go test ; notify-send 'Done testing'

clean-build:
	mkdir -p build
	rm -rf ./build/*

build:
	

clean