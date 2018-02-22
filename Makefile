all: build

make_build_dir:
	[ -d ./build ] | mkdir -p build

build: make_build_dir
	cd build && cmake ..
	make -C build

build-debug: make_build_dir
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
	make -C build
