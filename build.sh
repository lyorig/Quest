if ! [ -f type.txt ]; then
	echo "Build type file not found. Did you forget to run cfg.sh?"
	exit 1
fi

type=$(grep "CMAKE_BUILD_TYPE" build/CMakeCache.txt | cut -d= -f2)

echo "Building config ${type}"

cmake --build build -j --config ${type}
