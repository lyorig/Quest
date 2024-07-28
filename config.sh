configs="d (Debug), r (Release), rd (RelWithDebInfo), rs (MinSizeRel)"

build_type=""

if [[ "$#" -ne 1 ]]; then
	echo "A single parameter is required: ${configs}."
	exit 1
fi

cmd="cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

case $1 in 
	d)
		build_type="Debug"
		;;
	r)
		build_type="Release"
		;;
	rd)
		build_type="RelWithDebInfo"
		;;
	rs)
		build_type="MinSizeRel"
		;;
	*)
		echo "Invalid configuration \"$1\". Valid ones are ${configs}."
		exit 1
		;;
esac

eval "${cmd} -DCMAKE_BUILD_TYPE=${build_type}"
