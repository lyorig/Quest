configs="d (Debug), r (Release), rd (RelWithDebInfo), ms (MinSizeRel)"

build_type=""
tests="OFF"

if [[ "$#" -ne 1 ]]; then
	echo "A single parameter is required: ${configs}."
	exit 1
fi

cmd="cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHalcyon_BUILD_EXAMPLES=ON"

case $1 in 
	d)
		build_type="Debug"
		tests="ON"
		;;
	r)
		build_type="Release"
		;;
	rd)
		build_type="RelWithDebInfo"
		;;
	ms)
		build_type="MinSizeRel"
		;;
	*)
		echo "Invalid configuration \"$1\". Valid ones are ${configs}."
		exit 1
		;;
esac

echo "${build_type}" > type.txt

eval "${cmd} -DCMAKE_BUILD_TYPE=${build_type} -DHalcyon_BUILD_TESTS=${tests}"
