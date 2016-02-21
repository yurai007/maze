#!/bin/bash4
build_version=$1
if [[ $build_version != "debug" ]] && [[ $build_version != "release" ]]; then
	echo "Error! Choose proper build version"
	return 1
fi
players_number=$2
echo Running $build_version maze...
pushd .
cdmaze
cd $build_version
rm -rf client_tmp
cp -rf client client_tmp
cd server
./maze_server gui=off &
sleep 1
cd ../client
./maze_client 127.0.0.1 many $players_number &
cd ../client_tmp
./maze_client 127.0.0.1 gui-on &
popd
echo Done
